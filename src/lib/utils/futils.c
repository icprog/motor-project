/*******************************************************************************************
File utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "futils.h"
#include <ctype.h>	//lint !e537
#include <string.h>	//lint !e537


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
UINT8 FutilsDriveNum(char const *pName)
{
	UINT8 drv;
	if (pName[0] == '/')
	{
		drv = pName[1] - '0';
	}
	else if (isdigit(pName[0]))
	{
		drv = pName[0] - '0';
	}
	else
	{
		drv = 0;
	}
	return drv;
}

char const * FutilsFileName(char const *pName)
{
	char const *pPathEndPos;
	char ch;

	// find last folder delimiter '/' or '\'
	pPathEndPos = pName;
	while ((ch = *pName++) != '\0')
	{
		if ((ch == '/') || (ch == '\\'))
			pPathEndPos = pName;
	}
	return pPathEndPos;
}

char const * FutilsFileExtension(char const *pName)
{
	char const *pLastDot = NULL;
	char ch;

	// find last '.'
	while ((ch = *pName++) != '\0')
	{
		if (ch == '.')
			pLastDot = pName;
	}
	if (!pLastDot)
	{
		pLastDot = pName;
	}
	return pLastDot;
}


BOOL FutilsFilenameIsValid(char const *pName, BOOL wildCardOkay)
{
	char ch;

	if (!pName || (*pName == F_DELETED_CHAR))
		return FALSE;

	while ((ch = *pName++) != '\0')
	{
		if (((UINT8)ch < 0x20) || (ch == 0x22) ||
			 (ch == '|') || (ch == '<') || (ch == '>') ||
			 (ch == '/') || (ch == '\\') || (ch == ':'))
		{
			return FALSE;
		}
		if (!wildCardOkay && ((ch == '?') || (ch == '*')))
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Derived from Alessandro Cantatore (http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html)
/*
# The code mentioned below is the copyright property of Alessandro Felice Cantatore with the exception
  of those algorithms which are, as specified in the documentation, property of the respective owners.
# You are not allowed to patent or copyright this code as your!
# If you work for Microsoft you are not allowed to use this code!
# If you work for IBM you can use this code only if the target OS is OS/2 or linux.
# You are not allowed to use this code for projects or programs owned or used by the army, the
  department of defense or the government of any country violating human rights or international laws
  (including those western countries pretending to "export" democracy).
If you are unsure just check what Amnesty International reports about your country.
# You are not allowed to use this code in programs used in activities causing harm to living beeings
  or to the environment.
# You are free to use the algorithms described below in your programs, or modify them to suit your
  needs, provided that you include their source, including these notes and my name, (you are not
  required to include the whole source of your program) in your program package.
# You are not allowed to sell this code, but you can include it into a commercial program.
# If you have any doubt ask the author.
*/
BOOL FutilsWildcardNameIsMatch(char const *pat, char const *str)
{
   char const *s;
	char const *p;
   BOOL star = FALSE;

loopStart:
   for (s = str, p = pat; *s; ++s, ++p) {
      switch (*p) {
         case '?':
            if (*s == '.') goto starCheck;	//lint !e801
            break;
         case '*':
            star = TRUE;
            str = s, pat = p;
            if (!*++pat) return TRUE;
            goto loopStart;						//lint !e801
         default:
            if (toupper(*s) != toupper(*p))
               goto starCheck;					//lint !e801
            break;
      } /* endswitch */
   } /* endfor */
   if (*p == '*') ++p;
   return (BOOL)(!*p);

starCheck:
   if (!star) return FALSE;
   str++;
   goto loopStart;									//lint !e801
}	//lint !e438

BOOL FutilsNameIsOnlyDots(char const *pName)
{
	char ch;
	while ((ch = *pName++) != '\0')
	{
		if (ch != '.')
			return FALSE;
	}
	return TRUE;
}

// Strips the last folder from the current path. Will return the folder name that was removed, so
// the incoming string may require room for one extra '\0' character
char * FutilsUpDir(char *pPath)
{
	UINT32 len = strlen(pPath);
	BOOL trailingSlash = FALSE;
	char *pRetName = NULL;
	char *pSlash;

	if ((pPath[len-1] == '\\') || (pPath[len-1] == '/'))
	{
		pPath[len-1] = '\0';
		trailingSlash = TRUE;
	}

	pSlash = strrchr(pPath, '\\');
	if (!pSlash)
		pSlash = strrchr(pPath, '/');

	if (pSlash)
	{
		if (trailingSlash)
		{
			pSlash++;
			memmove(pSlash+1, pSlash, (UINT32)(len - (UINT32)(pSlash - pPath)));
		}
		pRetName = pSlash+1;
		*pSlash = '\0';
	}
	else if (trailingSlash)
		pPath[len-1] = '/';		// restore original string

	return pRetName;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

