/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Quinn Miller
********************************************************************************************/


#include "mainMenu.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
static const char menuLinks2[6][20] = {"Led Show\n", "Music Player\n", "Option 3\n",
  "Option 4\n", "Option 5\n", "Option 6\n"};

/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static u8 currLink = 0;         // Link for selector
static u8 topLink = 0;          // Link on top row of LCD
static u8 botLink = 1;          // Link on bottom row of LCD
static u8 lastScroll = 0;       // Last scroll direction (0 down, 1 up)

static u8 currMenu = 255;       // Current menu
 
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void scrollDown(u8* linkA, u8* linkB, u8* lastScroll);
static void scrollUp(u8* linkA, u8* linkB, u8* lastScroll);
static void forwardMenu(u8 linkA, u8 currLink);
static void backMenu();

/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void MainMenuInit()
{
  //LedSequence(100);
  //LedRgbCycleSet(5000, UINT8_MAX, UINT8_MAX);
}

void MainMenuUpdate()
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
    forwardMenu(topLink, currLink);
  }
    
  // Screen update
  LcdClear();
  
  if(currLink == 0)
  {
    LcdPutc('*');
    LcdPuts(menuLinks2[topLink]);
    LcdPuts(menuLinks2[botLink]);
  }
  else
  {
    LcdPuts(menuLinks2[topLink]);
    LcdPutc('*');
    LcdPuts(menuLinks2[botLink]);
  }
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void scrollDown(u8* linkA, u8* linkB, u8* lastScroll)
{
  u8 temp = *linkB;
  
  // Adjusting pointer
  if (currLink == 1)
    currLink = 0;
  
  if (*linkB == 5)
    currLink = 1;
  
  // Translating links
  if (*linkB < 5)
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
  if (currLink == 0)
    currLink = 1;
  
  if (*linkA == 0)
    currLink = 0;
  
  // Translating links
  if (*linkA > 0)
  {
    *linkA = temp - 1;
    *linkB = temp;
    *lastScroll = 1;
  }
}

static void forwardMenu(u8 linkA, u8 currLink)
{
  u8 nextMenu = linkA + currLink;
  
  switch (nextMenu)
  {
  case 0:
    LedShowInit();
    
  case 1:
    MusicPlayerInit();
    MusicPlayerUpdate();
  default:
    break;
  }
}