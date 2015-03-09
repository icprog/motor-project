/*******************************************************************************************
TIMER driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "timer.h"
#include "bsp.h"
#include "irq.h"
#include "core_cm3.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define MS_TIMER           AT91C_BASE_TC0
#define MS_TIMER_IRQ       IRQn_TC0
#define MS_TIMER_PERIPH    PERIPH_TC0
#define US_TIMER           AT91C_BASE_TC1
#define US_TIMER_PERIPH    PERIPH_TC1


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
	void     (*pfnCb)(void);
	u32      compareTime;
	volatile TIMER_ID timId;
} volatile TIME_X_MS;

typedef struct
{
   TIME_X_MS user[TIMER_MAX_CALLBACKS];
   vu32 numUsers;
   vu32 timerId;
   vu32 ms;
   bool waitActive;
}TIMER_DRIVER;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static TIMER_DRIVER timer;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void TimerWaitXMsCallback(void);
static bool TimerWaitMsActive(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void TimerInit(void)
{
   memset(&timer, 0, sizeof(timer));
   timer.timerId = 0x1;

   // millisecond wait and callback timer
   // Configure to count MAIN_CLK/2 up to RC compare, interrupt and repeat...
   BspPeriphOn(MS_TIMER_PERIPH);
   MS_TIMER->TC_CCR = AT91C_TC_CLKDIS;
   MS_TIMER->TC_CMR = AT91C_TC_WAVESEL_UP_AUTO; // all other bits = 0 (clk = MAIN_CLK/2)
   MS_TIMER->TC_RC = BspGetCpuClkFreq() / (2 * 1000); // (MAIN_CLK/2) @ 1ms interval
   MS_TIMER->TC_SR;                    // dummy read to clear
   MS_TIMER->TC_IDR = 0xFF;            // disable all interrupts
   MS_TIMER->TC_IER = AT91C_TC_CPCS;   // interrupt on RC compare match
   NVIC_EnableIRQ(MS_TIMER_IRQ);
   NVIC_SetPriority(MS_TIMER_IRQ, IRQ_PRIO_TIMER_MS);
   MS_TIMER->TC_CCR = AT91C_TC_CLKEN | AT91C_TC_SWTRG;

	// Microsecond delay timer:
   // Configure to count a 1us rate
   BspPeriphOn(US_TIMER_PERIPH);
   US_TIMER->TC_CCR = AT91C_TC_CLKDIS;
   US_TIMER->TC_CMR = AT91C_TC_WAVESEL_UP_AUTO; // all other bits = 0 (clk = MAIN_CLK/2)
   US_TIMER->TC_RC = BspGetCpuClkFreq() / 2; // (MAIN_CLK/2) @ 1us interval
   US_TIMER->TC_IDR = 0xFF;            // disable all interrupts
   // keep timer disabled until needed...
}

void TimerKill(void)
{
   MS_TIMER->TC_CCR = AT91C_TC_CLKDIS;
   US_TIMER->TC_CCR = AT91C_TC_CLKDIS;
   BspPeriphOff(MS_TIMER_PERIPH);
   BspPeriphOff(US_TIMER_PERIPH);
}


TIMER_ID TimerTimeXMs(u32 ms, PFV pfn)
{
   return TimerTimeXMsRestart(ms, pfn, TIMER_INVALID);
}

TIMER_ID TimerTimeXMsRestart(u32 ms, PFV pfn, TIMER_ID tid)
{
   for (TIME_X_MS *pTm = &timer.user[TIMER_MAX_CALLBACKS-1]; pTm >= timer.user; pTm--)
   {
      if (pTm->timId == tid)
      {
         PREPARE_CRITICAL();
         ENTER_CRITICAL();
         if (pTm->timId == tid)
         {
            MS_TIMER->TC_IER = AT91C_TC_CPCS;
            if (tid == TIMER_INVALID)
               timer.numUsers++;
            pTm->compareTime = timer.ms + ms;
            pTm->pfnCb = pfn;
            pTm->timId = tid = timer.timerId;
            if (++timer.timerId == 0)
               timer.timerId = 1;
            EXIT_CRITICAL();
            return tid;
         }
         EXIT_CRITICAL();
      }
   }
   return (tid == TIMER_INVALID) ? TIMER_INVALID : TimerTimeXMsRestart(ms, pfn, TIMER_INVALID);
}

void TimerTimeXMsCancel(TIMER_ID tid)
{
   for (TIME_X_MS *pTm = &timer.user[TIMER_MAX_CALLBACKS-1]; pTm >= timer.user; pTm--)
   {
      if (pTm->timId == tid)
      {
         PREPARE_CRITICAL();
         ENTER_CRITICAL();
         if (pTm->timId == tid)
         {
            TimerMsStopIfUnused();
            pTm->pfnCb = NULL;
            pTm->timId = TIMER_INVALID;
         }
         EXIT_CRITICAL();
      }
   }
}


void TimerWaitXMs(u32 ms)
{
   timer.waitActive = true;
   TIMER_ID tid = TimerTimeXMs(ms, TimerWaitXMsCallback);
   if (tid != TIMER_INVALID)
   {
      BspSleepWhile(TimerWaitMsActive);
   }
   else
   {
      while (ms-- > 0)
      {
         TimerWaitXUs(1000);
      }
   }
}

void TimerWaitXUs(u32 us)
{
   US_TIMER->TC_CCR = AT91C_TC_CLKEN | AT91C_TC_SWTRG;
   s32 endTick = (s32)(us << 16);
   do{} while ((endTick - (s32)(US_TIMER->TC_CV << 16)) >= 0);
   US_TIMER->TC_CCR = AT91C_TC_CLKDIS;
}


u32 TimerMsGet(void)
{
   timer.numUsers++;
   MS_TIMER->TC_IER = AT91C_TC_CPCS;
   return timer.ms;
}

bool TimerMsHasElapsed(u32 checkMs)
{
   return (bool)(((s32)timer.ms - (s32)checkMs) >= 0);
}

s32 TimerMsRemaining(u32 checkMs)
{
   return (s32)checkMs - (s32)timer.ms;
}

void TimerWaitMsElapsed(u32 checkMs)
{
   s32 remain = TimerMsRemaining(checkMs);
   if (remain > 0)
   {
      TimerWaitXMs((u32)remain);
   }
}

void TimerMsStopIfUnused(void)
{
   if ((timer.numUsers > 0) && (--timer.numUsers == 0))
   {
      MS_TIMER->TC_IDR = 0xFF;
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void TimerWaitXMsCallback(void)
{
   timer.waitActive = false;
}

static bool TimerWaitMsActive(void)
{
   return timer.waitActive;
}

void TC0_IrqHandler(void)
{
   u32 msTimer = ++timer.ms;
   MS_TIMER->TC_SR;  // dummy read to clear flags

   for (TIME_X_MS *pTm = &timer.user[TIMER_MAX_CALLBACKS-1]; pTm >= timer.user; pTm--)
   {
      if (pTm->pfnCb && (pTm->compareTime == msTimer))
      {
         TimerMsStopIfUnused();
         PFV pfn = pTm->pfnCb;
         pTm->pfnCb = NULL;
         pTm->timId = TIMER_INVALID;
         pfn();   //lint !e746
      }
   }
}

