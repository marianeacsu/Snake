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
int ct = 0;
int ok;
int previouscore;
int ok2 = 0;
int ok3 = 0;

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
};

Coordinate food;
Coordinate wall;
Coordinate wall1;

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
  wall.x = -1;
  wall.y = -1;
  wall1.x = -1;
  wall1.y = -1;
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
  lcd.setCursor(0,0);
   if(score> 5){
    ct = 1;
  }
  if(ct == 1){
    lcd.print("Level 2:");
    delay(2000);
  }
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
        if (isSnake(snakeCoord[0]) || ( snakeCoord[0].x == wall.x && snakeCoord[0].y == wall.y) || ( snakeCoord[0].x == wall1.x && snakeCoord[0].y == wall1.y) ) {
          gameOver();
        }
        ok3=0;
        if(ct == 1){ 
          generateWall();
          delay(500);
          generateFood();
        }
        else{
          generateFood();
        }
        
      } 
      else {
        len--;
        if( !(snakeCoord[len].x == wall.x && snakeCoord[len].y == wall.y) && !(snakeCoord[len].x == wall1.x && snakeCoord[len].y == wall1.y) ){
          matrix[snakeCoord[len].x][snakeCoord[len].y] = false;
          lc.setLed(0, snakeCoord[len].x, snakeCoord[len].y, false);
        }
        
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
  if(score < 5){
    lcd.setCursor(0,1);
    lcd.print("Speed: ");
    lcd.setCursor(7,1);
    lcd.print(speedlvl);
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.setCursor(7, 0);
    lcd.print(score);
    lcd.setCursor(0,1);
    if(ct==0){
      lcd.print("U got to 2nd lvl");
    }
    else{
      lcd.print("Keep going!");
    }
  }
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

bool verifyNeighbours(int a, int b){
  if ( (food.x == a-1 && food.y == b) || (food.x == a && food.y == b+1) || (food.x == a+1 && food.y == b) || (food.x == a && food.y == b-1)){
    return true;
  }
  return false;
}


void generateFood(){
  score++;
  //if(score > previouscore){
  //  deleteWall();
  //}
  //deleteWall();
  buzzTone = buzzTone + 100;
  tone(buzzPin, buzzTone, 20);
  speedlvl = speedlvl + 10;
  printScore();
  while(true) {
    food.x = random(8);
    food.y = random(8);
    //if(!snake.isSnake(food) || !snake.isSnake(wall) || !snake.isSnake(wall1) || (food.x == wall.x && food.y == wall.y) || (food.x == wall1.x && food.y == wall1.y) || verifyNeighbours(wall.x, wall.y)==true || verifyNeighbours(wall1.x, wall1.y)==true ) {
    if(!snake.isSnake(food)){
      break;
    }
  }
  matrix[food.x][food.y] = true;
  lc.setLed(0, food.x, food.y, true);
  snakeSpeed /= 1.1;
  previouscore = score;
 // deleteWall();
}

void generateWall(){
  if(ok2==1){
    deleteWall();
  }
  while(true) {
    wall.x = random(8);
    wall.y = random(8);
    if(!snake.isSnake(wall) || matrix[food.x][food.y] == true) {
      break;
    }
  }
  matrix[wall.x][wall.y] = true;
  lc.setLed(0, wall.x, wall.y, true);
  ok=0;
  if(wall.x == 0 && wall.y == 0 && ok ==0){
    matrix[1][0] = true;
    lc.setLed(0, 1, 0, true);
    wall1.x = 1;
    wall1.y = 0;
    ok = 1;
  }

  if(wall.x == 0 && wall.y == 7 && ok ==0){
    matrix[1][7] = true;
    lc.setLed(0, 1, 7, true);
    wall1.x = 1;
    wall1.y = 7;
    ok = 1;
  }

   if(wall.x == 7 && wall.y == 0 && ok ==0){
    matrix[7][1] = true;
    lc.setLed(0, 7, 1, true);
    wall1.x = 7;
    wall1.y = 1;
    ok = 1;
  }

   if(wall.x == 7 && wall.y == 7 && ok ==0){
    matrix[7][6] = true;
    lc.setLed(0, 7, 6, true);
    wall1.x = 7;
    wall1.y = 6;
    ok = 1;
  }

  if(wall.x == 0 && ok ==0){
    matrix[1][wall.y] = true;
    lc.setLed(0, 1, wall.y, true);
    wall1.x = 1;
    wall1.y = wall.y;
    ok = 1;
  }

  if(wall.x == 7 && ok ==0){
    matrix[6][wall.y] = true;
    lc.setLed(0, 6, wall.y, true);
    wall1.x = 6;
    wall1.y = wall.y;
    ok = 1;
  }

  if(wall.y == 0 && ok ==0){
    matrix[wall.x][1] = true;
    lc.setLed(0, wall.x, 1, true);
    wall1.x = wall.x;
    wall1.y = 1;
    ok = 1;
  }

  if(wall.y == 7 && ok ==0){
    matrix[wall.x][6] = true;
    lc.setLed(0, wall.x, 6, true);
    wall1.x = wall.x;
    wall1.y = 6;
    ok = 1;
  }
  if(ok == 0){
    matrix[wall.x + 1][wall.y] = true;
    lc.setLed(0, wall.x + 1, wall.y, true);
    wall1.x = wall.x + 1;
    wall1.y = wall.y;
    ok = 1; 
  }
  ok2=1;
  //delay(5000);
}

void deleteWall(){
  matrix[wall.x][wall.y] = false;
  lc.setLed(0,wall.x, wall.y, false);
  matrix[wall1.x][wall1.y] = false;
  lc.setLed(0,wall1.x, wall1.y, false);
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
