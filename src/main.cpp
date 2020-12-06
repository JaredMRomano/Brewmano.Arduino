#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //F. Malpartida LCD's driver
#include <menu.h>              //menu macros and objects
#include <menuIO/lcdOut.h>     //malpartidas lcd menu output
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include "config.h"
#include <TaskScheduler.h>
#include <Bounce2.h>

//#include "TimeModule.h"

using namespace Menu;

// Time Module
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define MAX_DEPTH 2

const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

tmElements_t rtcTime;

int motorPercent = 0;
int motorRelayValue = LOW;
tmElements_t motorTimerTime;

Bounce *motorOverrideButton = new Bounce();

void ChangeMotorOutput();

// Scheduler Callbacks
void menuTaskCallback();
void encoderTaskCallback();
void timeTaskCallback();
void motorTaskCallback();
void motorTimerCountdown();
bool MotorTimerOnEnable();
void MotorTimerOnDisable();

// Scheduler Tasks
Scheduler scheduler;
Task menuTask(500, TASK_FOREVER, &menuTaskCallback);
Task encoderTask(1, TASK_FOREVER, &encoderTaskCallback);
Task timeTask(100, TASK_FOREVER, &timeTaskCallback);
Task motorTask(100, TASK_FOREVER, &motorTaskCallback);
Task motorTimerTask(&motorTimerCountdown, &scheduler);

// LCD
LiquidCrystal_I2C lcd(LCD_I2C_Addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address and pinout

// Click Encodeer
ClickEncoder clickEncoder(ENC_PIN_DT, ENC_PIN_CLK, ENC_PIN_BTN, 2);
ClickEncoderStream encStream(clickEncoder, 1);

// Init menu input
MENU_INPUTS(in, &encStream);

void StartTimer()
{
  int timeInSec = numberOfSeconds(makeTime(motorTimerTime));
  motorTimerTask.set(1000, timeInSec, &motorTimerCountdown);
  motorTimerTask.enable();
}

//Timer Menu
PADMENU(motorTimerMenu, "", StartTimer, exitEvent, noStyle,
        FIELD(motorTimerTime.Hour, "", ":", 1, 12, 1, 0, doNothing, noEvent, wrapStyle),
        FIELD(motorTimerTime.Minute, "", ":", 0, 60, 1, 0, doNothing, noEvent, wrapStyle),
        FIELD(motorTimerTime.Second, "", " ", 0, 60, 1, 0, doNothing, noEvent, wrapStyle));

// Motor on off menu object
TOGGLE(motorRelayValue, setMotorRelay, "Motor: ", doNothing, noEvent, noStyle,
       VALUE("On", HIGH, doNothing, noEvent), VALUE("Off", LOW, doNothing, noEvent));

// Main Menu
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle,
     SUBMENU(motorTimerMenu),
     SUBMENU(setMotorRelay),
     FIELD(motorPercent, "Motor", "%", 0, 100, 10, 0, ChangeMotorOutput, anyEvent, noStyle));

MENU_OUTPUTS(out, MAX_DEPTH, LCD_OUT(lcd, {0, 1, 20, 3}), NONE);
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

bool getTime(const char *str, tmElements_t &tm)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
    return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str, tmElements_t &tm)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3)
    return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++)
  {
    if (strcmp(Month, monthName[monthIndex]) == 0)
      break;
  }
  if (monthIndex >= 12)
    return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void encoderTaskCallback()
{
  clickEncoder.service();
}

void timeTaskCallback()
{
  RTC.read(rtcTime);
  time_t time = makeTime(rtcTime);
  // m_year = m_year = tmYearToCalendar(tm.Year);
  // m_month = tm.Month;
  // m_day = tm.Day;
  // m_amPm = isAM(time) ? 0 : 1;

  char buf[12];
  sprintf(buf, "%2d:%02d:%02d %s", hour(time), rtcTime.Minute, rtcTime.Second, isAM(time) ? "AM" : "PM");
  lcd.setCursor(0, 0);
  lcd.print(buf);
}

void ChangeMotorOutput()
{
  analogWrite(Mot_PIN, (motorPercent * 255) / 100);
  Serial.print("Motor Value Changed: ");
  Serial.println(motorPercent);
}

void menuTaskCallback()
{
  // Run menu functions
  nav.poll();
}

void motorTaskCallback()
{
  // Set motor relay
  digitalWrite(Mot_RELAY_PIN, motorRelayValue);
}

void motorTimerCountdown()
{
  Task &t = scheduler.currentTask();
  time_t timeInSec = numberOfSeconds(makeTime(motorTimerTime));
  timeInSec--;
  breakTime(timeInSec, motorTimerTime);

  if (t.isLastIteration())
  {
    motorRelayValue = LOW;
  }
}

void setup()
{
  //Init Motor pins
  pinMode(Mot_PIN, OUTPUT);
  pinMode(Mot_RELAY_PIN, OUTPUT);
  motorOverrideButton->attach(Mot_OVERRIDE_PIN, INPUT_PULLUP); //setup the bounce instance for the current button
  motorOverrideButton->interval(25);

  // Init Encoder pins
  pinMode(ENC_PIN_BTN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  // Init Serial and LCD
  Serial.begin(9600);
  while (!Serial)
    ;
  lcd.begin(20, 4);

  //Init Time Module
  RTC.read(rtcTime);
  tmElements_t compileTime;
  // get the date and time the compiler was run
  if (getDate(__DATE__, compileTime) && getTime(__TIME__, compileTime))
  {
    // Check if the compile time is greater than the RTC time
    if (makeTime(compileTime) > makeTime(rtcTime))
    {
      RTC.write(compileTime);
    }
  }

  // Init scheduler
  scheduler.init();

  // Init Scheduler Tasks
  scheduler.addTask(menuTask);
  scheduler.addTask(encoderTask);
  scheduler.addTask(timeTask);
  scheduler.addTask(motorTask);

  // Enable Scheduler Tasks
  scheduler.enableAll();

  nav.showTitle = false;
}

void loop()
{
  // Execute the scheduler
  scheduler.execute();

  // Check if buttons have been pressed
  motorOverrideButton->update();
  if (motorOverrideButton->fell())
  {
    motorRelayValue = motorRelayValue ? LOW : HIGH;
    if (motorRelayValue == LOW)
    {
      motorTimerTask.disable();
      scheduler.deleteTask(motorTimerTask);
    }
  }
}
