/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __MENU2_H__
#define __MENU2_H__

#include "types.h"
#include "lcd.h"
#include "button.h"
#include "songTwinkleTwinkle.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct menu2
{
  char* name;
  void (*Action)(void);
  struct menu2 const *next;
} MENU2;

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void Menu2Init();
void Menu2Update();

#endif
