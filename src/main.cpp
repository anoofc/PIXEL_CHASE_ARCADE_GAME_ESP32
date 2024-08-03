#define DEBUG 1

#include <Arduino.h>

#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN 18
#define PIXEL_PIN 27
#define NUM_PIXELS 40

Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


uint8_t x, y, z;

uint8_t num = 0;
uint8_t last_num = 0;
int now_color = 0;
int next_color = 1;
float speed = 0.1;
float level = 0.005;
float final_level = 0.001;
bool new_target = true;
bool button_state = false;
unsigned long last_time = 0;

uint32_t colors[] = {
  pixels.Color(255, 0, 0),    // RED
  pixels.Color(255, 165, 0),  // ORANGE
  pixels.Color(255, 255, 0),  // YELLOW
  pixels.Color(0, 255, 0),    // GREEN
  pixels.Color(0, 128, 128),  // TEAL
  pixels.Color(0, 255, 255),  // CYAN
  pixels.Color(0, 0, 255),    // BLUE
  pixels.Color(128, 0, 128),  // PURPLE
  pixels.Color(255, 0, 255),  // MAGENTA
  pixels.Color(255, 215, 0),  // GOLD
  pixels.Color(0, 255, 255),  // AQUA
  pixels.Color(255, 192, 203) // PINK
};


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(int wait) {
  for (int j = 0; j < 256; j++) {
    for (int i = 0; i < NUM_PIXELS; i++) {
      pixels.setPixelColor(i, Wheel((i * 256 / NUM_PIXELS + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}



void colorChase(uint32_t c, int wait) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, c);
    delay(wait);
    pixels.show();
  }
  delay(500);
}

void gameOver() {
  colorChase(pixels.Color(0, 0, 0), 50);
  for (int i = 0; i < 3; i++) {
    pixels.fill(pixels.Color(255, 0, 0), 0, NUM_PIXELS);
    pixels.show();
    delay(500);
    pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);
    pixels.show();
    delay(500);
  }
}


void butttonStateHandler(){
  if (!digitalRead(BUTTON_PIN) && !button_state) {
    button_state = true;
  }
}

void newTargetHandler(){
  if (new_target) {
    y = random(5, NUM_PIXELS-5);
    x = y - 1;
    z = y + 1;
    new_target = false;
    pixels.setPixelColor(x, pixels.Color(255, 255, 255));
    pixels.setPixelColor(y, colors[next_color]);
    pixels.setPixelColor(z, pixels.Color(255, 255, 255));
    if (DEBUG) { Serial.println("x: " + String(x) + "\t y: " + String(y) + "\t z: " + String(z));}
  }
}

void gameRunningHandler(){
  
  if (millis() - last_time > speed * 1000) {
    if (num > 0) {
      last_num = num - 1;
      pixels.setPixelColor(last_num, pixels.Color(0, 0, 0));
      pixels.show();
    }

    if (last_num == x || last_num == y || last_num == z) {
      pixels.setPixelColor(x, pixels.Color(255, 255, 255));
      pixels.setPixelColor(y, colors[next_color]);
      pixels.setPixelColor(z, pixels.Color(255, 255, 255));
    }

    if (num < NUM_PIXELS) {
      pixels.setPixelColor(num, colors[now_color]);
      pixels.show();
      num++;
    }

    if (num == NUM_PIXELS) {
      last_num = num - 1;
      pixels.setPixelColor(last_num, pixels.Color(0, 0, 0));
      pixels.show();
      num = 0;
    }

    if ((last_num == x || last_num == y || last_num == z) && !digitalRead(BUTTON_PIN)) {
      button_state = false;
      pixels.fill(colors[next_color], 0, NUM_PIXELS);
      pixels.show();
      delay(500);
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);
      pixels.show();
      speed -= level;
      next_color = (next_color + 1) % 12;
      now_color = (now_color + 1) % 12;
      new_target = true;
      if (DEBUG) { Serial.println("speed is " + String(speed) + "\t Button is " + String (digitalRead(BUTTON_PIN)));}
      
     }

    if ((last_num != x && last_num != y && last_num != z) && !digitalRead(BUTTON_PIN)) {
      button_state = false;
      pixels.fill(colors[now_color], 0, NUM_PIXELS);
      pixels.show();
      gameOver();
      num = 0;
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);
      pixels.show();
      speed = 0.1;
      next_color = 1;
      now_color = 0;
      new_target = true;
      if (DEBUG) { Serial.println("speed is " + String(speed) + "\t Button is " + String (digitalRead(BUTTON_PIN)));}
      
    }

    if (speed < final_level) {
      rainbowCycle(10);
      delay(1000);
      num = 0;
      pixels.fill(pixels.Color(0, 0, 0), 0, NUM_PIXELS);
      pixels.show();
      speed = 0.1;
      next_color = 1;
      now_color = 0;
      new_target = true;
    }

    last_time = millis();
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pixels.begin();
  pixels.setBrightness(50);
}


void loop() {
  butttonStateHandler();
  newTargetHandler();
  gameRunningHandler();
}
