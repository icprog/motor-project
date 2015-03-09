/*******************************************************************************************
Button Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef BUTTON_HOLD_TIME_MS
#define BUTTON_HOLD_TIME_MS   (1000)
#endif
#ifndef BUTTON_UPDATE_RATE_MS
#define BUTTON_UPDATE_RATE_MS (SYS_TICK_RATE_MS)
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   BTN_L,
   BTN_LM,
   BTN_RM,
   BTN_R,
   BTN_COUNT
}BUTTONS;

typedef void (*PFN_BTN_EVENT)(BUTTONS);
typedef struct
{
   PFN_BTN_EVENT pfnBtnPressed;
   PFN_BTN_EVENT pfnBtnReleased;
   PFN_BTN_EVENT pfnBtnHeld;
}BTN_EVENTS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void ButtonInit(void);
void ButtonUpdate(void);
void ButtonNotify(BUTTONS btn, BTN_EVENTS const *pBtnEvents);

bool ButtonIsPressed(BUTTONS btn);
bool ButtonWasPressed(BUTTONS btn);
bool ButtonIsHeld(BUTTONS btn);
bool ButtonIsHeldXMs(BUTTONS btn, u32 ms);
void ButtonPressAck(BUTTONS btn);
void ButtonHoldAck(BUTTONS btn);

#endif
