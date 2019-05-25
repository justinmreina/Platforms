/***********************************************************************************
  Filename:     t1_mod.c

  Description:  Uses Timer 1 to function in Modulo mode. Channel 0 is set up to 
                toggle P1_0 (P0_2 on CC2544Dongle) which is I/O location alternative 2. 
                P1_0 is connected to LED1 on the SmartRF05EB (Green LED1 on CC2544Dongle). 
                The Timer tick speed is set to 125 kHz. Channel 0 also controls the terminal 
                count value, which we set to 62500 to achieve toggling at 2 Hz.
                Default clock source and speed (HS RCOSC at 16 MHz).

  Description:  Uses Timer 1 to function in Modulo mode. Channel 1 is set up to 
                toggle P1_0 which is I/O location alternative 2 for CC2543 and 
                CC2545 and channel 2 for CC2541. P1_0 is
                connected to LED1 on the SmartRF05EB. 
                P0_2 is used on the CC2544Dongle.
                The Timer tick speed is
                set to 125 kHz. Channel 0 also controls the terminal count
                value, which we set to 62500 to achieve toggling at 2 Hz.
                Default clock source and speed (HS RCOSC at 16 MHz).

                Timer settings:
                    - Modulo mode
                    - Output compare mode with toggling on compare
                    - 125 kHz tickspeed
                    - Terminal count value 62500

  Notes:        The tickspeed is set via Timer 1's tick speed divider, there is
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

/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn         main
*
* @brief      Uses Timer 1 to function in Modulo mode. Channel 0 is set up to 
*             toggle P1_1 which is I/O location alternative 1. P1_1 is
*             connected to LED3 on the SmartRF05EB. The Timer tick speed is
*             set to 125 kHz. Channel 0 also controls the terminal count
*             value, which we set to 62500 to achieve toggling at 2 Hz.
*
* @param      void
*
* @return     0
*/


int main(void)
{
    /***************************************************************************
     * Setup peripheral I/O for Timer
     */
  
#if (chip==2541 || chip==2543 || chip==2545)
    P1SEL |= BIT0;            // Selects P1_0 as peripheral I/O.
    P1DIR |= BIT0;            // and output.
    PERCFG |= PERCFG_T1CFG;   // Alternative 2 location.
#elif (chip==2544)
    // Select P0_2 for peripheral function Timer1 on channel 1.
    P0SEL1 |= 0x04;
#endif
   
#if (chip==2541)
    // Timer 1 channels 0-1 has priority, then USART 1, then USART 0, then Timer 1 channels 2-3.
    P2DIR |= P2DIR_PRIP0_T1_0_1;
#elif (chip==2543)
    // Set P1_0 Timer 1 to have priority over Timer 3 and 4, and over USART0.
    PPRI = (PPRI & ~PPRI_PRI_P1_1) | PPRI_PRI_P1_1_T1 | PPRI_PRI0P1; 
#elif (chip==2545)
    // Set P1_0 Timer 1 to have priority over Timer 4.
    PPRI = (PPRI & ~PPRI_PRI0P1); 
#endif

  /***************************************************************************
  * Setup Timer settings
  */
  
#if (chip==2541)
    // Set channel 1 to compare mode and to toggle on compare.
    T1CCTL2 = (T1CCTL2 & ~T1CCTLn_CMP) | T1CCTLn_MODE | T1CCTLn_CMP_TOG_ON_CMP;
  
    // Set compare register of channel 1 (terminal count value) to 62500 to get a period of 2 Hz
    T1CC2H = 0x24;
    T1CC2L = 0xF4;
#elif (chip==2543 || chip==2545 || chip==2544)
    // Set channel 1 to compare mode and to toggle on compare.
    T1CCTL1 = (T1CCTL1 & ~T1CCTLn_CMP) | T1CCTLn_MODE | T1CCTLn_CMP_TOG_ON_CMP;
  
    // Set compare register of channel 1 (terminal count value) to 62500 to get a period of 2 Hz
    T1CC1H = 0x24;
    T1CC1L = 0xF4;
#endif
  
    // Set prescalar divider value to 128 to get a tickspeed of 125 kHz and
    // set Timer 1 to modulo mode
    T1CTL = (T1CTL & ~(T1CTL_MODE | T1CTL_DIV)) | T1CTL_MODE_FREERUN | T1CTL_DIV_128;

    // Timer 1 will now start counting...
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