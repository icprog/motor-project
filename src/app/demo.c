/*******************************************************************************************
Demo Application

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "demo.h"
#include "led.h"
#include "lcd.h"
#include "button.h"
#include "usart.h"
#include "sd.h"
#include "file.h"
#include "gpio.h"
#include "timer.h"
#include "rtc.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   char teBuf[100];
   char usartRxBuf[32];
   char usartTxBuf[32];
   u32 tickCount;
   time_t lastTm;
}DEMO;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
//static DEMO demo;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
//static void DemoTextEditorInit(void);



/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void DemoInit(void)
{
   LedSequence(100);
   LedRgbCycleSet(5000, UINT8_MAX, UINT8_MAX);
// DemoTextEditorInit();
// DemoUsartInit();
// DemoSd();
}

void DemoUpdate(void)
{
   //DemoClockUpdate();
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
