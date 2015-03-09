/*******************************************************************************************
Description:
Determines revolution period (ms) and frequency (rev/sec and rev/min)

Quinn Miller
********************************************************************************************/


#include "tact.h"
#include "gpio.h"
#include "timer.h"


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
  // GpioIrqInstall(GPIO_PIN_TACT, GPIO_IRQ_FALLING_EDGE, tactUpdate)
}

int GetPeriod() {
  return period;
}

float GetRPS() {
  return freqRPS;
}

float GetRPM() {
  return freqRPM;
}

int GetTimeSinceTick() {
  currTime = TimerMsGet();
  sinceTickTime = currTime - lastTickTime;
  
  return sinceTickTime;
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

void tactUpdate() {
  period = GetTimeSinceTick();
  freqRPS = 1/period;
  freqRPM = 60/period;
  
  lastTickTime = TimerMsGet();
}


