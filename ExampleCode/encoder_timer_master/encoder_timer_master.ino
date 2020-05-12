//Libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
const byte Start= 4; // start/stop button
byte hours = 0;
byte minutes = 0;
byte seconds = 0;
boolean timeState = false;
// Rotary Encoder Module connections
const int PinSW= 5;   // Rotary Encoder Switch
const int PinDT= 3;    // DATA signal
const int PinCLK= 2;    // CLOCK signal
int HMS = 1;
int encoderPos = 0; // a counter for the dial
unsigned int lastReportedPos = 1; // change management
static boolean rotating = false; // debounce management
boolean A_set = false;
boolean B_set = false;

void setup() {
//enabling pullups
pinMode(Start, INPUT_PULLUP);
pinMode(PinCLK, INPUT_PULLUP); 
pinMode(PinDT, INPUT_PULLUP);
pinMode(PinSW, INPUT_PULLUP);
attachInterrupt(0, doEncoderA, CHANGE); //pin 2
attachInterrupt(1, doEncoderB, CHANGE); //pin 3
Serial.begin(1000000); // output
lcd.begin(20, 4); // initialize the lcd for 16 chars 2 lines, turn on backlight
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Mash Time");
lcd.setCursor(10, 0);
lcd.print("00:00:00");

}

void loop() {

if (digitalRead(PinSW) == LOW)
{
HMS = HMS + 1;
if (HMS == 4)
{
HMS = 1;
}
delay(1000);
}
rotating = true; // reset the debouncer
encoderPos = constrain(encoderPos, -1, 1);
if (lastReportedPos != encoderPos) {

if (HMS == 1) {
hours = hours + encoderPos;
hours = constrain(hours, 0, 48);
}
else if (HMS == 2) {
minutes = minutes + encoderPos;
minutes = constrain(minutes, 0, 60);
}
else if (HMS == 3) {
seconds = seconds + encoderPos;
seconds = constrain(seconds, 0, 60);
}

lcd.setCursor(10, 0);
if (hours <= 9)
{
lcd.print("0");
}
lcd.print(hours);
lcd.print(":");
if (minutes <= 9)
{
lcd.print("0");
}
lcd.print(minutes);
lcd.print(":");
if (seconds <= 9)
{
lcd.print("0");
}
lcd.print(seconds);
encoderPos = 0;
lastReportedPos = encoderPos;

}
if (digitalRead(Start) == LOW) { //start count down timer
timeState = true;
//delay(1000);
while (timeState == true) {
if (minutes == 0 && hours >= 1) {
minutes = 60;
hours = hours - 1;
}
if (seconds == 0 && minutes >= 1) {
seconds = 60;
minutes = minutes - 1;
}
else if (minutes == 0 && hours == 0 && seconds == 0) { //count down alarm
while (timeState == true) {
tone(11, 600, 250);
delay(250);
tone(11, 800, 250);
delay(250);
if (digitalRead(Start) == LOW) { // turn alarm off
timeState = false;
seconds = 1; /////////////////////////////////////////////
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Mash Time");
lcd.setCursor(10, 0);
lcd.print("00:00:00");

break;
}
}
}
delay(992); // delay for keping time master setting!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
seconds = seconds - 1;

lcd.setCursor(10, 0);
if (hours <= 9)
{
lcd.print("0");
}
lcd.print(hours);
lcd.print(":");
if (minutes <= 9)
{
lcd.print("0");
}
lcd.print(minutes);
lcd.print(":");
if (seconds <= 9)
{
lcd.print("0");
}
lcd.print(seconds);

if (digitalRead(Start) == LOW) {
delay(1000);
timeState = false;
break;
}
}
}
}

// Interrupt on A changing state
void doEncoderA() {
// debounce
if ( rotating ) delay (1); // wait a little until the bouncing is done

// Test transition, did things really change?
if ( digitalRead(PinCLK) != A_set ) { // debounce once more
A_set = !A_set;

// adjust counter + if A leads B
if ( A_set && !B_set )
encoderPos = 1;

rotating = false; // no more debouncing until loop() hits again
}
}

// Interrupt on B changing state
void doEncoderB() {
if ( rotating ) delay (1);
if ( digitalRead(PinDT) != B_set ) {
B_set = !B_set;
// adjust counter – 1 if B leads A
if ( B_set && !A_set )
encoderPos = -1;

rotating = false;
}
}
