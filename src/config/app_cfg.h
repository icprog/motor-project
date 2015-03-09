/*******************************************************************************************
Configures the Application build

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __APP_CFG_H__
#define __APP_CFG_H__

#include "types.h"


/**************************************************************************
 *                                  Major compile options:
 **************************************************************************/
#define OSC_CLK_HZ         ((u32)(12e6))     // 12 MHz crystal
#define MAIN_CLK_HZ        ((u32)(48e6))     // internal PLL: 48 MHz
#define SYS_TICK_RATE_MS   (1)

#ifndef UNIT_TESTING
#define UNIT_TESTING 	   (0)
#endif
#if UNIT_TESTING
#define UUT_STATIC
#else
#define UUT_STATIC	static
#endif


/**************************************************************************
 *                                  Driver Constants
 **************************************************************************/
#define MS_TO_TICKS(ms)       ((ms) / SYS_TICK_RATE_MS)
#define MS_FROM_TICKS(ticks)  ((ticks) * SYS_TICK_RATE_MS)

#define LED_UPDATE_RATE_MS	   (SYS_TICK_RATE_MS)
#define LED_PWM_FREQ_HZ       (50)

// Interrupt priorities (0-15, 0 = highest)
#define IRQ_PRIO_DEBUG        (0)
#define IRQ_PRIO_FAULT        (1)
#define IRQ_PRIO_SYS_TICK     (2)
#define IRQ_PRIO_GPIO         (5)
#define IRQ_PRIO_TWI          (6)
#define IRQ_PRIO_USART        (7)
#define IRQ_PRIO_TIMER_MS     (15)


/**************************************************************************
 *                                  Application Constants
 **************************************************************************/
// File System:
#define FILE_SYSTEMS		      FS_NAME(FileFat)
#define F_MAX_NAME				(255)
#define F_MAX_FILES_PER_DISK	(3)
#define F_MAX_FINDS_PER_DISK	(2)


/**************************************************************************
 *                                  Printf Defines:
 **************************************************************************/
#ifndef _DEBUG_PRINTF_ENABLE
#define _DEBUG_PRINTF_ENABLE	(1)
#endif

#if _DEBUG_PRINTF_ENABLE
#define DPRINTF(...)			printf( __VA_ARGS__)
#define DPUTS(str)			puts(str)
#else
#define DPRINTF(...)
#define DPRINTS(str)
#endif


#endif
