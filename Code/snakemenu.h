#ifndef SNAKEMENU_H  
#define SNAKEMENU_H
#include <LiquidCrystal.h>

int verif = 0;

class Menu{
 /* private:
    int length = 26;
    char notes[] = "eeeeeeegcde fffffeeeeddedg";
    int beats[] = { 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};

    int tempo = 300;*/
  
  public:

    void startgame(LiquidCrystal &lcd){
       //lcd.begin(16,2);// Print a message to the LCD.
       lcd.setCursor(0, 0);
       lcd.print("Don't be a SNAKE");
       lcd.setCursor(0, 1);
       lcd.print("Play SNAKE!");
       delay(6000);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Author: ");
       lcd.setCursor(0, 1);
       lcd.print("Neacsu Maria");
       delay(4000);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Github user :");
       lcd.setCursor(0,1);
       lcd.print("marianeacsu");
       delay(3000);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Repo :");
       lcd.setCursor(0,1);
       lcd.print("Snake");
       delay(3000);
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Level 1:");
       delay(3000);
       lcd.clear();
    }
    
    void endgame(LiquidCrystal &lcd){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Try again? :)");
      delay(2000);
      lcd.clear();
    }

    
}snakeMenuObject;

#endif
