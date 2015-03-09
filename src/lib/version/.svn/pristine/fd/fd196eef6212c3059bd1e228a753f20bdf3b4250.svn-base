/*******************************************************************************************
Versioning information

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "version.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
__root const PROGRAM_ID programId VERSION_LOCATION =
{
   .marker     =  "_FwMk_",         //lint !e784 Upgrade Image marker -- always "_FwMk_"
   .product    =  PRODUCT_STR,      // Product string
   .productId  =  PRODUCT_ID,       // Product ID from 0 to 255
   .hwVersion  =  HW_VERSION,       // Current hardware version (1 to 255)
   .fwVersion  =  FW_VERSION,       // Current firmware version in Major.Minor.Rev
   .fwMinimum  =  FW_MINIMUM,       // Minimum firmware version to upgrade to this version
   .fwDownMin  =  FW_DOWN_MINIMUM,  // Minimum firmware version to allow downgrades to
   .buildDate  =  __DATE__,         // Build date inserted by compiler
   .buildTime  =  __TIME__,         // Build time inserted by compiler
   .programCRC =  0                 // Program CRC (filled in by linker)
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static time_t versionBuildTime = 0;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static time_t	VersionBuildTime(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
time_t VersionGetBuildTime(void)
{
	if (versionBuildTime == 0)
	{
		versionBuildTime = VersionBuildTime();
	}
	return versionBuildTime;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
// Converts the ASCII buildDate and buildTime into a time_t type (# seconds sinc Jan 1, 1970)
static time_t VersionBuildTime(void)
{
	const char *months [] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	struct tm rtc;
	int i;

	memset(&rtc, 0, sizeof(rtc));

	// buildDate in "MMM dd yyy" format
	for (i = 0; i < 12; i++)
	{
		if (strncmp(programId.buildDate, months[i], 3) == 0)
		{
			rtc.tm_mon = i;
			break;
		}
	}
	rtc.tm_mday = (programId.buildDate[4] & 0x0F)*10 + (programId.buildDate[5] & 0x0F);
	rtc.tm_year = atoi(&programId.buildDate[7]) - 1900;

	// Build time in "hh:mm:ss" format
	rtc.tm_hour = atoi(&programId.buildTime[0]);
	rtc.tm_min = atoi(&programId.buildTime[3]);
	rtc.tm_sec = atoi(&programId.buildTime[6]);

	return mktime(&rtc);
}

