/*******************************************************************************************
TIMER driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef TIMER_MAX_CALLBACKS
#define TIMER_MAX_CALLBACKS   8
#endif
#define TIMER_INVALID	((TIMER_ID)0)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef u32	TIMER_ID;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void TimerInit(void);
void TimerKill(void);

// Interrupt-based timer callbacks:
TIMER_ID TimerTimeXMs(u32 ms, PFV pfn);   //lint -esym(534, TimerTimeXMs)
TIMER_ID TimerTimeXMsRestart(u32 ms, PFV pfn, TIMER_ID tid);
void TimerTimeXMsCancel(TIMER_ID tid);

// Inline delays
void TimerWaitXMs(u32 ms);
void TimerWaitXUs(u32 us);

u32 TimerMsGet(void);
bool TimerMsHasElapsed(u32 checkMs);
s32 TimerMsRemaining(u32 checkMs);
void TimerWaitMsElapsed(u32 checkMs);
void TimerMsStopIfUnused(void);

extern void TC0_IrqHandler(void);

#endif
