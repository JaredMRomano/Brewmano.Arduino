# Brewmano.Arduino

Brewmano.Arduino is a small arduino project designed to control a TopsFlow PWM controlled brew pump, however it can be used to control any 5V PWM controlled pump.  It uses Arduino.Menu for a UI the implements a rotary encoder as the input and a standard 20 x 4 LCD display as the output.  The project has been designed to be less than 30 KB so that it can fit on an Arduino Nano or Uno.

Time is kept using a DS1307RTC.  To reduce numer of menu objects, on setup the time gets set from the compile time, however if the saved RTC value is greater than the compile time, the RTC time is used.  It additionally implements a basic timer for timing a mash.
