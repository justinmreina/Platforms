//-----------------------------------------------------------------------------
// \file    evmomapl138_rtc.h
// \brief   OMAP-L138 rtc registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_RTC_H
#define EVMOMAPL138_RTC_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t SECOND;        // 0x0000
   volatile uint32_t MINUTE;        // 0x0004
   volatile uint32_t HOUR;          // 0x0008
   volatile uint32_t DAY;           // 0x000C
   volatile uint32_t MONTH;         // 0x0010
   volatile uint32_t YEAR;          // 0x0014
   volatile uint32_t DOTW;          // 0x0018
   volatile uint32_t RSVD0;         // 0x001C
   volatile uint32_t ALARMSECOND;   // 0x0020
   volatile uint32_t ALARMMINUTE;   // 0x0024
   volatile uint32_t ALARMHOUR;     // 0x0028
   volatile uint32_t ALARMDAY;      // 0x002C
   volatile uint32_t ALARMMONTH;    // 0x0030
   volatile uint32_t ALARMYEAR;     // 0x0034
   volatile uint32_t RSVD1[2];      // 0x0038
   volatile uint32_t CTRL;          // 0x0040
   volatile uint32_t STATUS;        // 0x0044
   volatile uint32_t INTERRUPT;     // 0x0048
   volatile uint32_t COMPLSB;       // 0x004C
   volatile uint32_t COMPMSB;       // 0x0050
   volatile uint32_t OSC;           // 0x0054
   volatile uint32_t RSVD2[2];      // 0x0058
   volatile uint32_t SCRATCH0;      // 0x0060
   volatile uint32_t SCRATCH1;      // 0x0064
   volatile uint32_t SCRATCH2;      // 0x0068
   volatile uint32_t KICK0;         // 0x006C
   volatile uint32_t KICK1;         // 0x0070
} rtc_regs_t;

#define RTC          ((rtc_regs_t *)RTC_REG_BASE)

// bitmask defines for CTRL.
#define RTC_RUN               (0x00000001)

// bitmask defines for STATUS.
#define RTC_BUSY              (0x00000001)

// bitmask defines for KICK.
#define RTCKICK0_UNLOCK       (0x83E70B13)
#define RTCKICK1_UNLOCK       (0x95A4F1E0)
#define RTCKICK0_LOCK         (0xFFFFFFFF)
#define RTCKICK1_LOCK         (0xFFFFFFFF)

// bitmask defines for OSC.
#define OSC_SWRESET           (0x00000020)   // bit 5

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------
typedef struct
{
   uint8_t sec;
   uint8_t min;
   uint8_t hour;
   uint8_t day;
   uint8_t month;
   uint8_t year;
   uint8_t dayofweek;
} rtc_time_t;

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t RTC_init(void);
uint32_t RTC_getTime(rtc_time_t *time);
uint32_t RTC_setTime(rtc_time_t *in_time);

#endif

