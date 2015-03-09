/*******************************************************************************************
Demonstrate LCD, Tact, LED and button functionality


Quinn Miller
********************************************************************************************/


#include "MotorDemo.h"
#include "button.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
char lcdRowB[20] = "STOP  UP   DWN  POKE";
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/

char lcdRowA[19];
char setpointChar[3];

u16 setpoint = 1000;
u8 pokePerMin = 0;

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void updateLcdRow(char *pRowA, char *pRowB);
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void MotorDemoInit()
{
  LcdClear();
  LedRgbSet(0x000000FF);
}

void MotorDemoUpdate()
{
  
  if( ButtonWasPressed(BTN_LM)) {
    ButtonPressAck(BTN_LM);
    setpoint+=50;
  }
  if( ButtonWasPressed(BTN_RM)) {
    ButtonPressAck(BTN_RM);
    if( setpoint>0) setpoint-=50;
  }
  
  sprintf(lcdRowA,"%d",setpoint);  
  
  updateLcdRow(&lcdRowA[0], &lcdRowB[0]);
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
void updateLcdRow(char *pRowA, char *pRowB) {
  LcdClear();
  LcdSetPos(1,0);
  LcdPrintf(pRowB);
  LcdSetPos(0,0);
  LcdPrintf(pRowA);
}

