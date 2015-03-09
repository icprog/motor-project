/*******************************************************************************************
 Generic Threading Utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __THREAD_UTILS_H__
#define __THREAD_UTILS_H__

#include <time.h>
#include "bsp.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
char *asctime_r(const struct tm *pTime, char *pDest);
char *ctime_r(const time_t *pTime, char *pDest);
struct tm *gmtime_r(const time_t *pTime, struct tm *pDest);
struct tm *localtime_r(const time_t *pTime, struct tm *pDest);

#endif
