// Created: 03-08-2024 10:45:00
// Last Modified: 03-08-2024 16:00:00
// Author: ANOOF C
// Company: Interactive Technical Services LLC (ITS) Dubai
// File Description:  A simple game for ESP32 with a button and a neopixel strip. 
//                    The game is to press the button when the light is on the target color. 
//                    The speed of the light will increase as the game goes on. 
//                    The game will end if the button is pressed when the light is not on the target color. 
//                    The game will restart if the button is pressed when the light is on the target color. 
//                    The game will restart if the speed is too fast. 
//                    The game will show a rainbow cycle when the game is over.

#define DEBUG 0         // 1 for debug mode, 0 for normal mode


#define BUTTON_PIN 18   // Button pin
#define PIXEL_PIN 27    // Neopixel pin
#define NUM_PIXELS 51   // Number of neopixels
#define SPEED 105       // Speed of the light
#define FINAL_LEVEL 10  // Final level of the speed
#define LEVEL 5         // How much the speed increase

#include <Arduino.h>              // Include the Arduino library  
#include <Adafruit_NeoPixel.h>    // Include the Adafruit NeoPixel library
#include <ShiftRegister74HC595.h>

// Create an instance of the shift register
ShiftRegister74HC595 sr (2, 23, 21, 19);
uint8_t digit1, digit2;

uint8_t  numberB[] = {B10101111, //0
                      B00001100, //1 
                      B11000111, //2
                      B11001110, //3 
                      B01101100, //4
                      B11101010, //5
                      B11101011, //6
                      B00001110, //7
                      B11101111, //8
                      B11101110  //9
                     };


Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);    // Create a NeoPixel object


uint8_t score = 0;            // Score

int8_t x, y, z;               // Target color position
int8_t num = 0;               // Current color position
int8_t last_num = 0;          // Last color position
int now_color = 0;            // Current color
int next_color = 1;           // Next color
int speed = SPEED;            // Speed of the light
uint8_t level = LEVEL;            // How much the speed will increase
bool new_target = true;       // New target color
bool button_state = false;    // Button state
bool game_running = false;    // Game state
unsigned long last_time = 0;  // Last time of the light

uint32_t colors[] = {         // Colors
  pixels.Color(255, 0,     0),    // RED
  pixels.Color(255, 165,   0),  // ORANGE
  pixels.Color(0,   255,   0),    // GREEN
  pixels.Color(0,   255, 255),  // CYAN
  pixels.Color(0,   0,   255),    // BLUE
  pixels.Color(255, 255,   0),  // YELLOW
  pixels.Color(128, 0,   128),  // PURPLE
  pixels.Color(0,   128, 128),  // TEAL
  pixels.Color(255, 0,   255),  // MAGENTA
  pixels.Color(0,   255, 128),  // SPRING GREEN
  pixels.Color(0,   255, 255),  // AQUA
  pixels.Color(255, 0,   128)   // ROSE
};


void blink(){
  for(int i = 0; i<3; i++){
    sr.setAllHigh(); // set all pins LOW
    delay(300);
    sr.setAllLow(); // set all pins HIGH
    delay(300);
    
  }
}   

void displayScore(uint16_t score) {
    digit1=score % 10 ;
    digit2=(score / 10) % 10 ;
    
    uint8_t numberToPrint[]= {numberB[digit1],numberB[digit2]};
    sr.setAll(numberToPrint); 
}

// Wheel function to create a rainbow color
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;                                      // Wheel position is 255 - Wheel position
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);     // Return the color
  }
  if (WheelPos < 170) {                                           // If the Wheel position is less than 170
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);     // Return the color
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);       // Return the color
}

// Rainbow cycle function
void rainbowCycle(int wait) {
  for (int j = 0; j < 256; j++) {                                       // For loop from 0 to 256
    for (int i = 0; i < NUM_PIXELS; i++) {                              // For loop from 0 to NUM_PIXELS
      pixels.setPixelColor(i, Wheel((i * 256 / NUM_PIXELS + j) & 255)); // Set the pixel color to the Wheel color
    }
    pixels.show();
    delay(wait);
  }
}


//  Color chase function
void colorChase(uint32_t c, int wait) {    
  for (int i = 0; i < NUM_PIXELS; i++) {    // For loop from 0 to NUM_PIXELS
    pixels.setPixelColor(i, c);             // Set the pixel color to c
    delay(wait);
    pixels.show();                          // Show the neopixels
  }
  delay(500);
}


// Game over function
void gameOver() {
  colorChase(pixels.Color(0, 0, 0), 10);    // Color chase with black color

  for (int i = 0; i < 3; i++) {             // Flash the neopixels
    pixels.fill(pixels.Color(255, 0, 0), 0, NUM_PIXELS);    // Fill the neopixels with red color
    pixels.show();                                          // Show the neopixels
    delay(300);                                             // Delay 300ms
    pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);      // Fill the neopixels with black color
    pixels.show();                                          // Show the neopixels
    delay(300);                                                   
    
  }
  delay(1000);
  for (uint8_t i = 0; i<3; i++  ){
    displayScore(score);
    delay(300);
    sr.setAllLow(); // set all pins HIGH
    delay(300);
  }
  score = 0;    // Score is 0
  displayScore(score);    // Display the score
  game_running = false;                     // Game state is false
  if (DEBUG) { Serial.println("Game Over");}    // Print Game Over
  
}

// Button state handler
void butttonStateHandler(){
  
  if (!digitalRead(BUTTON_PIN) && !button_state) {    // If the button is pressed and the button state is false
    button_state = true;                              // Button state is true
  }
}

void newTargetHandler(){                  // New target handler
  if (new_target) {                       // If new target is true
    y = random(5, NUM_PIXELS-5);          // Random number between 5 and NUM_PIXELS-5
    x = y - 1;                            // x is y - 1
    z = y + 1;                            // z is y + 1
    new_target = false;                   // New target is false
    pixels.setPixelColor(x, pixels.Color(255, 255, 255));   // Set the x pixel color to white
    pixels.setPixelColor(y, colors[next_color]);            // Set the y pixel color to the next color
    pixels.setPixelColor(z, pixels.Color(255, 255, 255));   // Set the z pixel color to white
    if (DEBUG) { Serial.println("x: " + String(x) + "\t y: " + String(y) + "\t z: " + String(z));}      // Print the x, y, z values
  }
}

// Game running handler
void gameRunningHandler(){
  if (millis() - last_time > speed) {    // If the current time - last time is greater than speed * 1000
    if (num > 0) {                              // If num is greater than 0
      last_num = num - 1;                       // Last num is num - 1
      pixels.setPixelColor(last_num, pixels.Color(0, 0, 0));    // Set the last num pixel color to black
      pixels.show();                                            // Show the neopixels
    }

    if (last_num == x || last_num == y || last_num == z) {    // If the last num is x or y or z
      pixels.setPixelColor(x, pixels.Color(255, 255, 255));   // Set the x pixel color to white
      pixels.setPixelColor(y, colors[next_color]);            // Set the y pixel color to the next color
      pixels.setPixelColor(z, pixels.Color(255, 255, 255));   // Set the z pixel color to white
    }

    if (num < NUM_PIXELS) {                           // If num is less than NUM_PIXELS
      pixels.setPixelColor(num, colors[now_color]);   // Set the num pixel color to the now color
      pixels.show();                                  // Show the neopixels
      num++;                                          // Increment the num
    }

    if (num == NUM_PIXELS) {                                   // If num is equal to NUM_PIXELS
      last_num = num - 1;                                      // Last num is num - 1
      pixels.setPixelColor(last_num, pixels.Color(0, 0, 0));   // Set the last num pixel color to black
      pixels.show();                                           // Show the neopixels
      num = 0;                                                 // Num is 0
    }

    if ((last_num == x || last_num == y || last_num == z) && !digitalRead(BUTTON_PIN)) {   // If the last num is x or y or z and the button is pressed
      button_state = false;
      score++;                                            // Increment the score
      displayScore(score);  
      pixels.fill(colors[next_color], 0, NUM_PIXELS);                    // Fill the neopixels with the next color
      pixels.show();                                                     // Show the neopixels                  
      delay(500);
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);                 // Fill the neopixels with black color
      pixels.show();
      speed = speed - level;                                     // Speed is speed - level
      // level--; 
      Serial.println("Level: " + String(level) + "\t Speed: " + String(speed));                // Print the level
      Serial.println("Score: " + String(score));          // Print the score
      next_color = (next_color + 1) % (sizeof(colors)/sizeof(colors[0]));                 // Next color is next color + 1
      now_color = (now_color + 1) % (sizeof(colors)/sizeof(colors[0]));                   // Now color is now color + 1
      new_target = true;
      if (DEBUG) { Serial.println("speed is " + String(speed) + "\t Button is " + String (digitalRead(BUTTON_PIN)));}     // Print the speed and button state
      
     }

    if ((last_num != x && last_num != y && last_num != z) && !digitalRead(BUTTON_PIN)) {      // If the last num is not x or y or z and the button is pressed
      button_state = false;
      pixels.fill(colors[now_color], 0, NUM_PIXELS);                    // Fill the neopixels with the now color
      pixels.show();
      gameOver();                                                       // Game over function
      num = 0;
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);                // Fill the neopixels with black color
      pixels.show();
      speed = SPEED;                                                    // Speed is SPEED
      next_color = 1;
      now_color = 0;
      new_target = true;
      if (DEBUG) { Serial.println("speed is " + String(speed) + "\t Button is " + String (digitalRead(BUTTON_PIN)));}   // Print the speed and button state
      
    }

    if (speed < FINAL_LEVEL) {          // If the speed is less than the final level
      for (uint8_t i = 0; i<8; i++  ){
        rainbowCycle(10); 
      }
      delay(1000);
      for (uint8_t i = 0; i<3; i++  ){
        displayScore(score);
        delay(300);
        sr.setAllLow(); // set all pins HIGH
        delay(300);
      }
      score = 0;                        // Score is 0
      game_running = false;             // Game state is false
      displayScore(score);              // Display the score
      num = 0;                          // Num is 0
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);      // Fill the neopixels with black color
      pixels.show();
      speed = SPEED;                       // Speed is SPEED
      next_color = 1;                      // Next color is 1
      now_color = 0;                       // Now color is 0
      new_target = true;                   // New target is true
    }

    last_time = millis();                                     // Last time is millis
  }
}

void setup() {
  Serial.begin(9600);                     // Begin the serial communication
  pinMode(BUTTON_PIN, INPUT_PULLUP);      // Set the button pin as input pullup
  pixels.begin();                         // Begin the neopixels
  pixels.setBrightness(255);              // Set the brightness of the neopixels
  blink();
  displayScore(score);  
}


void loop() {
  if (!game_running) {                    // If the game is not running
    if (!digitalRead(BUTTON_PIN)) {       // If the button is pressed
      game_running = true;                // Game state is true
      delay(500);
    }
  }
  if (game_running){                    // If the game is running
    butttonStateHandler();              // Button state handler
    newTargetHandler();                 // New target handler
    gameRunningHandler();               // Game running handler
  }
}
