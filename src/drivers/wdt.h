/*******************************************************************************************
Watchdog Timer

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __WDT_H__
#define __WDT_H__

#include "includes.h"
#include "bsp.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef WDT_INTERVAL_MS
// Valid range is between 3.906ms and 16 seconds (32.768 kHz clock / 128 in a 12-bit counter)
#define WDT_INTERVAL_MS       (0)
#endif

/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void WdtInit(void);
void WdtKill(void);
#define WdtReset()      {AT91C_BASE_WDTC->WDTC_WDCR = 0xA5000001UL;}

#endif
