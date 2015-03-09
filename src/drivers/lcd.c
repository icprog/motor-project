/*******************************************************************************************
NHD-C0220BiZ-FS(RGB)-FBW-3VM LCD driver
2x20 characters, I2C, FSTN

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "lcd.h"
#include "st7036_def.h"
#include "i2c.h"
#include "gpio.h"
#include "timer.h"
#include "itemQ.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define LCD_I2C_ADDR				(0x78)
#define LCD_I2C_BUS_SPEED_HZ	(200000)
#define LCD_I2C_DEV				(I2C_DEV0)
#define LCD_RESET_TIME_US		(150)
#define LCD_INIT_TIME_MS		(45)
#define LCD_GPIO_RST          (GPIO_PIN_LCD_RST)

#define LCD_FUNC_SET				(ST7036_CMD_FUNC_SET | ST7036_FUNC_8_BIT)
#define LCD_FUNC_SET_2_LINE	(LCD_FUNC_SET | ST7036_FUNC_2_LINE | ST7036_FUNC_INSN_EXT1)
#define LCD_FUNC_SET_DBL_HT	(LCD_FUNC_SET | ST7036_FUNC_DBL_HEIGHT | ST7036_FUNC_INSN_EXT1)
#define LCD_DEF_CONTRAST		(0x28)
#define LCD_FLWR_AMP				(0x05)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   ITEM_Q i2cQ;
   u8 i2cQMem[LCD_NUM_CHARS + 16];
   u8 i2cTxBuf[LCD_NUM_CHARS_PER_LINE + 1];
   char newText[LCD_NUM_CHARS];
   char lcdText[LCD_NUM_CHARS];
   u8 row;
   u8 col;
   bool cursorUpdate;
   bool i2cErr;
   LCD_CFG config;
}LCD_INFO;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static LCD_INFO lcd;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void LcdCtrl(LCD_CFG const *pCfg);
static void LcdSetContrast(u8 val);
static void LcdSetDblHeight(bool on);
static void LcdSetCursor(u8 row, u8 col);
static void LcdCmd(u8 cmd);
static void LcdCmdQueue(void const *pSrc, u8 len);
static void LcdCmdDequeue(void);

static bool LcdI2cWait(void);
static void LcdI2cDone(void);
static void LcdI2cErr(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void LcdInit(void)
{
   u8 lcdInitCmds[] =
   {
      LCD_FUNC_SET | ST7036_FUNC_2_LINE,
      LCD_FUNC_SET_2_LINE,
      ST7036_CMD_DISP_SHIFT | ST7036_SHIFT_RIGHT,
      ST7036_CMD_CONTRAST | ST7036_CONTRAST_CONT(LCD_DEF_CONTRAST),
      ST7036_CMD_ICON_CTRL | ST7036_ICON_ON | ST7036_ICON_BOOST_ON | ST7036_CONTRAST_ICON(LCD_DEF_CONTRAST),
      ST7036_CMD_FLWR_CTRL | ST7036_FLWR_ON | ST7036_FLWR_AMP(LCD_FLWR_AMP),
      ST7036_CMD_DISP_CTRL | ST7036_DISP_ON,
   };
   I2C_CBS cbs =
   {
      .doneCallback = (PFN_I2C_CB)LcdI2cDone,
      .errorCallback = (PFN_I2C_CB)LcdI2cErr,
   };
   memset(&lcd, 0, sizeof(lcd));
   I2cOpen(LCD_I2C_DEV, LCD_I2C_BUS_SPEED_HZ, &cbs);
   ItemQInit(&lcd.i2cQ, lcd.i2cQMem, sizeof(lcd.i2cQMem), ITEM_Q_FIFO, sizeof(u8));

   for (u8f lcdRetry = 0; lcdRetry < 5; lcdRetry++)
   {
      GpioClear(LCD_GPIO_RST);
      TimerWaitXUs(LCD_RESET_TIME_US);
      GpioSet(LCD_GPIO_RST);
      TimerWaitXMs(LCD_INIT_TIME_MS);
      LcdCmdQueue(lcdInitCmds, NELEMENTS(lcdInitCmds));
      if (LcdI2cWait())
      {
         break;
      }
   }
   // NOTE: Sometimes the display doesn't fully initialize properly. Ensure that it gets enabled
   //       by resending the command to turn on the display.
   LcdCmd(ST7036_CMD_DISP_CTRL | ST7036_DISP_ON);
   lcd.config.on = true;
   lcd.config.contrast = LCD_DEF_CONTRAST;
}

void LcdConfigSet(LCD_CFG const *pNew)
{
   LCD_CFG *pOld = &lcd.config;
   if ((pOld->on != pNew->on) || (pOld->cursorOn != pNew->cursorOn) || (pOld->blinkOn != pNew->blinkOn))
   {
      LcdCtrl(pNew);
   }
   if (pOld->contrast != pNew->contrast)
   {
      LcdSetContrast(pNew->contrast);
   }
   if (pOld->doubleHeight != pNew->doubleHeight)
   {
      LcdSetDblHeight(pNew->doubleHeight);
   }
   memcpy(pOld, pNew, sizeof(LCD_CFG));
}

LCD_CFG const * LcdConfigGet(void)
{
   return &lcd.config;
}


void LcdSetPos(u8 row, u8 col)
{
   if ((row < LCD_NUM_LINES) && (col < LCD_NUM_CHARS_PER_LINE))
   {
      lcd.cursorUpdate = true;
      lcd.row = row;
      lcd.col = col;

      if (lcd.config.autoUpdate)
      {
         LcdUpdate();
      }
   }
}

void LcdClear(void)
{
   memset(lcd.newText, ' ', sizeof(lcd.newText));
   lcd.row = lcd.col = 0;

   if (lcd.config.autoUpdate)
   {
      LcdUpdate();
   }
}

void LcdPutc(char c)
{
   LcdWrite(&c, 1);
}

void LcdPuts(char const *pSrc)
{
   LcdWrite(pSrc, (u8)strlen(pSrc));
}

void LcdPrintf(char const *pFormat, ...)
{
   char newText[LCD_NUM_CHARS];
   va_list pArg;
   va_start(pArg, pFormat);
   vsnprintf(newText, sizeof(newText), pFormat, pArg); //lint !e534
   va_end(pArg);
   LcdPuts(newText);
}

void LcdWrite(void const *pSrcVoid, u8 len)
{
   char const *pSrc = pSrcVoid;
   while (len > 0)
   {
      u8 pos = LCD_ROW_COL_TO_POS(lcd.row, lcd.col);
      if (*pSrc == '\n')
      {
         // Fill remainder of line with spaces
         memset(&lcd.newText[pos], ' ', LCD_NUM_CHARS_PER_LINE - lcd.col);
         lcd.col = 0;
         if (!lcd.config.doubleHeight)
            lcd.row ^= 1;
      }
      else if (!isprint(*pSrc))
      {}    // ignore it
      else
      {
         lcd.newText[pos] = *pSrc;
         lcd.col++;
         if (lcd.col >= LCD_NUM_CHARS_PER_LINE)
         {
            lcd.col = 0;
            if (!lcd.config.doubleHeight)
               lcd.row ^= 1;
         }
      }
      pSrc++;
      len--;
   }

   if (lcd.config.autoUpdate)
   {
      LcdUpdate();
   }
}


void LcdUpdate(void)
{
   for (u8f line = 0; line < LCD_NUM_LINES; line++)
   {
      u8f offset = line * LCD_NUM_CHARS_PER_LINE;
      if (lcd.i2cErr ||
          (memcmp(&lcd.newText[offset], &lcd.lcdText[offset], LCD_NUM_CHARS_PER_LINE) != MEM_CMP_MATCH))
      {
         char buf[LCD_NUM_CHARS_PER_LINE + 1];
         buf[0] = ST7036_CMD_SET_CGRAM;

         LcdSetCursor((u8)line, 0);
         memcpy(&buf[1], &lcd.newText[offset], LCD_NUM_CHARS_PER_LINE);
         memcpy(&lcd.lcdText[offset], &lcd.newText[offset], LCD_NUM_CHARS_PER_LINE);
         LcdCmdQueue(buf, sizeof(buf));
         lcd.cursorUpdate = true;
      }
   }

   if (lcd.cursorUpdate || lcd.i2cErr)
   {
      LcdSetCursor(lcd.row, lcd.col);
      lcd.cursorUpdate = false;
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void LcdCtrl(LCD_CFG const *pCfg)
{
   u8 cmd = ST7036_CMD_DISP_CTRL;
   if (pCfg->on)
      cmd |= ST7036_DISP_ON;
   if (pCfg->cursorOn)
      cmd |= ST7036_DISP_CURSOR_ON;
   if (pCfg->blinkOn)
      cmd |= ST7036_DISP_CURSOR_BLINK_ON;
   LcdCmd(cmd);
}

static void LcdSetContrast(u8 val)
{
   if (val > ST7036_CONTRAST_MAX)
   {
      val = ST7036_CONTRAST_MAX;
   }
   LcdCmd(ST7036_CMD_ICON_CTRL | ST7036_CONTRAST_ICON(val));
   LcdCmd(ST7036_CMD_CONTRAST | ST7036_CONTRAST_CONT(val));
}

static void LcdSetDblHeight(bool on)
{
   LcdCmd(on? LCD_FUNC_SET_DBL_HT : LCD_FUNC_SET_2_LINE);
   lcd.row = 0;
}

static void LcdSetCursor(u8 row, u8 col)
{
   u8 cmd;
   cmd = (row == 0) ? 0x00 : 0x40;  // (2-line display)
   cmd |= (ST7036_CMD_SET_DDRAM | col);
   LcdCmd(cmd);
}

static void LcdCmd(u8 cmd)
{
   u8 pkt[2] = { 0x00, cmd };
   LcdCmdQueue(pkt, sizeof(pkt));
}

static void LcdCmdQueue(void const *pSrc, u8 len)
{
   ItemQPush(&lcd.i2cQ, pSrc, len);
   if (!I2cIsBusy(LCD_I2C_DEV))
   {
      LcdCmdDequeue();
   }
}

static void LcdCmdDequeue(void)
{
   u8 len = (u8)ItemQPop(lcd.i2cTxBuf, sizeof(lcd.i2cTxBuf), &lcd.i2cQ);
   if (len > 0)
   {
      I2C_TRANSFER_PARAMS i2cTx =
      {
         .destAddr = LCD_I2C_ADDR,
         .pTxData = lcd.i2cTxBuf,
         .txLen = len,
      };

      // NOTE: most cmds require 14-26us delay (clear display takes 590-1080us)
      // experimentation shows that no delay is necessary between commands except
      // the "Clear Display" command
      I2cWriteRead(LCD_I2C_DEV, &i2cTx);
   }
}


static bool LcdI2cWait(void)
{
   return I2cWait(LCD_I2C_DEV);
}

static void LcdI2cDone(void)
{
   lcd.i2cErr = false;
   LcdCmdDequeue();
}

static void LcdI2cErr(void)
{
   lcd.i2cErr = true;
   LcdCmdDequeue();
}

