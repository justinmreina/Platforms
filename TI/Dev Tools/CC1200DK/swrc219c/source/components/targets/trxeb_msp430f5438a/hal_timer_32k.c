/*******************************************************************************
    Filename:     hal_timer_32k_trxeb.c

    Description:  hal 32KHz timer for TRXEB using a MSP430F5438A

*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/

#include <msp430.h>
#include "hal_types.h"
#include "hal_int.h"
#include "hal_timer_32k.h"


/*******************************************************************************
 * LOCAL VARIABLES
 */

static ISR_FUNC_PTR fptr;
static uint16 mode;

/*******************************************************************************
 * @fn          halTimer32kInit
 *
 * @brief       Set up timer B to periodically count upwards specified cycles
 *              of 32768 Hz clock without generating an interrupt using compare
 *              latch 0.  To enable an interrupt:
 *               - Before calling this function, connect an ISR to the interrupt
 *                 vector using halTimer32kIntConnect(ISR_FUNC_PTR isr) if not
 *                 allready done.
 *               - Call this function to set the periodical number of 32768 Hz
 *                 cycles to count.
 *               - Call halTimer32kIntEnable()
 *
 * input parameters
 *
 * @param       cycles  - Number of cycles between interrupt
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kInit(uint16 cycles)
{
  /* Blocking interrupts when configuring timer */
  istate_t key;
  HAL_INT_LOCK(key);
  /* - Stopping the timer in case it's running
   * - Clears & disables Timer B TBIFG interrupts and all compare latch interrups
   * - Resets TB0R(the counter)
   */
  TB0CTL = 0;
  /* Avoid compiler optimization (skipping the line above) */
  asm(" nop");
  TB0CTL = TBCLR;


  /* Setting up TB0CCTL0( capture/compare control register
   * - Compare mode
   * - Clear interrupt pending flag and disable interrupt on comapre latch 0
   * - TB0CL0 loads immediately when writing TB0CCR0
   * -
   */
  TB0CCTL0 = 0;

  /* Set compare value in the TB0CCR0 register */
  TB0CCR0 = cycles;

  /* Setting up the Timer B mode:
   * Timer source ACLK
   * 16 bit counter mode
   * Each TB0CLn latch loads independantly
   * Dont't divide input ACKL
   * Count up to TB0CCR0
   * Clear timer
   */
  mode = TBSSEL_1 | CNTL_0 | TBCLGRP_0 | ID_0 | MC_1 ;
  TB0CTL = mode;
  TB0EX0 = TBIDEX_0;
  HAL_INT_UNLOCK(key);
}

/*******************************************************************************
 * @fn          halTimer32kAbort
 *
 * @brief       Abort Timer B function, clear and disables interrrupt on Timer B and
 *              and compare latch 0
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kAbort(void)
{
  /* - Stopping the timer in case it's running
   * - Clears & disables Timer B TBIFG interrupts and all compare latch interrups
   * - Resets TB0R(the counter)
   */
  TB0CTL = 0;
  /* Avoid compiler optimization (skipping the line above) */
  asm(" nop");
  TB0CTL = TBCLR;
  TB0CCTL0 = 0;
  asm(" nop");
}

/*******************************************************************************
 * @fn          halTimer32kRestart
 *
 * @brief       Restart timer B. The timer is first stopped, then restarted,
 *              counting up from 0
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kRestart(void)
{
  istate_t key;
  HAL_INT_LOCK(key);
  /* - Stopping the timer in case it's running
   * - Clears & disables Timer B TBIFG interrupts and all compare latch interrups
   * - Resets TB0R(the counter)
   */
  TB0CTL = 0;
  /* Avoid compiler optimization (skipping the line above) */
  asm(" nop");
  TB0CTL = TBCLR;
  TB0CCTL0 = 0;
  asm(" nop");
  TB0CTL = mode;
  HAL_INT_UNLOCK(key);
}

/*******************************************************************************
 * @fn          halTimer32kIntConnect
 *
 * @brief       Connect function to timer interrupt
 *
 * input parameters
 *
 * @param       isr  - pointer to function
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kIntConnect(ISR_FUNC_PTR isr)
{
  istate_t key;
  HAL_INT_LOCK(key);
  fptr = isr;
  HAL_INT_UNLOCK(key);
}

/***********************************************************************************
 * @fn          halTimer32kIntEnable
 *
 * @brief       Enable 32KHz timer interrupt on compare latch 0
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kIntEnable(void)
{
  istate_t key;
  HAL_INT_LOCK(key);
  TB0CCTL0 |= CCIE;
  HAL_INT_UNLOCK(key);
}

/***********************************************************************************
 * @fn          halTimer32kIntDisable
 *
 * @brief       Disable 32KHz timer interrupt on compare latch 0
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kIntDisable(void)
{
  TB0CCTL0 &= ~CCIE;
}

/***********************************************************************************
 * @fn          halTimer32kSetIntFrequency
 *
 * @brief       Configure timer interrupts for application. Uses 32KHz timer
 *
 * input parameters
 *
 * @param       rate  - Frequency of timer interrupt. This value must be
 *                      between 1 and 32768 Hz
 *
 * output parameters
 *
 * @return      none
 */
void halTimer32kSetIntFrequency(uint16 rate)
{
    halTimer32kInit(TIMER_32K_CLK_FREQ/rate);
}

/******************************************************************************
 * @fn          halTimer32kMcuSleepTicks
 *
 * @brief       This function uses Timer B to sleep for a specfied number of
 *              ACLK ticks, that is less than 2^15 tics(1s).
 *              Assumes that the only interrupt source is
 *              generated by Timer B.
 *
 *              NOTE: When called, this function will assign NO ISR to the
 *              TIMERB0_VECTOR interrupt(NULL pointer). When interrupt triggers
 *              it will just wake up the MCU. Conflicts are possible if not
 *              used carefully since other parts of a program might use the
 *              TIMER B.
 *
 * input parameters
 *
 * @param       ticks  - Number of ACLK(32768Hz) ticks that the MCU will sleep
 *
 * output parameters
 *
 * @return      void
 */
void halTimer32kMcuSleepTicks(uint16 ticks)
{
  halTimer32kIntConnect(NULL);
  halTimer32kInit(ticks);
  halTimer32kIntEnable();
  __low_power_mode_3();
  halTimer32kAbort();
}

/***********************************************************************************
 * @fn          halTimer32kReadTimerValue
 *
 * @brief       Reads the current counter value of the TB0R register
 *
 * input parameters
 *
 * output parameters
 *
 * @return      16 bit timer value
 */
uint16 halTimer32kReadTimerValue(void)
{
	uint16 timerValue;
	/*stop timer*/
	TB0CTL &= ~MC_1;
  timerValue = TB0R;
  /*restart timer*/
  TB0CTL |= MC_1;
  return timerValue;
}

/******************************************************************************
 * @fn      timer32k0_ISR
 *
 * @brief   ISR framework for the 32KHz timer component
 *
 * input parameters
 *
 * output parameters
 *
 * @return  none
 */
#pragma vector=TIMERB0_VECTOR
__interrupt void timer32k0_ISR(void)
{
  if (fptr != NULL)
  {
      (*fptr)();
  }
  __low_power_mode_off_on_exit();
}

/******************************************************************************
  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
*******************************************************************************/
