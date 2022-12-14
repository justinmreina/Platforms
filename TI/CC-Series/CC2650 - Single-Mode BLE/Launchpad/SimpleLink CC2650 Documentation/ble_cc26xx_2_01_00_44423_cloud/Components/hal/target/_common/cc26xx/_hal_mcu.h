/**************************************************************************************************
  Filename:       hal_mcu.h
  Revised:        $Date: 2015-07-22 10:45:09 -0700 (Wed, 22 Jul 2015) $
  Revision:       $Revision: 44392 $

  Description:    Describe the purpose and contents of the file.


  Copyright 2006 - 2015 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef HAL_MCU_H
#define HAL_MCU_H



/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_defs.h"
#include "hal_types.h"
#include <inc/hw_nvic.h>
#include <inc/hw_ints.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>
#include <driverlib/systick.h>
#include <driverlib/interrupt.h>
#include <driverlib/uart.h>
#include <driverlib/flash.h>

/* ------------------------------------------------------------------------------------------------
 *                                        Target Defines
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_MCU_CC2538


/* ------------------------------------------------------------------------------------------------
 *                                     Compiler Abstraction
 * ------------------------------------------------------------------------------------------------
 */

/* ---------------------- IAR Compiler ---------------------- */
#if defined (__IAR_SYSTEMS_ICC__)
#define HAL_COMPILER_IAR
#define HAL_MCU_LITTLE_ENDIAN()   __LITTLE_ENDIAN__

/* ---------------------- Keil Compiler ---------------------- */
#elif defined (__KEIL__)
#define HAL_COMPILER_KEIL
#define HAL_MCU_LITTLE_ENDIAN()   0


/* ------------------ Unrecognized Compiler ------------------ */
#elif defined (ccs)
#define HAL_MCU_LITTLE_ENDIAN()   0
//do nothing for now
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                       Interrupt Macros
 * ------------------------------------------------------------------------------------------------
 */

typedef tBoolean halIntState_t;

/* Enable RF interrupt */
#define HAL_ENABLE_RF_INTERRUPT()    \
{                                    \
  IntEnable(INT_RFCORERTX);          \
}

/* Enable RF error interrupt */
#define HAL_ENABLE_RF_ERROR_INTERRUPT() \
{                                       \
  IntEnable(INT_RFCOREERR);             \
}

/* Enable interrupts */
#define HAL_ENABLE_INTERRUPTS()     IntMasterEnable()

/* Disable interrupts */
#define HAL_DISABLE_INTERRUPTS()    IntMasterDisable()

static tBoolean halIntsAreEnabled(void)
{
  tBoolean status = !IntMasterDisable();
  if (status)
  {
    IntMasterEnable();
  }
  return status;
}

#ifdef TIMAC_ON_RTOS
#define HAL_INTERRUPTS_ARE_ENABLED() (rtosCSDepth == 0)
#else
#define HAL_INTERRUPTS_ARE_ENABLED() halIntsAreEnabled()
#endif

#ifdef TIMAC_ON_RTOS
/* TODO: HAL must not have a dependency on OS, but as it is now,
 * too many OS dependent components use HAL.
 * The FreeRTOS dependency has to be removed eventually when
 * OSAL heap is ported not to use the same critical section macro,
 * and when MAC thread unsafe API is called from within the same thread only.
 */
extern uint32 rtosCSDepth;
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define HAL_ENTER_CRITICAL_SECTION(x)  \
  do { \
    (void) (x); \
    vPortEnterCritical(); \
    rtosCSDepth++; \
  } while (0)
#else /* TIMAC_ON_RTOS */
//#define HAL_ENTER_CRITICAL_SECTION(x)
#define HAL_ENTER_CRITICAL_SECTION(x)  \
  do { (x) = !IntMasterDisable(); } while (0)
#endif /* TIMAC_ON_RTOS */

#ifdef TIMAC_ON_RTOS
#define HAL_EXIT_CRITICAL_SECTION(x)   \
  do { \
    (void) (x); \
    rtosCSDepth--; \
    vPortExitCritical(); \
 } while (0)
#else /* TIMAC_ON_RTOS */
//#define HAL_EXIT_CRITICAL_SECTION(x)
#define HAL_EXIT_CRITICAL_SECTION(x) \
  do { if (x) { (void) IntMasterEnable(); } } while (0)
#endif /* TIMAC_ON_RTOS */

#define HAL_NON_ISR_ENTER_CRITICAL_SECTION(x)  HAL_ENTER_CRITICAL_SECTION(x)
#define HAL_NON_ISR_EXIT_CRITICAL_SECTION(x)   HAL_EXIT_CRITICAL_SECTION(x)

/* Hal Critical statement definition */
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t s; HAL_ENTER_CRITICAL_SECTION(s); x; HAL_EXIT_CRITICAL_SECTION(s); )

/* Enable Key/button interrupts */
#define HAL_ENABLE_PUSH_BUTTON_PORT_INTERRUPTS()  \
{                                                 \
  IntEnable(INT_GPIOC);                           \
  IntEnable(INT_GPIOA);                           \
}

/* Disable Key/button interrupts */
#define HAL_DISABLE_PUSH_BUTTON_PORT_INTERRUPTS()  \
{                                                  \
  IntDisable(INT_GPIOC);                           \
  IntDisable(INT_GPIOA);                           \
}

/* ------------------------------------------------------------------------------------------------
 *                                        Reset Macro
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_SYSTEM_RESET()  *((uint32 *)0x40082270) = 1;
#define WD_EN               BV(3)
#define WD_MODE             BV(2)
#define WD_INT_1900_USEC    (BV(0) | BV(1))
#define WD_RESET1           (0xA0 | WD_EN | WD_INT_1900_USEC)
#define WD_RESET2           (0x50 | WD_EN | WD_INT_1900_USEC)
#define WD_KICK()           st( WDCTL = (0xA0 | WDCTL & 0x0F); WDCTL = (0x50 | WDCTL & 0x0F); )

/* ------------------------------------------------------------------------------------------------
 *                                        CC253x rev numbers
 * ------------------------------------------------------------------------------------------------
 */
#define REV_A          0x00    /* workaround turned off */
#define REV_B          0x11    /* PG1.1 */
#define REV_C          0x20    /* PG2.0 */
#define REV_D          0x21    /* PG2.1 */
#define REV_CC2538     0xB964  /* CC2538 Rev*/

/* ------------------------------------------------------------------------------------------------
 *                                        CC2538 sleep common code
 * ------------------------------------------------------------------------------------------------
 */

/* PCON bit definitions */
#define PCON_IDLE  BV(0)            /* Writing 1 to force CC2530 to enter sleep mode */

/* STLOAD */
#define LDRDY            BV(0) /* Load Ready. This bit is 0 while the sleep timer
                                * loads the 24-bit compare value and 1 when the sleep
                                * timer is ready to start loading a new compare value. */

#ifdef POWER_SAVING
extern volatile uint8 halSleepOverride;

// Any ISR that is used to wake the CM3 must call this macro to prevent a race
// condition that results when the ISR that sets an OSAL event occurs after
// the OSAL loop finishes checking for events.
#define CLEAR_SLEEP_MODE()        st( halSleepOverride = TRUE; )
#define ALLOW_SLEEP_MODE()        st( halSleepOverride = FALSE; )

#else

#define CLEAR_SLEEP_MODE()
#define ALLOW_SLEEP_MODE()

#endif

/**************************************************************************************************
 */
#endif

