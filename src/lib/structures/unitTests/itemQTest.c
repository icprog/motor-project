/*******************************************************************************************
Tests the Circular Buffer code

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "embUnit/embUnit.h"
#include "includes.h"
#include "itemQ.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static ITEM_Q itemQ;
static u8 qMem[32];
static u8 wrVals[64];
static u8 rdVals[64];


/**************************************************************************
 *                                  Setup/Teardown
 **************************************************************************/
static void ItemQTestSetup(ITEM_Q_ORDER order, u8 itemLenBytes)
{
   memset(&itemQ, 0xAA, sizeof(itemQ));
   for (u8f i = 0; i < sizeof(wrVals); i++)
   {
      wrVals[i] = (u8)i;
   }
   memset(rdVals, 0xFF, sizeof(rdVals));
   ItemQInit(&itemQ, qMem, sizeof(qMem), order, itemLenBytes);
}

static void ItemQFifoLen1TestSetup(void)     {ItemQTestSetup(ITEM_Q_FIFO, 1);}
static void ItemQFifoLen2TestSetup(void)     {ItemQTestSetup(ITEM_Q_FIFO, 2);}
static void ItemQFifoLen3TestSetup(void)     {ItemQTestSetup(ITEM_Q_FIFO, 3);}
static void ItemQFifoLen4TestSetup(void)     {ItemQTestSetup(ITEM_Q_FIFO, 4);}
static void ItemQLifoLen1TestSetup(void)     {ItemQTestSetup(ITEM_Q_LIFO, 1);}
static void ItemQLifoLen2TestSetup(void)     {ItemQTestSetup(ITEM_Q_LIFO, 2);}
static void ItemQLifoLen3TestSetup(void)     {ItemQTestSetup(ITEM_Q_LIFO, 3);}
static void ItemQLifoLen4TestSetup(void)     {ItemQTestSetup(ITEM_Q_LIFO, 4);}
static void ItemQFifoLen1TestTeardown(void)  {}
static void ItemQFifoLen2TestTeardown(void)  {}
static void ItemQFifoLen3TestTeardown(void)  {}
static void ItemQFifoLen4TestTeardown(void)  {}
static void ItemQLifoLen1TestTeardown(void)  {}
static void ItemQLifoLen2TestTeardown(void)  {}
static void ItemQLifoLen3TestTeardown(void)  {}
static void ItemQLifoLen4TestTeardown(void)  {}


/**************************************************************************
 *                                  Test Functions
 **************************************************************************/
static void ItemQTestInit(void)
{
	TEST_ASSERT_EQUAL_INT(sizeof(qMem), itemQ.memSize);
	TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
	TEST_ASSERT_EQUAL_INT(sizeof(qMem) - itemQ.itemLenBytes, ItemQBytesFree(&itemQ));
}

static void ItemQTestOnePushPop(void)
{
   ItemQPush(&itemQ, wrVals, 10);
   TEST_ASSERT_EQUAL_INT(10+itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize - 10 - (2*itemQ.itemLenBytes), ItemQBytesFree(&itemQ));
   u32 itemLen = ItemQPop(rdVals, 10, &itemQ);
   TEST_ASSERT_EQUAL_INT(10, itemLen);
   TEST_ASSERT_EQUAL_BUF(wrVals, rdVals, 10);
   for (u8 i = 0; i < sizeof(rdVals); i++)
   {
      TEST_ASSERT_EQUAL_INT(i, wrVals[i]);
      if (i < 10)
      {
         TEST_ASSERT_EQUAL_INT(i, rdVals[i]);
      }
      else
      {
         TEST_ASSERT_EQUAL_INT(0xFF, rdVals[i]);
      }
   }
}

static void ItemQTestExactlyFull(void)
{
   ItemQPush(&itemQ, wrVals, itemQ.memSize - itemQ.itemLenBytes);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesFree(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize, ItemQBytesUsed(&itemQ));

   ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   ItemQPush(&itemQ, wrVals, itemQ.memSize - (2*itemQ.itemLenBytes));
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesFree(&itemQ));
   TEST_ASSERT_EQUAL_INT(true, ItemQIsFull(&itemQ));
}

static void ItemQTestPushLeavesDataAlone(void)
{
   ItemQPush(&itemQ, wrVals, 10);
   TEST_ASSERT_EQUAL_INT(10+itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   ItemQPush(&itemQ, wrVals, itemQ.memSize);
   TEST_ASSERT_EQUAL_INT(10+itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   u32 len = ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   TEST_ASSERT_EQUAL_INT(10, len);
   TEST_ASSERT_EQUAL_BUF(wrVals, rdVals, 10);
}

static void ItemQTestPushTooBig(void)
{
   ItemQPush(&itemQ, wrVals, itemQ.memSize);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
}

static void ItemQTestPushEmpty(void)
{
   ItemQPush(&itemQ, wrVals, 0);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
}

static void ItemQTestPopTooBig(void)
{
   ItemQPush(&itemQ, wrVals, 10);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   u32 len = ItemQPop(rdVals, 0, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   len = ItemQPop(rdVals, 1, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   len = ItemQPop(rdVals, 9, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
}

static void ItemQTestPopsLeaveDataAlone(void)
{
   ItemQPush(&itemQ, wrVals, 10);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   u32 len = ItemQPop(rdVals, 5, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   for (u8 i = 0; i < sizeof(rdVals); i++)
   {
      TEST_ASSERT_EQUAL_INT(0xFF, rdVals[i]);
   }

   len = ItemQPop(rdVals, 5, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   len = ItemQPop(rdVals, 5, &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   len = ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   TEST_ASSERT_EQUAL_INT(10, len);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_BUF(wrVals, rdVals, 10);
}

static void ItemQTestPopEmpty(void)
{
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
   u32 len = ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   TEST_ASSERT_EQUAL_INT(0, len);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
}

static void ItemQTestOverwrite(void)
{
   ItemQPush(&itemQ, wrVals, 10);
   TEST_ASSERT_EQUAL_INT(10 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));

   ItemQPush(&itemQ, &wrVals[10], itemQ.memSize - itemQ.itemLenBytes);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesFree(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize, ItemQBytesUsed(&itemQ));

   u32 len = ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   TEST_ASSERT_EQUAL_INT(len, itemQ.memSize - itemQ.itemLenBytes);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize - itemQ.itemLenBytes, ItemQBytesFree(&itemQ));
}

static void ItemQTestItemCountAndOrder(void)
{
   // fill with items
   u32 bytesUsed = 0;
   for (u8 i = 0; i < (itemQ.memSize - itemQ.itemLenBytes); i += (1 + itemQ.itemLenBytes))
   {
      ItemQPush(&itemQ, &wrVals[i], 1);
      bytesUsed += itemQ.itemLenBytes + 1;
      TEST_ASSERT_EQUAL_INT(bytesUsed, ItemQBytesUsed(&itemQ));
   }

   // Read back
   for (u8 i = 0; i < (itemQ.memSize - itemQ.itemLenBytes); i += (1 + itemQ.itemLenBytes))
   {
      u32 len = ItemQPop(rdVals, 1, &itemQ);
      bytesUsed -= (len + itemQ.itemLenBytes);
      TEST_ASSERT_EQUAL_INT(1, len);
      TEST_ASSERT_EQUAL_INT(bytesUsed, ItemQBytesUsed(&itemQ));
      for (u8 i = 1; i < sizeof(rdVals); i++)
      {
         TEST_ASSERT_EQUAL_INT(0xFF, rdVals[i]);
      }
      if (itemQ.order == ITEM_Q_FIFO)
      {
         TEST_ASSERT_EQUAL_INT(wrVals[i], rdVals[0]);
      }
      else
      {
         TEST_ASSERT_EQUAL_INT(wrVals[bytesUsed], rdVals[0]);
      }
   }
}

static void ItemQTestFlush(void)
{
   ItemQPush(&itemQ, wrVals, 2);
   TEST_ASSERT_EQUAL_INT(2 + itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize - 2 - (2*itemQ.itemLenBytes), ItemQBytesFree(&itemQ));

   ItemQFlush(&itemQ);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(itemQ.memSize - itemQ.itemLenBytes, ItemQBytesFree(&itemQ));
}

static void ItemQTestPushPullWrapAround(void)
{
   for (u8 wrSize = 1; wrSize < 5; wrSize++)
   {
      for (u8 i = 0; i < sizeof(wrVals) - wrSize; i++)
      {
         ItemQPush(&itemQ, &wrVals[i], wrSize);
         u32 len = ItemQPop(rdVals, sizeof(rdVals), &itemQ);
         TEST_ASSERT_EQUAL_INT(wrSize, len);
         TEST_ASSERT_EQUAL_BUF(&wrVals[i], rdVals, len);
      }
   }
}

static void ItemQTestEmptyFull(void)
{
   TEST_ASSERT_EQUAL_INT(true, ItemQIsEmpty(&itemQ));
   TEST_ASSERT_EQUAL_INT(false, ItemQIsFull(&itemQ));

   ItemQPush(&itemQ, wrVals, 2);
   TEST_ASSERT_EQUAL_INT(false, ItemQIsEmpty(&itemQ));
   TEST_ASSERT_EQUAL_INT(false, ItemQIsFull(&itemQ));

   ItemQPop(rdVals, 2, &itemQ);
   TEST_ASSERT_EQUAL_INT(true, ItemQIsEmpty(&itemQ));
   TEST_ASSERT_EQUAL_INT(false, ItemQIsFull(&itemQ));

   ItemQPush(&itemQ, wrVals, itemQ.memSize - itemQ.itemLenBytes);
   TEST_ASSERT_EQUAL_INT(false, ItemQIsEmpty(&itemQ));
   TEST_ASSERT_EQUAL_INT(true, ItemQIsFull(&itemQ));

   ItemQPop(rdVals, sizeof(rdVals), &itemQ);
   TEST_ASSERT_EQUAL_INT(true, ItemQIsEmpty(&itemQ));

   ItemQPush(&itemQ, wrVals, itemQ.memSize - (2*itemQ.itemLenBytes));
   u32 len = ItemQBytesFree(&itemQ);
   TEST_ASSERT_EQUAL_INT(false, ItemQIsEmpty(&itemQ));
   TEST_ASSERT_EQUAL_INT(true, ItemQIsFull(&itemQ));
}

static void ItemQTestPeek(void)
{
   ItemQPush(&itemQ, wrVals, 2);
   TEST_ASSERT_EQUAL_INT(2+itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   u32 itemLen = ItemQPeek(rdVals, 2, &itemQ);
   TEST_ASSERT_EQUAL_INT(2, itemLen);
   TEST_ASSERT_EQUAL_BUF(wrVals, rdVals, 2);
   TEST_ASSERT_EQUAL_INT(2+itemQ.itemLenBytes, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(false, ItemQIsEmpty(&itemQ));

   memset(rdVals, 0, sizeof(rdVals));
   itemLen = ItemQPop(rdVals, 2, &itemQ);
   TEST_ASSERT_EQUAL_INT(2, itemLen);
   TEST_ASSERT_EQUAL_BUF(wrVals, rdVals, 2);
   TEST_ASSERT_EQUAL_INT(0, ItemQBytesUsed(&itemQ));
   TEST_ASSERT_EQUAL_INT(true, ItemQIsEmpty(&itemQ));
}


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
static const TestFixture fixtures[] =
{
   new_TestFixtureFn(ItemQTestInit),
   new_TestFixtureFn(ItemQTestOnePushPop),
   new_TestFixtureFn(ItemQTestExactlyFull),
   new_TestFixtureFn(ItemQTestPushLeavesDataAlone),
   new_TestFixtureFn(ItemQTestPushTooBig),
   new_TestFixtureFn(ItemQTestPushEmpty),
   new_TestFixtureFn(ItemQTestPopsLeaveDataAlone),
   new_TestFixtureFn(ItemQTestPopTooBig),
   new_TestFixtureFn(ItemQTestPopEmpty),
   new_TestFixtureFn(ItemQTestOverwrite),
   new_TestFixtureFn(ItemQTestItemCountAndOrder),
   new_TestFixtureFn(ItemQTestFlush),
   new_TestFixtureFn(ItemQTestPushPullWrapAround),
   new_TestFixtureFn(ItemQTestEmptyFull),
   new_TestFixtureFn(ItemQTestPeek),
};

TestRef ItemQFifoTestsLen1(void) { EMB_UNIT_TEST_CALLER(ItemQFifoLen1Test); }
TestRef ItemQFifoTestsLen2(void) { EMB_UNIT_TEST_CALLER(ItemQFifoLen2Test); }
TestRef ItemQFifoTestsLen3(void) { EMB_UNIT_TEST_CALLER(ItemQFifoLen3Test); }
TestRef ItemQFifoTestsLen4(void) { EMB_UNIT_TEST_CALLER(ItemQFifoLen4Test); }
TestRef ItemQLifoTestsLen1(void) { EMB_UNIT_TEST_CALLER(ItemQLifoLen1Test); }
TestRef ItemQLifoTestsLen2(void) { EMB_UNIT_TEST_CALLER(ItemQLifoLen2Test); }
TestRef ItemQLifoTestsLen3(void) { EMB_UNIT_TEST_CALLER(ItemQLifoLen3Test); }
TestRef ItemQLifoTestsLen4(void) { EMB_UNIT_TEST_CALLER(ItemQLifoLen4Test); }

