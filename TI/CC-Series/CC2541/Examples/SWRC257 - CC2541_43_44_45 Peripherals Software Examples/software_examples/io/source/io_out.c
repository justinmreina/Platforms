/***********************************************************************************
  Filename:     io_out.c

  Description:  Setup P0_3 to generate interrupt when output is changed.
                Button S1 on SmartRF05EB or the CC2544Dongle is used to toggle P0_3. 
                When a rising edge is detected on the pin, the ISR for the pin
                will be executed. This will toggle LED1 (Green LED on the CC2544Dongle).

  Note:         The debug interface uses the I/O pins P2_1 as Debug Data and
                P2_2 as Debug Clock during Debug mode. These I/O pins can be
                used as general purpose I/O only while the device is not in
                Debug mode. Thus the debug interface does not interfere with any
                peripheral I/O pins. If interrupt is enabled for port 2, the
                interrupt will be triggered continuously when in Debug mode.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file.
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
#if (chip==2541 || chip==2543 || chip==2545)
#define BUTTON1_0PORT_PIN         2
#elif (chip==2544)
#define BUTTON1_0PORT_PIN         1
#endif


/***********************************************************************************
* LOCAL VARIABLES
*/


/***********************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* @fn          p0_ISR
*
* @brief       Interrupt Service Routine for port 0. Clears flags for pin 0
*              and toggles LED1.
*
* @param       void
*
* @return      void
*/

#pragma vector = P0INT_VECTOR
__interrupt void p0_ISR(void)
{
    /* Note that the order in which the following flags are cleared is important.
       For level triggered interrupts (port interrupts) one has to clear the module
       interrupt flag prior to clearing the CPU interrupt flags. */
    
    // Clear status flag for pin with R/W0 method, see datasheet.
    P0IFG = ~BIT3;
    
#if (chip==2541 || chip==2543 || chip==2545)
    // Toggle SRF05EB LED1.
    P1_0 ^= 1;
#elif (chip==2544)
    // Toggle GREEN LED2 on CC2544Dongle.
    P0_2 ^= 1;
#endif
    // Clear CPU interrupt status flag for P0.
    P0IF = 0;
}


/***********************************************************************************
* @fn          main
*
* @brief       Setup one pin on port 0 to generate interrupts when output is
*              changed
*
* @param       void
*
* @return      0
*/
int main(void)
{
    /***************************************************************************
     * Setup I/O
     *
     */
  
#if(chip==2541 || chip==2543 || chip==2545)
    // Initialize P1_0 for SRF05EB LED1.
    P1SEL &= ~BIT0;           // Function as General Purpose I/O.
    P1_0 = 1;                 // LED1 on.
    P1DIR |= BIT0;            // Output.

    // Initialize P0_1 for SRF05EB S1 button.
    P0SEL &= ~BIT1;           // Function as General Purpose I/O.
    P0DIR &= ~BIT1;           // Input.
    P0INP |= BIT1;            // Select input type as 3-state, S1 has external pulldown.
    
    // Select P0_3 to function as General Purpose I/O and output.
    P0SEL &= ~BIT3;
    P0DIR |= BIT3;
#elif (chip==2544) 
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED2 on.
    PDIR |= PDIR_DIRP0_2;       // Output.

    // Initialize P0_0 for CC2544Dongle S1 button.
    P0SEL0 &= ~P0SEL0_SELP0_0;  // Function as General Purpose I/O.
    PDIR &= ~PDIR_DIRP0_0;      // Input.
    
    // Select P0_3 to function as General Purpose I/O and output.
    P0SEL1 &= ~P0SEL1_SELP0_3;
    PDIR |= PDIR_DIRP0_3;
#endif
       

    
    /***************************************************************************
     * Setup interrupt
     *
     */

    // Clear interrupt flags for P0.
    P0IFG = ~BIT3;                     // Clear status flag for P0_0.
    P0IF = 0;                          // Clear CPU interrupt status flag for P0.

    // Set individual interrupt enable bit in the peripherals SFR.
    P0IEN = BIT3;

#if(chip==2541)
    // Set interrupt on rising edge for P0_[7:0].
    PICTL &= ~PICTL_P0ICON;
#elif(chip==2543 || chip==2545)
    // Set interrupt on rising edge for P0_[3:0].
    PICTL &= ~PICTL_P0ICONL;
#elif (chip==2544) 
    // Set interrupt on rising edge for P0_[3:0].
    PICTL &= ~PICTL_P0ICON_3;
#endif

    // Enable P0 interrupts.
    P0IE = 1;

    // Enable global interrupt by setting the [IEN0.EA=1].
    EA = 1;

    
    /***************************************************************************
     * Main loop
     *
     */
    
    // Loop counting variable.     
    unsigned short i; 
  
    while(1)
    {
        // If button is pushed, toggle P0_0.
        if(P0 & BUTTON1_0PORT_PIN)
        {
          // Debounce function for button S1.
          for(i=0; i<500; i++) 
          { 
            if(P0 & BUTTON1_0PORT_PIN)
            {
              i = 0; 
            }
          } 

          // Toggle P0_3
          P0_3 ^= 1;
        }
    }
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
***********************************************************************************/
