/*******************************************************************************************
 Generic Utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "utils.h"
#include <ctype.h>	//lint !e537
#include <stdlib.h>	//lint !e537
#include <string.h>	//lint !e537
#include <stdio.h>	//lint !e537
#include <stdlib.h>	//lint !e537
#include <stdarg.h>	//lint !e537


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
BOOL CpuIsLittleEndian(void)
{
	UINT16 n = 1;
	char *p = (char *)&n;
	return (BOOL)(*p == 1);
}

INT32 antoi(char const *pStr, UINT32 maxChars)
{
	char buffer[12];
	maxChars = MIN(sizeof(buffer)-1, maxChars);
	memcpy(buffer, pStr, maxChars);
	buffer[maxChars] = '\0';
	return atoi(buffer);
}

UINT32 GetStringIndexFromTable(char const *const *ppTable, char const *pSearchStr, UINT32 maxIndex)
{
	UINT32 stringIndex;
	for (stringIndex = 0; stringIndex < maxIndex; stringIndex++)
	{
		if (strncmp(pSearchStr, ppTable[stringIndex], strlen(ppTable[stringIndex])) == STR_CMP_MATCH)
		{
			break;
		}
	}
	return stringIndex;
}

int stricmp(char const *pStr1, char const *pStr2)
{
	UINT32 len1 = strlen(pStr1);
	UINT32 len2 = strlen(pStr2);

	return strnicmp(pStr1, pStr2, (len1 > len2)? len1: len2);
}
int strnicmp(char const *pStr1, char const *pStr2, UINT32 len)
{
	char c1, c2;
	while (len--)
	{
		c1 = *pStr1++;
		c2 = *pStr2++;
		c1 = (char)toupper((int)c1);
		c2 = (char)toupper((int)c2);

		if (c1 < c2)
			return -1;
		else if (c1 > c2)
			return 1;
		else if (c1 == '\0')
			return 0;
	}
	return 0;
}
char * stristr(char *pStr1, char *pStr2)
{
	char c1, c2;
	UINT32 len2 = strlen(pStr2);

	c1 = (char)toupper((int)*pStr2);
	while (*pStr1)
	{
		c2 = (char)toupper((int)*pStr1);
		if (c1 == c2)
		{
			if (strnicmp(pStr1, pStr2, len2) == 0)
			{
				return pStr1;
			}
		}
		pStr1++;
	}
	return NULL;
}//lint !e818

char const * strTrimFront(char const *pStr)
{
	if (pStr)
	{
		while ((*pStr != '\0') && isspace(*pStr))
		{
			pStr++;
		}
	}
	return pStr;
}
char * strTrimTail(char *pStr)
{
	if (pStr)
	{
		char *pEnd = &pStr[(strlen(pStr) - 1)];
		while (pEnd > pStr)
		{
			if (!isspace(*pEnd))
			{
				pEnd[1] = '\0';
				break;
			}
			pEnd--;
		}
	}
	return pStr;
}
char * strTrim(char *pStr)
{
	if (pStr)
	{
		while ((*pStr != '\0') && isspace(*pStr))
		{
			pStr++;
		}
		pStr = strTrimTail(pStr);
	}
	return pStr;
}

char * Strxtoa(UINT32 v,char *pStr, INT32 r, INT32 isNeg)
{
	char *pStart = pStr;
	char buf[33],*p;

	p = buf;

	do
	{
		*p++ = "0123456789abcdef"[(v % (UINT32)r) & 0xf];
	} while (v /= (UINT32)r);	//lint !e720

	if (isNeg)
	{
		*p++ = '-';
	}

	do
	{
		*pStr++ = *--p;
	} while (buf != p);

	*pStr = '\0';

	return pStart;
}
char * Stritoa(INT32 v, char *pStr, INT32 r)
{
	if ((r == 10) && (v < 0))
	{
		return Strxtoa((UINT32)(-v), pStr, r, 1);
	}
	return Strxtoa((UINT32)(v), pStr, r, 0);
}

void strncatf(char *pDest, UINT32 maxLen, char const *pFormat, ...)
{
	va_list pArg;
	UINT32 len = strlen(pDest);
	if (maxLen > len)
	{
		va_start(pArg, pFormat);
		vsnprintf(pDest + len, maxLen - len, pFormat, pArg);	//lint !e534
		va_end(pArg);
	}
}


UINT32 aton(char const *pAddr)
{
	// Example: 192.168.1.120 = 0x7801ABC0
	UINT32 retAddr = 0;
	char const *pStart = pAddr;
	for (UINT32 i = 0; i < 4; i++)
	{
		retAddr |= ((UINT32)atoi(pStart) & 0xFF) << (UINT32)(8 * i);
		pStart = strchr(pStart, '.');
		if (pStart)
		{
			pStart++;	// move past the '.' char
		}
		else
		{
			break;
		}
	}
	return retAddr;
}

char const * ntoa(UINT32 addr)
{
	static char a[16];
	ntoa_r(a, addr);
	return a;
}

void ntoa_r(char *pDest, UINT32 addr)
{
#define UINT_FIELD(b)		(((UINT32)b) & 0xFF)
	// Example: 0x7801ABC0 = 192.168.1.120
	char *baddr = (char *)&addr;
	sprintf(pDest, "%u.%u.%u.%u",
			  UINT_FIELD(baddr[0]), UINT_FIELD(baddr[1]), UINT_FIELD(baddr[2]), UINT_FIELD(baddr[3]));
}

void macToa(char *pDest, char const *pMac)
{
	sprintf(pDest, "%02X:%02X:%02X:%02X:%02X:%02X",
			  UINT_FIELD(pMac[0]), UINT_FIELD(pMac[1]), UINT_FIELD(pMac[2]),
			  UINT_FIELD(pMac[3]), UINT_FIELD(pMac[4]), UINT_FIELD(pMac[5]));
}
void atoMac(char *pDest, char const *pMac)
{
	for (UINT32 i = 0; i < 6; i++)
	{
		pDest[i] = (UINT8)strtoul(&pMac[i*3], NULL, 16);	// each field is 3 digits (i.e. 00:11:22:33:44:55)
	}
}


//http://stackoverflow.com/questions/746171/best-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c
//http://graphics.stanford.edu/~seander/bithacks.html
UINT32 BitReverseWord(UINT32 x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));
}


// "maxUlps" is the "Units in the Last Place", which specifies how big an error the caller is willing
// to accept in terms of the value of the least significant digits of the FP number's representation.
// AKA how many representable floats we are willing to accept between A and B
BOOL Fp32AlmostEqual(FP32 A, FP32 B, INT32 maxUlps)
{
   // Make sure maxUlps is non-negative and small enough that the
   // default NAN won't compare as equal to anything.
	ASSERT_RET((maxUlps > 0) && (maxUlps < (4*1024*1024)), FALSE);

	// Make aInt lexicographically ordered as a twos-complement int
	INT32 aInt = *(INT32*)&A;		//lint !e740
	if (aInt < 0)
		aInt = (INT32)0x80000000 - aInt;

	// Make bInt lexicographically ordered as a twos-complement int
	INT32 bInt = *(INT32*)&B;  	//lint !e740
	if (bInt < 0)
		bInt = (INT32)0x80000000 - bInt;

	INT32 intDiff = ABS(aInt - bInt);
	return (BOOL)(intDiff <= maxUlps);
}

/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

