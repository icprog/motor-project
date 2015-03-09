/*******************************************************************************************
Character display, 4-button text editor

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/


#include "textEditor.h"
#include "lcd.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
static char TeNextFull        (char, s8);
static char TeNextAlpha       (char, s8);
static char TeNextAlphaUpper  (char, s8);
static char TeNextAlphaLower  (char, s8);
static char TeNextNumeric     (char, s8);
static char TeNextAlphaNumeric(char, s8);

static const PFN_NEXT_CHAR TeModeNextCharPfns[TE_MODE_COUNT-1] =
{
   [TE_MODE_FULL]          = TeNextFull,
   [TE_MODE_ALPHA]         = TeNextAlpha,
   [TE_MODE_ALPHA_UPPER]   = TeNextAlphaUpper,
   [TE_MODE_ALPHA_LOWER]   = TeNextAlphaLower,
   [TE_MODE_NUMERIC]       = TeNextNumeric,
   [TE_MODE_ALPHA_NUMERIC] = TeNextAlphaNumeric,
// [TE_MODE_CUSTOM]        = NULL,
};


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   u16 row;
   u16 col;
}TE_POSN;

typedef struct
{
   TE_CFG cfg;
   TE_STATUS status;
}TE;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static TE te;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static char TeNextRange(char c, s8 dir, char low, char high);
static void TeProcessKey(TE_KEY key);
static bool TeIsReadOnly(void);
static bool TeHasReadOnly(void);
static char * TeCharPtr(void);
static void TeFinished(void);
static void TeCharDown(void);
static void TeCharUp(void);
static void TeInsert(char c);
static void TeBackSpace(void);
static void TeCursorLeft(void);
static void TeCursorRight(void);
static void TeCursorDown(void);
static void TeCursorUp(void);
static TE_POSN TePtFromStrIndex(u16 index);
static u16 TeStrIndexFromPt(u16 row, u16 col);
static u16 TeStrLenForLine(u16 strIndex);
static void TeDisplayIndexUpdate(void);
static u16 TeIndexPreviousLine(u16 strIndex);
static void TeDraw(void);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void TextEditorInit(TE_CFG const *pTe, u16 cursorIndex)
{
   memcpy(&te.cfg, pTe, sizeof(TE_CFG));
   te.status.finished = false;
   te.status.cursorIndex = (cursorIndex < te.cfg.editStartIndex)? te.cfg.editStartIndex : cursorIndex;
   te.status.strLen = (u16)strlen((char *)te.cfg.pStrBuf);
   te.status.displayIndex = 0;
   te.status.anchorCol = TePtFromStrIndex(cursorIndex).col;
   if ((te.cfg.mode == TE_MODE_CUSTOM) && !te.cfg.pfnNextChar)
      te.cfg.mode = TE_MODE_FULL;

   if (te.cfg.editEndIndex >= te.cfg.strBufSize)
      te.cfg.editEndIndex = (u16)(te.cfg.strBufSize-1);
   char *pCh = (char *)te.cfg.pStrBuf + (te.cfg.strBufSize-1);
   *pCh = '\0';
   TeDraw();
}

void TextEditorUpdate(TE_KEY key)
{
   if (te.status.finished)
      return;

   TeProcessKey(key);
}

TE_STATUS const * TextEditorStatus(void)
{
   return &te.status;
}

TE_CFG const * TextEditorCfg(void)
{
   return &te.cfg;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static char TeNextFull(char c, s8 dir)
{
   return TeNextRange(c, dir, 0x20, 0x7E);
}

static char TeNextAlpha(char c, s8 dir)
{
   do
   {
      c = (char)(c + dir);
   }while (!isalpha(c));
   return c;
}

static char TeNextAlphaUpper  (char c, s8 dir)
{
   return TeNextRange(c, dir, 'A', 'Z');
}

static char TeNextAlphaLower  (char c, s8 dir)
{
   return TeNextRange(c, dir, 'a', 'z');
}

static char TeNextNumeric     (char c, s8 dir)
{
   return TeNextRange(c, dir, '0', '9');
}

static char TeNextAlphaNumeric(char c, s8 dir)
{
   do
   {
      c = (char)(c + dir);
   }while (!isalnum(c));
   return c;
}

static char TeNextRange(char c, s8 dir, char low, char high)
{
   c = (char)(c + dir);
   if (c > high)
      c = low;
   else if (c < low)
      c = high;
   return c;
}

static char TeNextChar(char c, s8 dir)
{
   if (te.cfg.mode >= TE_MODE_CUSTOM)
   {
      return te.cfg.pfnNextChar(c, dir);
   }
   else
   {
      return TeModeNextCharPfns[te.cfg.mode](c, dir);
   }
}

static void TeProcessKey(TE_KEY key)
{
   switch (key)
   {
   case TE_KEY_DOWN:    TeIsReadOnly()? TeCursorDown() : TeCharDown();  break;
   case TE_KEY_UP:      TeIsReadOnly()? TeCursorUp()   : TeCharUp();    break;
   case TE_KEY_LEFT:    TeCursorLeft();                                 break;
   case TE_KEY_RIGHT:   TeCursorRight();                                break;
   case TE_KEY_ENTER:   TeFinished();                                   break;
   case TE_KEY_BKSPACE: TeBackSpace();                                  break;
   default:             return;
   }
   TeDraw();
}

static bool TeIsReadOnly(void)
{
   return ((te.cfg.editStartIndex > te.cfg.editEndIndex) || (te.cfg.editStartIndex >= te.cfg.strBufSize));
}

static bool TeHasReadOnly(void)
{
   return ((te.cfg.editStartIndex != 0) || (te.cfg.editEndIndex < te.cfg.strBufSize));
}

static char * TeCharPtr(void)
{
   return (char *)te.cfg.pStrBuf + te.status.cursorIndex;
}

static void TeFinished(void)
{
   if (TeIsReadOnly() || ((te.status.cursorIndex + 1) >= te.status.strLen))
   {
      te.status.finished = true;
   }
   else
   {
      TeInsert(TeNextChar(*TeCharPtr() - 1, 1));
   }
}

static void TeCharDown(void)
{
   char *pCh = TeCharPtr();
   if (*pCh == '\0')
   {
      pCh[0] = TeNextChar(0, -1);
      pCh[1] = '\0';
   }
   else
   {
      *pCh = TeNextChar(*pCh, -1);
   }
}

static void TeCharUp(void)
{
   char *pCh = TeCharPtr();
   if (*pCh == '\0')
   {
      pCh[0] = TeNextChar(0, 1);
      if (pCh[0] == ' ')
         pCh[0] = pCh[0] + 1;
      pCh[1] = '\0';
   }
   else
   {
      *pCh = TeNextChar(*pCh, 1);
   }
}

static void TeInsert(char c)
{
   if ((te.status.cursorIndex + 1) < te.cfg.editEndIndex)
   {
      char *pCh = TeCharPtr();
      memmove(pCh + 1, pCh, te.cfg.strBufSize - te.status.cursorIndex);
      te.status.strLen++;
      *pCh = c;
      TeDisplayIndexUpdate();
   }
}

static void TeBackSpace(void)
{
   if (!TeIsReadOnly() && (te.status.cursorIndex > te.cfg.editStartIndex))
   {
      te.status.cursorIndex--;
      char *pCh = TeCharPtr();
      memmove(pCh, pCh+1, te.cfg.editEndIndex - te.status.cursorIndex);
      te.status.strLen--;
		te.status.displayIndex = 0;
      TeDisplayIndexUpdate();
   }
}

static void TeCursorLeft(void)
{
   while (te.status.cursorIndex > (TeHasReadOnly()? te.cfg.editStartIndex : 0))
   {
      te.status.cursorIndex--;
      if (isprint(*TeCharPtr()))
         break;
   }
   te.status.anchorCol = TePtFromStrIndex(te.status.cursorIndex).col;
   TeDisplayIndexUpdate();
}

static void TeCursorRight(void)
{
   while (te.status.cursorIndex < (TeHasReadOnly()? te.cfg.editEndIndex : te.cfg.strBufSize))
   {
      char *pCh = TeCharPtr();
      if ((pCh[0] == '\0') || (pCh[1] == '\0'))
      {
         if (!TeIsReadOnly())
         {
            if (pCh[0] != '\0')
               te.status.cursorIndex++;
            TeInsert(TeNextChar(*pCh - 1, 1));
         }
         break;
      }
      else
      {
         pCh++;
         te.status.cursorIndex++;
         if (isprint(*pCh))
            break;
      }
   }
   te.status.anchorCol = TePtFromStrIndex(te.status.cursorIndex).col;
   TeDisplayIndexUpdate();
}

static void TeCursorDown(void)
{
   if (te.status.cursorIndex < te.cfg.strBufSize)
   {
      TE_POSN posn = TePtFromStrIndex(te.status.cursorIndex);
      te.status.cursorIndex = TeStrIndexFromPt(posn.row+1, te.status.anchorCol);
      TeDisplayIndexUpdate();
   }
}

static void TeCursorUp(void)
{
   if ((te.status.cursorIndex > 0) && (te.cfg.numLcdRows > 1))
   {
      TE_POSN posn = TePtFromStrIndex(te.status.cursorIndex);
      if (posn.row > 0)
      {
         te.status.cursorIndex = TeStrIndexFromPt((u16)(posn.row-1), te.status.anchorCol);
      }
      else if (te.status.displayIndex > 0)
      {
         te.status.displayIndex = TeIndexPreviousLine(te.status.displayIndex);
         te.status.cursorIndex = TeStrIndexFromPt(0, te.status.anchorCol);
      }
   }
}

static TE_POSN TePtFromStrIndex(u16 index)
{
	TE_POSN posn = {0,0};
   u16 strIndex = te.status.displayIndex;
   char const *pCh = (char const *)te.cfg.pStrBuf + strIndex;
   while (*pCh != '\0' && (strIndex < index))
   {
      u16 lineLen = TeStrLenForLine(strIndex);
      if (((lineLen + strIndex) <= index) &&
          ((lineLen + strIndex) < te.status.strLen))
      {
         posn.row++;
      }
      else
      {
         posn.col = (u16)(index - strIndex);
      }
      strIndex += lineLen;
      pCh += lineLen;
   }
   return posn;
}

static u16 TeStrIndexFromPt(u16 row, u16 col)
{
   u16 strIndex = te.status.displayIndex;
   while (row-- > 0)
   {
      strIndex += TeStrLenForLine(strIndex);
   }

   u16 lastLineLen = TeStrLenForLine(strIndex);
   if (*((char *)te.cfg.pStrBuf + strIndex + (u16)(lastLineLen-1)) == '\n')
      lastLineLen--;
   strIndex += MIN(col, lastLineLen);
   if ((strIndex > 0) && (strIndex >= te.status.strLen))
      strIndex = te.status.strLen - 1;
   return strIndex;
}

static u16 TeStrLenForLine(u16 strIndex)
{
   s16 colAtWordBreak = -1;
   u16 col = 0;
   char const *pCh = (char const *)te.cfg.pStrBuf + strIndex;
   while ((*pCh != '\0') && (col < te.cfg.numLcdCols))
   {
      if (*pCh == '\n')
      {
         col++;
         colAtWordBreak = (s16)col;
         break;
      }
      else if (!isgraph(*pCh))
      {
         colAtWordBreak = (s16)col + 1;
      }

      pCh++;
      col++;
   }
   return ((*pCh != '\0') && (colAtWordBreak >= 0))? (u16)colAtWordBreak : col;
}

static void TeDisplayIndexUpdate(void)
{
   while ((te.status.displayIndex > 0) && (te.status.cursorIndex < te.status.displayIndex))
   {
      te.status.displayIndex = (te.cfg.numLcdRows == 1)
         ? te.status.displayIndex-1
         : TeIndexPreviousLine(te.status.displayIndex);
   }
   while (TePtFromStrIndex(te.status.cursorIndex).row >= te.cfg.numLcdRows)
   {
      te.status.displayIndex += (te.cfg.numLcdRows == 1)
         ? 1
         : TeStrLenForLine(te.status.displayIndex);
   }
}

static u16 TeIndexPreviousLine(u16 strIndex)
{
   u16 index = 0;
   u16 lineLen = 0;
   do
   {
      index += lineLen;
      lineLen = TeStrLenForLine(index);
   } while (index + lineLen < strIndex);
   return index;
}

static void TeDraw(void)
{
   LcdClear();
   u16 index = te.status.displayIndex;
   for (u16 row = 0; row < te.cfg.numLcdRows; row++)
   {
      LcdSetPos((u8)row, 0);
      for (u16 lineLen = TeStrLenForLine(index); lineLen > 0; lineLen--)
      {
         char ch = *((char const *)te.cfg.pStrBuf + index);
         index++;
         if (isprint(ch))
         {
            LcdPutc(ch);
         }
      }
   }
   TE_POSN posn = TePtFromStrIndex(te.status.cursorIndex);
   LcdSetPos((u8)posn.row, (u8)posn.col);
}

