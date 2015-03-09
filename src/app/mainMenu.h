/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __MAINMENU_H__
#define __MAINMENU_H__

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
static const char menuLinks[6][20] = {"Led Show\n", "Music Player\n", "Option 3\n",
  "Option 4\n", "Option 5\n", "Option 6\n"};

/**************************************************************************
 *                                  Types
 **************************************************************************/


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void MainMenuInit();
void MainMenuUpdate();

#endif
