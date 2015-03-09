/*******************************************************************************************
Character display, 4-button text editor

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __TEXT_EDITOR_H__
#define __TEXT_EDITOR_H__

#include "includes.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   TE_KEY_DOWN,
   TE_KEY_UP,
   TE_KEY_LEFT,
   TE_KEY_RIGHT,
   TE_KEY_ENTER,
   TE_KEY_BKSPACE,
   TE_KEY_NONE,
   TE_KEY_COUNT
}TE_KEY;

typedef enum
{
   TE_MODE_FULL,
   TE_MODE_ALPHA,
   TE_MODE_ALPHA_UPPER,
   TE_MODE_ALPHA_LOWER,
   TE_MODE_NUMERIC,
   TE_MODE_ALPHA_NUMERIC,
   TE_MODE_CUSTOM,
   TE_MODE_COUNT
}TE_MODE;

typedef char (*PFN_NEXT_CHAR)(char, s8);  // given a char and +/- 1, return next char in sequence
typedef struct
{
   PFN_NEXT_CHAR pfnNextChar;
   void  *pStrBuf;
   u16   strBufSize;

   u16   lcdPos;
   u16   editStartIndex;
   u16   editEndIndex;
   u16   numLcdRows;
   u16   numLcdCols;
   TE_MODE mode;
}TE_CFG;

typedef struct
{
   bool  finished;
   u16   strLen;
   u16   cursorIndex;
   u16   displayIndex;
   u16   anchorCol;
}TE_STATUS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
/* This API allows the text editor to be configured with the "Init" call
   Then every time a key is pressed, the Update() routine should be called with that key
   The TextEditorStatus() can be polled to know when the user is finished editing

Example (prompt user to enter name. User can only edit past the "Enter Name" string:
   pStrBuf = RAM for "Enter Name" plus room for user to enter name
   strBufSize = size of RAM
   lcdPos = Where to start writing the string
   editStartPos - editEndPos = Where to allow edits. Can be set > strBufSize to make it readonly
*/
void TextEditorInit(TE_CFG const *pTe, u16 cursorIndex);
void TextEditorUpdate(TE_KEY key);
TE_STATUS const * TextEditorStatus(void);
TE_CFG const * TextEditorCfg(void);

#endif
