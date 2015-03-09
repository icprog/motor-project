/*******************************************************************************************
 Circular Buffer

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
// Circular buffer options
#define CIRCULAR_BUF_WRITE_UNTIL_FULL	(BIT0)	// else overwrite oldest
#define CIRCULAR_BUF_OPTION_MASK			(BIT0)

#define CIRCULAR_BUF_IS_FULL				(BIT7)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
	UINT8	*pStart;
	UINT32 rdOffset;
	UINT32 wrOffset;
	UINT32 len;
	UINT8	flags;
} CIRCULAR_BUF;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
// NOTE: This code does not use malloc, so the caller is responsible for allocating all memory.
//			The 'init' routine will just set all arguments into the CIRCULAR_BUF structure
//lint -esym(534, CBufInit, CBufGets)
CIRCULAR_BUF const * CBufInit(CIRCULAR_BUF *pBuf, UINT8 *pStart, UINT32 len, UINT8 optionFlags);
UINT32	CBufFree(CIRCULAR_BUF const *pBuf);
UINT32	CBufUsed(CIRCULAR_BUF const *pBuf);
BOOL		CBufIsEmpty(CIRCULAR_BUF const *pBuf);
BOOL		CBufIsFull(CIRCULAR_BUF const *pBuf);

char *	CBufGets(char *pDest, UINT32 len, CIRCULAR_BUF *pBuf);
INT32		CBufGetc(CIRCULAR_BUF *pBuf);
UINT32	CBufRead(void *pDest, UINT32 len, CIRCULAR_BUF *pBuf);
UINT32	CBufPeek(void *pDest, UINT32 len, CIRCULAR_BUF *pBuf);
void		CBufSeek(CIRCULAR_BUF *pBuf, INT32 offset);
void		CBufFlush(CIRCULAR_BUF *pBuf);

UINT32	CBufPuts(CIRCULAR_BUF *pBuf, char const *pSrc);
UINT32	CBufPutc(CIRCULAR_BUF *pBuf, char ch);
UINT32	CBufWrite(CIRCULAR_BUF *pBuf, void const *pSrc, UINT32 len);

#endif
