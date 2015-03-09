/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __MENU_H__
#define __MENU_H__

#include "types.h"
#include "lcd.h"
#include "led.h"
#include "button.h"
#include "ledShow.h"
#include "musicPlayer.h"
#include "includes.h"

/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
struct menu {
  u8 prevMenu;
  u8 numLinks;
  u8 nextMenu[6];       // must be declared to have enough room for menu links
  char menuLink[6][20];     // same as above
};

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void MenuInit();
void MenuUpdate();

#endif
