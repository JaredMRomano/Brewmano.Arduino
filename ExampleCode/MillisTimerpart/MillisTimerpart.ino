//Libraries +

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address

unsigned long startTime = millis ();
unsigned long interval = 60000;



void setup() {
  Serial.begin(1000000);
  lcd.begin(20,4);
  lcd.clear();
  lcd.setCursor(0,0);
  

if (millis () - startTime >= interval)
  {
  // do something
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
