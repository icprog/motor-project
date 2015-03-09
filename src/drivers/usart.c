/*******************************************************************************************
ATSAM3U2C USART Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "usart.h"
#include "bsp.h"
#include "irq.h"
#include "core_cm3.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define UART_IS_VALID(num) ((num) < USART_COUNT)
#define UART_IS_OPEN(num)	(UART_IS_VALID(num) && usartCfg[num].initialized)

static const USART_SETUP UsartDefault =
{
	.baud = 115200,
   .lsbFirst = true,
   .mode = USART_MODE_ASYNC,

	.parity = USART_PARITY_NONE,
	.dataBits = 8,
	.stopBits = USART_STOP_BITS_1,

   .spiMode = SPI_MODE0,
};

typedef struct
{
   AT91PS_USART const pReg;
   PERIPH_ID periphId;
}USART_DEFS;
static const USART_DEFS UsartDefs[USART_COUNT] =
{
   [USART0] = { .pReg = AT91C_BASE_US0, .periphId = PERIPH_US0 },
   [USART1] = { .pReg = AT91C_BASE_US1, .periphId = PERIPH_US1 },  //lint !e485
   [USART2] = { .pReg = AT91C_BASE_US2, .periphId = PERIPH_US2 },  //lint !e485
   [USART3] = { .pReg = AT91C_BASE_US3, .periphId = PERIPH_US3 },  //lint !e485
   [UART0]  = { .pReg = (AT91PS_USART)AT91C_BASE_DBGU,.periphId = PERIPH_DBGU },  //lint !e485 !e740
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   AT91PS_USART pReg;
   USART_CBS cbs;
   u32 actualBaud;
   s32 rxLen;
   bool initialized;
   USART_DEV usartNum;
} USART_BASE;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static USART_BASE usartCfg[USART_COUNT];


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void UsartSetup(USART_BASE *pUsart, USART_SETUP const *pSetup);
static void UsartSetBaud(USART_BASE *pUsart, USART_SETUP const *pSetup);
static void UsartSetupCallbacks(USART_BASE *pUsart, USART_CBS const *pCbs);
static s32 UsartSpiSingle(USART_BASE const *pUsart, u32 txByte);
static void UsartSpiTxfr(USART_BASE const *pUsart, USART_TRANSFER_PARAMS const *pTxfr);
static void UsartStopTxfr(USART_BASE const *pUsart, USART_TXFR_FLAG txfr);
static s32 UsartStatus(USART_BASE const *pUsart, USART_STATUS cmd);
static s32 UsartRxRemain(USART_BASE const *pUsart);
static s32 UsartTxRemain(USART_BASE const *pUsart);
static void UsartHandler(USART_BASE const *pUsart);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
s32 UsartOpen (s32 usart, USART_SETUP const *pSetup)
{
	USART_BASE *pUsart = &usartCfg[usart];
	if (!UART_IS_VALID(usart) || pUsart->initialized)
      return ERROR;

   pUsart->initialized = TRUE;
   pUsart->usartNum = (USART_DEV)usart;
   pUsart->rxLen = 0;
   memset(&pUsart->cbs, 0, sizeof(pUsart->cbs));
   pUsart->pReg = UsartDefs[usart].pReg;
   BspPeriphOn(UsartDefs[usart].periphId);

   // Leave interrupts disabled until a callback function is registered
   pUsart->pReg->US_IDR = UINT32_MAX;
   UsartSetup(pUsart, pSetup? pSetup: &UsartDefault);
   return NO_ERROR;
}

s32 UsartClose(s32 usart)
{
   if (!UART_IS_OPEN(usart))
      return ERROR;

   USART_BASE *pUsart = &usartCfg[usart];
   pUsart->initialized = FALSE;
   pUsart->pReg->US_IDR = UINT32_MAX;
   NVIC_DisableIRQ((IRQn_Type)UsartDefs[usart].periphId);
   BspPeriphOff(UsartDefs[usart].periphId);
   return NO_ERROR;
}

s32 UsartRead (s32 usart, void *pDest, s32 len)
{
   if (!UART_IS_OPEN(usart))
      return ERROR;

   AT91PS_USART const pReg = usartCfg[usart].pReg;
   // NOTE: Using the 'Next' PDC registers will automatically transfer to the 'current' registers
   //       immediately if there is no current transfer. Otherwise, this will queue up the RX.
   usartCfg[usart].rxLen = (s32)pReg->US_RCR + len;
   pReg->US_RNPR = (u32)pDest;
   pReg->US_RNCR = (u32)len;
   pReg->US_CR = AT91C_US_RXEN;
   pReg->US_PTCR = AT91C_PDC_RXTEN;
   if (usartCfg[usart].cbs.pfnRxDoneOrTimeout || usartCfg[usart].cbs.pfnRxErr)
   {
      pReg->US_IER = (AT91C_US_ENDRX | AT91C_US_FRAME | AT91C_US_OVRE | AT91C_US_PARE | AT91C_US_TIMEOUT);
   }
   return len;
}

s32 UsartWrite(s32 usart, const void *pSrc, s32 len)
{
   if (!UART_IS_OPEN(usart))
      return ERROR;

   AT91PS_USART const pReg = usartCfg[usart].pReg;
   // NOTE: Using the 'Next' PDC registers will automatically transfer to the 'current' registers
   //       immediately if there is no current transfer. Otherwise, this will queue up the RX.
   pReg->US_MR &= ~AT91C_US_INACK;
   pReg->US_TNPR = (u32)pSrc;
   pReg->US_TNCR = (u32)len;
   pReg->US_CR = AT91C_US_TXEN;
   pReg->US_PTCR = AT91C_PDC_TXTEN;
   if (usartCfg[usart].cbs.pfnTxDone)
   {
      pReg->US_IER = AT91C_US_ENDTX;
   }
   return len;
}

s32 UsartIoctl(s32 usart, s32 cmd, u32 arg)
{
   if (!UART_IS_OPEN(usart))
      return ERROR;

   USART_BASE *pUsart = &usartCfg[usart];
   s32 res = NO_ERROR;
   switch ((USART_IOCTL_CMD)cmd)
   {
   case USART_IOCTL_SETUP:       UsartSetup(pUsart, (USART_SETUP const *)arg);               break;
   case USART_IOCTL_INSTALL_CBS: UsartSetupCallbacks(pUsart, (USART_CBS const *)arg);        break;
   case USART_IOCTL_SPI_SINGLE:  res = UsartSpiSingle(pUsart, arg);                          break;
   case USART_IOCTL_SPI_WR_RD:   UsartSpiTxfr(pUsart, (USART_TRANSFER_PARAMS const *)arg);   break;
   case USART_IOCTL_STOP_TXFR:   UsartStopTxfr(pUsart, (USART_TXFR_FLAG)arg);                break;
   case USART_IOCTL_STATUS:      res = UsartStatus(pUsart, (USART_STATUS)arg);               break;
   default:                      res = ERROR;                                                break;
   }
   return res;
}


void DBGU_IrqHandler(void)    {UsartHandler(&usartCfg[UART0]);}
void USART0_IrqHandler(void)  {UsartHandler(&usartCfg[USART0]);}
void USART1_IrqHandler(void)  {UsartHandler(&usartCfg[USART1]);}
void USART2_IrqHandler(void)  {UsartHandler(&usartCfg[USART2]);}
void USART3_IrqHandler(void)  {UsartHandler(&usartCfg[USART3]);}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void UsartSetup(USART_BASE *pUsart, USART_SETUP const *pSetup)
{
   AT91PS_USART const pReg = pUsart->pReg;
   if (pSetup->mode == USART_MODE_ASYNC)
   {
      pReg->US_MR = (AT91C_US_USMODE_NORMAL | AT91C_US_CLKS_CLOCK | AT91C_US_CHMODE_NORMAL |
                     ((pSetup->dataBits - 5U) << 6U) |
                     (pSetup->lsbFirst? 0 : AT91C_US_MSBF) |
                     (AT91C_US_CHMODE_NORMAL) |
                     ((u8)pSetup->parity << 9U) |
                     ((u8)pSetup->stopBits << 12U));
      pReg->US_RTOR = 10;  // standard timeout is 1 frame (8N1 = 10 bits)
   }
   else
   {
      pReg->US_MR = (0xEUL | AT91C_US_CLKS_CLOCK | AT91C_US_CHMODE_NORMAL |
                     ((pSetup->dataBits - 5U) << 6) |
                     (((u8)pSetup->spiMode & BIT0)? AT91C_US_SYNC : 0) |
                     ((u8)USART_PARITY_NONE << 9) |
                     (AT91C_US_CHMODE_NORMAL) |
                     (((u8)pSetup->spiMode & BIT1)? AT91C_US_MSBF : 0) |
                     (AT91C_US_CKLO));
      pReg->US_RTOR = 0;
   }
   UsartSetBaud(pUsart, pSetup);
   pReg->US_CR = (AT91C_US_RSTRX | AT91C_US_RXDIS | AT91C_US_RSTTX | AT91C_US_TXDIS | AT91C_US_RSTSTA);
}

static void UsartSetBaud(USART_BASE *pUsart, USART_SETUP const *pSetup)
{
   // normal: 48MHz / (CD * 16)
   // Fractional: 48MHz / (CD * 16 * FP/8)
   // SPI: 48MHz / CD
   u32 mck = BspGetCpuClkFreq();
   u32 bestCd, bestFp;
   u32 bestBaudDif = UINT32_MAX;

   for (u32 fp = 8; (fp > 0) || ((fp == 7) && (pUsart->usartNum == UART0)); fp--)
   {
      for (u32 cd = ((pSetup->mode == USART_MODE_SPI)? 6 : 1); cd < 256; cd++)
      {
         u32 actualBaud = (pSetup->mode == USART_MODE_SPI)
            ? ((mck * fp) / (cd * 8))
            : ((mck * fp) / (cd * (8 * 16)));
         if (ABS_DIF(actualBaud, pSetup->baud) < bestBaudDif)
         {
            bestBaudDif = ABS_DIF(actualBaud, pSetup->baud);
            pUsart->actualBaud = actualBaud;
            bestCd = cd;
            bestFp = (fp == 8)? 0 : fp;
            if ((bestBaudDif == 0) && (fp == 8))
               goto USART_BAUD_FOUND;  //lint !e801
         }
         else if (actualBaud < pSetup->baud)
         {
            break;
         }
      }
   }
USART_BAUD_FOUND:
   pUsart->pReg->US_BRGR = bestCd | (bestFp << 16);   //lint !e644
}

static void UsartSetupCallbacks(USART_BASE *pUsart, USART_CBS const *pCbs)
{
   IRQn_Type irq = (IRQn_Type)UsartDefs[pUsart->usartNum].periphId;
   if ((pCbs == NULL) ||
       ((pCbs->pfnRxDoneOrTimeout == NULL) && (pCbs->pfnRxErr == NULL) && (pCbs->pfnTxDone == NULL)))
   {
      NVIC_DisableIRQ(irq);
      memset(&pUsart->cbs, 0, sizeof(pUsart->cbs));
   }
   else
   {
      memcpy(&pUsart->cbs, pCbs, sizeof(pUsart->cbs));
      NVIC_SetPriority(irq, IRQ_PRIO_USART);
      NVIC_ClearPendingIRQ(irq);
      NVIC_EnableIRQ(irq);
   }
}

static s32 UsartSpiSingle(USART_BASE const *pUsart, u32 txByte)
{
   AT91PS_USART const pReg = pUsart->pReg;
   pReg->US_IDR = UINT32_MAX;
   pReg->US_CR = (AT91C_US_RXEN | AT91C_US_TXEN);
   pReg->US_RHR;  // dummy read to clear
   pReg->US_THR = txByte;
   do{} while (!(pReg->US_CSR & AT91C_US_RXRDY));
   s32 retVal = (s32)pReg->US_RHR;
   pReg->US_CR = (AT91C_US_RXDIS | AT91C_US_TXDIS);
   return retVal;
}

static void UsartSpiTxfr(USART_BASE const *pUsart, USART_TRANSFER_PARAMS const *pTxfr)
{
   AT91PS_USART const pReg = pUsart->pReg;
   pReg->US_MR |= AT91C_US_INACK;
   pReg->US_IDR = UINT32_MAX;

   pReg->US_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
   pReg->US_RNPR = (u32)pTxfr->pRxData;
   pReg->US_TNPR = (u32)pTxfr->pTxData;
   pReg->US_RNCR = (u32)pTxfr->len;
   pReg->US_TNCR = (u32)pTxfr->len;
   pReg->US_CR = (AT91C_US_RXEN | AT91C_US_TXEN);
   pReg->US_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
   pReg->US_IER = AT91C_US_ENDRX;   // IRQ on RX (not TX)
}

static void UsartStopTxfr(USART_BASE const *pUsart, USART_TXFR_FLAG txfr)
{
   AT91PS_USART const pReg = pUsart->pReg;
   if ((u8f)txfr & (u8f)USART_TXFR_FLAG_RD)
   {
      pReg->US_PTCR = AT91C_PDC_RXTDIS;
      pReg->US_CR = AT91C_US_RXDIS;
   }
   if ((u8f)txfr & (u8f)USART_TXFR_FLAG_WR)
   {
      pReg->US_PTCR = AT91C_PDC_TXTDIS;
      pReg->US_CR = AT91C_US_TXDIS;
   }
}

static s32 UsartStatus(USART_BASE const *pUsart, USART_STATUS cmd)
{
   s32 res;
   switch (cmd)
   {
   case USART_STATUS_REG:        res = (s32)pUsart->pReg->US_CSR; break;
   case USART_STATUS_BAUD:       res = (s32)pUsart->actualBaud;   break;
   case USART_STATUS_RX_REMAIN:  res = UsartRxRemain(pUsart);     break;
   case USART_STATUS_TX_REMAIN:  res = UsartTxRemain(pUsart);     break;
   default:                      res = ERROR;                     break;
   }
   return res;
}

static s32 UsartRxRemain(USART_BASE const *pUsart)
{
   s32 rxRemain = (s32)pUsart->pReg->US_RCR;
   rxRemain += (s32)pUsart->pReg->US_RNCR;
   return (pUsart->rxLen - rxRemain);
}

static s32 UsartTxRemain(USART_BASE const *pUsart)
{
   s32 txRemain = (s32)pUsart->pReg->US_TCR;
   txRemain += (s32)pUsart->pReg->US_TNCR;
   return (pUsart->rxLen - txRemain);
}

static void UsartHandler(USART_BASE const *pUsart)
{
   AT91PS_USART const pReg = pUsart->pReg;
   u32 irq = pReg->US_CSR;
   irq &= pReg->US_IMR;
   if (irq & (AT91C_US_RXRDY | AT91C_US_TIMEOUT))
   {
      if (irq & AT91C_US_TIMEOUT)
      {
         pReg->US_CR = AT91C_US_STTTO;
         usartCfg[pUsart->usartNum].rxLen = UsartRxRemain(pUsart);
         pReg->US_RCR = 0;
      }
      if (pReg->US_RCR == 0)
      {
         pReg->US_IDR = AT91C_US_RXRDY;
      }
      if (pUsart->cbs.pfnRxDoneOrTimeout)
      {
         pUsart->cbs.pfnRxDoneOrTimeout((s32)pUsart->usartNum);
      }
   }
   if (irq & AT91C_US_ENDTX)
   {
      if (pReg->US_TCR == 0)
      {
         pReg->US_IDR = AT91C_US_ENDTX;
      }
      if (pUsart->cbs.pfnTxDone)
      {
         pUsart->cbs.pfnTxDone((s32)pUsart->usartNum);
      }
   }
   if (irq & (AT91C_US_FRAME | AT91C_US_OVRE | AT91C_US_PARE))
   {
      pReg->US_CR = AT91C_US_RSTSTA;
      if (pUsart->cbs.pfnRxErr)
      {
         pUsart->cbs.pfnRxErr((s32)pUsart->usartNum);
      }
   }
}

