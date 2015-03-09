/*******************************************************************************************
System Tick

(c) Copyright 2012, Gas Clip Technologies Inc. All rights reserved.
This source code contains confidential, trade secret material. Any attempt or participation
in deciphering, decoding, reverse engineering or in any way altering the source code is
strictly prohibited, unless the prior written consent of Gas Clip Technologies Inc is obtained.
********************************************************************************************/

#include "systick.h"
#include "bsp.h"
#include "irq.h"
#include "core_cm3.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef MS_TO_CPU_TICKS
#define MS_TO_CPU_TICKS(ms)      ((BspGetCpuClkFreq()/8000) * (ms))
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static vu32 sysTick;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void SysTickInitMs(u32 sysTickMs)
{
   SysTickInit(MS_TO_CPU_TICKS(sysTickMs));
}

void SysTickInit(u32 sysTicks)
{
   SysTickStop();
   NVIC_SetPriority(SysTick_IRQn, IRQ_PRIO_SYS_TICK);
   sysTick = 0;

   // NOTE: SysTick is a 24-bit number.
   SysTick->LOAD  = sysTicks - 1;
   SysTick->VAL  = 0;
   SysTick->CTRL = (0<<SYSTICK_CLKSOURCE) | (1<<SYSTICK_ENABLE) | (1<<SYSTICK_TICKINT);
}

void SysTickStop(void)
{
   SysTick->CTRL = 0;
}

#ifndef _lint
void SysTick_Handler(void)
{
   sysTick++;
}
#endif


u32 SysTickGet(void)
{
   return sysTick;
}

bool SysTickHasElapsed(u32 checkTick)
{
	return (bool)(((s32)sysTick - (s32)checkTick) >= 0);
}

s32 SysTickRemaining(u32 checkTick)
{
	return (s32)checkTick - (s32)sysTick;
}

void SysTickWait(void)
{
   SysTickWaitTick(sysTick + 1);
}

void SysTickWaitTick(u32 checkTick)
{
   while (!SysTickHasElapsed(checkTick))
   {
      __disable_interrupt();
      if (!SysTickHasElapsed(checkTick))
      {
         __WFI();
      }
      __enable_interrupt();
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

