/*******************************************************************************
  Filename:    wdt_timer.c

  Description: Uses the WDT in Timer mode to toggle a LED on Timer overflow
               interrupt. The LED is in no way directly related to the Watchdog
               Timer, it is just used to indicate timeout of the Watchdog Timer.

*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2544)
#include "ioCC2544.h"
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/*******************************************************************************
* CONSTANTS
*/


/*******************************************************************************
* LOCAL VARIABLES
*/


/*******************************************************************************
* LOCAL FUNCTIONS
*/

/*******************************************************************************
* @fn          wdt_interrupt
*
* @brief       Interrupt handler for Watchdog Timer overflow interrupts. A
*              LED is toggled to indicate the overflow. The LED is not directly
*              related to the Watchdog Timer.
*
* @param       void
*
* @return      void
*******************************************************************************/
#pragma vector = WDT_VECTOR
__interrupt void wdt_isr(void) {

    /* Clears the CPU interrupt flag. */
    WDTIF = 0;

    /* Clears the Watchdog Timer. This simple way of clearing is possible only
     * in Timer mode, not in Watchdog mode.
     */
    WDCTL |= WDCTL_CLR0;

    /* Toggle LED. */
#if (chip==2541 || chip==2543 || chip==2545)
    P1_0 ^= 1;  // Toggle SRF05EB LED1
#elif (chip==2544)
    P0_2 ^= 1;  // Toggle GREEN LED1 on CC2544Dongle
#endif
}

/*******************************************************************************
* @fn          main
*
* @brief       Runs the WDT in Timer mode. The Watchdog Timer causes an interrupt
*              approx. every 1/4 second, which toggles LED1. The LED is
*              in no way directly related to the Watchdog Timer, it is just used
*              to indicate what is happening.
*
* @param       void
*
* @return      0
*******************************************************************************/
int main(void)
{
    /* Setup I/O */
#if (chip==2541 || chip==2543 || chip==2545)
    // Configure P1_0 as GPIO output for LED1
    P1SEL &= BIT0;      // GPIO
    P1DIR |= BIT0;      // Output
    P1_0 = 0;           // LED1 off
#elif (chip==2544)
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O
    P0_2 = 1;                   // LED1 on
    PDIR |= PDIR_DIRP0_2;       // Output
#endif

    /* Enables global interrupts (IEN0.EA = 1) and interrupts from the Watchdog
     * Timer.
     */
    EA = 1;
    IEN2 |= IEN2_WDTIE;

    /* Sets the Watchdog Timer timeout interval to approx. 250 ms. The low power
     * LS RCOSC is used (default); the accuracy of timeout interval will vary
     * with the choise of low speed OSC, see the datasheet for details.
     */
    WDCTL = (WDCTL & ~WDCTL_INT) | WDCTL_INT_250_MSEC;

    /* Selects Watchdog Timer mode and enables (starts) the Watchdog Timer
     * in Timer mode.
     */
    WDCTL = (WDCTL & ~WDCTL_MODE) | WDCTL_MODE_TIMER;

    /* Infinite loop. */
    while (1);
}

/*******************************************************************************
  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

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