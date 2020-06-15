#include <Wire.h>
#include <string>

const int pumpRelayPin;
const int alarmPin;

bool isAlarmOnlyMode;

struct countdownTimer
{
  long startTime;
  volatile long duration;
  bool isRunning;
};

countdownTimer pumpTimer{0, 0, false};
countdownTimer mashTimer{0, 0, false};

void setup()
{
  init_pump();
}

void loop()
{
  // check the pump timer
  pumpTimer.isRunning = check_timer(pumpTimer, pumpTimerElapsed(););
  mashTimer.isRunning = check_timer(mashTimer, mashTimerElapsed());
  outputTimeRemaining();
}

// Checks the status of a countdownTimer, if the time has elapsed, action gets called.  If the timer is still running, prints the remaining seconds to printOutput function
bool check_timer(countdownTimer countdownTimer, void (*action)())
{
  unsigned long currentMillis = millis();
  if ((currentMillis > (countdownTimer.startTime + countdownTimer.duration)) && countdownTimer.isRunning)
  {
    countdownTimer.isRunning = false;
    action();
  }

  if (countdownTimer.isRunning)
  {
    int timeRemaining = ((countdownTimer.startTime + countdownTimer.duration) - currentMillis) / 1000;
    printOutput(std::to_string(timeRemaining););
  }

  return isRunning;
}

// Change this to print to lcd
void printOutput(str value)
{
  Serial.print(value)
}

// Actions when the pump timer elapses
void pumpTimerElapsed()
{
  if (isAlarmOnlyMode)
  {
    digitalWrite(alarmPin, !pumpTimer.isRunning)
  }
  digitalWrite(pumpRelayPin, pumpTimer.isRunning)
}

// Actions when the pump timer elapses
void mashTimerElapsed()
{
  if (isAlarmOnlyMode)
  {
    digitalWrite(alarmPin, !mashTimer.isRunning)
  }
  else
  {
    digitalWrite(pumpRelayPin, pumpTimer.isRunning)
  }
}

// Responds to when the encoder button is pressed
void on_encoderPress()
{
  pumpTimer.startTime = millis();
  pumpTimer.isRunning = true;
  digitalWrite(pumpRelayPin, pumpTimer.isRunning);
}

// Initializes the pump pin
void init_pump()
{
  pinMode(pumpRelayPin, OUTPUT);
}
