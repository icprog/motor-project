/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef FALSE
#define FALSE           ((bool)false)
#endif
#ifndef TRUE
#define TRUE            ((bool)true)
#endif

#ifndef NULL
#define NULL            ((void *) 0)
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
#ifndef __PFV__
#define __PFV__
typedef void (*PFV)(void);		// FN Pointer returning void
typedef int  (*PFI)(void);		// FN Pointer return int
#endif

// concise style:
typedef int8_t          s8;
typedef int16_t         s16;
typedef int32_t         s32;
typedef int64_t         s64;

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

typedef float           fp32;
typedef double          fp64;

typedef const uint32_t  cu32;
typedef const uint16_t  cu16;
typedef const uint8_t   cu8;

typedef volatile u32    vu32;
typedef volatile u16    vu16;
typedef volatile u8     vu8;
typedef vu8             reg8;
typedef vu16            reg16;
typedef vu32            reg32;

typedef vu32 const      vcu32;
typedef vu16 const      vcu16;
typedef vu8	 const      vcu8;

typedef int_least8_t    s8l;
typedef int_least16_t   s16l;
typedef int_least32_t   s32l;
typedef int_least64_t   s64l;
typedef uint_least8_t   u8l;
typedef uint_least16_t  u16l;
typedef uint_least32_t  u32l;
typedef uint_least64_t  u64l;

typedef int_fast8_t     s8f;
typedef int_fast16_t    s16f;
typedef int_fast32_t    s32f;
typedef int_fast64_t    s64f;
typedef uint_fast8_t    u8f;
typedef uint_fast16_t   u16f;
typedef uint_fast32_t   u32f;
typedef uint_fast64_t   u64f;

// Capitalized style:
typedef bool            BOOL;
typedef int8_t          INT8;
typedef int16_t         INT16;
typedef int32_t         INT32;
typedef int64_t         INT64;

typedef uint8_t         UINT8;
typedef uint16_t        UINT16;
typedef uint32_t        UINT32;
typedef uint64_t        UINT64;

typedef float           FP32;
typedef double          FP64;

typedef const uint32_t  CUINT32;
typedef const uint16_t  CUINT16;
typedef const uint8_t   CUINT8;

typedef volatile u32    VUINT32;
typedef volatile u16    VUINT16;
typedef volatile u8     VUINT8;
typedef vu8             REG8;
typedef vu16            REG16;
typedef vu32            REG32;

typedef vu32 const      VCUINT32;
typedef vu16 const      VCUINT16;
typedef vu8	 const      VCUINT8;

typedef int_least8_t    INT8L;
typedef int_least16_t   INT16L;
typedef int_least32_t   INT32L;
typedef int_least64_t   INT64L;
typedef uint_least8_t   UINT8L;
typedef uint_least16_t  UINT16L;
typedef uint_least32_t  UINT32L;
typedef uint_least64_t  UINT64L;

typedef int_fast8_t     INT8F;
typedef int_fast16_t    INT16F;
typedef int_fast32_t    INT32F;
typedef int_fast64_t    INT64F;
typedef uint_fast8_t    UINT8F;
typedef uint_fast16_t   UINT16F;
typedef uint_fast32_t   UINT32F;
typedef uint_fast64_t   UINT64F;


/*************************************************************************
*                                   CONSTANTS
**************************************************************************/
#ifndef NULL
#define NULL		((void *) 0)
#endif

#define BIT0    ((UINT8)0x01)
#define BIT1    ((UINT8)0x02)
#define BIT2    ((UINT8)0x04)
#define BIT3    ((UINT8)0x08)
#define BIT4    ((UINT8)0x10)
#define BIT5    ((UINT8)0x20)
#define BIT6    ((UINT8)0x40)
#define BIT7    ((UINT8)0x80)
#define BIT8    ((UINT16)0x0100)
#define BIT9    ((UINT16)0x0200)
#define BIT10   ((UINT16)0x0400)
#define BIT11   ((UINT16)0x0800)
#define BIT12   ((UINT16)0x1000)
#define BIT13   ((UINT16)0x2000)
#define BIT14   ((UINT16)0x4000)
#define BIT15   ((UINT16)0x8000)
#define BIT16   ((UINT32)0x00010000)
#define BIT17   ((UINT32)0x00020000)
#define BIT18   ((UINT32)0x00040000)
#define BIT19   ((UINT32)0x00080000)
#define BIT20   ((UINT32)0x00100000)
#define BIT21   ((UINT32)0x00200000)
#define BIT22   ((UINT32)0x00400000)
#define BIT23   ((UINT32)0x00800000)
#define BIT24   ((UINT32)0x01000000)
#define BIT25   ((UINT32)0x02000000)
#define BIT26   ((UINT32)0x04000000)
#define BIT27   ((UINT32)0x08000000)
#define BIT28   ((UINT32)0x10000000)
#define BIT29   ((UINT32)0x20000000)
#define BIT30   ((UINT32)0x40000000)
#define BIT31   ((UINT32)0x80000000)

#ifndef _memcpy
#define _memcpy(d,s,l) memcpy(d,s,l)
#endif
#ifndef _memset
#define _memset(d,c,l) memset(d,c,l)
#endif
#ifndef _memcmp
#define _memcmp(d,s,l) memcmp(d,s,l)
#endif
#define MEM_CMP_MATCH	(0)
#define STR_CMP_MATCH	(0)

#define NO_ERROR			((INT32) 0)
#ifndef SUCCESS
#define SUCCESS			(NO_ERROR)
#endif
#define ERROR				((INT32)-1)

#endif // TYPES_H

