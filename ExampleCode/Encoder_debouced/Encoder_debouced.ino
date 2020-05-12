//Libraries & global include
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


// Rotary Encoder Module connections
const int PinSW=5;   // Rotary Encoder Switch
const int PinDT=3;    // DATA signal
const int PinCLK=2;    // CLOCK signal

// Variables to debounce Rotary Encoder
long TimeOfLastDebounce = 0;
int DelayofDebounce = 0.01;

// Store previous Pins state
byte PreviousCLK;   
byte PreviousDATA;

byte displaycounter=0;

void setup() {
  // Put current pins state in variables
  PreviousCLK=digitalRead(PinCLK);
  PreviousDATA=digitalRead(PinDT);

  // Set the Switch pin to use Arduino PULLUP resistors
  pinMode(PinSW, INPUT_PULLUP);

  // Start and setup the LED MATRIX at startup
  lcd.begin(20,4);
  lcd.setCursor(0,0);
  lcd.print(displaycounter);
  

}
void loop() {
  // If enough time has passed check the rotary encoder
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce) {
    
    check_rotary();  // Rotary Encoder check routine below
    
    PreviousCLK=digitalRead(PinCLK);
    PreviousDATA=digitalRead(PinDT);    
    TimeOfLastDebounce=millis();  // Set variable to current millis() timer
  }
  
  // Check if Rotary Encoder switch was pressed
  if (digitalRead(PinSW) == LOW) {
    displaycounter=0;  // Reset counter to zero
    lcd.print(displaycounter);
  }
}


// Check if Rotary Encoder was moved
void check_rotary() {

 if ((PreviousCLK == 0) && (PreviousDATA == 1)) {
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)) {
      displaycounter++;
      lcd.clear();
      lcd.print(displaycounter);
    }
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)) {
      displaycounter--;
      lcd.clear();
      lcd.print(displaycounter);
    }
  }

if ((PreviousCLK == 1) && (PreviousDATA == 0)) {
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 1)) {
      displaycounter++;
      lcd.clear();
      lcd.print(displaycounter);
    }
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 0)) {
      displaycounter--;
      lcd.clear();
      lcd.print(displaycounter);
    }
  }

if ((PreviousCLK == 1) && (PreviousDATA == 1)) {
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 1)) {
      displaycounter++;
      lcd.clear();
      lcd.print(displaycounter);
    }
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 0)) {
      displaycounter--;
      lcd.clear();
      lcd.print(displaycounter);
    }
  }  

if ((PreviousCLK == 0) && (PreviousDATA == 0)) {
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)) {
      displaycounter++;
      lcd.clear();
      lcd.print(displaycounter);
    }
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)) {
      displaycounter--;
      lcd.clear();
      lcd.print(displaycounter);
    }
  }            
 }
