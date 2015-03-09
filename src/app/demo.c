/*******************************************************************************************
Demo Application

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "demo.h"
#include "led.h"
#include "lcd.h"
#include "songTwinkleTwinkle.h"
#include "button.h"
#include "textEditor.h"
#include "usart.h"
#include "sd.h"
#include "file.h"
#include "gpio.h"
#include "timer.h"
#include "rtc.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   char teBuf[100];
   char usartRxBuf[32];
   char usartTxBuf[32];
   u32 tickCount;
   time_t lastTm;
}DEMO;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static DEMO demo;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void DemoTextEditorInit(void);
static void DemoTextEditorUpdate(void);
static TE_KEY DemoTeKeyFromButton(void);
static void DemoNameEntered(void);
static void DemoClockUpdate(void);
static void DemoUsartInit(void);
static void DemoUsartRxIsr(USART_DEV dev);
static void DemoSd(void);
static void DemoSdFind(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void DemoInit(void)
{
   LedSequence(100);
   LedRgbCycleSet(5000, UINT8_MAX, UINT8_MAX);
// DemoTextEditorInit();
// DemoUsartInit();
// DemoSd();
}

void DemoUpdate(void)
{
   DemoClockUpdate();
// DemoTextEditorUpdate();
// if (!TextEditorStatus()->finished)
   {
      MusicPlaySong(&TwinkleTwinkle, 50, 30);
   }
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void DemoTextEditorInit(void)
{
   LCD_CFG lcdCfg =
   {
      .on = true,
      .cursorOn = true,
      .blinkOn = false,
      .doubleHeight = false,
      .autoUpdate = false,
      .contrast = LcdConfigGet()->contrast,  //lint !e446 (leave at default)
   };
   LcdConfigSet(&lcdCfg);

   TE_CFG teCfg =
   {
      .pStrBuf = demo.teBuf,
      .strBufSize = sizeof(demo.teBuf),
      .lcdPos = 0,
      .editStartIndex = 12,    /* Change to UINT16_MAX to make read-only */
      .editEndIndex = sizeof(demo.teBuf),
      .numLcdRows = LCD_NUM_LINES,
      .numLcdCols = LCD_NUM_CHARS_PER_LINE,
      .mode = TE_MODE_ALPHA,
   };
   strcpy(demo.teBuf, "Enter Name:\n ");
   TextEditorInit(&teCfg, 0);
}

static void DemoTextEditorUpdate(void)
{
   if (!TextEditorStatus()->finished)
   {
      TextEditorUpdate(DemoTeKeyFromButton());
      if (TextEditorStatus()->finished)
      {
         DemoNameEntered();
      }
   }
}

static TE_KEY DemoTeKeyFromButton(void)
{
   static const TE_KEY KeyFromPress[BTN_COUNT] =
   {
      [BTN_L]  = TE_KEY_DOWN,
      [BTN_LM] = TE_KEY_UP,
      [BTN_RM] = TE_KEY_LEFT,
      [BTN_R]  = TE_KEY_RIGHT,
   };
   static const TE_KEY KeyFromHold[BTN_COUNT] =
   {
      [BTN_L]  = TE_KEY_DOWN,
      [BTN_LM] = TE_KEY_UP,
      [BTN_RM] = TE_KEY_BKSPACE,
      [BTN_R]  = TE_KEY_ENTER,
   };
   static u32 buttonHoldMs[BTN_COUNT] = {0,0,0,0};

   TE_KEY key = TE_KEY_NONE;
   for (BUTTONS btn = (BUTTONS)0; (btn < BTN_COUNT) && (key == TE_KEY_NONE); btn++)
   {
      if (ButtonWasPressed(btn))
      {
         ButtonPressAck(btn);
         key = KeyFromPress[btn];
      }
      if (!ButtonIsPressed(btn))
         buttonHoldMs[btn] = 500;
      else if ((key == TE_KEY_NONE) &&
               ButtonIsHeldXMs(btn, buttonHoldMs[btn]))
      {
         key = KeyFromHold[btn];
         if ((key == TE_KEY_BKSPACE) || (key == TE_KEY_ENTER))
         {
            ButtonHoldAck(btn);
         }
         else
         {
            buttonHoldMs[btn] += 50;
         }
      }
   }
   return key;
}

static void DemoNameEntered(void)
{
   MusicStop();
   LedSequence(30);
   LcdClear();
   LcdPrintf("Hello %s!\nWelcome to MPG!", &demo.teBuf[TextEditorCfg()->editStartIndex]);

}


static void DemoClockUpdate(void)
{
   if (demo.tickCount++ > MS_TO_TICKS(100))
   {
      demo.tickCount = 0;
      time_t t = time(NULL);
      if (t != demo.lastTm)
      {
         LcdClear();
         struct tm *rtc = localtime(&t);
         LcdPrintf("%04d/%02d/%02d %02d:%02d:%02d",
                  rtc->tm_year + 1900, rtc->tm_mon + 1, rtc->tm_mday,
                  rtc->tm_hour, rtc->tm_min, rtc->tm_sec);
      }
   }
}


static void DemoUsartInit(void)
{
   USART_SETUP usartSetup =
   {
      .baud = 115200,
      .lsbFirst = true,
      .mode = USART_MODE_ASYNC,
      .parity = USART_PARITY_NONE,
      .dataBits = 8,
      .stopBits = USART_STOP_BITS_1,
      .spiMode = SPI_MODE0,   // don't care for UART mode
   };
   UsartOpen(USART0, &usartSetup);

   USART_CBS cbs =
   {
      .pfnRxDoneOrTimeout = (PFN_USART_CB)DemoUsartRxIsr,
      .pfnRxErr = NULL,
      .pfnTxDone = NULL,
   };
   UsartIoctl(USART0, USART_IOCTL_INSTALL_CBS, (u32)&cbs);
   strcpy(demo.usartTxBuf, "MPG USART echo(echo)\r\n");
   UsartWrite(USART0, demo.usartTxBuf, (s32)strlen(demo.usartTxBuf));
   UsartRead(USART0, demo.usartRxBuf, (s32)sizeof(demo.usartRxBuf));
}

static void DemoUsartRxIsr(USART_DEV dev)
{
   u32 len = (u32)UsartIoctl(dev, USART_IOCTL_STATUS, USART_STATUS_RX_REMAIN);
   len = MIN(len, sizeof(demo.usartRxBuf));
   memcpy(demo.usartTxBuf, demo.usartRxBuf, len);
   UsartWrite(dev, demo.usartTxBuf, (s32)len);
   UsartRead(dev, demo.usartRxBuf, (s32)sizeof(demo.usartRxBuf));
}

static void DemoSd(void)
{
   if (GpioIsSet(GPIO_PIN_SD_DETECT))
      return;

	#define SD_TEST_FILE		FILE_SD "test.txt"
	#define SD_TEST_STR		"Testing file write..."
   SdInit();
	if (FileDevIsMounted(FDISK_SD))
	{
		FDISK_STATS sdStats;
		FileDevInfo(FDISK_SD, &sdStats);
		if (sdStats.sectorFreeCount > 0)
		{
			FFILE fil = FileOpen(SD_TEST_FILE, FOPEN_WRITE_PLUS);
			if (fil)
			{
				if (FilePuts(fil, SD_TEST_STR) > 0)
				{
					char readBuf[64];
					memset(readBuf, 0, sizeof(readBuf));
					FileSeek(fil, 0, FSEEK_SET);
					if (FileGets(fil, readBuf, sizeof(readBuf)) != NULL)
					{
						if (strcmp(readBuf, SD_TEST_STR) == STR_CMP_MATCH)
						{
							FileClose(fil);
							DemoSdFind();
							return;
						}
					}
				}
				FileClose(fil);
			}
		}
	}
}


static void DemoSdFind(void)
{
	int res;
	FFIND find;
	if (FileFindFirst(FILE_SD "*.*", TRUE, &find) == FERR_NONE)
	{
		do
		{
			LcdClear();
			LcdPuts(find.fileName);
         LcdUpdate();
			do{} while (!ButtonIsPressed(BTN_L));
			TimerWaitXMs(25);
			do{} while (ButtonIsPressed(BTN_L));
			TimerWaitXMs(25);
			res = FileFindNext(&find);
		} while ((res == FERR_NONE) && (find.foundType != FFIND_TYPE_DONE));
	}
	LcdClear();

	// Example remove mask (CAUTION: will delete all files on the disk!)
	// FileRemove(FILE_SD "*");
}


