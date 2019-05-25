/***********************************************************************************
 Filename:    t1_cap.c

 Description: Use of Timer 1 in Free Running mode. Channel 0 is used to
              capture events from P0_2 (P0_3 on the CC2544Dongle) on falling edge. 
              A channel 0 interrupt is generated when an event is captured. 
              In the ISR, LED1 (green LED2 on the CC2544Dongle) is toggled.

              For simple Timer 1 capture testing, connect PIN16 on Debug
              Connector P20 (button S2 on SmartRF05EB) to PIN 9 on Debug
              Connector P18 (P0_2), such that Button 2 can toggle the capture
              pin and thus feed Timer 1 capture with random pulses. On the 
              CC2544Dongle the button S1 is used. Note that the pulses fed 
              to the Timer 1 capture must have a duration longer than the 
              system clock. You can use the variable timer1Ch0CapResult to 
              read the captured value.

              Settings:
                - Channel 0
                - Free-running mode
                - Input capture on P0_2
                - Interrupt generated on capture
                - Default clock source and speed (HS RCOSC at 16 MHz)
                - 500 kHz tickspeed

  Notes:       The tickspeed is set via Timer 1's tick speed divider, there is
               also a global prescaler for Timer 1, Timer 3 and Timer 4 within
               the clock control register [CLKCONCMD].

***********************************************************************************/

/***********************************************************************************
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


/***********************************************************************************
* CONSTANTS
*/

/***********************************************************************************
* LOCAL VARIABLES
*/

// Variable defined for debugging/testing purpose.
static uint16 timer1Ch0CapResult;

/***********************************************************************************
* LOCAL FUNCTIONS
*/


/***********************************************************************************
* @fn          timer1_ISR
*
* @brief       ISR for Timer 1. Interrupt occurs when a capture occurs on
*              channel 0 (P0_2). Toggles LED1 upon capture. The CPU
*              interrupt flag (IRCON) for Timer 1 is cleared automatically
*              by hardware.
*
* @param       void
*
* @return      void
*/

#pragma vector = T1_VECTOR
__interrupt void timer1_ISR(void)
{
    // Clear Timer 1 channel 0 interrupt flag. The proper way to clear interrupt
    // flags is by just writing 0, for R/W0 operations search datasheet.
    T1STAT = ~T1STAT_CH0IF;

    // The Timer value when a capture has occured.
    timer1Ch0CapResult = T1CC0L;
    timer1Ch0CapResult |= (T1CC0H << 8);

#if (chip==2541 || chip==2543 || chip==2545)
    // Toggle SRF05EB LED1.
    P1_0 ^= 1;
#elif (chip==2544)
    // Toggle GREEN LED2 on CC2544Dongle.
    P0_2 ^= 1;
#endif
}

/***********************************************************************************
* @fn          main
*
* @brief       Uses Timer 1 to function in Free Running mode. Channel 0 is used to
*              capture events from P0_2 on falling edge. A channel 0 interrupt
*              is generated when an event is captured and LED1 is toggled.
*
* @param       void
*
* @return      0
*/

int main(void)
{
    /***************************************************************************
     * Setup peripheral I/O for Timer
     *
     * We can also choose the Alternative 2 location for Timer 1, or for the peripherals
     * that use the same pins as Timer 1. This can be done by setting PERCFG-register
     */
  
#if (chip==2541 || chip==2543 || chip==2545)
    // Select P0_2 for peripheral function.
    P0SEL |= BIT2;
#elif (chip==2544)
    // Select P0_3 for peripheral function Timer1 on channel 0.
    P0SEL1 |= 0x30;
#endif

#if (chip==2541)
    // Timer 1 channels 0-1 has priority, then USART 1, then USART 0, then Timer 1 channels 2-3.
    P2DIR |= P2DIR_PRIP0_T1_0_1;
#elif (chip==2543 || chip==2545)
    // Set port 0 peripheral priority to Timer 1 over USART0. 
    PPRI |= PPRI_PRI0P0;
#endif

#if (chip==2541 || chip==2543 || chip==2545)
    // Configure P1_0 as GPIO output for LED1.
    P1SEL &= BIT0;      // GPIO.
    P1DIR |= BIT0;      // Output.
    P1_0 = 0;           // LED1 off.
#elif (chip==2544)
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED1 on
    PDIR |= PDIR_DIRP0_2;       // Output.
#endif
  
  
    /***************************************************************************
    * Setup interrupt
    */

    // Clear Timer 1 channel 0 interrupt flag.
    // CPU interrupt flag (IRCON) for Timer 1 is cleared automatically by hardware.
    T1STAT = ~T1STAT_CH0IF;

    // Set individual interrupt enable bit in the peripherals SFR.
    T1CCTL0 |= T1CCTLn_IM;      // Enable interrupt on channel 0.
    T1CCTL1 &= ~T1CCTLn_IM;     // Disable interrupt on channel 1.
    T1CCTL2 &= ~T1CCTLn_IM;     // Disable interrupt on channel 2.
    T1CCTL3 &= ~T1CCTLn_IM;     // Disable interrupt on channel 3.
    T1CCTL4 &= ~T1CCTLn_IM;     // Disable interrupt on channel 4.
    T1OVFIM = 0;                // Disable overflow interrupt.

    // Enable Timer 1 interrupts by setting [IEN1.T1IE=1].
    T1IE = 1;

    // Enable global interrupt by setting the [IEN0.EA=1].
    EA = 1;


    /***************************************************************************
     * Setup Timer settings
     *
     * Here we will select which channel(s) that will be used. We can choose to
     * use them in compare mode or capture mode.
     *
     * We can also select what mode the Timer shall operate on. When the mode is
     * selected, the Timer will start to run. Please see the data sheet for more
     * information.
     *
     * Notes:
     * - T1CCO is used by channel 1 & 2 for some compare modes, in case channels
     *   are used simultaneously.
     * - In compare mode using modulo mode or up-down mode, channel 0 will
     *   generate spike signals when [T1CCTL0.CMP = 3 or 4] since T1CC0 will
     *   then be both the compare value and the overflow value.
     * - The input signal (pulse), when in capture mode, must have a duration
     *   longer than the system clock.
     */
  
    // Select capture mode [T1CCTLn.MODE = 0], and set capture mode to capture on falling edge.
    T1CCTL0 = (T1CCTL0 & ~(T1CCTLn_CAP | T1CCTLn_MODE)) | T1CCTLn_CAP_FALL_EDGE;

    // Set prescalar divider value to 32 to get a tickspeed of 500 kHz and
    // set Timer 1 to free running mode.
    T1CTL = (T1CTL & ~(T1CTL_MODE | T1CTL_DIV)) | T1CTL_MODE_FREERUN | T1CTL_DIV_32;

    // Timer 1 will now start counting...
  
    // Loop forever. 
    while(1);
}


/***********************************************************************************
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
***********************************************************************************/