/*******************************************************************************************
Description:
Determines revolution period (ms) and frequency (rev/sec and rev/min)

Quinn Miller
********************************************************************************************/


#include "tact.h"
#include "gpio.h"
#include "timer.h"

/* Testing */
#include "led.h"
/* Testing */


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
int period;
int lastTickTime;
int sinceTickTime;
int currTime;

float freqRPS;
float freqRPM;
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void tactUpdate(GPIO_PIN pin);
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void TactInit() {
  
  // initailize variables
  period = 0;
  lastTickTime = 0;
  sinceTickTime = 0;
  currTime = 0;
  
  freqRPS = 0;
  freqRPM = 0;
  
  // initialize interrupt
  GpioIrqInstall(GPIO_PIN_BUTTON0, GPIO_IRQ_FALLING_EDGE, tactUpdate);
}

int GetPeriod() {
  return period;
}

float GetRPS() {
  int freq = (int)freqRPS;
  return freq;
}

float GetRPM() {
  int freq = (int)freqRPM;
  return freq;
}

int GetTimeSinceTick() {
  currTime = TimerMsGet();
  sinceTickTime = currTime - lastTickTime;
  
  return sinceTickTime;
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

void tactUpdate(GPIO_PIN pin) {
  period = GetTimeSinceTick();
  freqRPS = 1000/period;
  freqRPM = 60000/period;
  
  lastTickTime = TimerMsGet();
  LedToggle(LED_RED);
}


