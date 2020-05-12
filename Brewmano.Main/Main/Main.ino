//Libraries & global include
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Rotary Encoder Module connections
const int encoder0PinA = 2; // CLOCK signal
const int encoder0PinB = 3; // DATA signal
const int PinSW = 5;        // Rotary Encoder Switch
volatile unsigned int encoder0Pos = 0;
unsigned int oldEncoder0Position = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0;

// Variables to debounce Rotary Encoder
long TimeOfLastDebounce = 0;
int DelayofDebounce = 0.01;
bool PrintDisplayCounter = true;

// Variables for timer
long TimerStartTime = 0;
bool IsTimerRunning = false;

void setup()
{
  initializeRotaryEncoder();
}

void loop()
{
  if (hasEncoderMoved)
  {
    // Do encoder moved stuff stuff
    //Ex:
    String test = String(encoder0Pos, DEC);
    print_1Line(test);
  }
  // Check if Rotary Encoder switch was pressed
  if (digitalRead(PinSW) == LOW)
  {
    // Do button pressed stuff
    printDisplayCounter(encoder0Pos);
  }
}

// Returns true if the encoder has moved from last position
bool hasEncoderMoved()
{
  if (oldEncoder0Position != encoder0Pos)
  {
    oldEncoder0Position = encoder0Pos;
    return true;
  }
  return false;
}

void printDisplayCounter(int encoder0Pos)
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
  attachInterrupt(0, doEncoderA, CHANGE);
  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
}

// Interrupt on A changing state
void doEncoderA()
{
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce)
  {
    Bnew ^ Aold ? encoder0Pos++ : encoder0Pos--;
    Aold = digitalRead(encoder0PinA);
    TimeOfLastDebounce = millis(); // Set variable to current millis() timer
  }
}
// Interrupt on B changing state
void doEncoderB()
{
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce)
  {
    Bnew = digitalRead(encoder0PinB);
    Bnew ^ Aold ? encoder0Pos++ : encoder0Pos--;
    TimeOfLastDebounce = millis(); // Set variable to current millis() timer
  }
}
