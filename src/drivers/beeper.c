/*******************************************************************************************
Beeper Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "beeper.h"
#include "bsp.h"
#include "gpio.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
typedef struct
{
   AT91PS_PWMC_CH pChan;
   u8             chanBit;
   GPIO_PIN       pin;
}BEEPER_CHAN;
static const BEEPER_CHAN Beepers[BEEPER_COUNT] =
{
   // Beepers are on PA28 (PWMH0) and PA29 (PWMH1)
   [BEEPER_RIGHT] = { .pChan = &AT91C_BASE_PWMC->PWMC_CH[0], .chanBit = BIT0, .pin = GPIO_PIN_BUZZER1 },
   [BEEPER_LEFT] =  { .pChan = &AT91C_BASE_PWMC->PWMC_CH[1], .chanBit = BIT1, .pin = GPIO_PIN_BUZZER2 }, //lint !e485
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   u32 pwmClk;
}BEEPER;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static BEEPER beeper;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void BeeperChannelSetup(AT91S_PWMC_CH *pChan);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void BeeperInit(void)
{
   BspPeriphOn(PERIPH_PWMC);
   AT91C_BASE_PWMC->PWMC_DIS = (Beepers[BEEPER_RIGHT].chanBit | Beepers[BEEPER_LEFT].chanBit);

   BeeperChannelSetup(Beepers[BEEPER_RIGHT].pChan);
   BeeperChannelSetup(Beepers[BEEPER_LEFT].pChan);
   BspPeriphOff(PERIPH_PWMC);
}

void BeeperOn(u16f freqHz, BEEPER_NUM beeperNum)
{
   u32 periodTicks = beeper.pwmClk / freqHz;
   AT91S_PWMC_CH *pChan = Beepers[beeperNum].pChan;
   if (AT91C_BASE_PWMC->PWMC_SR & Beepers[beeperNum].chanBit)
   {
      // Channel is currently running
      pChan->PWMC_CPRDUPDR = periodTicks;
      pChan->PWMC_CDTYUPDR = periodTicks/2;
   }
   else
   {
      // Channel is currently disabled
      BspPeriphOn(PERIPH_PWMC);
      pChan->PWMC_CPRDR = periodTicks;
      pChan->PWMC_CDTYR = periodTicks/2;
      GpioSetPeriphMode(Beepers[beeperNum].pin);
      AT91C_BASE_PWMC->PWMC_ENA = Beepers[beeperNum].chanBit;
   }
}

void Beeper1On(u16f freqHz)
{
   BeeperOn(freqHz, BEEPER_RIGHT);
}

void Beeper2On(u16f freqHz)
{
   BeeperOn(freqHz, BEEPER_LEFT);
}

void BeeperOff(BEEPER_NUM beeperNum)
{
   GpioSetGpioMode(Beepers[beeperNum].pin);
   AT91C_BASE_PWMC->PWMC_DIS = Beepers[beeperNum].chanBit;
   if (!(AT91C_BASE_PWMC->PWMC_SR & 0x0FUL))
   {
      BspPeriphOff(PERIPH_PWMC);
   }
}

void Beeper1Off(void)
{
   BeeperOff(BEEPER_RIGHT);
}

void Beeper2Off(void)
{
   BeeperOff(BEEPER_LEFT);
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void BeeperChannelSetup(AT91S_PWMC_CH *pChan)
{
   beeper.pwmClk = BspGetCpuClkFreq();
   pChan->PWMC_CMR = AT91C_PWMC_CPRE_MCK;   // all other options left off
   pChan->PWMC_CPRDR = 0;
   pChan->PWMC_CPRDUPDR = 0;
   pChan->PWMC_CDTYR = 0;
   pChan->PWMC_CDTYUPDR = 0;
}

