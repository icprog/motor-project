/*******************************************************************************************
Watchdog Timer

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "wdt.h"


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
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void WdtInit(void)
{
#if WDT_INTERVAL_MS == 0
   WdtKill();
#else
   // Configure to reboot at the selected timeout
#define WDT_TICKS               ((((WDT_INTERVAL_MS) * 32768UL) / 128000UL) & AT91C_WDTC_WDV)
   AT91C_BASE_WDTC->WDTC_WDMR = (WDT_TICKS | AT91C_WDTC_WDRSTEN | AT91C_WDTC_WDDBGHLT);
   WdtReset();
#endif
}

void WdtKill(void)
{
   AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/



