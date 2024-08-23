
# Pixel Chase Arcade Game with ESP32

Pixel Chase Arcade game inspired by Adafruit Industries youtube video. They originally made the game in Circuit python. Following the same logic, I rewrote the whole adding additional functions for displaying the score on a 7 segment display. 

Press the button once to start the game. There will be three pixels stationary and one pixel moving around in circle. When the moving circle reach inside stationary circle, you have to press the button to capture it. When pressed it on wrong time game is over. If captured correctly, the next pixel colour will flash once for a second and move on. Score will update on 7 seg display. For each score update, speed of running pixel will increase. When you reach score of 20 you win. Then a rainbow animation is shown on led ring. 


I’m currently working on this game, a few modifications are to be made. 

## Future plans : 
    — Make a stand for mounting the MDF plate. 
    — Paint all in White colour. 
    — Give separation inside the led ring for each pixels. 
    — Make the speed increment upon level increment more linear, currently having a exponential increase in speed. 
    — Implement a multiplier mode by adding one more set. 
## Authors

- [@anoofc](https://www.github.com/anoofc)

