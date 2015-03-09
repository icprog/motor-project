/*******************************************************************************************
UNIT Test program

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "includes.h"
#include "embUnit/embUnit.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define AS_PROTOTYPES(a)   extern TestRef a(void);
#define AS_TEST(a)         TestRunner_runTest(a());

#define UNIT_TESTS(_)   \
_(CBufTests)            \
_(ItemQFifoTestsLen1)   \
_(ItemQLifoTestsLen1)   \
_(ItemQFifoTestsLen2)   \
_(ItemQLifoTestsLen2)   \
_(ItemQFifoTestsLen3)   \
_(ItemQLifoTestsLen3)   \
_(ItemQFifoTestsLen4)   \
_(ItemQLifoTestsLen4)   \


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Variables
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
UNIT_TESTS(AS_PROTOTYPES)


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
/**************************************************************************
 *                                 Functions
 **************************************************************************/
int  main(void)
{
	TestRunner_start();
	{
      UNIT_TESTS(AS_TEST)
	}

	return TestRunner_end();
}
