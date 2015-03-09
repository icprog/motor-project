/*******************************************************************************************
Item Queue for storing variable-sized items (FIFO or LIFO) in a circular queue

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __ITEMQ_H__
#define __ITEMQ_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   ITEM_Q_FIFO,
   ITEM_Q_LIFO
}ITEM_Q_ORDER;

typedef struct
{
   void *   pMem;
   u32      memSize;
   u32      headIndex;
   u32      tailIndex;
   ITEM_Q_ORDER   order;
   u8       itemLenBytes;
   bool     isFull;
}ITEM_Q;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void ItemQInit(ITEM_Q *pQ, void *pMem, u32 memSize, ITEM_Q_ORDER order, u8 itemLenBytes);
void ItemQFlush(ITEM_Q *pQ);

u32 ItemQBytesUsed(ITEM_Q const *pQ);
u32 ItemQBytesFree(ITEM_Q const *pQ);
bool ItemQIsEmpty(ITEM_Q const *pQ);
bool ItemQIsFull(ITEM_Q const *pQ);

void ItemQPush(ITEM_Q *pQ, void const *pSrc, u32 len);
u32 ItemQPop(void *pDest, u32 maxLen, ITEM_Q *pQ);
u32 ItemQPeek(void *pDest, u32 maxLen, ITEM_Q *pQ);

#endif
