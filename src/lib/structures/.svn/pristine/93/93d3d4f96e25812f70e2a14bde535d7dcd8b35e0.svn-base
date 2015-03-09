/*******************************************************************************************
Item Queue for storing variable-sized items (FIFO or LIFO) in a circular queue

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "itemQ.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define QIsFifo(pQ)    ((pQ)->order == ITEM_Q_FIFO)


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void ItemQPushBytes(ITEM_Q *pQ, void const *pSrc, u32 len);
static void ItemQIndexUpdateFromWrite(ITEM_Q *pQ, u32 wrLen);
static void ItemQPopBytes(void *pDest, u32 len, ITEM_Q *pQ);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void ItemQInit(ITEM_Q *pQ, void *pMem, u32 memSize, ITEM_Q_ORDER order, u8 itemLenBytes)
{
   pQ->pMem = pMem;
   pQ->memSize = memSize;
   pQ->order = order;
   pQ->itemLenBytes = itemLenBytes;
   ItemQFlush(pQ);
}

void ItemQFlush(ITEM_Q *pQ)
{
   pQ->headIndex = 0;
   pQ->tailIndex = 0;
   pQ->isFull = false;
}

u32 ItemQBytesUsed(ITEM_Q const *pQ)
{
   u32 bytesUsed;
   if (pQ->isFull)
   {
      bytesUsed = pQ->memSize;
   }
   else if (pQ->headIndex >= pQ->tailIndex)
   {
      bytesUsed = (u32)(pQ->headIndex - pQ->tailIndex);
   }
   else
   {
      bytesUsed = (u32)(pQ->memSize - pQ->tailIndex) + pQ->headIndex;
   }
   return bytesUsed;
}

u32 ItemQBytesFree(ITEM_Q const *pQ)
{
   u32 bytesFree;
   if (pQ->isFull)
   {
      bytesFree = 0;
   }
   else if (pQ->headIndex >= pQ->tailIndex)
   {
      bytesFree = (u32)(pQ->memSize - pQ->headIndex) + pQ->tailIndex;
   }
   else
   {
      bytesFree = (u32)(pQ->tailIndex - pQ->headIndex);
   }

   if (bytesFree >= pQ->itemLenBytes)
      bytesFree -= pQ->itemLenBytes;

   return bytesFree;
}

bool ItemQIsEmpty(ITEM_Q const *pQ)
{
   return (ItemQBytesFree(pQ) >= (u32)(pQ->memSize - pQ->itemLenBytes));
}

bool ItemQIsFull(ITEM_Q const *pQ)
{
   return (ItemQBytesFree(pQ) == 0);
}

void ItemQPush(ITEM_Q *pQ, void const *pSrc, u32 len)
{
   if ((len == 0) || (len > (pQ->memSize - pQ->itemLenBytes)))
      return;

   // Make room first (lock-free queue)
   while (ItemQBytesFree(pQ) < len)
   {
      ItemQPop(NULL, UINT32_MAX, pQ);  //lint !e534
   }

   if (QIsFifo(pQ))
   {
      // [len][data]
      ItemQPushBytes(pQ, &len, pQ->itemLenBytes);
      ItemQPushBytes(pQ, pSrc, len);
   }
   else
   {
      // [data][len]
      ItemQPushBytes(pQ, pSrc, len);
      ItemQPushBytes(pQ, &len, pQ->itemLenBytes);
   }
}

u32 ItemQPop(void *pDest, u32 maxLen, ITEM_Q *pQ)
{
   if ((maxLen == 0) || (ItemQBytesUsed(pQ) == 0))
      return 0;

   u32 oldHead = pQ->headIndex;
   u32 oldTail = pQ->tailIndex;
   bool oldFull = pQ->isFull;
   u32 len = 0;
   ItemQPopBytes(&len, pQ->itemLenBytes, pQ);
   if (len > maxLen)
   {
      pQ->headIndex = oldHead;
      pQ->tailIndex = oldTail;
      pQ->isFull = oldFull;
      len = 0;
   }
   else
   {
      ItemQPopBytes(pDest, len, pQ);
      pQ->isFull = false;
   }

   return len;
}

u32 ItemQPeek(void *pDest, u32 maxLen, ITEM_Q *pQ)
{
   u32  headIndex = pQ->headIndex;
   u32  tailIndex = pQ->tailIndex;
   bool isFull = pQ->isFull;

   u32 len = ItemQPop(pDest, maxLen, pQ);

   pQ->headIndex = headIndex;
   pQ->tailIndex = tailIndex;
   pQ->isFull = isFull;

   return len;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void ItemQPushBytes(ITEM_Q *pQ, void const *pSrc, u32 len)
{
   u32 wrLen = (u32)(pQ->memSize - pQ->headIndex);
   wrLen = MIN(wrLen, len);
   memcpy((u8 *)pQ->pMem + pQ->headIndex, pSrc, wrLen);
   ItemQIndexUpdateFromWrite(pQ, wrLen);

   if (wrLen != len)
   {
      pSrc = (u8 const *)pSrc + wrLen;
      wrLen = (u32)(len - wrLen);
      memcpy(pQ->pMem, pSrc, wrLen);
      ItemQIndexUpdateFromWrite(pQ, wrLen);
   }
}

static void ItemQIndexUpdateFromWrite(ITEM_Q *pQ, u32 wrLen)
{
   pQ->headIndex += wrLen;
   if (pQ->headIndex == pQ->memSize)
   {
      pQ->headIndex = 0;
   }
   if (pQ->headIndex == pQ->tailIndex)
   {
      pQ->isFull = true;
   }
}

static void ItemQPopBytes(void *pDest, u32 len, ITEM_Q *pQ)
{
   if (QIsFifo(pQ))
   {
      u32 rdLen = (u32)(pQ->memSize - pQ->tailIndex);
      rdLen = MIN(rdLen, len);
      if (pDest)
         memcpy(pDest, (u8 *)pQ->pMem + pQ->tailIndex, rdLen);
      pQ->tailIndex += rdLen;
      if (pQ->tailIndex == pQ->memSize)
         pQ->tailIndex = 0;

      if (rdLen != len)
      {
         if (pDest)
            pDest = (u8 *)pDest + rdLen;
         rdLen = (u32)(len - rdLen);
         if (pDest)
            memcpy(pDest, pQ->pMem, rdLen);
         pQ->tailIndex = rdLen;
      }
   }
   else
   {
      u32 rdLen = MIN(pQ->headIndex, len);
      pQ->headIndex -= rdLen;
      if (pDest)
         memcpy(pDest, (u8 *)pQ->pMem + pQ->headIndex, rdLen);

      if (rdLen != len)
      {
         if (pDest)
            pDest = (u8 *)pDest + rdLen;
         rdLen = (u32)(len - rdLen);
         pQ->headIndex = pQ->memSize - rdLen;
         if (pDest)
            memcpy(pDest, (u8 *)pQ->pMem + pQ->headIndex, rdLen);
      }
   }
}

