/*******************************************************************************************
 Circular Buffer

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "circularBuffer.h"
#include "includes.h"


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
CIRCULAR_BUF const * CBufInit(CIRCULAR_BUF *pBuf, UINT8 *pStart, UINT32 len, UINT8 optionFlags)
{
	pBuf->flags = optionFlags & CIRCULAR_BUF_OPTION_MASK;
	pBuf->len = len;
	pBuf->pStart = pStart;
	pBuf->rdOffset = pBuf->wrOffset = 0;
	return pBuf;
}

UINT32 CBufFree(CIRCULAR_BUF const *pBuf)
{
	return (UINT32)(pBuf->len - CBufUsed(pBuf));
}

UINT32 CBufUsed(CIRCULAR_BUF const *pBuf)
{
	UINT32 len;
	if (pBuf->flags & CIRCULAR_BUF_IS_FULL)
	{
		len = pBuf->len;
	}
	else if (pBuf->wrOffset >= pBuf->rdOffset)
	{
		len = (UINT32)(pBuf->wrOffset - pBuf->rdOffset);
	}
	else
	{
		len = (UINT32)(pBuf->len - pBuf->rdOffset) + pBuf->wrOffset;
	}
	return len;
}

BOOL CBufIsEmpty(CIRCULAR_BUF const *pBuf)
{
	return (BOOL)((pBuf->rdOffset == pBuf->wrOffset) && !(pBuf->flags & CIRCULAR_BUF_IS_FULL));
}

BOOL CBufIsFull(CIRCULAR_BUF const *pBuf)
{
	return (pBuf->flags & CIRCULAR_BUF_IS_FULL)? TRUE : FALSE;
}


char * CBufGets(char *pDest, UINT32 len, CIRCULAR_BUF *pBuf)
{
	char *pStr = pDest;
	INT32 ch;
	while ((len-- > 1) && ((ch = CBufGetc(pBuf)) >= 0) && (ch != '\n'))
	{
		*pDest++ = (char)ch;
	}
	*pDest = '\0';

	if (pStr == pDest)
		pStr = NULL;
	return pStr;
}

INT32 CBufGetc(CIRCULAR_BUF *pBuf)
{
	UINT8 ch;
	if (CBufRead(&ch, sizeof(ch), pBuf) != sizeof(ch))
	{
		return -1;
	}
	return (INT32)ch;
}

UINT32 CBufRead(void *pDest, UINT32 len, CIRCULAR_BUF *pBuf)
{
	UINT32 rdLen = 0;
	UINT32 blockLen;

	if ((len == 0) || CBufIsEmpty(pBuf))
		return rdLen;

	if (pBuf->rdOffset >= pBuf->wrOffset)
	{
		rdLen = (UINT32)(pBuf->len - pBuf->rdOffset) + pBuf->wrOffset;
		if (rdLen > len)
			rdLen = len;
		memcpy(pDest, &pBuf->pStart[pBuf->rdOffset], rdLen);

		pBuf->rdOffset += rdLen;
		if (pBuf->rdOffset >= pBuf->len)
		{
			pBuf->rdOffset = 0;
		}
		pBuf->flags &= ~CIRCULAR_BUF_IS_FULL;
		pDest = (UINT8 *)pDest + rdLen;	//lint !e826
		len -= rdLen;
	}
	blockLen = MIN(pBuf->wrOffset - pBuf->rdOffset, len);
	memcpy(pDest, &pBuf->pStart[pBuf->rdOffset], blockLen);
	pBuf->rdOffset += blockLen;
	rdLen += blockLen;
	return rdLen;
}

UINT32 CBufPeek(void *pDest, UINT32 len, CIRCULAR_BUF *pBuf)
{
	UINT8 flagBkp = pBuf->flags;
	UINT32 rdBkp = pBuf->rdOffset;
	UINT32 rdLen = CBufRead(pDest, len, pBuf);
	pBuf->flags = flagBkp;
	pBuf->rdOffset = rdBkp;
	return rdLen;
}

void CBufSeek(CIRCULAR_BUF *pBuf, INT32 offset)
{
	INT32 max;
	if (offset > 0)
	{
		if ((pBuf->flags & CIRCULAR_BUF_IS_FULL) &&
			 ((pBuf->rdOffset + (UINT32)offset) >= pBuf->len))
		{
			offset -= (INT32)(pBuf->len - pBuf->rdOffset);
			pBuf->rdOffset = 0;
			pBuf->flags &= ~CIRCULAR_BUF_IS_FULL;
		}
		max = (INT32)((pBuf->flags & CIRCULAR_BUF_IS_FULL)? pBuf->len : (pBuf->wrOffset - pBuf->rdOffset));
		pBuf->rdOffset += (UINT32)MIN(offset, max);
		pBuf->flags &= ~CIRCULAR_BUF_IS_FULL;
	}
	else if (!(pBuf->flags & CIRCULAR_BUF_IS_FULL))
	{
		offset = (offset == INT32_MIN)? INT32_MAX : -offset;
		if (pBuf->rdOffset >= pBuf->wrOffset)
		{
			max = (INT32)(pBuf->rdOffset - pBuf->wrOffset);
		}
		else if ((UINT32)offset > pBuf->rdOffset)
		{
			max = (INT32)(pBuf->len - pBuf->wrOffset);
			offset -= (INT32)pBuf->rdOffset;
			pBuf->rdOffset = pBuf->len;
		}
		else
		{
			max = offset;
		}
		pBuf->rdOffset -= (UINT32)MIN(offset, max);
		if (pBuf->rdOffset == pBuf->wrOffset)
			pBuf->flags |= CIRCULAR_BUF_IS_FULL;
	}
}

void CBufFlush(CIRCULAR_BUF *pBuf)
{
	pBuf->rdOffset = pBuf->wrOffset;
	pBuf->flags &= ~CIRCULAR_BUF_IS_FULL;
}


UINT32 CBufPuts(CIRCULAR_BUF *pBuf, char const *pSrc)
{
	return CBufWrite(pBuf, pSrc, strlen(pSrc)) + CBufPutc(pBuf, '\n');
}

UINT32 CBufPutc(CIRCULAR_BUF *pBuf, char ch)
{
	return CBufWrite(pBuf, &ch, sizeof(ch));
}

UINT32 CBufWrite(CIRCULAR_BUF *pBuf, void const *pSrc, UINT32 len)
{
	UINT32 wrLen = 0;
	UINT32 blockLen;

	if ((len > pBuf->len) && !(pBuf->flags & CIRCULAR_BUF_WRITE_UNTIL_FULL))
	{
		wrLen = (UINT32)(len - pBuf->len);
		pSrc = (UINT8 const *)pSrc + wrLen;	//lint !e826
		len = pBuf->len;
	}

	while (len > 0)
	{
		if ((pBuf->flags & (CIRCULAR_BUF_IS_FULL | CIRCULAR_BUF_WRITE_UNTIL_FULL)) ==
			  (CIRCULAR_BUF_IS_FULL | CIRCULAR_BUF_WRITE_UNTIL_FULL))
		{
			break;
		}

		blockLen = (pBuf->rdOffset > pBuf->wrOffset)? pBuf->rdOffset : pBuf->len;
		blockLen -= pBuf->wrOffset;
		blockLen = MIN(len, blockLen);
		memcpy(&pBuf->pStart[pBuf->wrOffset], pSrc, blockLen);

		wrLen += blockLen;
		len -= blockLen;
		pSrc = (UINT8 const *)pSrc + blockLen;	//lint !e826
		pBuf->wrOffset += blockLen;
		if (pBuf->wrOffset == pBuf->len)
			pBuf->wrOffset = 0;
		if (pBuf->wrOffset == pBuf->rdOffset)
			pBuf->flags |= CIRCULAR_BUF_IS_FULL;
	}
	return wrLen;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/

