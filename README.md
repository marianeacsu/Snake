# Snake

## Game description

Initially, you have two leds representing your snake and you can move along the Ox, Oy axes. The matrix is not bordered, so you will appear starting from the symmetrically position. The remaining led is the apple to be collected, when the apple is eaten, the length of the snake increases by 1 and your speed also goes up, making the game harder. Following this, the apple will respawn at a random position. If ever happen to bite yourself you die. You can reach the second level if your score is more than 5. Level 2 is defined by the walls which will randomly spawn on the matrix. Finally, you can die by hitting the wall.

## Menu

Menu for Snake game displayed on a LCD Display:
- the highscore is saved and showed at the end of the game
- different messages are displayed at the beginning, ending and during the game
- the score is constantly updating while playing the game
- different logic sounds are available (while eating, dying) 

## Inspiration

When I was little this was my favourite video game

## User experience

- you are a snake represented by a series of neighboring leds
- when you are eating an apple, your length will increases by 1
- your speed will go up while eating the apple
- when you are moving, the led in front of you will light up and the last led will turn off
- the wall is defined by two leds
- firstly, the wall will appear and then the apple

## Components

- Arduino UNO
- 16x2 LCD
- Joystick
- Active buzzer
- 8x8 led matrix
- Breadboards
- Wires
- Electrolytic capacitor
- Ceramic capacitor
- Resistors

## Photo of the setup

![264048071_941499923141862_960140267181666933_n](https://user-images.githubusercontent.com/58784210/145245553-6788742a-30a5-4687-a1de-1009b11d0686.jpg)

## Video

https://www.youtube.com/watch?v=Qs282kU9R_k

## [Code](https://github.com/marianeacsu/Snake/tree/main/Code)
