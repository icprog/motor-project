/*******************************************************************************************
Description:
Controls the timing of the solenoids based on a given period and voltage

Quinn Miller
********************************************************************************************/


#include "solenoid.h"
#include "tact.h"

/* Testing123 */
#include "led.h"
#include "lcd.h"
/* Testing123 */

/**************************************************************************
 *                                  Constants
 **************************************************************************/
int DEFAULT_VOLTS = 0;
int DEFAULT_PERIOD = 1000;

float BUFFER_PERCENT = 10;
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
int sol1Start;
int sol1Stop;
int sol2Start;
int sol2Stop;
int periodSol;
int voltage;

int tSinceTick;
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/

/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void SolenoidInit() {
     SetSolenoidParam(DEFAULT_VOLTS, DEFAULT_PERIOD);
}

void SolenoidUpdate() {
  
  // update solenoid outputs
  tSinceTick = GetTimeSinceTick();
  
  /* Testing123 */
  LcdSetPos(0,0);
  LcdPrintf("%d", tSinceTick);
  LcdSetPos(1,0);
  LcdPrintf("%d %d %d %d", sol1Start, sol1Stop, sol2Start, sol2Stop);
  /* Testing123 */
  
  if ( sol1Start<tSinceTick && tSinceTick<sol1Stop ) {
    LedOn(LED_GREEN);
    LedOff(LED_ORANGE);
  } else if ( sol2Start<tSinceTick && tSinceTick<sol2Stop ) {
    LedOn(LED_ORANGE);
    LedOff(LED_GREEN);
  } else {
    LedOff(LED_GREEN);
    LedOn(LED_ORANGE);
  }
  //LedUpdate();
}

void SetSolenoidParam(int volts, int periodMs) {
  periodSol = periodMs;
  voltage = volts;
  
    // initialize variables
  sol1Start = (int)(periodSol*BUFFER_PERCENT/100);
  sol1Stop = (int)(periodSol*(0.5-BUFFER_PERCENT/100));
  sol2Start = (int)(periodSol*(0.5+BUFFER_PERCENT/100));
  sol2Stop = (int)(periodSol*(1-BUFFER_PERCENT/100));
}
/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

