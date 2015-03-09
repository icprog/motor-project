/*******************************************************************************************
Interrupts

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __IRQ_H__
#define __IRQ_H__

#include "types.h"
#include "AT91SAM3U4.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define WEAK __weak


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
   NonMaskableInt_IRQn   = -14,    /*!< 2 Non Maskable Interrupt                             */
   MemoryManagement_IRQn = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
   BusFault_IRQn         = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
   UsageFault_IRQn       = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
   SVCall_IRQn           = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
   DebugMonitor_IRQn     = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
   PendSV_IRQn           = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
   SysTick_IRQn          = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  AT91SAM3U4 specific Interrupt Numbers *********************************************************/
   IRQn_SUPC             = AT91C_ID_SUPC  , // SUPPLY CONTROLLER
   IRQn_RSTC             = AT91C_ID_RSTC  , // RESET CONTROLLER
   IRQn_RTC              = AT91C_ID_RTC   , // REAL TIME CLOCK
   IRQn_RTT              = AT91C_ID_RTT   , // REAL TIME TIMER
   IRQn_WDG              = AT91C_ID_WDG   , // WATCHDOG TIMER
   IRQn_PMC              = AT91C_ID_PMC   , // PMC
   IRQn_EFC0             = AT91C_ID_EFC0  , // EFC0
   IRQn_EFC1             = AT91C_ID_EFC1  , // EFC1
   IRQn_DBGU             = AT91C_ID_DBGU  , // DBGU (UART)
   IRQn_HSMC4            = AT91C_ID_HSMC4 , // HSMC4
   IRQn_PIOA             = AT91C_ID_PIOA  , // Parallel IO Controller A
   IRQn_PIOB             = AT91C_ID_PIOB  , // Parallel IO Controller B
   IRQn_PIOC             = AT91C_ID_PIOC  , // Parallel IO Controller C
   IRQn_US0              = AT91C_ID_US0   , // USART 0
   IRQn_US1              = AT91C_ID_US1   , // USART 1
   IRQn_US2              = AT91C_ID_US2   , // USART 2
   IRQn_US3              = AT91C_ID_US3   , // USART 3
   IRQn_MCI0             = AT91C_ID_MCI0  , // Multimedia Card Interface
   IRQn_TWI0             = AT91C_ID_TWI0  , // TWI 0
   IRQn_TWI1             = AT91C_ID_TWI1  , // TWI 1
   IRQn_SPI0             = AT91C_ID_SPI0  , // Serial Peripheral Interface
   IRQn_SSC0             = AT91C_ID_SSC0  , // Serial Synchronous Controller 0
   IRQn_TC0              = AT91C_ID_TC0   , // Timer Counter 0
   IRQn_TC1              = AT91C_ID_TC1   , // Timer Counter 1
   IRQn_TC2              = AT91C_ID_TC2   , // Timer Counter 2
   IRQn_PWMC             = AT91C_ID_PWMC  , // Pulse Width Modulation Controller
   IRQn_ADCC0            = AT91C_ID_ADC12B, // ADC controller0
   IRQn_ADCC1            = AT91C_ID_ADC   , // ADC controller1
   IRQn_HDMA             = AT91C_ID_HDMA  , // HDMA
   IRQn_UDPHS            = AT91C_ID_UDPHS , // USB Device High Speed
   IRQn_COUNT
} IRQn_Type;
#include <core_cm3.h>


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/



// "WEAK" default IRQ handlers can be overridden by redefining somewhere else
//lint -esym(14, *_IrqHandler)
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void SUPC_IrqHandler(void);
WEAK void RSTC_IrqHandler(void);
WEAK void RTC_IrqHandler(void);
WEAK void RTT_IrqHandler(void);
WEAK void WDT_IrqHandler(void);
WEAK void PMC_IrqHandler(void);
WEAK void EFC0_IrqHandler(void);
WEAK void EFC1_IrqHandler(void);
WEAK void DBGU_IrqHandler(void);
WEAK void HSMC4_IrqHandler(void);
WEAK void PIOA_IrqHandler(void);
WEAK void PIOB_IrqHandler(void);
WEAK void PIOC_IrqHandler(void);
WEAK void USART0_IrqHandler(void);
WEAK void USART1_IrqHandler(void);
WEAK void USART2_IrqHandler(void);
WEAK void USART3_IrqHandler(void);
WEAK void MCI0_IrqHandler(void);
WEAK void TWI0_IrqHandler(void);
WEAK void TWI1_IrqHandler(void);
WEAK void SPI0_IrqHandler(void);
WEAK void SSC0_IrqHandler(void);
WEAK void TC0_IrqHandler(void);
WEAK void TC1_IrqHandler(void);
WEAK void TC2_IrqHandler(void);
WEAK void PWM_IrqHandler(void);
WEAK void ADCC0_IrqHandler(void);
WEAK void ADCC1_IrqHandler(void);
WEAK void HDMA_IrqHandler(void);
WEAK void UDPD_IrqHandler(void);
WEAK void IrqHandlerNotUsed(void);


#endif
