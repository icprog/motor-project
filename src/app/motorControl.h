/*******************************************************************************************
motorControl.h

Description:
Determines required frequency and voltage for given RPM input

Quinn Miller
********************************************************************************************/

#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void MotorControlInit(void);
void MotorControlUpdate(void);

#endif
