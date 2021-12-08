#include "LedControl.h" 
#include "snakemenu.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

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
int valueBuzzTone;

int address = 0;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
LiquidCrystal lcd(13, 9, 5, 4, 3, 2);

bool firstGame = true;


int joystickX;
int joystickY;
int xVal;
int yVal;
int swVal;

int dir;
int score = -1;
int highscore = -1;

int snakeSpeed = 1100;
int speedlvl = 0;

bool matrix[matrixSize][matrixSize];
bool started = false;


struct Coordinate{
  int x;
  int y;
} food;


void setup() 
{
  pinMode(buzzPin,OUTPUT);
  randomSeed(analogRead(0));
  pinMode(swPin, INPUT);
  digitalWrite(swPin, HIGH);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.setIntensity(0,2);
  lc.clearDisplay(0);
  pinMode(9, OUTPUT);
}

void verifyMemory(){
  address++;
  if (address == EEPROM.length()) {
    address = 0;
  }
}


void gameOver(){
  int happyface[8][8] = 
  {{0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
  };
  snakeMenuObject.endgame(lcd);
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      matrix[i][j] = happyface[i][j];

  verifyMemory();
  if(score > highscore){
    highscore = score;
    EEPROM.write(address, highscore);
  }
  highscore = EEPROM.read(address);
  lcd.setCursor(0,0);
  lcd.print("Highscore: ");
  lcd.setCursor(0,12);
  lcd.print(highscore);
  delay(1000);
  lcd.clear();
  speedlvl = 0;
  valueBuzzTone = buzzTone;
  for(int i = 0 ; i< 7; i++){
    tone(buzzPin, buzzTone, 20);
    delay(200);
    buzzTone = buzzTone - 100;
  }
  buzzTone = 200;

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, false);
    }
  
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, matrix[i][j]);
    }
 
  delay(5000);
  started = false;
  score = -1;
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      matrix[i][j] = 0;
  snakeSpeed = 1100;
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, false);
    }
}

class Snake{
  private:
    Coordinate snakeCoord[65];
    int len;
    int dir;
  public:
    Snake() {
      snakeCoord[0].x = 1;
      snakeCoord[0].y = 3;
      snakeCoord[1].x = 0;
      snakeCoord[1].y = 3;
      len = 2;
      dir = dirRight;
    }
    
    bool isSnake(Coordinate point) {
      int startPoz = 0; 
      if (snakeCoord[0].x == point.x && snakeCoord[0].y == point.y) {
        startPoz = 1; 
      }
      for (int i = startPoz; i < len; i++){
        if  (snakeCoord[i].x == point.x && snakeCoord[i].y == point.y) {
          return true;
        }
      }
      return false;
    }
    
    void moves(){
      len++;
      for (int i = len-1; i > 0; i--) {
        snakeCoord[i].x = snakeCoord[i-1].x;
        snakeCoord[i].y = snakeCoord[i-1].y;
      }
     if(dir == dirRight){
      snakeCoord[0].x++;
      if (snakeCoord[0].x >= 8) {
        snakeCoord[0].x -= 8;
      }
    }

    if(dir == dirLeft){
      snakeCoord[0].x--;
      if (snakeCoord[0].x < 0) {
       snakeCoord[0].x += 8;
      }
    }

    if(dir == dirUp){
      snakeCoord[0].y--;
      if (snakeCoord[0].y < 0) {
        snakeCoord[0].y += 8;
      }
    }

    if(dir == dirDown){
      snakeCoord[0].y++;
      if (snakeCoord[0].y >= 8) {
       snakeCoord[0].y -= 8;
       }
    }
      if (matrix[snakeCoord[0].x][snakeCoord[0].y] == true) {
        if (isSnake(snakeCoord[0])) {
          gameOver();
        }
        generateFood();
      } 
      else {
        len--;
        matrix[snakeCoord[len].x][snakeCoord[len].y] = false;
        lc.setLed(0, snakeCoord[len].x, snakeCoord[len].y, false);
        
        matrix[snakeCoord[0].x][snakeCoord[0].y] = true;
        lc.setLed(0, snakeCoord[0].x, snakeCoord[0].y, true);
      }
    }
    
    void moves(int newDir) {
      if (newDir != dirCenter && dir + newDir != 5) {
        dir = newDir;
      }
      moves();
    }
    
    
} snake;

void printScore(){
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("Speed: ");
  lcd.setCursor(7,1);
  lcd.print(speedlvl);
  if(speedlvl > 100){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0,1);
    lcd.print("You are fast!");
  }
  /*
  lcd.setCursor(0,1) ;
  if(score >= 2 && score < 5){
    lcd.print("Good!");
  }

  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0,1) ;
  if(score >= 5){
    lcd.print("Great!");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(7, 0);
  lcd.print(score);
  lcd.setCursor(0,1) ;
  if(score >= 10){
    lcd.print("Wow! Keep going!");
  }*/
}


void generateFood(){
  score++;
  buzzTone = buzzTone + 100;
  tone(buzzPin, buzzTone, 20);
  speedlvl = speedlvl + 10;
  printScore();
  while(true) {
    food.x = random(8);
    food.y = random(8);
    if(!snake.isSnake(food)) {
      break;
    }
  }
  matrix[food.x][food.y] = true;
  lc.setLed(0, food.x, food.y, true);
  snakeSpeed /= 1.1;
}


void joystick(){
  swVal = digitalRead(swPin);
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  joystickX = xVal - 511;
  joystickY = yVal - 511;
  if (abs(joystickX) - abs(joystickY) > 0) {
    if (xVal < 400) {
      dir = dirLeft;
    } 
    else if (xVal > 600) {
      dir = dirRight;
    } 
    else {
     dir = dirCenter;
    }
  } 
  else if (abs(joystickX) - abs(joystickY) < 0) {
    if (yVal < 400) {
      dir = dirDown;
    } 
    else if (yVal > 600) {
      dir = dirUp;
    } 
    else {
      dir = dirCenter;
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
    if (dir == dirRight) {
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
  
  snake.moves(dir);

  delay(snakeSpeed);

  
}
