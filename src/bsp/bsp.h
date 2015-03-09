/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __BSP_H__
#define __BSP_H__

#include "includes.h"
#include <intrinsics.h>
#include "AT91SAM3U4.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef CPU_CLK_HZ
#define CPU_CLK_HZ			   OSC_CLK_HZ
#endif

#define PREPARE_CRITICAL()		u32 cpu_sr;
#define PREP_ENT_CRITICAL()	u32 cpu_sr = __get_interrupt_state(); \
										__disable_interrupt();
#define ENTER_CRITICAL()		{cpu_sr = __get_interrupt_state(); \
										__disable_interrupt(); }
#define EXIT_CRITICAL()			{__set_interrupt_state(cpu_sr);}


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   PERIPH_SUPC   = AT91C_ID_SUPC  , // SUPPLY CONTROLLER
   PERIPH_RSTC   = AT91C_ID_RSTC  , // RESET CONTROLLER
   PERIPH_RTC    = AT91C_ID_RTC   , // REAL TIME CLOCK
   PERIPH_RTT    = AT91C_ID_RTT   , // REAL TIME TIMER
   PERIPH_WDG    = AT91C_ID_WDG   , // WATCHDOG TIMER
   PERIPH_PMC    = AT91C_ID_PMC   , // PMC
   PERIPH_EFC0   = AT91C_ID_EFC0  , // EFC0
   PERIPH_EFC1   = AT91C_ID_EFC1  , // EFC1
   PERIPH_DBGU   = AT91C_ID_DBGU  , // DBGU (standalone UART)
   PERIPH_HSMC4  = AT91C_ID_HSMC4 , // HSMC4
   PERIPH_PIOA   = AT91C_ID_PIOA  , // Parallel IO Controller A
   PERIPH_PIOB   = AT91C_ID_PIOB  , // Parallel IO Controller B
   PERIPH_PIOC   = AT91C_ID_PIOC  , // Parallel IO Controller C
   PERIPH_US0    = AT91C_ID_US0   , // USART 0
   PERIPH_US1    = AT91C_ID_US1   , // USART 1
   PERIPH_US2    = AT91C_ID_US2   , // USART 2
   PERIPH_US3    = AT91C_ID_US3   , // USART 3
   PERIPH_MCI0   = AT91C_ID_MCI0  , // Multimedia Card Interface
   PERIPH_TWI0   = AT91C_ID_TWI0  , // TWI 0
   PERIPH_TWI1   = AT91C_ID_TWI1  , // TWI 1
   PERIPH_SPI0   = AT91C_ID_SPI0  , // Serial Peripheral Interface
   PERIPH_SSC0   = AT91C_ID_SSC0  , // Serial Synchronous Controller 0
   PERIPH_TC0    = AT91C_ID_TC0   , // Timer Counter 0
   PERIPH_TC1    = AT91C_ID_TC1   , // Timer Counter 1
   PERIPH_TC2    = AT91C_ID_TC2   , // Timer Counter 2
   PERIPH_PWMC   = AT91C_ID_PWMC  , // Pulse Width Modulation Controller
   PERIPH_ADC12B = AT91C_ID_ADC12B, // 12-bit ADC Controller (ADC12B)
   PERIPH_ADC    = AT91C_ID_ADC   , // 10-bit ADC Controller (ADC)
   PERIPH_HDMA   = AT91C_ID_HDMA  , // HDMA
   PERIPH_UDPHS  = AT91C_ID_UDPHS , // USB Device High Speed
}PERIPH_ID;

typedef bool (*PFN_SLEEP)(void);


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void BspInit(void);
u32 BspGetCpuClkFreq(void);
void BspPeriphOn(PERIPH_ID periph);
void BspPeriphOff(PERIPH_ID periph);

void BspSleepWhile(PFN_SLEEP pfnSleep);

#endif
