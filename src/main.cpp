#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> //F. Malpartida LCD's driver
#include <menu.h>              //menu macros and objects
#include <menuIO/lcdOut.h>     //malpartidas lcd menu output
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <config.h>
#include <TaskScheduler.h>

#include <Time.h>
#include <DS1307RTC.h>

using namespace Menu;

// Scheduler
void menuTaskCallback();
void encoderTaskCallback();
Task menuTask(500, TASK_FOREVER, &menuTaskCallback);
Task encoderTask(1, TASK_FOREVER, &encoderTaskCallback);
Scheduler scheduler;

// LCD
LiquidCrystal_I2C lcd(LCD_I2C_Addr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address and pinout

// Click Encodeer
ClickEncoder clickEncoder(ENC_PIN_DT, ENC_PIN_CLK, ENC_PIN_BTN, 2);
ClickEncoderStream encStream(clickEncoder, 1);

// Init menu input
MENU_INPUTS(in, &encStream);
void encoderTaskCallback() { clickEncoder.service(); }
void timerIsr() { clickEncoder.service(); }

// Time
tmElements_t tm;
const char *monthName[12] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

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

void InitTime(void)
{
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__))
  {
    // and configure the RTC with this info
    RTC.write(tm);   
  }
}



result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e, navNode &nav, prompt &item)
{
  Serial.print("event: ");
  Serial.println(e);
  return proceed;
}

int test = 55;

result action1(eventMask e, navNode &nav, prompt &item)
{
  Serial.print("action1 event: ");
  Serial.print(e);
  Serial.println(", proceed menu");
  Serial.flush();
  return proceed;
}

result action2(eventMask e, navNode &nav, prompt &item)
{
  Serial.print("action2 event: ");
  Serial.print(e);
  Serial.println(", quiting menu.");
  Serial.flush();
  return quit;
}

int ledCtrl = LOW;

result myLedOn()
{
  ledCtrl = HIGH;
  return proceed;
}
result myLedOff()
{
  ledCtrl = LOW;
  return proceed;
}

// uint16_t year1 = 2017;
// uint16_t month1 = 10;
// uint16_t day1 = 7;

// //Time Display Menu
// PADMENU(dayTime, "dayTime", doNothing, noEvent, noStyle,
//         FIELD((&uint16_t) tm.Year, "", "/", 1900, 3000, 20, 1, doNothing, noEvent, noStyle),
//         FIELD(month1, "", "/", 1, 12, 1, 0, doNothing, noEvent, wrapStyle),
//         FIELD(day1, "", "", 1, 31, 1, 0, doNothing, noEvent, wrapStyle));

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       ,
       VALUE("On", HIGH, doNothing, noEvent),
       VALUE("Off", LOW, doNothing, noEvent));

int selTest = 0;

SELECT(selTest, selMenu, "Select", doNothing, noEvent, noStyle,
       VALUE("Zero", 0, doNothing, noEvent),
       VALUE("One", 1, doNothing, noEvent),
       VALUE("Two", 2, doNothing, noEvent));

int chooseTest = -1;

CHOOSE(chooseTest, chooseMenu, "Choose", doNothing, noEvent, noStyle,
       VALUE("First", 1, doNothing, noEvent),
       VALUE("Second", 2, doNothing, noEvent),
       VALUE("Third", 3, doNothing, noEvent),
       VALUE("Last", -1, doNothing, noEvent));

MENU(subMenu, "Sub-Menu", showEvent, anyEvent, noStyle,
     OP("Sub1", showEvent, anyEvent),
     OP("Sub2", showEvent, anyEvent),
     OP("Sub3", showEvent, anyEvent),
     EXIT("<Back"));

/*extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);*/

char *constMEM hexDigit MEMMODE = "0123456789ABCDEF";
char *constMEM hexNr[] MEMMODE = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

// Main Menu
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle,
     OP("OpA", action1, anyEvent),
     OP("OpB", action2, enterEvent)
     //,SUBMENU(togOp)
     ,
     FIELD(test, "Test", "%", 0, 100, 10, 1, doNothing, noEvent, wrapStyle),
     SUBMENU(subMenu),
     SUBMENU(setLed),
     OP("LED On", myLedOn, enterEvent),
     OP("LED Off", myLedOff, enterEvent),
     SUBMENU(selMenu),
     SUBMENU(chooseMenu),
     OP("Alert test", doAlert, enterEvent),
     EDIT("Hex", buf1, hexNr, doNothing, noEvent, noStyle),
     EXIT("<Back"));

#define MAX_DEPTH 2

/*const panel panels[] MEMMODE={{0,0,16,2}};
navNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,1);
idx_t tops[MAX_DEPTH];
lcdOut outLCD(&lcd,tops,pList);//output device for LCD
menuOut* constMEM outputs[] MEMMODE={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs
*/

MENU_OUTPUTS(out, MAX_DEPTH, LCD_OUT(lcd, {0, 0, 20, 4}), NONE);
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out); //the navigation root object

result alert(menuOut &o, idleEvent e)
{
  if (e == idling)
  {
    o.setCursor(0, 0);
    o.print("alert test");
    o.setCursor(0, 1);
    o.print("[select] to continue...");
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item)
{
  nav.idleOn(alert);
  return proceed;
}

result idle(menuOut &o, idleEvent e)
{
  switch (e)
  {
  case idleStart:
    o.print("suspending menu!");
    break;
  case idling:
    o.print("suspended...");
    break;
  case idleEnd:
    o.print("resuming menu.");
    break;
  }
  return proceed;
}

void menuTaskCallback()
{
  nav.poll();
}

void setup()
{
  pinMode(ENC_PIN_BTN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Arduino Menu Library");
  Serial.flush();
  lcd.begin(20, 4);

  // Init scheduler
  scheduler.init();
  scheduler.addTask(menuTask);
  scheduler.addTask(encoderTask);
  menuTask.enable();
  encoderTask.enable();

  nav.idleTask = idle; //point a function to be used when menu is suspended
  mainMenu[1].enabled = disabledStatus;
  nav.showTitle = false;
  lcd.setCursor(0, 0);
  lcd.print("Menu 4.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("r-site.net");
  // Timer1.initialize(1000);
  //Timer1.attachInterrupt(timerIsr);
  delay(2000);
}

void loop()
{
  scheduler.execute();
}