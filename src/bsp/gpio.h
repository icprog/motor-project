/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

#include "includes.h"
#include "gpio.h"
#include "bsp.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define GPIO_PIN_PORT(pin)	         (pin >> 5)
#define GPIO_PIN_BIT(pin)	         (1UL << (pin & 31))
#define GPIO_PIN_TO_PORT_BIT(pin)	GPIO_PIN_PORT(pin), GPIO_PIN_BIT(pin)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
	// Port A:
	GPIO_PIN_PA0 = 0,       // PA0 (UNUSED)
   GPIO_PIN_SD_WP,         // PA1
   GPIO_PIN_SD_DETECT,     // PA2
   GPIO_PIN_HSMCI_MCCK,    // PA3
   GPIO_PIN_HSMCI_MCCDA,   // PA4
   GPIO_PIN_HSMCI_MCDA0,   // PA5
   GPIO_PIN_HSMCI_MCDA1,   // PA6
   GPIO_PIN_HSMCI_MCDA2,   // PA7
   GPIO_PIN_SD_CS_MCDA3,   // PA8
   GPIO_PIN_BLADE_SDA,     // PA9
   GPIO_PIN_BLADE_SCL,     // PA10
   GPIO_PIN_BLADE_RX,      // PA11
   GPIO_PIN_BLADE_TX,      // PA12
   GPIO_PIN_BLADE_MISO,    // PA13
   GPIO_PIN_BLADE_MOSI,    // PA14
   GPIO_PIN_BLADE_SCK,     // PA15
   GPIO_PIN_BLADE_CS,      // PA16
   GPIO_PIN_BUTTON0,       // PA17
   GPIO_PIN_DBG_TX,        // PA18
   GPIO_PIN_DBG_RX,        // PA19
   GPIO_PIN_SD_MOSI,       // PA20
   GPIO_PIN_SD_MISO,       // PA21
   GPIO_PIN_ANT_MISO,      // PA22
   GPIO_PIN_ANT_MOSI,      // PA23
   GPIO_PIN_SD_SCK,        // PA24
   GPIO_PIN_ANT_SCK,       // PA25
   GPIO_PIN_PA26,          // PA26 (UNUSED)
   GPIO_PIN_CLK_OUT,       // PA27
   GPIO_PIN_BUZZER1,       // PA28
   GPIO_PIN_BUZZER2,       // PA29
   GPIO_PIN_ADC_POT,       // PA30
   GPIO_PIN_HEARTBEAT,     // PA31

   // Port B:
   GPIO_PIN_BUTTON1,       // PB0
   GPIO_PIN_BUTTON2,       // PB1
   GPIO_PIN_BUTTON3,       // PB2
   GPIO_PIN_BLADE_ADC0,    // PB3
   GPIO_PIN_BLADE_ADC1,    // PB4
   GPIO_PIN_PB5,           // PB5 (UNUSED)
   GPIO_PIN_PB6,           // PB6 (UNUSED)
   GPIO_PIN_PB7,           // PB7 (UNUSED)
   GPIO_PIN_PB8,           // PB8 (UNUSED)
   GPIO_PIN_LCD_RST,       // PB9
   GPIO_PIN_LCD_BL_RED,    // PB10
   GPIO_PIN_LCD_BL_GREEN,  // PB11
   GPIO_PIN_LCD_BL_BLUE,   // PB12
   GPIO_PIN_LED_WHITE,     // PB13
   GPIO_PIN_LED_PURPLE,    // PB14
   GPIO_PIN_LED_ORANGE,    // PB15
   GPIO_PIN_LED_CYAN,      // PB16
   GPIO_PIN_LED_YELLOW,    // PB17
   GPIO_PIN_LED_BLUE,      // PB18
   GPIO_PIN_LED_GREEN,     // PB19
   GPIO_PIN_LED_RED,       // PB20
   GPIO_PIN_ANT_RST,       // PB21
   GPIO_PIN_ANT_CS,        // PB22
   GPIO_PIN_ANT_MRDY,      // PB23
   GPIO_PIN_ANT_SRDY,      // PB24

	GPIO_PIN_COUNT
}GPIO_PIN;

typedef enum
{
   GPIO_IRQ_OFF,
   GPIO_IRQ_CHANGE,
   GPIO_IRQ_RISING_EDGE,
   GPIO_IRQ_FALLING_EDGE,
   GPIO_IRQ_HIGH_LEVEL,
   GPIO_IRQ_LOW_LEVEL,
   GPIO_IRQ_COUNT
}GPIO_IRQ;
typedef void (*PFN_GPIO_IRQ)(GPIO_PIN);


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void GpioInit(void);
void GpioTest(void);
void GpioIrqInstall(GPIO_PIN pin, GPIO_IRQ irq, PFN_GPIO_IRQ pfnGpioIrq);

void GpioSet(GPIO_PIN pin);
void GpioClear(GPIO_PIN pin);
void GpioSetState(GPIO_PIN pin, bool pinState);
void GpioToggle(GPIO_PIN pin);
bool GpioIsSet(GPIO_PIN pin);

void GpioDirSetOutput(GPIO_PIN pin);
void GpioDirSetInput(GPIO_PIN pin);
void GpioDirSetState(GPIO_PIN pin, bool state);
void GpioDirToggle(GPIO_PIN pin);
bool GpioDirIsOutput(GPIO_PIN pin);

void GpioSetPeriphMode(GPIO_PIN pin);
void GpioSetGpioMode(GPIO_PIN pin);

void PIOA_IrqHandler(void);
void PIOB_IrqHandler(void);

#endif
