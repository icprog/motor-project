/*******************************************************************************************
Beeper Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __BEEPER_H__
#define __BEEPER_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   BEEPER_RIGHT,
   BEEPER_LEFT,
   BEEPER_COUNT
}BEEPER_NUM;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void BeeperInit(void);

void BeeperOn(u16f freqHz, BEEPER_NUM beeperNum);
void Beeper1On(u16f freqHz);
void Beeper2On(u16f freqHz);

void BeeperOff(BEEPER_NUM beeperNum);
void Beeper1Off(void);
void Beeper2Off(void);

#endif
