/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "includes.h"
#include "bsp.h"
#include "gpio.h"
#include "sysTick.h"
#include "timer.h"
#include "wdt.h"
#include "beeper.h"
#include "lcd.h"
#include "version.h"
#include "led.h"
#include "button.h"
#include "rtc.h"
#include "demo.h"
#include "motorControl.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void MainInit(void);

/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void main(void)
{
   MainInit();
   MotorControlInit();

   for (;;)
   {
      // Drivers
      LedUpdate();
      ButtonUpdate();
      LcdUpdate();

      // Application
      MotorControlUpdate();

      // Heartbeat
      GpioSet(GPIO_PIN_HEARTBEAT);   // active low
      TimerWaitXMs(SYS_TICK_RATE_MS);
      GpioClear(GPIO_PIN_HEARTBEAT);
      
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void MainInit(void)
{
   BspInit();
   SysTickInitMs(SYS_TICK_RATE_MS);
   TimerInit();
   RtcInit();
   BeeperInit();
   ButtonInit();
   LedInit();
   LcdInit();
   WdtInit();
}

