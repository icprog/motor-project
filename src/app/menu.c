/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "menu.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
struct menu menu[3] =
{
  // Main menu menuId = 0
  {
    {0},{6},{1,2,3,4,5,6},
    {"Led Show\n","Music Player\n","Option 3\n","Option 4\n","Option 5\n","Option 6\n"}
  },
  
  // LED show menuId = 1
  {
    {0},{3},{255,255,255,255,255,255},
    {"Show 1\n","Show 2\n","Show 3\n"}
  }
};
     
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static u8 selector = 0;         // Selector position
static u8 topLink = 0;          // Link on top row of LCD
static u8 botLink = 1;          // Link on bottom row of LCD
static u8 lastScroll = 0;       // Last scroll direction (0 down, 1 up)

static u8 menuId = 0;         // Current Menu ID

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void scrollDown(u8* linkA, u8* linkB, u8* lastScroll);
static void scrollUp(u8* linkA, u8* linkB, u8* lastScroll);
static void nextMenu(u8 linkA, u8 selector, u8* id);
static void prevMenu(u8* id);
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void MenuInit()
{
  //LedSequence(100);
  //LedRgbCycleSet(5000, UINT8_MAX, UINT8_MAX);
}

void MenuUpdate()
{
  // Check for button presses
  if (ButtonWasPressed(BTN_RM))         // Scroll Down
  {
    ButtonPressAck(BTN_RM);
    scrollDown(&topLink, &botLink, &lastScroll);
  }
  
  if (ButtonWasPressed(BTN_LM))         // Scroll Up
  {
    ButtonPressAck(BTN_LM);
    scrollUp(&topLink, &botLink, &lastScroll);
  }
  
  if (ButtonWasPressed(BTN_R))
  {
    ButtonPressAck(BTN_R);
    nextMenu(topLink, selector, &menuId);
    topLink = 0;
    botLink = 1;
    selector = 0;
  }
  
  if (ButtonWasPressed(BTN_L))
  {
    ButtonPressAck(BTN_L);
    prevMenu(&menuId);
    topLink = 0;
    botLink = 1;
    selector = 0;
  }
  
  // Screen update
  LcdClear();
  
  if(selector == 0)
  {
    LcdPutc('*');
    LcdPuts(menu[menuId].menuLink[topLink]);
    LcdPuts(menu[menuId].menuLink[botLink]);
  }
  else
  {
    LcdPuts(menu[menuId].menuLink[topLink]);
    LcdPutc('*');
    LcdPuts(menu[menuId].menuLink[botLink]);
  }
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void scrollDown(u8* linkA, u8* linkB, u8* lastScroll)
{
  u8 temp = *linkB;
  
  // Adjusting pointer
  if (selector == 1)
    selector = 0;
  
  if (*linkB == (menu[menuId].numLinks - 1))
    selector = 1;
  
  // Translating links
  if (*linkB < (menu[menuId].numLinks - 1))
  {
    *linkB = temp + 1;
    *linkA = temp;
    *lastScroll = 0;
  }
}

static void scrollUp(u8* linkA, u8* linkB,u8* lastScroll)
{
  u8 temp = *linkA;
  
  // Adjusting pointer
  if (selector == 0)
    selector = 1;
  
  if (*linkA == 0)
    selector = 0;
  
  // Translating links
  if (*linkA > 0)
  {
    *linkA = temp - 1;
    *linkB = temp;
    *lastScroll = 1;
  }
}

static void nextMenu(u8 linkA, u8 selector, u8* id)
{
  *id = menu[menuId].nextMenu[(linkA + selector)];
}

static void prevMenu(u8* id)
{
  *id = menu[menuId].prevMenu;
}