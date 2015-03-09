/*******************************************************************************************
File utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __FUTILS_H__
#define __FUTILS_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef F_DELETED_CHAR
#define F_DELETED_CHAR	((char)0xE5)
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
UINT8 FutilsDriveNum(char const *pName);
char const * FutilsFileName(char const *pName);
char const * FutilsFileExtension(char const *pName);
BOOL FutilsFilenameIsValid(char const *pName, BOOL wildCardOkay);
BOOL FutilsWildcardNameIsMatch(char const *pWildName, char const *pName);
BOOL FutilsNameIsOnlyDots(char const *pName);
char * FutilsUpDir(char *pPath);	//lint -esym(534, FutilsUpDir) Strip last folder from path and return folder name

#endif
