/*******************************************************************************************
 Generic Utilities

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define  NUM_HRS_PER_DAY	(24uL)

#define  NUM_MIN_PER_HOUR	(60uL)
#define  NUM_MIN_PER_DAY	(NUM_MIN_PER_HOUR  * NUM_HRS_PER_DAY)

#define  NUM_SEC_PER_MIN	(60uL)
#define  NUM_SEC_PER_HOUR	(NUM_SEC_PER_MIN * NUM_MIN_PER_HOUR)
#define  NUM_SEC_PER_DAY	(NUM_SEC_PER_HOUR  * NUM_HRS_PER_DAY)

#define  NUM_MS_PER_SEC		(1000uL)
#define  NUM_US_PER_SEC		(1000000uL)
#define  NUM_NS_PER_SEC		(1000000000uL)

#define  MS_TO_TICKS_RATE(ms,rateMs) ((u32)((u32)(ms) / (u32)(rateMs)))

#define	_PI					((FP32)3.141592654f)


/**************************************************************************
 *                                  Pre-processor Macros
 **************************************************************************/

// Misc:
#define MEMBER_SIZE(type, field)	(sizeof(((type *)0)->field))
#define NELEMENTS(array)  			(sizeof (array) / sizeof (array[0]))

#define QUOTE_VAR_NAME(x) 	#x
#define QUOTE_VAR_VALUE(x) QUOTE_VAR_NAME(x)

#ifndef ASSERT_ACTIVE
#define ASSERT_ACTIVE		(1)
#endif
#if ASSERT_ACTIVE
#define ASSERT(x)				{if (!(x))	return;}
#define ASSERT_RET(x, ret)	{if (!(x))	return (ret);}
#else
#define ASSERT(x)
#define ASSERT_RET(x, ret)
#endif


// Math-related:
#ifndef MAX
#define MAX(x, y)		( ((x) > (y))? (x) : (y) )
#endif
#ifndef MIN
#define MIN(x, y)		( ((x) < (y))? (x) : (y) )
#endif
#ifndef ABS
#define ABS(x)			( ((x) < 0)? -(x) : (x) )
#endif
#ifndef ABS_DIF
#define ABS_DIF(x,y)	( ((x) >= (y))? ((x) - (y)) : ((y) - (x)) )
#endif
#define CLAMP(x,low,high)	(((x) > (high))? (high) : (((x) < (low))? (low) : (x)))
#define IN_RANGE(x, low, high)      (((x) >= (low)) && ((x) <= (high)))

#define DOUBLE(x)		( 2 * (x) )
#define SQUARE(x)		( (x) * (x) )
#define CUBE(x)		( (x) * (x) * (x) )


// Binary helpers:
#define NUM_BYTES_FOR_BITS(sizeType, numBits)	((numBits + ((sizeType * 8)-1)) / (sizeType * 8))
#define NUM_TO_BIT_ARR_INDEX(arr, num)	((num) >> (3 * sizeof(arr[0])))			// divide by 8
#define NUM_TO_BIT(arr, num)				(1u << ((num) & (sizeof(arr[0] * 8) - 1)))
#define NUM_TO_BIT_SET(arr, num)			{arr[NUM_TO_BIT_ARR_INDEX(arr, num)] |=  NUM_TO_BIT(arr, num);}
#define NUM_TO_BIT_CLR(arr, num)			{arr[NUM_TO_BIT_ARR_INDEX(arr, num)] &= ~NUM_TO_BIT(arr, num);}
#define NUM_TO_BIT_TST(arr, num)			(arr[NUM_TO_BIT_ARR_INDEX(arr, num)] &   NUM_TO_BIT(arr, num))
#define BIT_TO_NUM(arr, index, bit)		(((arrIndex) * (sizeof(arr[0]) * 8)) + (bit))

//lint -emacro((572,778), NEXT_POWER_OF_2)
#define NEXT_POWER_OF_2(x) (NEXT_B32(x-1) + 1)
// Example usage: bytesRequired = NEXT_POWER_OF_2(115)	// 128

#define _BIT(n)							(1U << (n))
#define _BIT_SET(var, mask)				{ (var) |=  (mask); }
#define _BIT_CLEAR(var,mask)				{ (var) &= ~(mask); }
#define _BIT_IS_SET(var, mask)			((((var) & (mask)) == (mask)) ? (TRUE) : (FALSE))
#define _BIT_IS_CLEAR(var, mask)			(((var) & (mask)) ? (FALSE) : (TRUE))
#define _BIT_IS_ANY_SET(var, mask)		(((var) & (mask)) ? (TRUE) : (FALSE))
#define _BIT_IS_ANY_CLEAR(var, mask)	((((var) & (mask)) != (mask)) ? (TRUE) : (FALSE))
#define _BIT_MASK(_BIT)						((_BIT) - 1)
#define COMBINE_NIBBLES(upper, lower)	( ((upper) << 4) | ((lower) & 0x0f) )

#define B8(d) 							((UINT8)B8__(HEX__(d)))
#define B16(dmsb,dlsb)				(((UINT16)B8(dmsb)<<8) + B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) 	(((unsigned long)B8(dmsb)<<24) \
											+ ((unsigned long)B8(db2)<<16) \
											+ ((unsigned long)B8(db3)<<8) \
											+ B8(dlsb))
// Example usage of B8, B16 and B32:
// B8(01010101) // 85
// B16(10101010,01010101) // 43,605
// B32(10000000,11111111,10101010,01010101) // 2,164,238,933
// reg = ( (B8(010) << 5) | (B8(11) << 3) | (B8(101) << 0) )


/*************************************************************************
*                                   ENDIAN
**************************************************************************/
#define RD_LE16(p)	((UINT16)(((UINT16)((p)[1])<<8) + (UINT16)((p)[0])))
#define WR_LE16(p,v)	{ (p)[0]=(UINT8)(v&0xff); \
								(p)[1]=(UINT8)((v>>8)&0xff); }

#define RD_BE16(p)		(UINT16)(((UINT16)((p)[0])<<8) + (UINT16)((p)[1]))
#define WR_BE16(p,v)	{ (p)[1]=(UINT8)(v&0xff); \
							(p)[0]=(UINT8)((v>>8)&0xff); }

#define RD_LE24(p)	((UINT32)(((UINT32)((p)[2])<<16) + ((UINT32)((p)[1])<<8) + (UINT32)((p)[0])))
#define WR_LE24(p,v)	{(p)[0]=(UINT8)(v&0xff); \
						 (p)[1]=(UINT8)((v>>8)&0xff); \
						 (p)[2]=(UINT8)((v>>16)&0xff); }

#define RD_BE24(p)	((UINT32)(((UINT32)((p)[0])<<16) + ((UINT32)((p)[1])<<8) + (UINT32)((p)[2])))
#define WR_BE24(p,v)	{(p)[2]=(UINT8)(v&0xff); \
						 (p)[1]=(UINT8)((v>>8)&0xff); \
						 (p)[0]=(UINT8)((v>>16)&0xff); }

#define RD_LE32(p)	((UINT32)(((UINT32)((p)[3])<<24) + ((UINT32)((p)[2])<<16) + \
							  ((UINT32)((p)[1])<<8)  + (UINT32)((p)[0])))
#define WR_LE32(p,v)	{(p)[0]=(UINT8)(v&0xff); \
						 (p)[1]=(UINT8)((v>>8)&0xff); \
						 (p)[2]=(UINT8)((v>>16)&0xff); \
						 (p)[3]=(UINT8)((v>>24)&0xff); }

#define RD_BE32(p)	((UINT32)(((UINT32)((p)[0])<<24) + ((UINT32)((p)[1])<<16) + \
							  ((UINT32)((p)[2])<<8)  + (UINT32)((p)[3])))
#define WR_BE32(p,v)	{(p)[3]=(UINT8)(v&0xff); \
						 (p)[2]=(UINT8)((v>>8)&0xff); \
						 (p)[1]=(UINT8)((v>>16)&0xff); \
						 (p)[0]=(UINT8)((v>>24)&0xff); }


/**************************************************************************
 *                                  "Private" helpers to the macros
 **************************************************************************/
#define NEXT_B2(x)	( (x) | ( (x) >> 1) )
#define NEXT_B4(x)	( NEXT_B2(x) | ( NEXT_B2(x) >> 2) )
#define NEXT_B8(x)	( NEXT_B4(x) | ( NEXT_B4(x) >> 4) )
#define NEXT_B16(x)	( NEXT_B8(x) | ( NEXT_B8(x) >> 8) )
#define NEXT_B32(x)	(NEXT_B16(x) | (NEXT_B16(x) >>16) )

#define HEX__(n)	0x##n##LU
#define B8__(x)	((x&0x0000000FLU)?1:0) \
						+((x&0x000000F0LU)?2:0) \
						+((x&0x00000F00LU)?4:0) \
						+((x&0x0000F000LU)?8:0) \
						+((x&0x000F0000LU)?16:0) \
						+((x&0x00F00000LU)?32:0) \
						+((x&0x0F000000LU)?64:0) \
						+((x&0xF0000000LU)?128:0)


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
BOOL CpuIsLittleEndian(void);

INT32 antoi(char const *pStr, UINT32 maxChars);
UINT32 GetStringIndexFromTable (char const *const *ppTable, char const *pSearchStr, UINT32 maxIndex);

int stricmp(char const *pStr1, char const *pStr2);
int strnicmp(char const *pStr1, char const *pStr2, UINT32 num);
char * stristr(char *pStr1, char *pStr2);
char const * strTrimFront(char const *pStr);		// Removes leading whitespace
char * strTrimTail(char *pStr);	//lint -esym(534, strTrimTail)
char * strTrim(char *pStr);
char * Strxtoa(UINT32 v,char *pStr, INT32 r, INT32 isNeg);
char * Stritoa(INT32 v, char *pStr, INT32 r);
void strncatf(char *pDest, UINT32 maxLen, char const *pFormat, ...);

UINT32 aton(char const *pAddr);
char const * ntoa(UINT32 addr);
void ntoa_r(char *pDest, UINT32 addr);
void macToa(char *pDest, char const *pMac);
void atoMac(char *pDest, char const *pMac);


UINT32 BitReverseWord(UINT32 x);

// "maxUlps" is the "Units in the Last Place", which specifies how big an error the caller is willing
// to accept in terms of the value of the least significant digits of the FP number's representation.
// AKA how many representable floats we are willing to accept between A and B
BOOL Fp32AlmostEqual(FP32 A, FP32 B, INT32 maxUlps);

#endif
