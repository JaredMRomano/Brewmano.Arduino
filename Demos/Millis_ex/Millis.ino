#include <Wire.h>
//#include <string>


// Digital pin for pump relay
#define pumpRelayPin            1

// Digital pin for alarm
#define alarmPin                2

// Enable this to output to serial
#define LOG_OUTPUT_TYPE_SERIAL  1

// Enable this to output to LCD
#define LOG_OUTPUT_TYPE_LCD     0


bool isAlarmOnlyMode;

struct countdownTimer
{
  long startTime;
  volatile long duration;
  bool isRunning;
};

static countdownTimer pumpTimer{0, 0, false};
static countdownTimer mashTimer{0, 0, false};

void setup()
{
  init_debugger();
  init_pump();
  on_encoderPress();
}

void loop()
{
  // check the pump timer
  check_timer(pumpTimer, pump_timer_elapsed_handler);
  check_timer(mashTimer, mash_timer_elapsed_handler);
  
}

// Initialize the debugger
void init_debugger(void)
{
#if LOG_OUTPUT_TYPE_SERIAL
    Serial.begin(9600);
    while(!Serial);
    printOutput("Serial monitor enabled");
#endif
#if LOG_OUTPUT_TYPE_LCD
#endif  
}

// Initializes the pump pin
void init_pump()
{
  pinMode(pumpRelayPin, OUTPUT);
}

// Checks the status of a countdownTimer, if the time has elapsed, action gets called.  If the timer is still running, prints the remaining seconds to printOutput function
void check_timer(countdownTimer countdownTimer, void (*action)(void))
{
  bool isRunning = false;
  unsigned long currentMillis = millis();
 
  if ((currentMillis > (countdownTimer.startTime + countdownTimer.duration)) && countdownTimer.isRunning)
  {   
    countdownTimer.isRunning = false;
    printOutput(countdownTimer.isRunning ? "running" : "stopped");
    action();
  }

  if (countdownTimer.isRunning)
  {
    int timeRemaining = ((countdownTimer.startTime + countdownTimer.duration) - currentMillis) / 1000;
    printOutput(String(timeRemaining));
  }
}

// Change this to print to lcd
void printOutput(String value)
{
#if LOG_OUTPUT_TYPE_SERIAL
    Serial.println(value);
#endif
#if LOG_OUTPUT_TYPE_LCD
#endif
  
}

// Actions when the pump timer elapses
static void pump_timer_elapsed_handler()
{
  if (isAlarmOnlyMode)
  {
    digitalWrite(alarmPin, !pumpTimer.isRunning);
  }
  digitalWrite(pumpRelayPin, pumpTimer.isRunning);
}

// Actions when the pump timer elapses
static void mash_timer_elapsed_handler()
{
  if (isAlarmOnlyMode)
  {
    digitalWrite(alarmPin, !mashTimer.isRunning);
  }
  else
  {
    digitalWrite(pumpRelayPin, pumpTimer.isRunning);
  }
}

// Responds to when the encoder button is pressed
void on_encoderPress()
{
  pumpTimer.startTime = millis();
  pumpTimer.isRunning = true;
  pumpTimer.duration = 15000;
  digitalWrite(pumpRelayPin, pumpTimer.isRunning);
}
