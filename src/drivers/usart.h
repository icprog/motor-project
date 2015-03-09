/*******************************************************************************************
ATSAM3U2C USART Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __USART_H__
#define __USART_H__

#include "types.h"
#include "spi.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   USART0,
   USART1,
   USART2,
   USART3,
   UART0,   // asynchronous UART only
   USART_COUNT
}USART_DEV;

typedef enum
{
   USART_IOCTL_SETUP,         // arg = UART_SETUP const *
   USART_IOCTL_INSTALL_CBS,   // arg = UART_CBS const *
   USART_IOCTL_SPI_SINGLE,    // arg = txByte, response = received val;
   USART_IOCTL_SPI_WR_RD,     // arg = USART_TRANSFER_PARAMS const *
   USART_IOCTL_STOP_TXFR,     // arg = USART_TXFR_FLAG(s)
   USART_IOCTL_STATUS,        // arg = USART_STATUS
} USART_IOCTL_CMD;

typedef enum
{
   USART_PARITY_EVEN,
   USART_PARITY_ODD,
   USART_PARITY_ZERO,   // aka 'space'
   USART_PARITY_ONE,    // aka 'mark'
   USART_PARITY_NONE,
   USART_PARITY_MULTIDROP,
} USART_PARITY;

typedef enum
{
   USART_STOP_BITS_1,
   USART_STOP_BITS_1_5,
   USART_STOP_BITS_2
} USART_STOP_BITS;

typedef enum
{
   USART_MODE_ASYNC,
   USART_MODE_SPI,
   USART_MODE_COUNT
}USART_MODE;

typedef enum
{
   USART_TXFR_FLAG_RD = BIT0,
   USART_TXFR_FLAG_WR = BIT1,
}USART_TXFR_FLAG;

typedef enum
{
   USART_STATUS_REG,
   USART_STATUS_BAUD,
   USART_STATUS_RX_REMAIN,
   USART_STATUS_TX_REMAIN,
}USART_STATUS;

typedef struct
{
   u32 baud;
   bool lsbFirst;
   USART_MODE mode;

   // USART_MODE_ASYNC:
   USART_PARITY parity;
   u8 dataBits;
   USART_STOP_BITS stopBits;

   // USART_MODE_SPI:
   SPI_MODE spiMode;
} USART_SETUP;

typedef void (*PFN_USART_CB)(s32);	// USART_DEV
typedef struct
{
   PFN_USART_CB pfnRxDoneOrTimeout;
   PFN_USART_CB pfnTxDone;
   PFN_USART_CB pfnRxErr;
} USART_CBS;

typedef struct
{
   void	const *pTxData;
   void	*pRxData;
   u32 len;
} USART_TRANSFER_PARAMS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
//lint -esym(534, UsartOpen, UsartClose, UsartIoctl, UsartRead, UsartWrite)
s32 UsartOpen (s32 usart, USART_SETUP const *pSetup);
s32 UsartClose(s32 usart);
s32 UsartRead (s32 usart, void *pDest, s32 len);      // sets next PDC transfer
s32 UsartWrite(s32 usart, const void *pSrc, s32 len); // sets next PDC transfer
s32 UsartIoctl(s32 usart, s32 cmd, u32 arg);

void DBGU_IrqHandler(void);
void USART0_IrqHandler(void);
void USART1_IrqHandler(void);
void USART2_IrqHandler(void);
void USART3_IrqHandler(void);

#endif
