/*******************************************************************************************
LED driver with manual PWM control

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "led.h"
#include "gpio.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
#define LED_IS_VALID(led)		((u8)led < (u8)LED_COUNT)
#define LED_ASSERT_VALID(led)	{if (!LED_IS_VALID(led))	return;}

static const GPIO_PIN ledPins[LED_COUNT] =
{
	[LED_RGB_RED] = GPIO_PIN_LCD_BL_RED,
	[LED_RGB_GREEN] = GPIO_PIN_LCD_BL_GREEN,
	[LED_RGB_BLUE] = GPIO_PIN_LCD_BL_BLUE,
	[LED_PURPLE] = GPIO_PIN_LED_PURPLE,
	[LED_WHITE] = GPIO_PIN_LED_WHITE,
	[LED_BLUE] = GPIO_PIN_LED_BLUE,
	[LED_RED] = GPIO_PIN_LED_RED,
	[LED_GREEN] = GPIO_PIN_LED_GREEN,
	[LED_ORANGE] = GPIO_PIN_LED_ORANGE,
	[LED_YELLOW] = GPIO_PIN_LED_YELLOW,
	[LED_CYAN] = GPIO_PIN_LED_CYAN,
};

#if LED_COLOR_MODE == LED_COLOR_MODE_SINE
static const u8 ledSinTable[128] =
{
	0,1,2,2,3,4,5,5,6,7,8,9,9,10,11,12,12,13,14,14,15,16,16,17,
	18,18,19,20,20,21,21,22,23,23,24,24,25,25,26,26,27,27,27,28,
	28,29,29,29,30,30,30,30,31,31,31,31,31,32,32,32,32,32,32,32,
	32,32,32,32,32,32,32,32,31,31,31,31,31,30,30,30,30,29,29,29,
	28,28,27,27,27,26,26,25,25,24,24,23,23,22,21,21,20,20,19,18,
	18,17,16,16,15,14,14,13,12,12,11,10,9,9,8,7,6,5,5,4,3,2,2,1
};
#endif


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
	u8 duty;
	u8 cycleCount;
	u16 blinkTickCount;
	u16 blinkOnTicks;
	u16 blinkOffTicks;
}LED_STATE;

typedef struct
{
	u16 cycleCountTicks;	// counts number of times RGB update has been called
	u16 cycleRateTicks;	// user-specified cycle rate
	u8 stepSize;		   // controls how fast to sweep the RGB values
	u8 saturation;		   // 0 = no color, 255 = full color
	u8 brightness;		   // 255 = max brightness
	u16 state;			   // Which state of the algorithm we're at
}LED_RGB_STATE;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static LED_STATE ledState[LED_COUNT];
static LED_RGB_STATE ledRgbState;

/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void LedRgbUpdate(void);
static void LedIoSetState(LEDS led, bool isOn);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void LedOn(LEDS led)
{
	LED_ASSERT_VALID(led);
	ledState[led].duty = 100;
	ledState[led].blinkOnTicks = 0;
}

void LedOff(LEDS led)
{
	LED_ASSERT_VALID(led);
	ledState[led].duty = 0;
	ledState[led].blinkOnTicks = 0;
}

void LedToggle(LEDS led)
{
  if(GpioIsSet(ledPins[led])) LedOff(led);
  else LedOn(led);
}

void LedSet(LEDS led, bool isOn)
{
	if (isOn)
	{
		LedOn(led);
	}
	else
	{
		LedOff(led);
	}
}

void LedSetPwm(LEDS led, u8 dutyPercent)
{
	ledState[led].duty = dutyPercent;
	ledState[led].cycleCount = 0;

	if (IN_RANGE((u8f)led, LED_RGB_RED, LED_RGB_BLUE))
	{
		ledRgbState.cycleRateTicks = 0;
	}
}

void LedBlink(LEDS led, u16 onMs, u16 offMs)
{
   ledState[led].duty = 100;
   ledState[led].blinkOnTicks = onMs / LED_UPDATE_RATE_MS;
   ledState[led].blinkOffTicks = offMs / LED_UPDATE_RATE_MS;
   ledState[led].blinkTickCount = 0;
}

void LedBlinkPwm(LEDS led, u16 onMs, u16 offMs, u8 dutyPercent)
{
	LedBlink(led, onMs, offMs);
	LedSetPwm(led, dutyPercent);
}

void LedSequence(u16 cycleRateMs)
{
   u16 cycleRateTicks = (cycleRateMs / LED_UPDATE_RATE_MS);
   cycleRateMs *= (u16)LED_RED;  // number of LEDs
   for (LEDS led = (LEDS)0; led <= LED_RED; led++)
   {
      LedBlink(led, cycleRateMs, cycleRateMs);
      ledState[led].blinkTickCount = (led == (LEDS)0)
         ? 0
         : (u16)(cycleRateTicks * (u16)((u16)LED_RED + ((u16)LED_RED - (u16)led)));
   }
}


// RGB functions:
void LedRgbSet(COLOR rgb)
{
	LedSetPwm(LED_RGB_RED,	(u8)((u32)rgb >> 16));	// bits 23-16
	LedSetPwm(LED_RGB_GREEN,(u8)((u32)rgb >>  8));	// bits 15-8
	LedSetPwm(LED_RGB_BLUE, (u8)((u32)rgb      ));	// bits  7-0
}

void LedRgbCycleSet(u32 cycleTimeMs, u8 saturation, u8 brightness)
{
   u32 cycleTimeTicks = cycleTimeMs / LED_UPDATE_RATE_MS;
	if (saturation == 0)
	{
		LedRgbSet(COLOR_Black);
	}
	else if (cycleTimeTicks > 0)
	{
		u32 numSteps;
#if LED_COLOR_MODE == LED_COLOR_MODE_RGB
		numSteps = (u32)((brightness - (UINT8_MAX - saturation)) + 1) * 6;	// (6 = up + down for each RGB)
#elif LED_COLOR_MODE == LED_COLOR_MODE_HSB
		numSteps = 360;		// 360 degrees in a circle
#elif LED_COLOR_MODE == LED_COLOR_MODE_SINE
		numSteps = sizeof(ledSinTable);
		// saturation isn't used
		// SINE wave is scaled from 0-32, so we can multiply that by 8
		brightness = (u8)(((u32)brightness * 8uL) / 256);
#endif
		if (cycleTimeTicks >= numSteps)
		{
			// small steps at some slow rate
			ledRgbState.cycleRateTicks = (u16)(cycleTimeTicks / numSteps);
			ledRgbState.stepSize = 1;
		}
		else
		{
			// bigger steps at faster rate
			ledRgbState.cycleRateTicks = 1;
			ledRgbState.stepSize = (u8)(numSteps / cycleTimeTicks);
		}
		ledRgbState.cycleCountTicks = 0;
		ledRgbState.saturation = saturation;
		ledRgbState.brightness = brightness;
		ledRgbState.state = 0;
	}
	else
	{
		ledRgbState.cycleRateTicks = 0;
	}
}


// Application level init and update functions
void LedInit(void)
{
	memset(ledState, 0, sizeof(ledState));
	memset(&ledRgbState, 0, sizeof(ledRgbState));
	for (LEDS led = (LEDS)0; led < LED_COUNT; led++)
   {
      LedIoSetState(led, false);
   }
}

void LedUpdate(void)
{
	if ((ledRgbState.cycleRateTicks > 0) &&
		 (++ledRgbState.cycleCountTicks > ledRgbState.cycleRateTicks))
	{
			ledRgbState.cycleCountTicks = 0;
			LedRgbUpdate();
	}

	for (LEDS led = (LEDS)0; led < LED_COUNT; led++)
	{
		LED_STATE *pLed = &ledState[led];

		// blinking control first:
		if (pLed->blinkTickCount++ > pLed->blinkOnTicks)
		{
			// off-cycle of blinking
			if (pLed->blinkTickCount > (pLed->blinkOnTicks + pLed->blinkOffTicks))
			{
				pLed->blinkTickCount = 0;
			}
			LedIoSetState(led, false);
		}
		else
		{
			// on-cycle of blinking
			if (pLed->blinkOnTicks == 0)
			{
				pLed->blinkTickCount = 0;
			}

			// PWM control during on-cycle of blinking:
			pLed->cycleCount += LED_DUTY_RESOLUTION;
			if (pLed->cycleCount >= (101 - LED_DUTY_RESOLUTION))
			{
				pLed->cycleCount = 0;
			}
			LedIoSetState(led, (bool)(pLed->duty > pLed->cycleCount));
		}
	}
}



/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void LedRgbUpdate(void)
{
#if LED_COLOR_MODE == LED_COLOR_MODE_RGB
	u8 stepsRemaining = ledRgbState.stepSize;

	// RGB sequence: R down, G up, B down, R up, G down, B up
	// --> Can be reduced to RGB,RGB (alternating down/up)
	do
	{
		// NOTE: This works because the enum has RGB defined first (in that order)
		LEDS led = (LEDS)ledRgbState.state;
		if ((u8f)led > (u8f)LED_RGB_BLUE)
			led = (LEDS)((u8f)led - 3);

		// alternate between decrementing and incrementing values
		if (ledRgbState.state & BIT0)
		{
			u16 newDuty = (u16)ledState[led].duty + (u16)stepsRemaining;
			if (newDuty >= (u16)ledRgbState.brightness)
			{
				// max'd out. Continue with the next LED...
				stepsRemaining = (u8)(newDuty - (u16)ledRgbState.brightness);
				ledState[led].duty = ledRgbState.brightness;
				ledRgbState.state++;
				if (ledRgbState.state >= 6)
				{
					ledRgbState.state = 0;
				}
			}
			else
			{
				stepsRemaining = 0;
				ledState[led].duty = (u8)newDuty;
			}
		}
		else
		{
			u8 newDuty = ledState[led].duty >= stepsRemaining?
				(u8)(ledState[led].duty - stepsRemaining) : 0;
			u8 minBrightness = (u8)(UINT8_MAX - ledRgbState.saturation);
			if (newDuty <= minBrightness)
			{
				// min'd out. Continue with the next LED...
				stepsRemaining = (u8)(minBrightness - newDuty);
				ledState[led].duty = minBrightness;
				ledRgbState.state++;
			}
			else
			{
				stepsRemaining = 0;
				ledState[led].duty = (u8)newDuty;
			}
		}

	} while ((stepsRemaining > 0) && (stepsRemaining < ledRgbState.stepSize));

#elif LED_COLOR_MODE == LED_COLOR_MODE_HSB
	u32 r,g,b;
	u32 val = (u32)ledRgbState.brightness;
	u32 base = (u32)(((255uL - (u32)ledRgbState.saturation) * val) >> 8);
	u32 hue;
	ledRgbState.state += ledRgbState.stepSize;
	if (ledRgbState.state >= 360)
		ledRgbState.state = 0;
	hue = ledRgbState.state;

	switch(hue / 60)
	{
	default:
	case 0:
		r = val;
		g = (((val - base) * hue) / 60) + base;
		b = base;
		break;
	case 1:
		r = (((val - base) * (60 - (hue % 60))) / 60) + base;
		g = val;
		b = base;
		break;
	case 2:
		r = base;
		g = val;
		b = (((val - base) * (hue % 60)) / 60) + base;
		break;
	case 3:
		r = base;
		g = (((val - base) * (60 - (hue % 60))) / 60) + base;
		b = val;
		break;
	case 4:
		r = (((val - base) * (hue % 60)) / 60) + base;
		g = base;
		b = val;
		break;
	case 5:
		r = val;
		g = base;
		b = (((val - base) *(60 - (hue % 60))) / 60) + base;
		break;
	}
	ledState[LED_RGB_RED].duty = (u8)r;
	ledState[LED_RGB_GREEN].duty = (u8)g;
	ledState[LED_RGB_BLUE].duty = (u8)b;

#elif LED_COLOR_MODE == LED_COLOR_MODE_SINE
	u8 brightness = ledRgbState.brightness;
	ledRgbState.state += ledRgbState.stepSize;
	// Note: each LED is 120 degress out of phase.
	ledState[LED_RGB_RED].duty = (u8)(ledSinTable[ledRgbState.state & 0x7F] * brightness);
	ledState[LED_RGB_GREEN].duty = (u8)(ledSinTable[(ledRgbState.state+0x55) & 0x7F] * brightness);
	ledState[LED_RGB_BLUE].duty = (u8)(ledSinTable[(ledRgbState.state+0xAA) & 0x7F] * brightness);
#endif
}


static void LedIoSetState(LEDS led, bool isOn)
{
   // NOTE: all LEDs are active high
   GpioSetState(ledPins[led], isOn);
}

