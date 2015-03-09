/*******************************************************************************************
Tests the Circular Buffer code

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "embUnit/embUnit.h"
#include "includes.h"
#include "circularBuffer.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
static CIRCULAR_BUF cbuf;
static UINT8 ctestArr[10];


/**************************************************************************
 *                                  Setup/Teardown
 **************************************************************************/
static void CBufTestSetup(void)
{
	memset(&cbuf, 0xAA, sizeof(cbuf));
	CBufInit(&cbuf, ctestArr, sizeof(ctestArr), 0);
}

static void CBufTestTeardown(void)
{
}


/**************************************************************************
 *                                  Test Functions
 **************************************************************************/
static void CBufTestInit(void)
{
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), cbuf.len);
	TEST_ASSERT_EQUAL_INT(0, cbuf.flags);
	TEST_ASSERT_EQUAL_INT(0, cbuf.rdOffset);
	TEST_ASSERT_EQUAL_INT(0, cbuf.wrOffset);
	TEST_ASSERT(ctestArr == cbuf.pStart);
}

static void CBufTestUsedFreeEmpty(void)
{
	UINT32 ret;
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

static void CBufTestWriteUntilFull(void)
{
	UINT8 wr[sizeof(ctestArr)+1];
	UINT8 rd[sizeof(ctestArr)];
	UINT32 ret;
	CBufInit(&cbuf, ctestArr, sizeof(ctestArr), CIRCULAR_BUF_WRITE_UNTIL_FULL);
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	TEST_ASSERT(CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);

	// Now ensure that it doesn't keep writing
	memset(wr, 0x11, sizeof(wr));
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT_EQUAL_INT(0, ret);
	TEST_ASSERT(CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);

	// Now verify that we read out the first bytes
	memset(rd, 0x22, sizeof(rd));
	ret = CBufRead(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(rd), ret);
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	TEST_ASSERT_EQUAL_BUF(wr, rd, sizeof(ctestArr));
	TEST_ASSERT(!CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

static void CBufTestWritePastFull(void)
{
	UINT8 wr[sizeof(ctestArr) + 4];
	UINT8 rd[sizeof(ctestArr)];
	UINT32 ret;
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT_EQUAL_INT(sizeof(wr), ret);
	TEST_ASSERT(CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);

	memset(rd, 0xAA, sizeof(rd));
	ret = CBufRead(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(rd), ret);
	TEST_ASSERT_EQUAL_BUF(&wr[sizeof(wr)-sizeof(rd)], rd, sizeof(rd));
	TEST_ASSERT(!CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
}

static void CBufTestPeek(void)
{
	UINT8 wr[sizeof(ctestArr)];
	UINT8 rd[sizeof(ctestArr)];
	UINT32 ret;
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	ret = CBufPeek(rd, 3, &cbuf);
	TEST_ASSERT_EQUAL_INT(3, ret);
	TEST_ASSERT_EQUAL_BUF(wr, rd, 3);
	TEST_ASSERT_EQUAL_INT(0, cbuf.rdOffset);
	TEST_ASSERT(CBufIsFull(&cbuf));
	ret = CBufRead(rd, 3, &cbuf);
	TEST_ASSERT_EQUAL_INT(3, ret);
	TEST_ASSERT_EQUAL_BUF(wr, rd, 3);
}

static void CBufTestSeekPos(void)
{
	UINT8 wr[sizeof(ctestArr)];
	UINT8 rd[sizeof(ctestArr)];
	UINT32 ret;
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT(CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);

	CBufSeek(&cbuf, 3);
	TEST_ASSERT(!CBufIsFull(&cbuf));
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(3, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr)-3, ret);

	ret = CBufRead(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr)-3, ret);
	TEST_ASSERT_EQUAL_BUF(&wr[3], rd, ret);
}

static void CBufTestSeekNeg(void)
{
	UINT8 wr[sizeof(ctestArr)];
	UINT8 rd[sizeof(ctestArr)];
	UINT32 ret;
	for (UINT8 i = 0; i < sizeof(wr); i++)
		wr[i] = i;
	ret = CBufWrite(&cbuf, wr, sizeof(wr));
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), ret);
	ret = CBufRead(rd, 3, &cbuf);
	TEST_ASSERT_EQUAL_INT(3, ret);
	TEST_ASSERT_EQUAL_BUF(wr, rd, 3);
	ret = CBufRead(rd, 3, &cbuf);
	TEST_ASSERT_EQUAL_INT(3, ret);
	TEST_ASSERT_EQUAL_BUF(&wr[3], rd, 3);

	CBufSeek(&cbuf, -4);
	ret = CBufFree(&cbuf);
	TEST_ASSERT_EQUAL_INT(2, ret);
	ret = CBufUsed(&cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(wr)-2, ret);

	ret = CBufRead(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_EQUAL_INT(sizeof(wr)-2, ret);
	TEST_ASSERT_EQUAL_BUF(&wr[2], rd, ret);
}

static void CBufTestFlush(void)
{
	UINT32 ret;
	ret = CBufPuts(&cbuf, "123");
	TEST_ASSERT_EQUAL_INT(4, ret);
	TEST_ASSERT_EQUAL_INT(4, CBufUsed(&cbuf));
	CBufFlush(&cbuf);
	TEST_ASSERT_EQUAL_INT(0, CBufUsed(&cbuf));
	TEST_ASSERT_EQUAL_INT(sizeof(ctestArr), CBufFree(&cbuf));
	TEST_ASSERT_EQUAL_INT(4, cbuf.wrOffset);
	TEST_ASSERT_EQUAL_INT(4, cbuf.rdOffset);
}

static void CBufTestPutsGets(void)
{
	char rd[sizeof(ctestArr)];
	UINT32 ret;
	char *pStr;
	char *pStr1 = "abcd";
	char *pStr2 = "123";

	ret = CBufPuts(&cbuf, pStr1);
	TEST_ASSERT_EQUAL_INT(strlen(pStr1)+1, ret);
	TEST_ASSERT_EQUAL_INT(strlen(pStr1)+1, CBufUsed(&cbuf));
	ret = CBufPuts(&cbuf, pStr2);
	TEST_ASSERT_EQUAL_INT(strlen(pStr2)+1, ret);
	TEST_ASSERT_EQUAL_INT(strlen(pStr1)+strlen(pStr2)+2, CBufUsed(&cbuf));

	pStr = CBufGets(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_NOT_NULL(pStr);
	TEST_ASSERT_EQUAL_STRING(pStr1, rd);
	TEST_ASSERT_EQUAL_INT(strlen(pStr2)+1, CBufUsed(&cbuf));
	pStr = CBufGets(rd, sizeof(rd), &cbuf);
	TEST_ASSERT_NOT_NULL(pStr);
	TEST_ASSERT_EQUAL_STRING(pStr2, rd);
	TEST_ASSERT_EQUAL_INT(0, CBufUsed(&cbuf));
}


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
TestRef CBufTests(void)
{
	static const TestFixture fixtures[] =
	{
		new_TestFixtureFn(CBufTestInit),
		new_TestFixtureFn(CBufTestUsedFreeEmpty),
		new_TestFixtureFn(CBufTestWriteUntilFull),
		new_TestFixtureFn(CBufTestWritePastFull),
		new_TestFixtureFn(CBufTestPeek),
		new_TestFixtureFn(CBufTestSeekPos),
		new_TestFixtureFn(CBufTestSeekNeg),
		new_TestFixtureFn(CBufTestFlush),
		new_TestFixtureFn(CBufTestPutsGets),
	};
	EMB_UNIT_TEST_CALLER(CBufTest);
}



