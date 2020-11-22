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

//#include "TimeModule.h"

using namespace Menu;

// Time Module
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

tmElements_t tm;
//TimeModule tm;
int motorPercent = 0;

// Scheduler
void menuTaskCallback();
void encoderTaskCallback();
void timeTaskCallback();
Task menuTask(500, TASK_FOREVER, &menuTaskCallback);
Task encoderTask(1, TASK_FOREVER, &encoderTaskCallback);
Task TimeTask(100, TASK_FOREVER, &timeTaskCallback);
Scheduler scheduler;

// LCD
LiquidCrystal_I2C lcd(LCD_I2C_Addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address and pinout

// Click Encodeer
ClickEncoder clickEncoder(ENC_PIN_DT, ENC_PIN_CLK, ENC_PIN_BTN, 2);
ClickEncoderStream encStream(clickEncoder, 1);

// Init menu input
MENU_INPUTS(in, &encStream);
void encoderTaskCallback() { clickEncoder.service(); }
result doAlert(eventMask e, prompt &item);

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
    return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
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

int m_year;
int m_month;
int m_day;
int m_hour;
int m_minute;
int m_second;
int m_amPm;

void timeTaskCallback()
{
  RTC.read(tm);
  time_t time = makeTime(tm);
  m_year = tmYearToCalendar(tm.Year);
  m_month = tm.Month;
  m_day = tm.Day;
  m_hour = hour(time);
  m_minute = tm.Minute;
  m_second = tm.Second;

  m_amPm = isAM(time) ? 0 : 1;
}

//Time Display Menus
SELECT(m_amPm, selAmPmMenu, "", doNothing, noEvent, noStyle,
       VALUE("AM", 0, doNothing, noEvent),
       VALUE("PM", 1, doNothing, noEvent));

PADMENU(timeBanner, "", doNothing, noEvent, noStyle,
        FIELD(m_hour, "", ":", 1, 12, 1, 0, doNothing, noEvent, wrapStyle),
        FIELD(m_minute, "", ":", 0, 60, 1, 0, doNothing, noEvent, wrapStyle),
        FIELD(m_second, "", " ", 0, 60, 1, 0, doNothing, noEvent, wrapStyle),
        SUBMENU(selAmPmMenu));
// End Time Display Menus

void ChangeMotorOutput()
{
  analogWrite(Mot_PIN, (motorPercent * 255) / 100);
  Serial.print("Motor Value Changed: ");
  Serial.println(motorPercent);
}

int motorRelay = LOW;

TOGGLE(motorRelay, setMotorRelay, "Motor: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       ,
       VALUE("On", HIGH, doNothing, noEvent), VALUE("Off", LOW, doNothing, noEvent));

// Main Menu
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle,
     SUBMENU(timeBanner),
     SUBMENU(setMotorRelay),
     FIELD(motorPercent, "Motor", "%", 0, 100, 10, 1, ChangeMotorOutput, exitEvent, noStyle));

#define MAX_DEPTH 2

MENU_OUTPUTS(out, MAX_DEPTH, LCD_OUT(lcd, {0, 0, 20, 4}), NONE);
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out); //the navigation root object

void menuTaskCallback()
{
  nav.poll();
  digitalWrite(Mot_RELAY_PIN, motorRelay);
}

void setup()
{
  pinMode(ENC_PIN_BTN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial)
    ;
  lcd.begin(20, 4);

  //Init Time Module
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__))
  {
    // and configure the RTC with this info
    RTC.write(tm);
  }

  // Init scheduler
  scheduler.init();
  scheduler.addTask(menuTask);
  scheduler.addTask(encoderTask);
  scheduler.addTask(TimeTask);
  menuTask.enable();
  encoderTask.enable();
  TimeTask.enable();

  //Motor
  pinMode(Mot_PIN, OUTPUT);
  pinMode(Mot_RELAY_PIN, OUTPUT);

  mainMenu[0].enabled = disabledStatus;
  nav.showTitle = false;
}

void loop()
{
  scheduler.execute();
}
