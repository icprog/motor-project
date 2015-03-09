/*******************************************************************************************
Secure Digital (SD) Card (SDv3/SDv1/SDv2 and MMC) SPI Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __SD_H__
#define __SD_H__


/**************************************************************************
 *                                  Constants
 **************************************************************************/
//define SD_DEBUG_TYPE		// If not defined, debug messages are disabled


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/

void SdInit(void);
void SdKill(void);

#endif
