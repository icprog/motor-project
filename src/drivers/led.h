/*******************************************************************************************
LED driver with manual PWM control

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#ifndef __LED_H__
#define __LED_H__

#include "includes.h"
#include "color.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#ifndef LED_UPDATE_RATE_MS
#define LED_UPDATE_RATE_MS		(20)
#endif

/*   NOTE: this affects the PWM frequency for brightness/RGB coloring
           --> videos range from 23.97 to 60 Hz. LED flickering is almost unnoticeable above 50 Hz
     NOTE: The LED_UPDATE_RATE_MS must be fast enough to allow PWM resolution at the update rate
*/
#ifndef LED_PWM_FREQ_HZ
#define LED_PWM_FREQ_HZ       (50)
#endif
#define LED_PWM_PERIOD_MS     (1000 / LED_PWM_FREQ_HZ)
#define LED_DUTY_RESOLUTION	((u8)(((u32)(LED_UPDATE_RATE_MS * 100) + (LED_PWM_PERIOD_MS/2)) / LED_PWM_PERIOD_MS))

/* RGB Color Gradient:
There are a few common ways to cycle through RGB colors:
0) Change one RGB component at a time
	a) R=255, G=0, B=0
	b) ramp up B to 255
	c) ramp down R to 0
	d) ramp up G to 255
	e) ramp down B to 0
	f) ramp up R to 255
   g) ramp down G to 0
   --> Tends to "hang" at magenta, cyan and yellow

1) HSB/HSL hue cycling
	HSB = "Hue", "Saturation" Brightness"
	a) Set saturation and brightness as desired
	b) Increment Hue (0-359) around the color wheel
	c) Convert HSB to RGB values

2) Sine-Wave trefoil (aka 3-phase sine wave similar to 3-phase motor control)
	This is similar to HSB/HSL using 3-phase motor-like control. Essentially,
	it uses a sine wave to set each RGB component, with each component phase-shifted
	by 120 degress (same frequency and amplitude).
	a) Store sine table
	b) Lookup each component value, keeping them 120 degrees apart
	c) Add any base offset for brightness
*/
#define LED_COLOR_MODE_RGB		0
#define LED_COLOR_MODE_HSB		1
#define LED_COLOR_MODE_SINE	2

// Define one of the algorithms for RGB color cycling:
#ifndef LED_COLOR_MODE
#define LED_COLOR_MODE			LED_COLOR_MODE_SINE
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef enum
{
   LED_WHITE,
	LED_PURPLE,
	LED_BLUE,
   LED_CYAN,
	LED_GREEN,
	LED_YELLOW,
   LED_ORANGE,
	LED_RED,
	LED_RGB_RED,
	LED_RGB_GREEN,
	LED_RGB_BLUE,
   LED_COUNT,
} LEDS;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
// Core LED functions:
void LedOn(LEDS led);
void LedOff(LEDS led);
void LedToggle(LEDS led);
void LedSet(LEDS led, bool isOn);
void LedSetPwm(LEDS led, u8 dutyPercent);
void LedBlink(LEDS led, u16 onMs, u16 offMs);
void LedBlinkPwm(LEDS led, u16 onMs, u16 offMs, u8 dutyPercent);
void LedSequence(u16 cycleRateMs);

// RGB functions:
void LedRgbSet(COLOR rgb);

/* cycleTimeMs = total time that it will take to cycle all colors
   saturation = amount of color to show (0 = no color, 255 = full color)
	brightness = LED intensity (0 = off, 255 = max) */
void LedRgbCycleSet(u32 cycleTimeMs, u8 saturation, u8 brightness);

// Application level init and update functions
void LedInit(void);
void LedUpdate(void);	// Must be called every 1ms

#endif
