//Libraries & global include
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //LCD address and communications

// Timer variables
const byte Start= 4; // start/stop button
byte hours = 0;
byte minutes = 0;
byte seconds = 0;
boolean timeState = false;
// Rotary Encoder Module connections

const int PinSW = 5; // Rotary Encoder Switch
const int PinDT = 3;  // DATA signal
const int PinCLK = 2;  // CLOCK signal

//timer fields set?
boolean A_set = false;
boolean B_set = false;

// Variables to debounce Rotary Encoder
long TimeOfLastDebounce = 0;
int DelayofDebounce = 0.01;

// Store previous Pins state
byte PreviousCLK;
byte PreviousDT;
byte displaycounter = 0;
byte MS = 1;

void setup() 
{
  PreviousCLK = digitalRead(PinCLK);  // Put current pins state in variables
  PreviousDT = digitalRead(PinDT);
  
  pinMode(Start, INPUT_PULLUP); 
  pinMode(PinSW, INPUT_PULLUP); // Set the Switch pin to use Arduino PULLUP
  pinMode(PinCLK,INPUT_PULLUP); 
  pinMode(PinDT, INPUT_PULLUP);
  pinMode(PinSW, INPUT_PULLUP);

// Start and setup the LCD 
  Serial.begin(1000000);
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mash Time");
  lcd.setCursor(0,10);
  lcd.print("00:00");

}
void loop() {
// If enough time has passed check the rotary encoder
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce) {

    check_rotary();  // Rotary Encoder check routine below

    PreviousCLK = digitalRead(PinCLK);
    PreviousDT = digitalRead(PinDT);
    TimeOfLastDebounce = millis(); // Set variable to current millis() timer
  }
  
// Check if Rotary Encoder switch was pressed
  if (digitalRead(PinSW) == LOW) 
    displaycounter = 0; // Reset counter to zero
    lcd.print(displaycounter);
  }
  
// Check if Rotary Encoder was moved
void check_rotary() 
{
  if ((PreviousCLK == 0) && (PreviousDT == 1)) {
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)) {
      displaycounter++;
      lcd.clear();
      lcd.setCursor(10,0);
      
    }
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)) {
      displaycounter--;
      lcd.clear();
      lcd.print(displaycounter);
    }
  }

  if ((PreviousCLK == 1) && (PreviousDT == 0)) {
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

  if ((PreviousCLK == 1) && (PreviousDT == 1)) {
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

  if ((PreviousCLK == 0) && (PreviousDT == 0)) {
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
