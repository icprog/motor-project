/*******************************************************************************************
Secure Digital (SD) Card (SDv3/SDv1/SDv2 and SD) SPI Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "includes.h"
#include "sd.h"
#include "fileFat.h"
#include "gpio.h"
#include "usart.h"
#include "timer.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define SD_SPI_STARTUP_CLK		(400000)
#define SD_SPI_OP_CLK			(UINT32_MAX)	// SPI driver will ensure min operating clock
#define SD_SPI_WAIT_TIME_MS	(500)
#define SD_READ_TOKEN_MS		(200)
#define SD_INIT_TIMEOUT_MS		(1000)
#define SD_ERASE_TIMEOUT_MS	(30*NUM_MS_PER_SEC)
#define SD_PERIPH_DEV         (USART1)

#define SD_SPI_SEL()				GpioClear(GPIO_PIN_SD_CS_MCDA3)
#define SD_SPI_DESEL()			GpioSet(GPIO_PIN_SD_CS_MCDA3)

//lint -esym(750, SD_*CMD*, SD_R?_LEN) -esym(751, SD_*CMD*, SD_R?_LEN)
// SD/SD commands
#define SD_CMD0		(0)			/* GO_IDLE_STATE */
#define SD_CMD1		(1)			/* SEND_OP_COND (SD) */
#define SD_ACMD41	(0x80+41)		/* SEND_OP_COND (SDC) */
#define SD_CMD8		(8)			/* SEND_IF_COND */
#define SD_CMD9		(9)			/* SEND_CSD */
#define SD_CMD10		(10)			/* SEND_CID */
#define SD_CMD12		(12)			/* STOP_TRANSMISSION */
#define SD_ACMD13	(0x80+13)		/* SD_STATUS (SDC) */
#define SD_CMD16		(16)			/* SET_BLOCKLEN */
#define SD_CMD17		(17)			/* READ_SINGLE_BLOCK */
#define SD_CMD18		(18)			/* READ_MULTIPLE_BLOCK */
#define SD_CMD23		(23)			/* SET_BLOCK_COUNT (SD) */
#define SD_ACMD23	(0x80+23)		/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define SD_CMD24		(24)			/* WRITE_BLOCK */
#define SD_CMD25		(25)			/* WRITE_MULTIPLE_BLOCK */
#define SD_CMD32		(32)			/* ERASE_ER_BLK_START */
#define SD_CMD33		(33)			/* ERASE_ER_BLK_END */
#define SD_CMD38		(38)			/* ERASE */
#define SD_CMD55		(55)			/* APP_CMD */
#define SD_CMD58		(58)			/* READ_OCR */

#define SD_R1_LEN	(1)
#define SD_R2_LEN	(2)
#define SD_R3_LEN	(SD_R1_LEN + 4)	// 4-byte OCR
#define SD_R7_LEN	(SD_R1_LEN + 4)

/* Card type flags (CardType) */
#define SD_CT_MMC		0x01		/* SD ver 3 */
#define SD_CT_SD1		0x02		/* SD ver 1 */
#define SD_CT_SD2		0x04		/* SD ver 2 */
#define SD_CT_SDC		(SD_CT_SD1 | SD_CT_SD2)	/* SD */
#define SD_CT_BLOCK		0x08		/* Block addressing */

#define SD_DUMMY8    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
#define SD_DUMMY32   SD_DUMMY8 SD_DUMMY8 SD_DUMMY8 SD_DUMMY8
static const u32 SdDummy512[512/4] = {SD_DUMMY32 SD_DUMMY32 SD_DUMMY32 SD_DUMMY32};



/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   u8             cardType;
   FDISK_RESULT   status;
   FDISK          fdisk;
   FAT_DISK       fatFs;
}SD_STATE;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static SD_STATE sd;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void SdSpiSetup(u32 clk);
static bool SdSelect(void);
static void SdDeselect(void);

static bool SdWaitReady(u32 ms);
static u8 SdExchByte(u8 tx);  //lint -esym(534, SdExchByte)
static u8 SdCmd(u8 cmd, u32 arg);
static bool SdReadBlock(u8 *pDest, u16f len);
static bool SdWriteBlock(u8 const *pSrc, u8 token);

static FDISK_RESULT SdDiskOpen(void);
static FDISK_RESULT SdDiskClose(void);
static FDISK_RESULT SdDiskRead(void *pDestArg, u32 sector, u32 blkCnt);
static FDISK_RESULT SdDiskWrite(void const *pSrcArg, u32 sector, u32 blkCnt);
static FDISK_RESULT SdDiskIoctl(FDISK_IOCTLS ioctl, void *pArgBuf);

static FDISK_RESULT SdDiskSync(void);
static FDISK_RESULT SdSectorCount(u32 *pCount);
static FDISK_RESULT SdBlockSize(u32 *pSize);
static FDISK_RESULT SdEraseSector(u32 const *pEraseParams);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void SdInit(void)
{
   FDISK *pDisk = &sd.fdisk;
   pDisk->diskNum = FDISK_SD;
   pDisk->pFsData = &sd.fatFs; // Point to the FAT file system

   // Hardware driver for File I/O:
   pDisk->pfnOpen  = SdDiskOpen;
   pDisk->pfnClose = SdDiskClose;
   pDisk->pfnIoctl = SdDiskIoctl;
   pDisk->pfnRead  = SdDiskRead;
   pDisk->pfnWrite = SdDiskWrite;
   FileDiskReg(pDisk);

   if (!FileDiskMount(&sd.fdisk))
   {
      // error
   }
   else
   {
      // File system test?
   }
}

void SdKill(void)
{
   FileDiskUnmount(&sd.fdisk); //lint !e534
   SdDeselect();
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void SdSpiSetup(u32 clk)
{
   USART_SETUP usartSetup =
   {
      .baud = clk,
      .lsbFirst = false,
      .mode = USART_MODE_SPI,
      .parity = USART_PARITY_NONE,     // n/a
      .dataBits = 8,                   // n/a
      .stopBits = USART_STOP_BITS_1,   // n/a
      .spiMode = SPI_MODE0,
   };
   UsartClose(SD_PERIPH_DEV);
   UsartOpen(SD_PERIPH_DEV, &usartSetup);
}

static bool SdSelect(void)
{
   SD_SPI_SEL();
   if (SdWaitReady(SD_SPI_WAIT_TIME_MS))
   {
      return true;
   }
   SdDeselect();
   return false;
}

static void SdDeselect(void)
{
   SD_SPI_DESEL();
}

static bool SdWaitReady(u32 ms)
{
   u8 d;
   u32 endTick = TimerMsGet() + ms;
   do
   {
      d = SdExchByte(0xFF);
   }
   while ((d != 0xFF) && !TimerMsHasElapsed(endTick));
   return (d == 0xFF);
}

static u8 SdExchByte(u8 tx)
{
   return (u8)UsartIoctl(SD_PERIPH_DEV, USART_IOCTL_SPI_SINGLE, (s32)tx);
}

static u8 SdCmd(u8 cmd, u32 arg)
{
   u8 retry, resp;

   if (cmd & BIT7) // Send CMD55 prior to ACMD
   {
      cmd &= 0x7F;
      resp = SdCmd(SD_CMD55, 0);
      if (resp > 1)
         return resp;
   }

   SdDeselect();
   if (!SdSelect())
      return 0xFF;

   SdExchByte(0x40 | cmd);
   SdExchByte((u8)(arg >> 24));       // MSB
   SdExchByte((u8)(arg >> 16));
   SdExchByte((u8)(arg >> 8));
   SdExchByte((u8)arg);               // LSB
   SdExchByte((cmd == SD_CMD0) ? 0x95 : (cmd == SD_CMD8) ? 0x87 : 0x01);    // Valid CRC or dummy CRC + stop

   if (cmd == SD_CMD12)
      SdExchByte(0xFF);  // Discard one extra byte for command 12

   retry = 10;
   do
   {
      resp = SdExchByte(0xFF);
   }
   while ((resp & BIT7) && --retry);

   return resp;
}

static bool SdReadBlock(u8 *pDest, u16f len)
{
   u8 token;
   u32 endTick = TimerMsGet() + SD_READ_TOKEN_MS;
   do
   {
      token = SdExchByte(0xFF);
   } while ((token == 0xFF) && !TimerMsHasElapsed(endTick));

   if (token != 0xFE)
   {
      return false;
   }

   USART_TRANSFER_PARAMS txfr =
   {
      .pTxData = SdDummy512,
      .pRxData = pDest,
      .len = len,
   };
   UsartIoctl(SD_PERIPH_DEV, USART_IOCTL_SPI_WR_RD, (u32)&txfr);
   do{} while (UsartIoctl(SD_PERIPH_DEV, USART_IOCTL_STATUS, USART_STATUS_TX_REMAIN) != 0);
   SdExchByte(0xFF);
   SdExchByte(0xFF);   // Discard CRC
   return true;
}

static bool SdWriteBlock(u8 const *pSrc, u8 token)
{
   u8 resp;

   if (!SdWaitReady(SD_SPI_WAIT_TIME_MS))
      return false;

   SdExchByte(token);
   if (token != 0xFD)
   {
      UsartWrite(SD_PERIPH_DEV, pSrc, 512);
      do{} while (UsartIoctl(SD_PERIPH_DEV, USART_IOCTL_STATUS, USART_STATUS_TX_REMAIN) != 0);
      SdExchByte(0xFF);
      SdExchByte(0xFF);   // Dummy CRC

      resp = SdExchByte(0xFF);
      if ((resp & 0x1F) != 0x05)
         return false;
   }
   return true;
}



/**************************************************************************
 *                                 File System I/O Hooks
 **************************************************************************/
static FDISK_RESULT SdDiskOpen(void)
{
   u8 n, cmd, ty, ocr[4];
   u32 endTick;

   SdSpiSetup(SD_SPI_STARTUP_CLK);
   for (n = 10; n; n--)
   {
      SdExchByte(0xFF);  // Send 80 dummy clocks (10 bytes)
   }

   ty = 0;
   if (SdCmd(SD_CMD0, 0) == 0x01)      // Put card in SPI/Idle state
   {
      endTick = TimerMsGet() + SD_INIT_TIMEOUT_MS;
      if (SdCmd(SD_CMD8, 0x1AA) == 1) // SDv2?
      {
         for (n = 0; n < 4; n++)
         {
            ocr[n] = SdExchByte(0xFF); /* Get 32 bit return value of R7 resp */
         }
         if ((ocr[2] == 0x01) && (ocr[3] == 0xAA))   // Card supports VCC 2.7-3.6V?
         {
            do
            {}
            while (SdCmd(SD_ACMD41, BIT30) && !TimerMsHasElapsed(endTick));
            if ((SdCmd(SD_CMD58, 0) == 0) && !TimerMsHasElapsed(endTick))   // Check CCS bit in OCR
            {
               for (n = 0; n < 4; n++)
               {
                  ocr[n] = SdExchByte(0xFF);
               }
               ty = (ocr[0] & BIT6) ? (SD_CT_SD2 | SD_CT_BLOCK) : SD_CT_SD2;   // Card ID SDv2
            }
         }
      }
      else    // Not SDv2
      {
         if (SdCmd(SD_ACMD41, 0) <= 1)
         {
            ty = SD_CT_SD1; // SDv1
            cmd = SD_ACMD41;
         }
         else
         {
            ty = SD_CT_MMC; // MMCv3
            cmd = SD_CMD1;
         }
         do          // Wait for init
         {}
         while (SdCmd(cmd, 0) && !TimerMsHasElapsed(endTick));
         if (TimerMsHasElapsed(endTick) || (SdCmd(SD_CMD16, 512) != 0))  // Set block length to 512
         {
            ty = 0;
         }
      }
   }
   sd.cardType = ty;
   SdDeselect();

   if (ty)
   {
      SdSpiSetup(SD_SPI_OP_CLK);
      sd.status = FDISK_OK;
   }
   else
   {
      sd.status = FDISK_NOT_READY;
   }
   return sd.status;
}

static FDISK_RESULT SdDiskClose(void)
{
   return FDISK_OK;
}

static FDISK_RESULT SdDiskRead(void *pDestArg, u32 sector, u32 blkCnt)
{
   u32 rem = blkCnt;
   u32 retry;
   u8 *pDest;

   if (sd.status != FDISK_OK)
      return FDISK_NOT_READY;

   if (!(sd.cardType & SD_CT_BLOCK))   // LBA-to-BA conversion (byte-addressing cards)
      sector *= 512;

   for (retry = 2; rem && retry; retry--)
   {
      pDest = (u8*)pDestArg;
      rem = blkCnt;
      if (rem == 1)       // Single sector
      {
         if ((SdCmd(SD_CMD17, sector) == 0) && SdReadBlock(pDestArg, 512))
         {
            rem = 0;
         }
      }
      else
      {
         if (SdCmd(SD_CMD18, sector) == 0)
         {
            do
            {
               if (!SdReadBlock(pDest, 512))
                  break;
               pDest += 512;
            }
            while (--rem);
            SdCmd(SD_CMD12, 0);         //lint !e534 Stop transmission
         }
      }
      SdDeselect();
   }
   return (rem == 0) ? FDISK_OK : FDISK_ERROR;
}

static FDISK_RESULT SdDiskWrite(void const *pSrcArg, u32 sector, u32 blkCnt)
{
   u32 rem;
   u32 retry;
   u8 const *pSrc;

   if (sd.status != FDISK_OK)
      return FDISK_NOT_READY;

   rem = blkCnt;
   if (!(sd.cardType & SD_CT_BLOCK))       // LBA-to-BA conversion (byte-addressing cards)
      sector *= 512;

   for (retry = 2; rem && retry; retry--)
   {
      pSrc = (u8 const *)pSrcArg;
      rem = blkCnt;

      if (rem == 1)
      {
         if ((SdCmd(SD_CMD24, sector) == 0) && SdWriteBlock(pSrc, 0xFE))
         {
            rem = 0;
         }
      }
      else
      {
         if (sd.cardType & SD_CT_SDC)
            SdCmd(SD_ACMD23, rem);      //lint !e534

         if (SdCmd(SD_CMD25, sector) == 0)
         {
            do
            {
               if (!SdWriteBlock(pSrc, 0xFC))
                  break;
               pSrc += 512;
            }
            while (--rem);
            if (!SdWriteBlock(NULL, 0xFD))
               rem = 1;    // force a retry
         }
      }
      SdDeselect();
   }
   return (rem == 0) ? FDISK_OK : FDISK_ERROR;
}

static FDISK_RESULT SdDiskIoctl(FDISK_IOCTLS ioctl, void *pArgBuf)
{
   FDISK_RESULT result = FDISK_OK;
   switch (ioctl)
   {
   case FDISK_SYNC:              result = SdDiskSync();           break;
   case FDISK_GET_SECTOR_SIZE:   *(u32*)pArgBuf = 512;            break;
   case FDISK_GET_SECTOR_COUNT:  result = SdSectorCount(pArgBuf); break;
   case FDISK_GET_BLOCK_SIZE:    result = SdBlockSize(pArgBuf);   break;
   case FDISK_ERASE_SECTOR:      result = SdEraseSector(pArgBuf); break;
   case FDISK_STATUS:            result = sd.status;              break;
   default:                      result = FDISK_ARG_ERROR;        break;
   }
   SdDeselect();
   return result;
}

static FDISK_RESULT SdDiskSync(void)
{
   FDISK_RESULT result = SdSelect() ? FDISK_OK : FDISK_ERROR;
   return result;
}

static FDISK_RESULT SdSectorCount(u32 *pCount)
{
   FDISK_RESULT result;
   u8 csd[16];

   if ((SdCmd(SD_CMD9, 0) == 0) && SdReadBlock(csd, 16))
   {
      if ((csd[0] >> 6) == 1)     // SDC v2.00
      {
         *pCount = ((u32)csd[9] + ((u32)csd[8] << 8) + 1) << 10;
      }
      else                                // SDC v1.XX or SD v3
      {
         u8 n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
         *pCount = ((csd[8] >> 6) + ((u32)csd[7] << 2) + ((u32)(csd[6] & 3) << 10) + 1) << (n - 9);
      }
      result = FDISK_OK;
   }
   else
   {
      result = FDISK_ERROR;
   }
   return result;
}

static FDISK_RESULT SdBlockSize(u32 *pSize)
{
   FDISK_RESULT result = FDISK_ERROR;
   u8 csd[16];
   u8f n;

   if (sd.cardType & SD_CT_SD2)        // SDC v2.0
   {
      if (SdCmd(SD_ACMD13, 0) == 0)
      {
         SdExchByte(0xFF);
         if (SdReadBlock(csd, 16))
         {
            for (n = 64 - 16; n; n--)
               SdExchByte(0xFF);  // Purge trailing data

            *pSize = 16UL << (csd[10] >> 4);
            result = FDISK_OK;
         }
      }
   }
   else                                            // SDC v1.xx or SD
   {
      if ((SdCmd(SD_CMD9, 0) == 0) && SdReadBlock(csd, 16))
      {
         if (sd.cardType & SD_CT_SD1)
         {
            *pSize = (((csd[10] & 63) << 1) + ((u32)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
         }
         else
         {
            *pSize = ((u32)((csd[10] & 124) >> 2) + 1) *
               (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
         }
         result = FDISK_OK;
      }
   }
   return result;
}

static FDISK_RESULT SdEraseSector(u32 const *pEraseParams)
{
   FDISK_RESULT result = FDISK_ERROR;
   u8 csd[16];
   u32 start, end;

   if (sd.cardType & SD_CT_SDC)
   {
      if ((SdCmd(SD_CMD9, 0) == 0) && SdReadBlock(csd, 16))
      {
         if ((csd[0] >> 6) || (csd[10] & 0x40))      // Check if sector erase can be applied to the card
         {
            start = pEraseParams[0];
            end = pEraseParams[1];
            if (!(sd.cardType & SD_CT_BLOCK))
            {
               start *= 512;
               end *= 512;
            }
            if ((SdCmd(SD_CMD32, start) == 0) &&
                (SdCmd(SD_CMD33, end) == 0) &&
                (SdCmd(SD_CMD38, 0) == 0) &&
                (SdWaitReady(SD_ERASE_TIMEOUT_MS)))
            {
               result = FDISK_OK;
            }
         }
      }
   }
   return result;
}

