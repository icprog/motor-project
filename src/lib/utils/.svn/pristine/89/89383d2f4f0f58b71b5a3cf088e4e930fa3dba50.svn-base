/*******************************************************************************************
 Generic Threading Utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "threadUtils.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
char *asctime_r(const struct tm *pTime, char *pDest)
{
	if (pDest)
	{
		PREPARE_CRITICAL();
		ENTER_CRITICAL();
		char *pAscTim = asctime(pTime);
		strcpy(pDest, pAscTim);
		EXIT_CRITICAL();
	}
	return pDest;
}

char *ctime_r(const time_t *pTime, char *pDest)
{
	if (pDest)
	{
		PREPARE_CRITICAL();
		ENTER_CRITICAL();
		char *pTm = ctime(pTime);
		strcpy(pDest, pTm);
		EXIT_CRITICAL();
	}
	return pDest;
}

struct tm *gmtime_r(const time_t *pTime, struct tm *pDest)
{
	if (pDest)
	{
		PREPARE_CRITICAL();
		ENTER_CRITICAL();
		struct tm *pTm = gmtime(pTime);
		memcpy(pDest, pTm, sizeof(struct tm));
		EXIT_CRITICAL();
	}
	return pDest;
}

struct tm *localtime_r(const time_t *pTime, struct tm *pDest)
{
	if (pDest)
	{
		PREPARE_CRITICAL();
		ENTER_CRITICAL();
		struct tm *pTm = localtime(pTime);
		memcpy(pDest, pTm, sizeof(struct tm));
		EXIT_CRITICAL();
	}
	return pDest;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

