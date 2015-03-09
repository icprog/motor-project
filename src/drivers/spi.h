/*******************************************************************************************
This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{                // CPOL	CPHA
   SPI_MODE0,    // 0     0
   SPI_MODE1,    // 0     1
   SPI_MODE2,    // 1     0
   SPI_MODE3,    // 1     1
   NUM_SPI_MODES
} SPI_MODE;

typedef void (*PFN_SPI_SEL)(bool);	// Select Chip (TRUE) or deselect (FALSE)
typedef struct
{
   PFN_SPI_SEL pfnSelect;
   PFV pfnDone;
} SPI_CBS;

typedef struct
{
   SPI_CBS  cbs;
   u32      clockRateHz;
   SPI_MODE mode;
   bool     lsbFirst;
} SPI_SETUP;


typedef struct
{
   u8 const *pTxSrc;
   u8 *pRxDest;
   u16 txLen;
   u16 rxLen;
} SPI_TRANSFER_PARAMS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/


#endif
