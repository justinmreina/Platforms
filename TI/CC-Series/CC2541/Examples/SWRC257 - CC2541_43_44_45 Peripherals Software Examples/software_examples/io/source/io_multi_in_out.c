/***********************************************************************************
  Filename:     io_multi_in_out.c

  Description:  Use multiple pins to communicate with each other. P0_3 is
                configured as output and P1_4 is configured as input. Button S1
                on SMARTRF05EB is used to toggle P0_3. Port 1 will generate
                interrupt on incoming event. So, by connecting P0_3 (PIN11 on
                Debug Connector P20) and P1_4 (PIN18 on Debug Connector P18),
                with a wire, a port 1 interrupt will be generated when the
                button is pushed. When a falling edge is detected on P1_4, the
                ISR for that pin will be executed. This will toggle the LED1
                (green LED2 on the CC2544Dongle) on the SRF05EB.

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


/***********************************************************************************
* @fn          button_isr
*
* @brief       Interrupt Service Routine for port 1. Clears flags for pin 4
               and toggles green LED.
*
* @param       void
*
* @return      void
*/

#if(chip==2541 || chip==2543 || chip==2545)
#pragma vector = P0INT_VECTOR
#elif (chip==2544)
#pragma vector = P1INT_VECTOR
#endif
__interrupt void button_isr(void)
{
    /*
     * Note that the order in which the following flags are cleared is important.
     * For level triggered interrupts (port interrupts) one has to clear the module
     * interrupt flag prior to clearing the CPU interrupt flags.
     */

#if(chip==2541 || chip==2543 || chip==2545)
    // Clear interrupt status flag for P0_2.
    P0IFG  = ~BIT2;     // Clear interrupt flag by R/W0 method, see datasheet.
#elif (chip==2544)
    // Clear interrupt status flag for P1_0.
    P1IFG  = ~BIT0;     // Clear interrupt flag by R/W0 method, see datasheet.
#endif

#if (chip==2544)
    // Toggle CC2544Dongle green LED2.
    P0_2 ^= 1;
#else
    // Toggle SRF05EB LED1.
    P1_0 ^= 1;
#endif

#if(chip==2541 || chip==2543 || chip==2545)
    // Clear CPU interrupt flag for P0 (IRCON.P0IF).
    P0IF = 0;
#elif (chip==2544)
    // Clear CPU interrupt flag for P0 (IRCON.P0IF).
    P1IF = 0;
#endif

}


/***********************************************************************************
* @fn          main
*
* @brief       Setup P0_3 to function as an output pin which is toggled when
*              button is pushed. Setup P0_2 (P1.0 on the CC2544Dongle) so it generates interrupt upon
*              incoming events.
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
    P0SEL &= ~BIT1;           // Select P0_1 (S1) to function as General Purpose I/O (default).
    P0DIR &= ~BIT1;           // Set direction to input (default).
    P0INP |= BIT1;            // Select input type as 3-state, S1 has external pulldown.
    
    // Port 0.
    P0SEL &= ~BIT3;           // Select P0_3 to function as General Purpose I/O.
    P0DIR |= BIT3;            // Select direction to output.
#elif (chip==2544) 
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED1 on.
    PDIR |= PDIR_DIRP0_2;       // Output.

    // Initialize P0_0 for CC2544Dongle S1 button.
    P0SEL0 &= ~P0SEL0_SELP0_0;  // Function as General Purpose I/O.
    PDIR &= ~PDIR_DIRP0_0;      // Input.
    
    // Port 0
    P0SEL1 &= ~P0SEL1_SELP0_3;  // Select P0_3 to function as General Purpose I/O.
    PDIR |= PDIR_DIRP0_3;       // Select direction to output.
#endif  

#if(chip==2541)    
    P0SEL &= ~BIT2;           // Select P1_4 to function as General Purpose I/O.
    P0DIR &= ~BIT2;           // Set direction to input.
    P0INP &= ~BIT2;           // Select input type as pull up/pull down.
    P2INP &= ~P2INP_PDUP0;     // Port 0 pull down. 
#elif(chip==2543 || chip==2545)
    P0SEL &= ~BIT2;           // Select P1_4 to function as General Purpose I/O.
    P0DIR &= ~BIT2;           // Set direction to input.
    P0INP &= ~BIT2;           // Select input type as pull up/pull down.
    PPULL &= ~PPULL_PDUP1L;   // Pull up.
#elif(chip==2544)
    P1SEL0 &= ~P1SEL0_SELP1_0;  // Select P1_0 to function as General Purpose I/O.
    PDIR &= ~PDIR_DIRP1_0;      // Set direction to input.
#endif
    
    /***************************************************************************
     * Setup interrupt
     *
     */


#if(chip==2541 || chip==2543 || chip==2545)
    // Clear interrupt flags for P0.
    P0IFG = ~BIT2;            // Clear status flag for P0_2.
    P0IF = 0;                 // Clear CPU interrupt status flag for P0.
    
    // Set individual interrupt enable bit in the peripherals SFR.
    P0IEN |= BIT2;            // Enable interrupt from P0_2.
#elif (chip==2544) 
    // Clear interrupt flags for P1
    P1IFG = ~BIT0;            // Clear status flag for P1_0.
    P1IF = 0;                 // Clear CPU interrupt status flag for P1.

    // Set individual interrupt enable bit in the peripherals SFR.
    P1IEN |= BIT0;            // Enable interrupt from P1_0.
#endif
    
    // Generate interrupt on falling edge from P1_4.
#if(chip==2541)
    // Set interrupt on rising edge for P0_[7:0].
    PICTL |= ~PICTL_P0ICON;
#elif(chip==2543 || chip==2545)
    // Set interrupt on rising edge for P0_[3:0].
    PICTL |= ~PICTL_P0ICONL;    
#elif (chip==2544) 
    // Set interrupt on rising edge for P0_[3:0].
    PICTL |= ~PICTL_P1ICON_0;
#endif


#if(chip==2541 || chip==2543 || chip==2545)
    // Enable P0 interrupts.
    IEN1 |= IEN1_P0IE ;
#elif (chip==2544)
    // Enable P1 interrupts.
    IEN2 |= IEN2_P1IE ;
#endif

    // Enable global interrupts.
    EA = 1;
    
    // Loop counting variable.     
    unsigned short i; 
   
    /* Now everything is set up */
    while(1)
    {
        // if button is pushed, toggle P2_0.
        if ( (P0 & BUTTON1_0PORT_PIN) )
        {
          // Debounce function for button S1.
          for(i=0; i<500; i++) 
          { 
            if(P0 & BUTTON1_0PORT_PIN)
            {
              i = 0; 
            }
          } 
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