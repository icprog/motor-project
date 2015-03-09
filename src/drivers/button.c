/*******************************************************************************************
Button Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "button.h"
#include "gpio.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define BUTTON_IS_VALID(btn)     ((u8f)btn < (u8f)BTN_COUNT)
static const GPIO_PIN BtnToGpio[BTN_COUNT] =
{
   [BTN_L]  = GPIO_PIN_BUTTON0,
   [BTN_LM] = GPIO_PIN_BUTTON1,
   [BTN_RM] = GPIO_PIN_BUTTON2,
   [BTN_R]  = GPIO_PIN_BUTTON3,
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   BUTTONS btn;
   volatile bool wasPressed;
   volatile bool holdAck;
   volatile u32 pressTick;
   BTN_EVENTS btnEvents;
}BTN;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static BTN btns[BTN_COUNT];


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void ButtonChangeIrq(GPIO_PIN pin);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void ButtonInit(void)
{
   memset(btns, 0, sizeof(btns));
   for (BUTTONS btn = (BUTTONS)0; btn < BTN_COUNT; btn++)
   {
      btns[btn].btn = btn;
      ButtonChangeIrq(BtnToGpio[btn]);
      btns[btn].wasPressed = false;
      GpioIrqInstall(BtnToGpio[btn], GPIO_IRQ_CHANGE, ButtonChangeIrq);
   }
}

void ButtonUpdate(void)
{
   for (BUTTONS btn = (BUTTONS)0; btn < BTN_COUNT; btn++)
   {
      if ((btns[btn].pressTick < UINT32_MAX) && ButtonIsPressed(btn))
      {
         btns[btn].pressTick++;
         if (!btns[btn].holdAck && btns[btn].btnEvents.pfnBtnHeld && ButtonIsHeld(btn))
         {
            btns[btn].holdAck = true;
            btns[btn].btnEvents.pfnBtnHeld(btn);
         }
      }
   }
}

void ButtonNotify(BUTTONS btn, BTN_EVENTS const *pBtnEvents)
{
   if (BUTTON_IS_VALID(btn))
   {
      if (pBtnEvents == NULL)
      {
         memset(&btns[btn].btnEvents, 0, sizeof(BTN_EVENTS));
      }
      else
      {
         memcpy(&btns[btn].btnEvents, pBtnEvents, sizeof(BTN_EVENTS));
      }
   }
}

bool ButtonIsPressed(BUTTONS btn)
{
   return (btns[btn].pressTick > 0);
}

bool ButtonWasPressed(BUTTONS btn)
{
   return btns[btn].wasPressed;
}

bool ButtonIsHeld(BUTTONS btn)
{
   return ButtonIsHeldXMs(btn, BUTTON_HOLD_TIME_MS);
}

bool ButtonIsHeldXMs(BUTTONS btn, u32 ms)
{
   return (!btns[btn].holdAck && (btns[btn].pressTick >= MS_TO_TICKS_RATE(ms, BUTTON_UPDATE_RATE_MS)));
}

void ButtonPressAck(BUTTONS btn)
{
   if (BUTTON_IS_VALID(btn))
   {
      btns[btn].wasPressed = false;
   }
}

void ButtonHoldAck(BUTTONS btn)
{
   if (BUTTON_IS_VALID(btn))
   {
      btns[btn].holdAck = true;
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void ButtonChangeIrq(GPIO_PIN pin)
{
   for (BUTTONS btn = (BUTTONS)0; btn < BTN_COUNT; btn++)
   {
      if (BtnToGpio[btn] == pin)
      {
         //NOTE: All buttons are active low
         if (GpioIsSet(pin))
         {
            // button released
            if (!btns[btn].holdAck)
               btns[btn].wasPressed = true;
            btns[btn].pressTick = 0;
            btns[btn].holdAck = false;
            if (btns[btn].btnEvents.pfnBtnReleased)
               btns[btn].btnEvents.pfnBtnReleased(btn);
         }
         else
         {
            // button pressed
            btns[btn].pressTick = 1;
            if (btns[btn].btnEvents.pfnBtnPressed)
               btns[btn].btnEvents.pfnBtnPressed(btn);
         }
         break;
      }
   }
}

