/*
 * cslr_rtc.h
 *
 * This file contains the macros for Register Chip Support Library (CSL) which 
 * can be used for operations on the respective underlying hardware/peripheral
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef _CSLR_RTC_H_
#define _CSLR_RTC_H_

#include <ti/pspiom/cslr/cslr.h>
#include <ti/pspiom/cslr/tistdtypes.h>



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 SECOND;
    volatile Uint32 MINUTE;
    volatile Uint32 HOUR;
    volatile Uint32 DAY;
    volatile Uint32 MONTH;
    volatile Uint32 YEAR;
    volatile Uint32 DOTW;
    volatile Uint8 RSVD0[4];
    volatile Uint32 ALARMSECOND;
    volatile Uint32 ALARMMINUTE;
    volatile Uint32 ALARMHOUR;
    volatile Uint32 ALARMDAY;
    volatile Uint32 ALARMMONTH;
    volatile Uint32 ALARMYEAR;
    volatile Uint8 RSVD1[8];
    volatile Uint32 CTRL;
    volatile Uint32 STATUS;
    volatile Uint32 INTERRUPTS;
    volatile Uint32 COMPLSB;
    volatile Uint32 COMPMSB;
    volatile Uint32 OSC;
    volatile Uint8 RSVD2[8];
    volatile Uint32 SCRATCH0;
    volatile Uint32 SCRATCH1;
    volatile Uint32 SCRATCH2;
    volatile Uint32 KICK0;
    volatile Uint32 KICK1;
} CSL_RtcRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_RtcRegs         *CSL_RtcRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* SECOND */


#define CSL_RTC_SECOND_SEC1_MASK         (0x00000070u)
#define CSL_RTC_SECOND_SEC1_SHIFT        (0x00000004u)
#define CSL_RTC_SECOND_SEC1_RESETVAL     (0x00000000u)

#define CSL_RTC_SECOND_SEC0_MASK         (0x0000000Fu)
#define CSL_RTC_SECOND_SEC0_SHIFT        (0x00000000u)
#define CSL_RTC_SECOND_SEC0_RESETVAL     (0x00000000u)

#define CSL_RTC_SECOND_RESETVAL          (0x00000000u)

/* MINUTE */


#define CSL_RTC_MINUTE_MIN1_MASK         (0x00000070u)
#define CSL_RTC_MINUTE_MIN1_SHIFT        (0x00000004u)
#define CSL_RTC_MINUTE_MIN1_RESETVAL     (0x00000000u)

#define CSL_RTC_MINUTE_MIN0_MASK         (0x0000000Fu)
#define CSL_RTC_MINUTE_MIN0_SHIFT        (0x00000000u)
#define CSL_RTC_MINUTE_MIN0_RESETVAL     (0x00000000u)

#define CSL_RTC_MINUTE_RESETVAL          (0x00000000u)

/* HOUR */


#define CSL_RTC_HOUR_MERIDIEM_MASK       (0x00000080u)
#define CSL_RTC_HOUR_MERIDIEM_SHIFT      (0x00000007u)
#define CSL_RTC_HOUR_MERIDIEM_RESETVAL   (0x00000000u)
/*----MERIDIEM Tokens----*/
#define CSL_RTC_HOUR_MERIDIEM_AM         (0x00000000u)
#define CSL_RTC_HOUR_MERIDIEM_PM         (0x00000001u)


#define CSL_RTC_HOUR_HOUR1_MASK          (0x00000030u)
#define CSL_RTC_HOUR_HOUR1_SHIFT         (0x00000004u)
#define CSL_RTC_HOUR_HOUR1_RESETVAL      (0x00000000u)

#define CSL_RTC_HOUR_HOUR0_MASK          (0x0000000Fu)
#define CSL_RTC_HOUR_HOUR0_SHIFT         (0x00000000u)
#define CSL_RTC_HOUR_HOUR0_RESETVAL      (0x00000000u)

#define CSL_RTC_HOUR_RESETVAL            (0x00000000u)

/* DAY */


#define CSL_RTC_DAY_DAY1_MASK            (0x00000030u)
#define CSL_RTC_DAY_DAY1_SHIFT           (0x00000004u)
#define CSL_RTC_DAY_DAY1_RESETVAL        (0x00000000u)

#define CSL_RTC_DAY_DAY0_MASK            (0x0000000Fu)
#define CSL_RTC_DAY_DAY0_SHIFT           (0x00000000u)
#define CSL_RTC_DAY_DAY0_RESETVAL        (0x00000001u)

#define CSL_RTC_DAY_RESETVAL             (0x00000001u)

/* MONTH */


#define CSL_RTC_MONTH_MONTH1_MASK        (0x00000010u)
#define CSL_RTC_MONTH_MONTH1_SHIFT       (0x00000004u)
#define CSL_RTC_MONTH_MONTH1_RESETVAL    (0x00000000u)

#define CSL_RTC_MONTH_MONTH0_MASK        (0x0000000Fu)
#define CSL_RTC_MONTH_MONTH0_SHIFT       (0x00000000u)
#define CSL_RTC_MONTH_MONTH0_RESETVAL    (0x00000001u)

#define CSL_RTC_MONTH_RESETVAL           (0x00000001u)

/* YEAR */


#define CSL_RTC_YEAR_YEAR1_MASK          (0x000000F0u)
#define CSL_RTC_YEAR_YEAR1_SHIFT         (0x00000004u)
#define CSL_RTC_YEAR_YEAR1_RESETVAL      (0x00000000u)

#define CSL_RTC_YEAR_YEAR0_MASK          (0x0000000Fu)
#define CSL_RTC_YEAR_YEAR0_SHIFT         (0x00000000u)
#define CSL_RTC_YEAR_YEAR0_RESETVAL      (0x00000000u)

#define CSL_RTC_YEAR_RESETVAL            (0x00000000u)

/* DOTW */


#define CSL_RTC_DOTW_DOTW_MASK           (0x00000007u)
#define CSL_RTC_DOTW_DOTW_SHIFT          (0x00000000u)
#define CSL_RTC_DOTW_DOTW_RESETVAL       (0x00000000u)
#define CSL_RTC_DOTW_DOTW_SUN            (0x00000000u)
#define CSL_RTC_DOTW_DOTW_MON            (0x00000001u)
#define CSL_RTC_DOTW_DOTW_TUE            (0x00000002u)
#define CSL_RTC_DOTW_DOTW_WED            (0x00000003u)
#define CSL_RTC_DOTW_DOTW_THU            (0x00000004u)
#define CSL_RTC_DOTW_DOTW_FRI            (0x00000005u)
#define CSL_RTC_DOTW_DOTW_SAT            (0x00000006u)

#define CSL_RTC_DOTW_RESETVAL            (0x00000000u)

/* ALARMSECOND */


#define CSL_RTC_ALARMSECOND_SEC1_MASK    (0x00000070u)
#define CSL_RTC_ALARMSECOND_SEC1_SHIFT   (0x00000004u)
#define CSL_RTC_ALARMSECOND_SEC1_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMSECOND_SEC0_MASK    (0x0000000Fu)
#define CSL_RTC_ALARMSECOND_SEC0_SHIFT   (0x00000000u)
#define CSL_RTC_ALARMSECOND_SEC0_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMSECOND_RESETVAL     (0x00000000u)

/* ALARMMINUTE */


#define CSL_RTC_ALARMMINUTE_MIN1_MASK    (0x00000070u)
#define CSL_RTC_ALARMMINUTE_MIN1_SHIFT   (0x00000004u)
#define CSL_RTC_ALARMMINUTE_MIN1_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMMINUTE_MIN0_MASK    (0x0000000Fu)
#define CSL_RTC_ALARMMINUTE_MIN0_SHIFT   (0x00000000u)
#define CSL_RTC_ALARMMINUTE_MIN0_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMMINUTE_RESETVAL     (0x00000000u)

/* ALARMHOUR */


#define CSL_RTC_ALARMHOUR_MERIDIEM_MASK  (0x00000080u)
#define CSL_RTC_ALARMHOUR_MERIDIEM_SHIFT (0x00000007u)
#define CSL_RTC_ALARMHOUR_MERIDIEM_RESETVAL (0x00000000u)
/*----MERIDIEM Tokens----*/
#define CSL_RTC_ALARMHOUR_MERIDIEM_AM    (0x00000000u)
#define CSL_RTC_ALARMHOUR_MERIDIEM_PM    (0x00000001u)


#define CSL_RTC_ALARMHOUR_HOUR1_MASK     (0x00000030u)
#define CSL_RTC_ALARMHOUR_HOUR1_SHIFT    (0x00000004u)
#define CSL_RTC_ALARMHOUR_HOUR1_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMHOUR_HOUR0_MASK     (0x0000000Fu)
#define CSL_RTC_ALARMHOUR_HOUR0_SHIFT    (0x00000000u)
#define CSL_RTC_ALARMHOUR_HOUR0_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMHOUR_RESETVAL       (0x00000000u)

/* ALARMDAY */


#define CSL_RTC_ALARMDAY_DAY1_MASK       (0x00000030u)
#define CSL_RTC_ALARMDAY_DAY1_SHIFT      (0x00000004u)
#define CSL_RTC_ALARMDAY_DAY1_RESETVAL   (0x00000000u)

#define CSL_RTC_ALARMDAY_DAY0_MASK       (0x0000000Fu)
#define CSL_RTC_ALARMDAY_DAY0_SHIFT      (0x00000000u)
#define CSL_RTC_ALARMDAY_DAY0_RESETVAL   (0x00000001u)

#define CSL_RTC_ALARMDAY_RESETVAL        (0x00000001u)

/* ALARMMONTH */


#define CSL_RTC_ALARMMONTH_MONTH1_MASK   (0x00000010u)
#define CSL_RTC_ALARMMONTH_MONTH1_SHIFT  (0x00000004u)
#define CSL_RTC_ALARMMONTH_MONTH1_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMMONTH_MONTH0_MASK   (0x0000000Fu)
#define CSL_RTC_ALARMMONTH_MONTH0_SHIFT  (0x00000000u)
#define CSL_RTC_ALARMMONTH_MONTH0_RESETVAL (0x00000001u)

#define CSL_RTC_ALARMMONTH_RESETVAL      (0x00000001u)

/* ALARMYEAR */


#define CSL_RTC_ALARMYEAR_YEAR1_MASK     (0x000000F0u)
#define CSL_RTC_ALARMYEAR_YEAR1_SHIFT    (0x00000004u)
#define CSL_RTC_ALARMYEAR_YEAR1_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMYEAR_YEAR0_MASK     (0x0000000Fu)
#define CSL_RTC_ALARMYEAR_YEAR0_SHIFT    (0x00000000u)
#define CSL_RTC_ALARMYEAR_YEAR0_RESETVAL (0x00000000u)

#define CSL_RTC_ALARMYEAR_RESETVAL       (0x00000000u)

/* CTRL */


#define CSL_RTC_CTRL_SPLITPOWER_MASK     (0x00000080u)
#define CSL_RTC_CTRL_SPLITPOWER_SHIFT    (0x00000007u)
#define CSL_RTC_CTRL_SPLITPOWER_RESETVAL (0x00000000u)
/*----SPLITPOWER Tokens----*/
#define CSL_RTC_CTRL_SPLITPOWER_DISABLE  (0x00000000u)
#define CSL_RTC_CTRL_SPLITPOWER_ENABLE   (0x00000001u)

#define CSL_RTC_CTRL_RTCDISABLE_MASK     (0x00000040u)
#define CSL_RTC_CTRL_RTCDISABLE_SHIFT    (0x00000006u)
#define CSL_RTC_CTRL_RTCDISABLE_RESETVAL (0x00000000u)
/*----RTCDISABLE Tokens----*/
#define CSL_RTC_CTRL_RTCDISABLE_ENABLE   (0x00000000u)
#define CSL_RTC_CTRL_RTCDISABLE_DISABLE  (0x00000001u)

#define CSL_RTC_CTRL_SET32COUNTER_MASK   (0x00000020u)
#define CSL_RTC_CTRL_SET32COUNTER_SHIFT  (0x00000005u)
#define CSL_RTC_CTRL_SET32COUNTER_RESETVAL (0x00000000u)
/*----SET32COUNTER Tokens----*/
#define CSL_RTC_CTRL_SET32COUNTER_DISABLE (0x00000000u)
#define CSL_RTC_CTRL_SET32COUNTER_ENABLE (0x00000001u)


#define CSL_RTC_CTRL_HOURMODE_MASK       (0x00000008u)
#define CSL_RTC_CTRL_HOURMODE_SHIFT      (0x00000003u)
#define CSL_RTC_CTRL_HOURMODE_RESETVAL   (0x00000000u)
/*----HOURMODE Tokens----*/
#define CSL_RTC_CTRL_HOURMODE_24HR       (0x00000000u)
#define CSL_RTC_CTRL_HOURMODE_12HR       (0x00000001u)

#define CSL_RTC_CTRL_AUTOCOMP_MASK       (0x00000004u)
#define CSL_RTC_CTRL_AUTOCOMP_SHIFT      (0x00000002u)
#define CSL_RTC_CTRL_AUTOCOMP_RESETVAL   (0x00000000u)
/*----AUTOCOMP Tokens----*/
#define CSL_RTC_CTRL_AUTOCOMP_DISABLE    (0x00000000u)
#define CSL_RTC_CTRL_AUTOCOMP_ENABLE     (0x00000001u)

#define CSL_RTC_CTRL_ROUNDMIN_MASK       (0x00000002u)
#define CSL_RTC_CTRL_ROUNDMIN_SHIFT      (0x00000001u)
#define CSL_RTC_CTRL_ROUNDMIN_RESETVAL   (0x00000000u)
/*----ROUNDMIN Tokens----*/
#define CSL_RTC_CTRL_ROUNDMIN_DISABLE    (0x00000000u)
#define CSL_RTC_CTRL_ROUNDMIN_ENABLE     (0x00000001u)

#define CSL_RTC_CTRL_RUN_MASK            (0x00000001u)
#define CSL_RTC_CTRL_RUN_SHIFT           (0x00000000u)
#define CSL_RTC_CTRL_RUN_RESETVAL        (0x00000000u)
/*----RUN Tokens----*/
#define CSL_RTC_CTRL_RUN_STOP            (0x00000000u)
#define CSL_RTC_CTRL_RUN_RUN             (0x00000001u)

#define CSL_RTC_CTRL_RESETVAL            (0x00000000u)

/* STATUS */



#define CSL_RTC_STATUS_ALARM_MASK        (0x00000040u)
#define CSL_RTC_STATUS_ALARM_SHIFT       (0x00000006u)
#define CSL_RTC_STATUS_ALARM_RESETVAL    (0x00000000u)
/*----ALARM Tokens----*/
#define CSL_RTC_STATUS_ALARM_INACTIVE    (0x00000000u)
#define CSL_RTC_STATUS_ALARM_ACTIVE      (0x00000001u)

#define CSL_RTC_STATUS_DAYEVT_MASK       (0x00000020u)
#define CSL_RTC_STATUS_DAYEVT_SHIFT      (0x00000005u)
#define CSL_RTC_STATUS_DAYEVT_RESETVAL   (0x00000000u)
/*----DAYEVT Tokens----*/
#define CSL_RTC_STATUS_DAYEVT_INACTIVE   (0x00000000u)
#define CSL_RTC_STATUS_DAYEVT_ACTIVE     (0x00000001u)

#define CSL_RTC_STATUS_HREVT_MASK        (0x00000010u)
#define CSL_RTC_STATUS_HREVT_SHIFT       (0x00000004u)
#define CSL_RTC_STATUS_HREVT_RESETVAL    (0x00000000u)
/*----HREVT Tokens----*/
#define CSL_RTC_STATUS_HREVT_INACTIVE    (0x00000000u)
#define CSL_RTC_STATUS_HREVT_ACTIVE      (0x00000001u)

#define CSL_RTC_STATUS_MINEVT_MASK       (0x00000008u)
#define CSL_RTC_STATUS_MINEVT_SHIFT      (0x00000003u)
#define CSL_RTC_STATUS_MINEVT_RESETVAL   (0x00000000u)
/*----MINEVT Tokens----*/
#define CSL_RTC_STATUS_MINEVT_INACTIVE   (0x00000000u)
#define CSL_RTC_STATUS_MINEVT_ACTIVE     (0x00000001u)

#define CSL_RTC_STATUS_SECEVT_MASK       (0x00000004u)
#define CSL_RTC_STATUS_SECEVT_SHIFT      (0x00000002u)
#define CSL_RTC_STATUS_SECEVT_RESETVAL   (0x00000000u)
/*----SECEVT Tokens----*/
#define CSL_RTC_STATUS_SECEVT_INACTIVE   (0x00000000u)
#define CSL_RTC_STATUS_SECEVT_ACTIVE     (0x00000001u)

#define CSL_RTC_STATUS_RUN_MASK          (0x00000002u)
#define CSL_RTC_STATUS_RUN_SHIFT         (0x00000001u)
#define CSL_RTC_STATUS_RUN_RESETVAL      (0x00000000u)
/*----RUN Tokens----*/
#define CSL_RTC_STATUS_RUN_STOP          (0x00000000u)
#define CSL_RTC_STATUS_RUN_RUN           (0x00000001u)

#define CSL_RTC_STATUS_BUSY_MASK         (0x00000001u)
#define CSL_RTC_STATUS_BUSY_SHIFT        (0x00000000u)
#define CSL_RTC_STATUS_BUSY_RESETVAL     (0x00000000u)
/*----BUSY Tokens----*/
#define CSL_RTC_STATUS_BUSY_FREE         (0x00000000u)
#define CSL_RTC_STATUS_BUSY_BUSY         (0x00000001u)

#define CSL_RTC_STATUS_RESETVAL          (0x00000080u)

/* INTERRUPTS */


#define CSL_RTC_INTERRUPTS_ALARM_MASK    (0x00000008u)
#define CSL_RTC_INTERRUPTS_ALARM_SHIFT   (0x00000003u)
#define CSL_RTC_INTERRUPTS_ALARM_RESETVAL (0x00000000u)
/*----ALARM Tokens----*/
#define CSL_RTC_INTERRUPTS_ALARM_DISABLE (0x00000000u)
#define CSL_RTC_INTERRUPTS_ALARM_ENABLE  (0x00000001u)

#define CSL_RTC_INTERRUPTS_TIMER_MASK    (0x00000004u)
#define CSL_RTC_INTERRUPTS_TIMER_SHIFT   (0x00000002u)
#define CSL_RTC_INTERRUPTS_TIMER_RESETVAL (0x00000000u)
/*----TIMER Tokens----*/
#define CSL_RTC_INTERRUPTS_TIMER_DISABLE (0x00000000u)
#define CSL_RTC_INTERRUPTS_TIMER_ENABLE  (0x00000001u)

#define CSL_RTC_INTERRUPTS_EVERY_MASK    (0x00000003u)
#define CSL_RTC_INTERRUPTS_EVERY_SHIFT   (0x00000000u)
#define CSL_RTC_INTERRUPTS_EVERY_RESETVAL (0x00000000u)
/*----EVERY Tokens----*/
#define CSL_RTC_INTERRUPTS_EVERY_SECOND  (0x00000000u)
#define CSL_RTC_INTERRUPTS_EVERY_MINUTE  (0x00000001u)
#define CSL_RTC_INTERRUPTS_EVERY_HOUR    (0x00000002u)
#define CSL_RTC_INTERRUPTS_EVERY_DAY     (0x00000003u)

#define CSL_RTC_INTERRUPTS_RESETVAL      (0x00000000u)

/* COMPLSB */


#define CSL_RTC_COMPLSB_COMPLSB_MASK     (0x000000FFu)
#define CSL_RTC_COMPLSB_COMPLSB_SHIFT    (0x00000000u)
#define CSL_RTC_COMPLSB_COMPLSB_RESETVAL (0x00000000u)

#define CSL_RTC_COMPLSB_RESETVAL         (0x00000000u)

/* COMPMSB */


#define CSL_RTC_COMPMSB_COMPMSB_MASK     (0x000000FFu)
#define CSL_RTC_COMPMSB_COMPMSB_SHIFT    (0x00000000u)
#define CSL_RTC_COMPMSB_COMPMSB_RESETVAL (0x00000000u)

#define CSL_RTC_COMPMSB_RESETVAL         (0x00000000u)

/* OSC */


#define CSL_RTC_OSC_SWRESET_MASK         (0x00000020u)
#define CSL_RTC_OSC_SWRESET_SHIFT        (0x00000005u)
#define CSL_RTC_OSC_SWRESET_RESETVAL     (0x00000000u)
/*----SWRESET Tokens----*/
#define CSL_RTC_OSC_SWRESET_NOTINRESET   (0x00000000u)
#define CSL_RTC_OSC_SWRESET_RESET        (0x00000001u)



#define CSL_RTC_OSC_RESETVAL             (0x00000007u)

/* SCRATCH0 */

#define CSL_RTC_SCRATCH0_SCRATCH0_MASK   (0xFFFFFFFFu)
#define CSL_RTC_SCRATCH0_SCRATCH0_SHIFT  (0x00000000u)
#define CSL_RTC_SCRATCH0_SCRATCH0_RESETVAL (0x00000000u)

#define CSL_RTC_SCRATCH0_RESETVAL        (0x00000000u)

/* SCRATCH1 */

#define CSL_RTC_SCRATCH1_SCRATCH1_MASK   (0xFFFFFFFFu)
#define CSL_RTC_SCRATCH1_SCRATCH1_SHIFT  (0x00000000u)
#define CSL_RTC_SCRATCH1_SCRATCH1_RESETVAL (0x00000000u)

#define CSL_RTC_SCRATCH1_RESETVAL        (0x00000000u)

/* SCRATCH2 */

#define CSL_RTC_SCRATCH2_SCRATCH2_MASK   (0xFFFFFFFFu)
#define CSL_RTC_SCRATCH2_SCRATCH2_SHIFT  (0x00000000u)
#define CSL_RTC_SCRATCH2_SCRATCH2_RESETVAL (0x00000000u)

#define CSL_RTC_SCRATCH2_RESETVAL        (0x00000000u)

/* KICK0 */

#define CSL_RTC_KICK0_KICK0_MASK         (0xFFFFFFFFu)
#define CSL_RTC_KICK0_KICK0_SHIFT        (0x00000000u)
#define CSL_RTC_KICK0_KICK0_RESETVAL     (0x00000000u)
/*----KICK0 Tokens----*/
#define CSL_RTC_KICK0_KICK0_KEY          (0x7FFFFFFFu)

#define CSL_RTC_KICK0_RESETVAL           (0x00000000u)

/* KICK1 */

#define CSL_RTC_KICK1_KICK1_MASK         (0xFFFFFFFFu)
#define CSL_RTC_KICK1_KICK1_SHIFT        (0x00000000u)
#define CSL_RTC_KICK1_KICK1_RESETVAL     (0x00000000u)
/*----KICK1 Tokens----*/
#define CSL_RTC_KICK1_KICK1_KEY          (0x7FFFFFFFu)

#define CSL_RTC_KICK1_RESETVAL           (0x00000000u)


#endif
