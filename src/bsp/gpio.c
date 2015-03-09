/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "gpio.h"
#include "bsp.h"
#include "irq.h"
#include "core_cm3.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
//lint -e778 -e747 -e123
#define GPIOA  AT91C_BASE_PIOA
#define GPIOB  AT91C_BASE_PIOB
static AT91S_PIO * const pGpioReg[] = { GPIOA, GPIOB };

#define GPIO_PIN_BIT_SET(pin, val)  ((u32)val << (pin & 31))
#define GPIO_PIN_IS_VALID(pin)		((UINT8L)pin < (UINT8L)GPIO_PIN_COUNT)
#define GPIO_PIN_DEBOUNCE_MS        (10)
#define GPIO_PIN_DEBOUNCE_DIV       (((GPIO_PIN_DEBOUNCE_MS * 32768UL) / 2000) + 1)


/* Define all parameters for each pin in a table using X-Macros
   pin        = pin name from the enum in gpio.h
   I/O        = whether the pin is controlled by the GPIO peripheral
   output     = pin is driven by processor (1) or is an input (0)
   level      = default output state
   AB         = peripheral A (0) or peripheral B (1)
   pull-up    = internal pull-up resistor enabled
   open-drain = pin operates in open-drain mode
   debounce   = input pin debounce filter is active (else glitch filter)
*/
#define AS_PULLUP(    pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, pUp) |
#define AS_OPEN_DRAIN(pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, odrain) |
#define AS_LEVEL(     pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, level) |
#define AS_PERIPH_AB( pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, ab) |
#define AS_GPIO(      pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, io) |
#define AS_OUTPUT(    pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, output) |
#define AS_WRITABLE(  pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, (output & io)) |
#define AS_DEBOUNCE(  pin,io,output,level,ab,pUp,odrain,dbnc)  GPIO_PIN_BIT_SET(pin, dbnc) |

#define GPIO_PIN_TABLE_A(_)\
/* _(pin                  ,I/O,output,level, AB,pull-up,open-drain,debounce)*/ \
   _(GPIO_PIN_PA0         , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_WP       , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_DETECT   , 1 ,  0   ,  0  , 0 ,   0   ,     0    ,    1) \
   _(GPIO_PIN_HSMCI_MCCK  , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_HSMCI_MCCDA , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_HSMCI_MCDA0 , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_HSMCI_MCDA1 , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_HSMCI_MCDA2 , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_CS_MCDA3 , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_SDA   , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_SCL   , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_RX    , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_TX    , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_MISO  , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_MOSI  , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_SCK   , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_CS    , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BUTTON0     , 1 ,  0   ,  0  , 0 ,   0   ,     0    ,    1) \
   _(GPIO_PIN_DBG_TX      , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_DBG_RX      , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_MOSI     , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_MISO     , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_MISO    , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_MOSI    , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_SD_SCK      , 0 ,  1   ,  0  , 1 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_SCK     , 0 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_PA26        , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_CLK_OUT     , 0 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BUZZER1     , 0 ,  1   ,  0  , 1 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BUZZER2     , 0 ,  1   ,  0  , 1 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ADC_POT     , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_HEARTBEAT   , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \

#define GPIO_PIN_TABLE_B(_)\
/* _(pin                  ,I/O,output,level, AB,pull-up,open-drain,debounce)*/ \
   _(GPIO_PIN_BUTTON1     , 1 ,  0   ,  0  , 0 ,   0   ,     0    ,    1) \
   _(GPIO_PIN_BUTTON2     , 1 ,  0   ,  0  , 0 ,   0   ,     0    ,    1) \
   _(GPIO_PIN_BUTTON3     , 1 ,  0   ,  0  , 0 ,   0   ,     0    ,    1) \
   _(GPIO_PIN_BLADE_ADC0  , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_BLADE_ADC1  , 0 ,  0   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_PB5         , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_PB6         , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_PB7         , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_PB8         , 1 ,  1   ,  0  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LCD_RST     , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LCD_BL_RED  , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LCD_BL_GREEN, 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LCD_BL_BLUE , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_WHITE   , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_PURPLE  , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_ORANGE  , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_CYAN    , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_YELLOW  , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_BLUE    , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_GREEN   , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_LED_RED     , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_RST     , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_CS      , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_MRDY    , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \
   _(GPIO_PIN_ANT_SRDY    , 1 ,  1   ,  1  , 0 ,   0   ,     0    ,    0) \


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static PFN_GPIO_IRQ pfnGpioIrqs[GPIO_PIN_COUNT];


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void GpioIrqHandler(u32 pins, GPIO_PIN pin);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
// interrupt enable/disable (IER/IDR)
// interrupt mask (IMR)
// output write enable/disable (OWER/OWDR)
// additional interrupt modes enable/disable (AIMER/AIMDR, AIMMR)
// edge vs level interrupt
void GpioInit(void)
{
   BspPeriphOn(PERIPH_PIOA);
   BspPeriphOn(PERIPH_PIOB);

   // NOTE: Set the pin parameters BEFORE setting to an output (pins default to inputs on startup)
   // Set pull-up resistors
   GPIOA->PIO_PPUER =  (GPIO_PIN_TABLE_A(AS_PULLUP) 0);
   GPIOA->PIO_PPUDR = ~(GPIO_PIN_TABLE_A(AS_PULLUP) 0);
   GPIOB->PIO_PPUER =  (GPIO_PIN_TABLE_B(AS_PULLUP) 0);
   GPIOB->PIO_PPUDR = ~(GPIO_PIN_TABLE_B(AS_PULLUP) 0);

   // Set open drain pins
   GPIOA->PIO_MDER =  (GPIO_PIN_TABLE_A(AS_OPEN_DRAIN) 0);
   GPIOA->PIO_MDDR = ~(GPIO_PIN_TABLE_A(AS_OPEN_DRAIN) 0);
   GPIOB->PIO_MDER =  (GPIO_PIN_TABLE_B(AS_OPEN_DRAIN) 0);
   GPIOB->PIO_MDDR = ~(GPIO_PIN_TABLE_B(AS_OPEN_DRAIN) 0);

   // Set default pin output state
   GPIOA->PIO_SODR =  (GPIO_PIN_TABLE_A(AS_LEVEL) 0);
   GPIOA->PIO_CODR = ~(GPIO_PIN_TABLE_A(AS_LEVEL) 0);
   GPIOB->PIO_SODR =  (GPIO_PIN_TABLE_B(AS_LEVEL) 0);
   GPIOB->PIO_CODR = ~(GPIO_PIN_TABLE_B(AS_LEVEL) 0);

   // Set peripheral A vs B
   GPIOA->PIO_ABSR = (GPIO_PIN_TABLE_A(AS_PERIPH_AB) 0);
   GPIOB->PIO_ABSR = (GPIO_PIN_TABLE_B(AS_PERIPH_AB) 0);

   // Set peripheral vs GPIO mode
   GPIOA->PIO_PER =  (GPIO_PIN_TABLE_A(AS_GPIO) 0);
   GPIOA->PIO_PDR = ~(GPIO_PIN_TABLE_A(AS_GPIO) 0);
   GPIOB->PIO_PER =  (GPIO_PIN_TABLE_B(AS_GPIO) 0);
   GPIOB->PIO_PDR = ~(GPIO_PIN_TABLE_B(AS_GPIO) 0);

   // set input/output mode
   GPIOA->PIO_OER =  (GPIO_PIN_TABLE_A(AS_OUTPUT) 0);
   GPIOA->PIO_ODR = ~(GPIO_PIN_TABLE_A(AS_OUTPUT) 0);
   GPIOB->PIO_OER =  (GPIO_PIN_TABLE_B(AS_OUTPUT) 0);
   GPIOB->PIO_ODR = ~(GPIO_PIN_TABLE_B(AS_OUTPUT) 0);

   // Set pins that can adjust the ouput
   GPIOA->PIO_OWER =  (GPIO_PIN_TABLE_A(AS_WRITABLE) 0);
   GPIOA->PIO_OWDR = ~(GPIO_PIN_TABLE_A(AS_WRITABLE) 0);
   GPIOB->PIO_OWER =  (GPIO_PIN_TABLE_B(AS_WRITABLE) 0);
   GPIOB->PIO_OWDR = ~(GPIO_PIN_TABLE_B(AS_WRITABLE) 0);

   // set input debounce vs glitch filter
   GPIOA->PIO_DIFSR  =  (GPIO_PIN_TABLE_A(AS_DEBOUNCE) 0);
   GPIOA->PIO_SCIFSR = ~(GPIO_PIN_TABLE_A(AS_DEBOUNCE) 0);
   GPIOA->PIO_IFER   =  (GPIO_PIN_TABLE_A(AS_DEBOUNCE) 0);
   GPIOA->PIO_IFDR   = ~(GPIO_PIN_TABLE_A(AS_DEBOUNCE) 0);
   GPIOB->PIO_DIFSR  =  (GPIO_PIN_TABLE_B(AS_DEBOUNCE) 0);
   GPIOB->PIO_SCIFSR = ~(GPIO_PIN_TABLE_B(AS_DEBOUNCE) 0);
   GPIOB->PIO_IFER   =  (GPIO_PIN_TABLE_B(AS_DEBOUNCE) 0);
   GPIOB->PIO_IFDR   = ~(GPIO_PIN_TABLE_B(AS_DEBOUNCE) 0);

   // set debounce interval
   GPIOA->PIO_SCDR = GPIO_PIN_DEBOUNCE_DIV;
   GPIOB->PIO_SCDR = GPIO_PIN_DEBOUNCE_DIV;

   // Set up interrupts
   GPIOA->PIO_IDR = UINT32_MAX;
   GPIOB->PIO_IDR = UINT32_MAX;
   GPIOA->PIO_ISR;
   GPIOB->PIO_ISR;
   memset(pfnGpioIrqs, 0, sizeof(pfnGpioIrqs));
   NVIC_ClearPendingIRQ(IRQn_PIOA);
   NVIC_ClearPendingIRQ(IRQn_PIOB);
   NVIC_EnableIRQ(IRQn_PIOA);
   NVIC_EnableIRQ(IRQn_PIOB);
   NVIC_SetPriority(IRQn_PIOA, IRQ_PRIO_GPIO);
   NVIC_SetPriority(IRQn_PIOB, IRQ_PRIO_GPIO);
}

void GpioTest(void)
{
   for (GPIO_PIN pin = (GPIO_PIN)0; pin < GPIO_PIN_COUNT; pin++)
   {
      GpioToggle(pin);
      GpioToggle(pin);
   }
}

void GpioIrqInstall(GPIO_PIN pin, GPIO_IRQ irq, PFN_GPIO_IRQ pfnGpioIrq)
{
   if (GPIO_PIN_IS_VALID(pin))
   {
      AT91S_PIO *pGpio = pGpioReg[GPIO_PIN_PORT(pin)];
      u32 pinBit = GPIO_PIN_BIT(pin);
      if (irq == GPIO_IRQ_OFF)
      {
         pGpio->PIO_IDR = pinBit;
         pfnGpioIrqs[pin] = pfnGpioIrq;
      }
      else if (irq == GPIO_IRQ_CHANGE)
      {
         pfnGpioIrqs[pin] = pfnGpioIrq;
         pGpio->PIO_AIMDR = pinBit;
         pGpio->PIO_IER = pinBit;
      }
      else
      {
         pfnGpioIrqs[pin] = pfnGpioIrq;
         if ((irq == GPIO_IRQ_HIGH_LEVEL) || (irq == GPIO_IRQ_LOW_LEVEL))
            pGpio->PIO_LSR = pinBit;
         else
            pGpio->PIO_ESR = pinBit;

         if ((irq == GPIO_IRQ_HIGH_LEVEL) || (irq == GPIO_IRQ_RISING_EDGE))
            pGpio->PIO_REHLSR = pinBit;
         else
            pGpio->PIO_FELLSR = pinBit;

         pGpio->PIO_AIMER = pinBit;
         pGpio->PIO_IER = pinBit;
      }
   }
}


void GpioSet(GPIO_PIN pin)
{
   if (GPIO_PIN_IS_VALID(pin))
   {
      pGpioReg[GPIO_PIN_PORT(pin)]->PIO_SODR = GPIO_PIN_BIT(pin);
   }
}

void GpioClear(GPIO_PIN pin)
{
   if (GPIO_PIN_IS_VALID(pin))
   {
      pGpioReg[GPIO_PIN_PORT(pin)]->PIO_CODR = GPIO_PIN_BIT(pin);
   }
}

void GpioSetState(GPIO_PIN pin, bool pinState)
{
   if (pinState)
   {
      GpioSet(pin);
   }
   else
   {
      GpioClear(pin);
   }
}

void GpioToggle(GPIO_PIN pin)
{
   GpioSetState(pin, !GpioIsSet(pin));
}

bool GpioIsSet(GPIO_PIN pin)
{
   return (pGpioReg[GPIO_PIN_PORT(pin)]->PIO_PDSR & GPIO_PIN_BIT(pin))? true : false;
}


void GpioDirSetOutput(GPIO_PIN pin)
{
   if (GPIO_PIN_IS_VALID(pin))
   {
      pGpioReg[GPIO_PIN_PORT(pin)]->PIO_OER = GPIO_PIN_BIT(pin);
   }
}

void GpioDirSetInput(GPIO_PIN pin)
{
   if (GPIO_PIN_IS_VALID(pin))
   {
      pGpioReg[GPIO_PIN_PORT(pin)]->PIO_ODR = GPIO_PIN_BIT(pin);
   }
}

void GpioDirSetState(GPIO_PIN pin, bool pinState)
{
   if (pinState)
   {
      GpioDirSetOutput(pin);
   }
   else
   {
      GpioDirSetInput(pin);
   }
}

void GpioDirToggle(GPIO_PIN pin)
{
   GpioDirSetState(pin, !GpioDirIsOutput(pin));
}

bool GpioDirIsOutput(GPIO_PIN pin)
{
   return (pGpioReg[GPIO_PIN_PORT(pin)]->PIO_OSR & GPIO_PIN_BIT(pin))? true : false;
}


void GpioSetPeriphMode(GPIO_PIN pin)
{
   pGpioReg[GPIO_PIN_PORT(pin)]->PIO_PDR = GPIO_PIN_BIT(pin);
}

void GpioSetGpioMode(GPIO_PIN pin)
{
   pGpioReg[GPIO_PIN_PORT(pin)]->PIO_PER = GPIO_PIN_BIT(pin);
}


void PIOA_IrqHandler(void)
{
   u32 i = GPIOA->PIO_ISR;
   GpioIrqHandler(i & GPIOA->PIO_IMR, (GPIO_PIN)0);
}

void PIOB_IrqHandler(void)
{
   u32 i = GPIOB->PIO_ISR;
   GpioIrqHandler(i & GPIOB->PIO_IMR, (GPIO_PIN)32);
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void GpioIrqHandler(u32 pins, GPIO_PIN pin)
{
   while (pins != 0)
   {
      if (pins & BIT0)
      {
         pfnGpioIrqs[pin](pin);
      }
      pin++;
      pins >>= 1;
   }
}

