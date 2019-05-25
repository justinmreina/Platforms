//-----------------------------------------------------------------------------
// \file    evmomapl138_rtc.c
// \brief   implementation of a rtc driver for OMAP-L138.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_rtc.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// pinmux defines.

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the rtc.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...rtc ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t RTC_init(void)
{
   // unlock the rtc regs.
   RTC->KICK0 = RTCKICK0_UNLOCK;
   RTC->KICK1 = RTCKICK1_UNLOCK;
   
   // set software reset on RTC module and wait for 3 clock crystal ticks.
   SETBIT(RTC->OSC, OSC_SWRESET);
   USTIMER_delay(500);
   
   // init time...12:00:00 Sunday January 1.
   RTC->SECOND = 0x00;
   RTC->MINUTE = 0x00;
   RTC->HOUR = 0x12;
   RTC->DAY = 0x01;
   RTC->MONTH = 0x01;
   RTC->YEAR = 0x01;
   RTC->DOTW = 0x00;
   
   // set the rtc to run.
   SETBIT(RTC->CTRL, RTC_RUN);
   
   // lock the rtc regs.
//    RTC->KICK0 = RTCKICK0_LOCK;
//    RTC->KICK1 = RTCKICK1_LOCK;

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   returns the current rtc time.
//
// \param   rtc_time_t *time - pointer to struct for rtc time.
//
// \return  uint32_t
//    ERR_NO_ERROR
//-----------------------------------------------------------------------------
uint32_t RTC_getTime(rtc_time_t *time)
{
   // wait for rtc to not be busy.
   while (CHKBIT(RTC->STATUS, RTC_BUSY)) {}
   
   // get the time.
   time->sec = (uint8_t)RTC->SECOND;
   time->min = (uint8_t)RTC->MINUTE;
   time->hour = (uint8_t)RTC->HOUR;
   time->day = (uint8_t)RTC->DAY;
   time->month = (uint8_t)RTC->MONTH;
   time->year = (uint8_t)RTC->YEAR;
   time->dayofweek = (uint8_t)RTC->DOTW;
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   sets the current rtc time.
//
// \param   rtc_time_t *time - pointer to struct for rtc time.
//
// \return  uint32_t
//    ERR_NO_ERROR
//-----------------------------------------------------------------------------
uint32_t RTC_setTime(rtc_time_t *in_time)
{
   // wait for rtc to not be busy.
   while (CHKBIT(RTC->STATUS, RTC_BUSY)) {}
   
   // set time to input.
   RTC->SECOND = in_time->sec;
   RTC->MINUTE = in_time->min;
   RTC->HOUR = in_time->hour;
   RTC->DAY = in_time->day;
   RTC->MONTH = in_time->month;
   RTC->YEAR = in_time->year;
   RTC->DOTW = in_time->dayofweek;
   
   return (ERR_NO_ERROR);
}

