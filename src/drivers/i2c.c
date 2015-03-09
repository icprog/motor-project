/*******************************************************************************************
Inter-Integrated Circuit (I2C) Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "i2c.h"
#include "bsp.h"
#include "irq.h"
#include "core_cm3.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
typedef struct
{
   AT91PS_TWI const pRegs;
   PERIPH_ID periphId;
}I2C_DEFS;
static const I2C_DEFS i2cDefs[NUM_I2C_DEVS] =
{
   [I2C_DEV0] = { .pRegs = AT91C_BASE_TWI0, .periphId = PERIPH_TWI0 },
   [I2C_DEV1] = { .pRegs = AT91C_BASE_TWI1, .periphId = PERIPH_TWI1 },  //lint !e485
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   AT91PS_TWI pRegs;
	I2C_TRANSFER_PARAMS	transfer;
	I2C_CBS cbs;
	I2C_DEV dev;
	bool isOpened;
   volatile bool isBusy;
   volatile bool isError;
} I2C_CH;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static I2C_CH i2cDev[NUM_I2C_DEVS];


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void I2cSetupClock(I2C_CH const *pDev, u32 clockRateHz);
static void I2cStartTx(I2C_CH *pDev);
static void I2cIrqHandler(I2C_CH *pDev);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void I2cOpen(I2C_DEV dev, u32 clockRateHz, I2C_CBS const *pCallBacks)
{
	I2C_CH *pDev = &i2cDev[dev];
	if ((dev < NUM_I2C_DEVS) && !pDev->isOpened)
	{
		pDev->isOpened = true;
      pDev->isBusy = false;
		pDev->dev = dev;
      pDev->pRegs = i2cDefs[dev].pRegs;

		if (pCallBacks)
			memcpy(&pDev->cbs, pCallBacks, sizeof(I2C_CBS));
		else
			memset(&pDev->cbs, 0, sizeof(I2C_CBS));

      BspPeriphOn(i2cDefs[dev].periphId);
      pDev->pRegs->TWI_CR = AT91C_TWI_SWRST;
      I2cSetupClock(pDev, clockRateHz);
      NVIC_SetPriority((IRQn_Type)i2cDefs[pDev->dev].periphId, IRQ_PRIO_TWI);
      NVIC_ClearPendingIRQ((IRQn_Type)i2cDefs[pDev->dev].periphId);
      NVIC_EnableIRQ((IRQn_Type)i2cDefs[pDev->dev].periphId);
	}
}

void I2cClose(I2C_DEV dev)
{
	I2C_CH *pDev = &i2cDev[dev];
	if ((dev < NUM_I2C_DEVS) && pDev->isOpened)
	{
		pDev->isOpened = false;
      pDev->pRegs->TWI_CR = AT91C_TWI_SWRST;
      BspPeriphOff(i2cDefs[dev].periphId);
	}
}

// interrupt-based call
void I2cWriteRead(I2C_DEV dev, I2C_TRANSFER_PARAMS const *pTransfer)
{
	I2C_CH *pDev = &i2cDev[dev];
	if ((dev < NUM_I2C_DEVS) && pDev->isOpened)
	{
      if (I2cIsBusy(dev))
      {
         I2cWait(dev);  //lint !e534
      }
      memcpy(&pDev->transfer, pTransfer, sizeof(I2C_TRANSFER_PARAMS));
      pDev->transfer.destAddr &= 0xFE; // Only support writes with this driver, so last bit is always 0
      I2cStartTx(pDev);
	}
}

bool I2cIsBusy(I2C_DEV dev)
{
   return i2cDev[dev].isBusy;
}
bool I2c0IsBusy(void)
{
   return i2cDev[0].isBusy;
}
bool I2c1IsBusy(void)
{
   return i2cDev[1].isBusy;
}

// Blocking (polling) call
bool I2cWriteReadWait(I2C_DEV dev, I2C_TRANSFER_PARAMS const *pTransfer)
{
	I2cWriteRead(dev, pTransfer);
   return I2cWait(dev);
}

bool I2cWait(I2C_DEV dev)
{
   BspSleepWhile((dev ==I2C_DEV0)? I2c0IsBusy : I2c1IsBusy);
   return !i2cDev[dev].isError;
}


void TWI0_IrqHandler(void)
{
   I2cIrqHandler(&i2cDev[0]);
}
void Twi1_IrqHandler(void)
{
   I2cIrqHandler(&i2cDev[0]);
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void I2cSetupClock(I2C_CH const *pDev, u32 clockRateHz)
{
   if (clockRateHz > 400000)
      clockRateHz = 400000;

   // NOTE: Setting Tlow = Thigh (50% duty cycle)
   // From datasheet: Tlow = Thigh = 1/(2*clockRateHz) = ((CLHDIV * 2^CKDIV) + 4) * (1/Fmck)
   // CLHDIV = Fmck / (((2*clockRateHz) - 4) * 2^CKDIV)
   // Fi2c = Fmck/(2 * ((CLHDIV * 2^CKDIV) + 4))
   // solve for both CLHDIV and CKDIV by looping and finding the closest match
   u32 mclkDiv2 = BspGetCpuClkFreq()/2;
   u32 preCalc = ((mclkDiv2 / clockRateHz) - 4);
   u32 bestClkDiv = 0, bestClhDiv = 0;
   u32 bestClkRate = UINT32_MAX;
   for (u32 ckDiv = 0; ckDiv < 8; ckDiv++)
   {
      for (u32 clhDiv = ((preCalc / (1UL << ckDiv)) - 1); clhDiv < 0x100; clhDiv++)
      {
         u32 i2cClk = mclkDiv2 / ((clhDiv * (1UL << ckDiv)) + 4);
         if (ABS_DIF(i2cClk, clockRateHz) < ABS_DIF(bestClkRate, clockRateHz))
         {
            bestClkDiv = ckDiv;
            bestClhDiv = clhDiv;
            bestClkRate = i2cClk;
            if (i2cClk == clockRateHz)
               break;
         }
         if (i2cClk < clockRateHz)
            break;
      }
   }

   if (clockRateHz <= 100e3)
   {
      // 50% duty cycle
      pDev->pRegs->TWI_CWGR = (bestClhDiv | (bestClhDiv << 8) | (bestClkDiv << 16));
   }
   else
   {
      // 2/3 duty cycle
      pDev->pRegs->TWI_CWGR = (((bestClhDiv * 4) / 3) |
                               (((bestClhDiv * 2) / 3) << 8) |
                               (bestClkDiv << 16));
   }
}

static void I2cStartTx(I2C_CH *pDev)
{
   AT91PS_TWI const pRegs = pDev->pRegs;
   pDev->isBusy = true;
   pDev->isError = false;
   NVIC_ClearPendingIRQ((IRQn_Type)i2cDefs[pDev->dev].periphId);
   BspPeriphOn(i2cDefs[pDev->dev].periphId);
   pRegs->TWI_MMR = (pDev->transfer.destAddr & 0xFE) << 15;
   pRegs->TWI_CR = (AT91C_TWI_MSEN | AT91C_TWI_START);

   // Setup DMA (PDC) to feed data into the driver as it's needed:
   pRegs->TWI_TPR = (u32)pDev->transfer.pTxData;
   pRegs->TWI_TCR = pDev->transfer.txLen;
   pRegs->TWI_PTCR = AT91C_PDC_TXTEN;

   // Interrupt when the transfer is finished (or NAK error)
   pRegs->TWI_IER = (AT91C_TWI_TXBUFE | AT91C_TWI_NACK_MASTER);
}

static void I2cIrqHandler(I2C_CH *pDev)
{
   // Get the cause of the interrupt
   AT91PS_TWI const pRegs = pDev->pRegs;
   u32 status = pRegs->TWI_SR;
   if ((status & (AT91C_TWI_TXBUFE | AT91C_TWI_TXCOMP_MASTER)) == AT91C_TWI_TXBUFE)
   {
      // I2C STOP to end the transfer
      pRegs->TWI_CR = AT91C_TWI_STOP;
      pRegs->TWI_IDR = AT91C_TWI_TXBUFE;
      pRegs->TWI_IER = AT91C_TWI_TXCOMP_MASTER;
   }
   else
   {
      // turn off peripheral until it's needed again...
      pRegs->TWI_IDR = (AT91C_TWI_TXBUFE | AT91C_TWI_NACK_MASTER | AT91C_TWI_TXCOMP_MASTER);
      pRegs->TWI_PTCR = AT91C_PDC_TXTDIS;
      BspPeriphOff(i2cDefs[pDev->dev].periphId);

      // Inform user that transfer is complete
      pDev->isBusy = false;
      if (status & AT91C_TWI_TXCOMP_MASTER)
      {
         if (pDev->cbs.doneCallback)
         {
            pDev->cbs.doneCallback(pDev->dev);
         }
      }
      else //if (status & AT91C_TWI_NACK_MASTER)
      {
         pDev->isError = true;
         if (pDev->cbs.errorCallback)
         {
            pDev->cbs.errorCallback(pDev->dev);
         }
      }
   }
}

