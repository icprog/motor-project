/*******************************************************************************************
NHD-C0220BiZ-FS(RGB)-FBW-3VM LCD driver
2x20 characters, I2C, FSTN

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __LCD_H__
#define __LCD_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define LCD_NUM_CHARS_PER_LINE	(20)
#define LCD_NUM_LINES				(2)
#define LCD_NUM_CHARS				(LCD_NUM_LINES * LCD_NUM_CHARS_PER_LINE)
#define LCD_POS_TO_ROW(pos)      (((pos) < LCD_NUM_CHARS_PER_LINE)? 0 : 1)
#define LCD_POS_TO_COL(pos)      (((pos) < LCD_NUM_CHARS_PER_LINE)? (pos) : (u8)((pos) - LCD_NUM_CHARS_PER_LINE))
#define LCD_ROW_COL_TO_POS(r,c)  ((u8)(((r) * LCD_NUM_CHARS_PER_LINE) + c))


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   bool on;
   bool cursorOn;
   bool blinkOn;
   bool doubleHeight;
   bool autoUpdate;
   u8 contrast;
}LCD_CFG;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
void LcdInit(void);
void LcdConfigSet(LCD_CFG const *pConfig);
LCD_CFG const * LcdConfigGet(void);

void LcdSetPos(u8 row, u8 col);
void LcdClear(void);
void LcdPutc(char c);
void LcdPuts(char const *pSrc);
void LcdPrintf(char const *pSrc, ...);
void LcdWrite(void const *pSrc, u8 len);

void LcdUpdate(void);

#endif
