/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "menu2.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
static const MENU2 menu2[] =
{
  {
    "Test A",
    SongTwinkleTwinkle,
    NULL
  },
  {
    "Test B",
    SongTwinkleTwinkle,
    NULL
  },
  {
    "Test C",
    MusicStop,
    NULL
  }
};

static const MENU2 menu1[] =
{
  {
    "Test 1",
    NULL,
    menu2
  },
  {
    "Test 2",
    NULL,
    menu2
  },
  {
    "Test 3",
    NULL,
    menu2
  }
};

/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
u8 menuIndex = 0;
MENU2* currMenu = &menu1;
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void scrollDown(u8* index, MENU2* menu);
static void scrollUp(u8* index);
static void forward();

/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void Menu2Init()
{
  
}

void Menu2Update()
{
  // Check for button presses
  if (ButtonWasPressed(BTN_RM))         // Scroll Down
  {
    ButtonPressAck(BTN_RM);
    scrollDown(&menuIndex, currMenu);
  }
  
  if (ButtonWasPressed(BTN_LM))         // Scroll Up
  {
    ButtonPressAck(BTN_LM);
    scrollUp(&menuIndex);
  }
  
  if (ButtonWasPressed(BTN_R))
  {
    ButtonPressAck(BTN_R);
    forward();
  }
  
  // Screen update
  LcdClear();
  LcdPuts(currMenu[menuIndex].name); 
  LcdPuts("\n");
  LcdPuts(currMenu[menuIndex + 1].name);
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void scrollDown(u8* index, MENU2* menu)
{
  if (*index < ((u8)(NELEMENTS(*menu)) - 1))
  {
    *index++;
  }
}

static void scrollUp(u8* index)
{
  
}

static void forward()
{
  
}
