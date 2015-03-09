/*******************************************************************************************
Atmel SAM3U RTC driver

This is free, public domain software and there is NO WARRANTY.
No restriction on use. You can use, modify and redistribute it for
personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.

Sheldon Patterson
********************************************************************************************/

#include "rtc.h"
#include "bsp.h"
#include "version.h"


/**************************************************************************
 *                                  Constants
 **************************************************************************/
//#define RTC_WPMR           (*(AT91_REG *)(0x400E1344))
//#define RTC_UNLOCK()       {RTC_WPMR = 0x525443;}
//#define RTC_LOCK()         {RTC_WPMR = 0x535443;}
#define RTC                ((AT91PS_RTC)AT91C_BASE_RTC)


/**************************************************************************
 *                                  Types
 **************************************************************************/
typedef struct
{
   struct tm rtcTm;
   bool isValid;
}RTC_VAR;


/**************************************************************************
 *                                  Variables
 **************************************************************************/
static RTC_VAR rtc;


/**************************************************************************
 *                                  Prototypes
 **************************************************************************/
static void RtcRegToTm(struct tm *pTm);
static void RtcTmToReg(struct tm const *pTm);
static u8 RtcByteToBcd(u8 val);
static u8 RtcBcdToByte(u8 bcd);


/**************************************************************************
 *                                  Global Functions
 **************************************************************************/
void RtcInit(void)
{
   time_t buildTime = VersionGetBuildTime();
   if (RtcGet() < buildTime)
   {
      RtcSet(buildTime);
      rtc.isValid = false;
   }
   else
   {
      rtc.isValid = true;
   }
}

bool RtcIsValid(void)
{
   return rtc.isValid;
}

time_t RtcGet(void)
{
   return mktime(RtcGetTm());
}

void RtcSet(time_t newTime)
{
   struct tm *pTmStr = localtime(&newTime);
   RtcSetTm(pTmStr);
}

struct tm * RtcGetTm(void)
{
   RtcRegToTm(&rtc.rtcTm);
   return &rtc.rtcTm;
}

void RtcSetTm(struct tm const *pTm)
{
   if (rtc.isValid)
   {
      do{} while (!(RTC->RTC_SR & AT91C_RTC_SECEV));
   }
   RTC->RTC_CR |= (AT91C_RTC_UPDTIM | AT91C_RTC_UPDCAL);
   do{} while (!(RTC->RTC_SR & AT91C_RTC_ACKUPD));
   RTC->RTC_SCCR = (AT91C_RTC_ACKUPD | AT91C_RTC_SECEV | AT91C_RTC_TIMEV | AT91C_RTC_CALEV);
   RtcTmToReg(pTm);
   RTC->RTC_CR &= ~(AT91C_RTC_UPDTIM | AT91C_RTC_UPDCAL);
   rtc.isValid = true;
}


/**************************************************************************
 *                                 Private Functions
 **************************************************************************/
static void RtcRegToTm(struct tm *pTm)
{
   u32 date = RTC->RTC_CALR;
   u32 tim  = RTC->RTC_TIMR;

   pTm->tm_year = (int)RtcBcdToByte((date & AT91C_RTC_YEAR)  >>  8) + 100;
   pTm->tm_mon  = (int)RtcBcdToByte((date & AT91C_RTC_MONTH) >> 16) - 1;
   pTm->tm_mday = (int)RtcBcdToByte((date & AT91C_RTC_DATE)  >> 24);
   pTm->tm_wday = (int)            ((date & AT91C_RTC_DAY)   >> 21);
   if (pTm->tm_wday == 0x07)
      pTm->tm_wday = 0;
   pTm->tm_isdst = -1;

   pTm->tm_hour = (int)RtcBcdToByte((tim & AT91C_RTC_HOUR) >> 16);
   pTm->tm_min  = (int)RtcBcdToByte((tim & AT91C_RTC_MIN)  >> 8 );
   pTm->tm_sec  = (int)RtcBcdToByte((tim & AT91C_RTC_SEC)       );
}

static void RtcTmToReg(struct tm const *pTm)
{
   u32 wday = (pTm->tm_wday == 0)? (u32)0x07 : (u32)pTm->tm_wday;
   RTC->RTC_CALR = (((u32)RtcByteToBcd((u8)(pTm->tm_year - 100)) <<  8) | 0x20UL |
                    ((u32)RtcByteToBcd((u8)(pTm->tm_mon + 1   )) << 16) |
                    ((u32)RtcByteToBcd((u8)(pTm->tm_mday      )) << 24) |
                    (wday << 21));

   RTC->RTC_TIMR = (((u32)RtcByteToBcd((u8)pTm->tm_hour) << 16) |
                    ((u32)RtcByteToBcd((u8)pTm->tm_min ) << 8 ) |
                    ((u32)RtcByteToBcd((u8)pTm->tm_sec )      ));
}

static u8 RtcByteToBcd(u8 val)
{
   u8 tens = 0;
   while (val >= 10)
   {
      tens++;
      val -= 10;
   }
   return (u8)((tens << 4) | val);
}

static u8 RtcBcdToByte(u8 bcd)
{
   u8 val = (u8)((((bcd & 0xF0) >> 4) * 10) + (bcd & 0x0F));
   return val;
}

