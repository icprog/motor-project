/*******************************************************************************************
Sitronix ST7036 Dot Matrix LCD Controller/Driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __ST7036_DEF_H__
#define __ST7036_DEF_H__

#include "types.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
// Instructions
#define ST7036_CMD_CLR_DISP	(BIT0)
#define ST7036_CMD_HOME			(BIT1)
#define ST7036_CMD_ENT_MODE	(BIT2)	// see ST7036_ENT_MODE_* defines
#define ST7036_CMD_DISP_CTRL	(BIT3)	// see ST7036_DISP_* defines
#define ST7036_CMD_DISP_SHIFT	(BIT4)	// see ST7036_SHIFT_* defines
#define ST7036_CMD_FUNC_SET	(BIT5)	// see ST7036_FUNC_* defines
#define ST7036_CMD_SET_CGRAM	(BIT6)
#define ST7036_CMD_SET_DDRAM	(BIT7)

// INSN_EXT1
#define ST7036_CMD_BIAS_SET		(BIT2 | BIT4)	// see ST7036_BIAS_*
#define ST7036_CMD_ICON_RAM		(BIT6)
#define ST7036_CMD_ICON_CTRL		(BIT6 | BIT4)	// see ST7036_ICON_*
#define ST7036_CMD_FLWR_CTRL		(BIT6 | BIT5)	// see ST7036_FLWR_*
#define ST7036_CMD_CONTRAST		(BIT6 | BIT5 | BIT4)	// see ST7036_CONTRAST_CONT()

// INSN_EXT2
#define ST7036_CMD_DBL_HEIGHT		(BIT4)



// ST7036_CMD_ENT_MODE
#define ST7036_ENT_MODE_SHIFT_LEFT	(BIT0 | BIT1)
#define ST7036_ENT_MODE_SHIFT_RIGHT	(BIT0)
#define ST7036_ENT_MODE_INC_DDRAM	(BIT1)

// ST7036_CMD_DISP_CTRL
#define ST7036_DISP_ON					(BIT2)
#define ST7036_DISP_CURSOR_ON			(BIT1)
#define ST7036_DISP_CURSOR_BLINK_ON	(BIT0)

// ST7036_CMD_DISP_SHIFT
#define ST7036_SHIFT_SCREEN			(BIT3)	// else cursor
#define ST7036_SHIFT_RIGHT				(BIT2)

// ST7036_CMD_FUNC_SET
#define ST7036_FUNC_8_BIT				(BIT4)
#define ST7036_FUNC_2_LINE				(BIT3)
#define ST7036_FUNC_DBL_HEIGHT		(BIT2)
#define ST7036_FUNC_INSN_EXT2			(BIT1)
#define ST7036_FUNC_INSN_EXT1			(BIT0)

// ST7036_CMD_BIAS_SET
#define ST7036_BIAS_QUARTER			(BIT3)
#define ST7036_BIAS_FX_3_LINE			(BIT0)

// ST7036_CMD_ICON_CTRL
#define ST7036_ICON_ON					(BIT3)
#define ST7036_ICON_BOOST_ON			(BIT2)
#define ST7036_CONTRAST_ICON(cont)	(((cont) & 0x30) >> 4)

// ST7036_CMD_FLWR_CTRL
#define ST7036_FLWR_ON					(BIT3)
#define ST7036_FLWR_AMP(amp)			((amp) & 0x07)

// ST7036_CMD_CONTRAST
#define ST7036_CONTRAST_CONT(cont)	((cont) & 0x0F)
#define ST7036_CONTRAST_MAX			(63)	// 4 LSB here, 2 MSB in CMD_ICON_CTRL

// ST7036_CMD_DBL_HEIGHT
#define ST7036_DBL_HEIGHT_TOP			(BIT3)	// see ST7036_CMD_FUNC_SET as well


/**************************************************************************
 *                                  Types
 **************************************************************************/
/**************************************************************************
 *                                  Prototypes
 **************************************************************************/


#endif
