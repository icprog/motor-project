/*******************************************************************************************
System Tick

(c) Copyright 2012, Gas Clip Technologies Inc. All rights reserved.
This source code contains confidential, trade secret material. Any attempt or participation
in deciphering, decoding, reverse engineering or in any way altering the source code is
strictly prohibited, unless the prior written consent of Gas Clip Technologies Inc is obtained.
********************************************************************************************/

#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifdef MAIN_CLK_HZ
#define MS_TO_CPU_TICKS(ms)      ((MAIN_CLK_HZ/8000) * (ms))

#ifdef SYS_TICK_RATE_MS
#define MS_TO_SYS_TICKS(ms)      ((u32)(ms) / SYS_TICK_RATE_MS)
#define SYS_TICK_TO_MS(ticks)    ((u32)(ticks) * SYS_TICK_RATE_MS)
#endif
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void SysTickInitMs(u32 sysTickMs);
void SysTickInit(u32 sysTicks);
void SysTickStop(void);

u32 SysTickGet(void);
bool SysTickHasElapsed(u32 checkTick);
s32 SysTickRemaining(u32 checkTick);
void SysTickWait(void);
void SysTickWaitTick(u32 checkTick);

#endif
