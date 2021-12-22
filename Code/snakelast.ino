#include "LedControl.h"
#include "snakemenu.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define PinRS 13
#define PinE 9
#define D4 5
#define D5 4
#define D6 3
#define D7 2
#define numberOfTones 7
#define randomPin 0

const int swPin = 8;
const int xPin = A0;
const int yPin = A1;

const int dirCenter = 0;
const int dirLeft = 1;
const int dirUp = 2;
const int dirRight = 4;
const int dirDown = 3;
const int matrixSize = 8;

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int buzzPin = 7;
int buzzTone = 200;
int valueBuzzTone = 0;

int address = 0;
int ct = 0;
int ok = 0;
int previouscore = 0;
int ok2 = 0;
int ok3 = 0;
int transition = 5;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
LiquidCrystal lcd(PinRS, PinE, D4, D5, D6, D7);

bool firstGame = true;


int joystickX = 0;
int joystickY = 0;
int xVal = 0;
int yVal = 0;
int swVal = 0;

int directions = 0;
int score = -1;
int highscore = -1;

int snakeSpeed = 1200;
int speedlvl = 0;

bool matrix[matrixSize][matrixSize];
bool started = false;


struct Coordinate {
  int x;
  int y;
};

Coordinate food;
Coordinate wallFirstPiece;
Coordinate wallSecondPiece;

void setup()
{
  pinMode(buzzPin, OUTPUT);
  randomSeed(analogRead(randomPin));
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);
  pinMode(PinE, OUTPUT);
  wallFirstPiece.x = -1;
  wallFirstPiece.y = -1;
  wallSecondPiece.x = -1;
  wallSecondPiece.y = -1;
}

//manipulate the pointing address at the EEPROM

void verifyMemory() {
  address++;
  if (address == EEPROM.length()) {
    address = 0;
  }
}

//animation for the end of the game and transition between the two levels

void stepGame() {
  bool happyface[matrixSize][matrixSize] =
  { {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  };
  snakeMenuObject.endgame(lcd);
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++)
      matrix[i][j] = happyface[i][j];

  verifyMemory();
  if (score > highscore) {
    highscore = score;
    EEPROM.update(address, highscore);
  }
  highscore = EEPROM.read(address);
  lcd.setCursor(0, 0);
  lcd.print("Highscore: ");
  lcd.setCursor(0, 12);
  lcd.print(highscore);
  delay(1000); //print highscore
  lcd.clear();
  speedlvl = 0;
  valueBuzzTone = buzzTone;
  for (int i = 0 ; i < numberOfTones; i++) {
    tone(buzzPin, buzzTone, 20);
    delay(200); //for sound
    buzzTone = buzzTone - 100;
  }
  buzzTone = 200;
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, false);
    }

  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, matrix[i][j]);
    }

  delay(5000); //for happy face
  lcd.setCursor(0, 0);
  if (score > transition) {
    ct = 1;
  }
  if (ct == 1) {
    lcd.print("Level 2:");
    delay(2000); //print level 2
  }
  started = false;
  score = -1;
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++)
      matrix[i][j] = 0;
  snakeSpeed = 1100;
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, false);
    }
}

//define the snake, his actions and the start position

class Snake {
  private:
    Coordinate snakeCoord[matrixSize * matrixSize];
    int snakeLen;
    int directions;
  public:
    Snake() {
      snakeCoord[0].x = 1;
      snakeCoord[0].y = 3;
      snakeCoord[1].x = 0;
      snakeCoord[1].y = 3;
      snakeLen = 2;
      directions = dirRight;
    }

    // verify if the point is a cell of the snake

    bool isSnake(Coordinate point) {
      int startPoz = 0;
      if (snakeCoord[0].x == point.x && snakeCoord[0].y == point.y) {
        startPoz = 1;
      }
      for (int i = startPoz; i < snakeLen; i++) {
        if  (snakeCoord[i].x == point.x && snakeCoord[i].y == point.y) {
          return true;
        }
      }
      return false;
    }

    //do snake moves

    void moves() {
      snakeLen++; //consider our x lengthed snake x+1 lengthed shifting all the positions in our representation
      for (int i = snakeLen - 1; i > 0; i--) { //and modifying the head of the snake based on the desired move
        snakeCoord[i].x = snakeCoord[i - 1].x;
        snakeCoord[i].y = snakeCoord[i - 1].y;
      }
      if (directions == dirRight) {
        snakeCoord[0].x++;
        if (snakeCoord[0].x >= matrixSize) {
          snakeCoord[0].x -= matrixSize;
        }
      }

      if (directions == dirLeft) {
        snakeCoord[0].x--;
        if (snakeCoord[0].x < 0) {
          snakeCoord[0].x += matrixSize;
        }
      }

      if (directions == dirUp) {
        snakeCoord[0].y--;
        if (snakeCoord[0].y < 0) {
          snakeCoord[0].y += matrixSize;
        }
      }

      if (directions == dirDown) {
        snakeCoord[0].y++;
        if (snakeCoord[0].y >= matrixSize) {
          snakeCoord[0].y -= matrixSize ;
        }
      }
      // when you meet a lighted up led, you either die or remain the same length
      if (matrix[snakeCoord[0].x][snakeCoord[0].y] == true) {
        if (isSnake(snakeCoord[0]) || ( snakeCoord[0].x == wallFirstPiece.x && snakeCoord[0].y == wallFirstPiece.y) || ( snakeCoord[0].x == wallSecondPiece.x && snakeCoord[0].y == wallSecondPiece.y) ) {
          stepGame();
        }
        ok3 = 0;
        // for level 2
        if (ct == 1) {
          generateWall();
          delay(500); // to differentiate the leds of the wall and the apple
          generateFood();
        }
        //for level 1
        else {
          generateFood();
        }

      }
      else {
        snakeLen--; //if entering an empty cell my length decreases because I did not eat any apple
        //verify if the wall is created inside the snake and if it is in the last position I do not turn it off
        if ( !(snakeCoord[snakeLen].x == wallFirstPiece.x && snakeCoord[snakeLen].y == wallFirstPiece.y) && !(snakeCoord[snakeLen].x == wallSecondPiece.x && snakeCoord[snakeLen].y == wallSecondPiece.y) ) {
          matrix[snakeCoord[snakeLen].x][snakeCoord[snakeLen].y] = false;
          lc.setLed(0, snakeCoord[snakeLen].x, snakeCoord[snakeLen].y, false);
        }

        matrix[snakeCoord[0].x][snakeCoord[0].y] = true;
        lc.setLed(0, snakeCoord[0].x, snakeCoord[0].y, true);
      }
    }

    void moves(int newDir) {
      if (newDir != dirCenter && directions + newDir != 5) {
        directions = newDir;
      }
      moves();
    }


} snake;

void printScore() {
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(7, 0);
  lcd.print(score);
  if (score < 5) {
    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.setCursor(7, 1);
    lcd.print(speedlvl);
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0, 1);
    if (ct == 0) {
      lcd.print("U got to 2nd lvl");
    }
    else {
      lcd.print("Keep going!");
    }
  }
  if (speedlvl > 100) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0, 1);
    lcd.print("You are fast!");
  }

}

bool verifyNeighbours(int a, int b) {
  if ( (food.x == a - 1 && food.y == b) || (food.x == a && food.y == b + 1) || (food.x == a + 1 && food.y == b) || (food.x == a && food.y == b - 1)) {
    return true;
  }
  return false;
}

//to generate food

void generateFood() {
  score++;
  buzzTone = buzzTone + 100;
  tone(buzzPin, buzzTone, 20);
  speedlvl = speedlvl + 10;
  printScore();
  while (true) {
    food.x = random(matrixSize);
    food.y = random(matrixSize);
    if (!snake.isSnake(food)) {
      break;
    }
  }
  matrix[food.x][food.y] = true;
  lc.setLed(0, food.x, food.y, true);
  snakeSpeed /= 1.2;
  previouscore = score;

}

//to generate wall

void generateWall() {
  if (ok2 == 1) {
    deleteWall();
  }
  while (true) {
    wallFirstPiece.x = random(matrixSize);
    wallFirstPiece.y = random(matrixSize);
    if (!snake.isSnake(wallFirstPiece) || matrix[food.x][food.y] == true) {
      break;
    }
  }
  matrix[wallFirstPiece.x][wallFirstPiece.y] = true;
  lc.setLed(0, wallFirstPiece.x, wallFirstPiece.y, true);
  ok = 0;
  if (wallFirstPiece.x == 0 && wallFirstPiece.y == 0 && ok == 0) {
    matrix[1][0] = true;
    lc.setLed(0, 1, 0, true);
    wallSecondPiece.x = 1;
    wallSecondPiece.y = 0;
    ok = 1;
  }

  if (wallFirstPiece.x == 0 && wallFirstPiece.y == 7 && ok == 0) {
    matrix[1][7] = true;
    lc.setLed(0, 1, 7, true);
    wallSecondPiece.x = 1;
    wallSecondPiece.y = 7;
    ok = 1;
  }

  if (wallFirstPiece.x == 7 && wallFirstPiece.y == 0 && ok == 0) {
    matrix[7][1] = true;
    lc.setLed(0, 7, 1, true);
    wallSecondPiece.x = 7;
    wallSecondPiece.y = 1;
    ok = 1;
  }

  if (wallFirstPiece.x == 7 && wallFirstPiece.y == 7 && ok == 0) {
    matrix[7][6] = true;
    lc.setLed(0, 7, 6, true);
    wallSecondPiece.x = 7;
    wallSecondPiece.y = 6;
    ok = 1;
  }

  if (wallFirstPiece.x == 0 && ok == 0) {
    matrix[1][wallFirstPiece.y] = true;
    lc.setLed(0, 1, wallFirstPiece.y, true);
    wallSecondPiece.x = 1;
    wallSecondPiece.y = wallFirstPiece.y;
    ok = 1;
  }

  if (wallFirstPiece.x == 7 && ok == 0) {
    matrix[6][wallFirstPiece.y] = true;
    lc.setLed(0, 6, wallFirstPiece.y, true);
    wallSecondPiece.x = 6;
    wallSecondPiece.y = wallFirstPiece.y;
    ok = 1;
  }

  if (wallFirstPiece.y == 0 && ok == 0) {
    matrix[wallFirstPiece.x][1] = true;
    lc.setLed(0, wallFirstPiece.x, 1, true);
    wallSecondPiece.x = wallFirstPiece.x;
    wallSecondPiece.y = 1;
    ok = 1;
  }

  if (wallFirstPiece.y == 7 && ok == 0) {
    matrix[wallFirstPiece.x][6] = true;
    lc.setLed(0, wallFirstPiece.x, 6, true);
    wallSecondPiece.x = wallFirstPiece.x;
    wallSecondPiece.y = 6;
    ok = 1;
  }
  if (ok == 0) {
    matrix[wallFirstPiece.x + 1][wallFirstPiece.y] = true;
    lc.setLed(0, wallFirstPiece.x + 1, wallFirstPiece.y, true);
    wallSecondPiece.x = wallFirstPiece.x + 1;
    wallSecondPiece.y = wallFirstPiece.y;
    ok = 1;
  }
  ok2 = 1;
}

void deleteWall() {
  matrix[wallFirstPiece.x][wallFirstPiece.y] = false;
  lc.setLed(0, wallFirstPiece.x, wallFirstPiece.y, false);
  matrix[wallSecondPiece.x][wallSecondPiece.y] = false;
  lc.setLed(0, wallSecondPiece.x, wallSecondPiece.y, false);
}


void joystick() {
  swVal = digitalRead(swPin);
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  joystickX = xVal - 511;
  joystickY = yVal - 511;
  if (abs(joystickX) - abs(joystickY) > 0) {
    if (xVal < 400) {
      directions = dirLeft;
    }
    else if (xVal > 600) {
      directions = dirRight;
    }
    else {
      directions = dirCenter;
    }
  }
  else if (abs(joystickX) - abs(joystickY) < 0) {
    if (yVal < 400) {
      directions = dirDown;
    }
    else if (yVal > 600) {
      directions = dirUp;
    }
    else {
      directions = dirCenter;
    }
  }
  else {
    return;
  }
}


void loop()
{ joystick();
  if (!started)
  {
    if (directions == dirRight) {
      snake = Snake();
      started = true;
      if (firstGame) {
        snakeMenuObject.startgame(lcd);
      }
      matrix[0][3] = true;
      lc.setLed(0, 0, 3, true);
      matrix[1][3] = true;
      lc.setLed(0, 1, 3, true);
      if (firstGame) {
        firstGame = false;
        generateFood();
      }
    }
    return;
  }

  snake.moves(directions);

  delay(snakeSpeed); //change snake speed


}
