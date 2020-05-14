#include <LiquidCrystal_I2C.h>

//Libraries & global include
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20,4);

// Rotary Encoder Module connections
const int encoder0PinA = 2; // CLOCK signal
const int encoder0PinB = 3; // DATA signal
const int PinSW = 5;        // Rotary Encoder Switch
volatile byte encoder0Pos = 0;
byte oldEncoder0Position = 0;

// Variables to debounce Rotary Encoder
long TimeOfLastDebounce = 0;
int DelayofDebounce = 0.02;
bool PrintDisplayCounter = true;

// Variables for timer
long TimerStartTime = 0;
bool IsTimerRunning = false;

void setup()
{
  lcd.init();
  lcd.setBacklight(LCD_BACKLIGHT);
  lcd.clear();
  lcd.setCursor(0,0);
  Serial.begin(9600);



  lcd.print(encoder0Pos);
  initializeRotaryEncoder();
}

void loop()
{
  
  if (oldEncoder0Position != encoder0Pos)
  {
    oldEncoder0Position = encoder0Pos;
    // Do encoder moved stuff stuff
    //Ex:
    printDisplayCounter();
     Serial.print(encoder0Pos);
     Serial.print(oldEncoder0Position);
     Serial.println();
  }
//  // Check if Rotary Encoder switch was pressed
//  if (digitalRead(PinSW) == LOW)
//  {
//    // Do button pressed stuff
//    printDisplayCounter();
//  }
}

// Returns true if the encoder has moved from last position
bool hasEncoderMoved()
{
  if (oldEncoder0Position != encoder0Pos)
  {
    oldEncoder0Position = encoder0Pos;
    return true;
  }
  else
  {
    return false;
  }
  
}

void printDisplayCounter()
{
  if (PrintDisplayCounter)
  {
    lcd.clear();
    lcd.print(encoder0Pos);
  }
}

void print_1Line(String output)
{
  lcd.clear();
  lcd.print(output);
}

void print_2Line(String output, String output2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1);
  lcd.print(output);
  lcd.setCursor(0, 1);
  lcd.print(output2);
}

void initializeRotaryEncoder()
{
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoder, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  //attachInterrupt(1, doEncoder, CHANGE);
}

// Interrupt on A changing state
void doEncoder()
{
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce) {
    unsigned int A = digitalRead(encoder0PinA);
    unsigned int B = digitalRead(encoder0PinB);
  
    // A xor B == true ? increment, otherwise decrement
    A ^ B ? encoder0Pos++ : encoder0Pos--;
  //  // check for underflow (< 0)
  //  if (bitRead(encoder0Pos, 15) == 1) encoder0Pos = 0;
  //  // check for overflow (> 1023)
  //  if (bitRead(encoder0Pos, 10) == 1) encoder0Pos = 1023;
  //  constrain(encoder0Pos, 0, 1023);
  }
}
