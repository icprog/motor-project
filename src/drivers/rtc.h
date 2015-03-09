/*******************************************************************************************
Atmel SAM3U RTC driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __RTC_H__
#define __RTC_H__

#include "types.h"
#include <time.h>


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void RtcInit(void);
bool RtcIsValid(void);
time_t RtcGet(void);
void RtcSet(time_t newTime);
struct tm * RtcGetTm(void);
void RtcSetTm(struct tm const *pTm);

#endif
