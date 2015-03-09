/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "bsp.h"
#include "gpio.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
//lint -e506 -e514
#define PMC_MOR_KEY        (0x37 << 16)
#define PMC_MOSCXTST       (0xF0)      // crystal startup time = xx * (8/32768)
#define PMC_FAST_RC        (0x1 << 4)  // 0 = 4 MHz, 1 = 8 MHz, 2 = 12 MHz
#define PLLA_COUNT         (0x3F)      // PLL lock time = xx * (8/32768)
#define PMC_MCKR_UPLLDIV   (BIT13)

// PLL freq from (MUL + 1) / DIV.
// NOTE: PLL_CLK is divided by 2 (PMC_MCKR)
#define PLL_MUL            (7)
#define PLL_DIV            (1)
#define PLL_FREQ           ((OSC_CLK_HZ * (PLL_MUL + 1)) / (PLL_DIV * 2))


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void BspClockInit(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void BspInit(void)
{
   GpioInit();
   BspPeriphOn(PERIPH_PMC);
   BspClockInit();
   AT91C_BASE_PMC->PMC_FSMR &= ~AT91C_PMC_LPM;
   AT91C_BASE_NVIC->NVIC_SCR &= ~AT91C_NVIC_SLEEPDEEP;
}

u32 BspGetCpuClkFreq(void)
{
   // This product always uses the PLL ... no need to dynamically calculate the clk frequency
   return PLL_FREQ;
}

void BspPeriphOn(PERIPH_ID periph)
{
   AT91C_BASE_PMC->PMC_PCER = _BIT(periph);
}

void BspPeriphOff(PERIPH_ID periph)
{
   AT91C_BASE_PMC->PMC_PCDR = _BIT(periph);
}


void BspSleepWhile(PFN_SLEEP pfnSleep)
{
   for (;;)
   {
      __enable_interrupt();
      __disable_interrupt();
      if (pfnSleep())
      {
         __WFI();
      }
      else
      {
         break;
      }
   }
   __enable_interrupt();
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void BspClockInit(void)
{
   // Enable the master clock on the PKC0 clock out pin (PA_27_CLOCK_OUT)
   AT91C_BASE_PMC->PMC_PCKR[0] = AT91C_PMC_CSS_SYS_CLK | AT91C_PMC_PRES_CLK;
   AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_PCK0;

   // Turn on the main oscillator and wait for it to start up before switching to it
   AT91C_BASE_PMC->PMC_MOR = (!AT91C_CKGR_CFDEN | !AT91C_CKGR_MOSCSEL | PMC_MOR_KEY | PMC_MOSCXTST |
                              PMC_FAST_RC | AT91C_CKGR_MOSCRCEN | !AT91C_CKGR_WAITMODE |
                              !AT91C_CKGR_MOSCXTBY | AT91C_CKGR_MOSCXTEN);
   do{} while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCXTS));
   AT91C_BASE_PMC->PMC_MOR |= (AT91C_CKGR_MOSCSEL | PMC_MOR_KEY);

   // Initialize PLLA and wait for lock
   AT91C_BASE_PMC->PMC_PLLAR = (AT91C_CKGR_SRC | (PLL_MUL << 16) | (PLLA_COUNT << 8) | (PLL_DIV)) ;
   do{} while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA));

   // Before switching to new clock, ensure that Flash Wait State is valid for the new freq
   AT91C_BASE_EFC0->EFC_FMR = AT91C_EFC_FWS_2WS;

   // Now switch to the PLL clock (follow sequence from pg. 472 of manual)
   AT91C_BASE_PMC->PMC_MCKR = (PMC_MCKR_UPLLDIV | AT91C_PMC_PRES_CLK_2 | AT91C_PMC_CSS_MAIN_CLK);
   do{} while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
   AT91C_BASE_PMC->PMC_MCKR = (PMC_MCKR_UPLLDIV | AT91C_PMC_PRES_CLK_2 | AT91C_PMC_CSS_PLLA_CLK);
   do{} while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

   //TODO: CKGR_UCKR for USB? Or move to USB driver?
// AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_UPLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
// while ( !(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKU) );
}

