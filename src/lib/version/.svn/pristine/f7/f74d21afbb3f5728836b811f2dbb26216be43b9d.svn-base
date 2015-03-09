/*******************************************************************************************
Versioning information

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __VERSION_H__
#define __VERSION_H__

#include "includes.h"
#include "versionNum.h"
#include <time.h>


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef VERSION_LOCATION
#define VERSION_LOCATION		// Could also define as @ "VERSIONING"
#endif

#ifndef PRODUCT_STR
#error "PRODUCT_STR must be defined"
#endif
#ifndef PRODUCT_ID
#error "PRODUCT_ID must be defined"
#endif
#ifndef FW_VERSION
#error "FW_VERSION must be defined"
#endif
#ifndef FW_MINIMUM
#error "FW_MINIMUM must be defined"
#endif
#ifndef FW_DOWN_MINIMUM
#error "FW_DOWN_MINIMUM must be defined"
#endif
#ifndef HW_VERSION
#error "HW_VERSION must be defined"
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
	FW_VER_MAJOR,
	FW_VER_MINOR,
	FW_VER_REV,
	FW_VER_COUNT
}FW_VER;

typedef struct
{
   u8    marker[6];                // Upgrade Image marker -- always "_FwMk_"
   char  product[22];              // Product name
   u8    productId;                // Product ID from 0 to 255
   u8    hwVersion;                // Current hardware version (1 to 255)
   u8    fwVersion[FW_VER_COUNT];  // Current firmware version in Major.Minor.Rev
   u8    fwMinimum[FW_VER_COUNT];  // Minimum firmware version to upgrade to this version
   u8    fwDownMin[FW_VER_COUNT];  // Minimum firmware version to allow downgrades to
   char  buildDate[12];            // Build date ("MMM dd yyy", example "Oct 30 2008"
   char  buildTime[9];             // Build time ("hh:mm:ss")
   u32   programCRC;               // Program CRC (filled in by linker)
} PROGRAM_ID;
extern const PROGRAM_ID programId;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
time_t	VersionGetBuildTime(void);

#endif
