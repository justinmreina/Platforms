/***********************************************************************************
  Filename:     io_in.c

  Description:  Setup one pin on port 0 to generate interrupts on incoming events.
                When a rising edge is detected on the pin, the ISR for the pin
                will be executed. This will toggle LED1 on SRF05EB or the 
                green LED1 on the CC2544Dongle. 

                The pin can be chosen using the P0_PIN macro.

                To test this, the pin that is set up, can be toggled with GND,
                for example using a wire from GND to the pin. 

                The debug interface uses the I/O pins P2_1 as Debug Data and
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

/* Choose which pin to configure. BIT1 will configure pin 1, BIT2 will configure
   pin 2 and so on */
#if (chip==2541 || chip==2543 || chip==2545)
// Set button 1 (S1) on the SmartRF05EB as input interrupt. 
#define P0_PIN BIT1
#elif (chip==2544)
// Set button 1 (S1) on the CC2544Dongle as input interrupt. 
#define P0_PIN BIT0
#endif



/***********************************************************************************
* LOCAL VARIABLES
*/


/***********************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* @fn          p1_ISR
*
* @brief       Interrupt Service Routine for port 1 and the chosen pin.
               Clears interrupt flags and toggles LED.
*
* @param       void
*
* @return      void
*/

#pragma vector = P1INT_VECTOR
__interrupt void p1_ISR(void)
{
    /* Note that the order in which the following flags are cleared is important.
       For level triggered interrupts (port interrupts) one has to clear the module
       interrupt flag prior to clearing the CPU interrupt flags. */
  
    // Clear status flag for pin with R/W0 method, see datasheet.
    P0IFG = ~P0_PIN;
    // Clear CPU interrupt status flag for P0.
    P0IF = 0;

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
* @brief       Setup one pin on port 1 to generate interrupts on incoming events.
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
#if (chip==2541 || chip==2543 || chip==2545)
    P0SEL &= ~P0_PIN;           // Select pin to function as General Purpose I/O.
    P0DIR &= ~P0_PIN;           // Select direction as input.
    P0INP &= ~P0_PIN;           // Pull up/pull down.
    PPULL &= ~(PPULL_PDUP0H | PPULL_PDUP0L);     // P0_[7:4] and P0_[3:0] pull up.
#elif (chip==2544)
    P0SEL0 &= ~P0_PIN;          // Select pin to function as General Purpose I/O.
    PDIR &= ~P0_PIN;            // Select direction as input.
    PINP &= ~P0_PIN;            // Pull up/pull down.
    PPULL &= ~(PPULL_PDUP0_0 | PPULL_PDUP0_1 | PPULL_PDUP0_2 | PPULL_PDUP0_3);     // P0_0, P0_1, P0_2 and P0_3 pull up.
#endif

#if (chip==2541 || chip==2543 || chip==2545)
    // Configure P1_0 as GPIO output for LED1.
    P1SEL &= BIT0;      // GPIO.
    P1DIR |= BIT0;      // Output.
    P1_0 = 0;           // LED1 off.
#elif (chip==2544)
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED1 on.
    PDIR |= PDIR_DIRP0_2;       // Output.
#endif
    
    
    /***************************************************************************
     * Setup interrupt
     *
     */
    
    // Clear interrupt flags for P1.
    P0IFG = ~P0_PIN;            // Clear status flag for pin.
    P0IF = 0;                   // Clear CPU interrupt status flag for P0.

    // Set individual interrupt enable bit in the peripherals SFR.
    P0IEN |= P0_PIN;                              // Enable interrupt from pin.


#if (chip==2541 || chip==2543 || chip==2545)
    PICTL &= ~(PICTL_P0ICONH | PICTL_P0ICONL);    // Rising edge, P0_[7:4] and P0_[3:0].
#elif (chip==2544)
    PICTL &= ~(PICTL_P0ICON_0 | PICTL_P0ICON_1 | PICTL_P0ICON_2 | PICTL_P0ICON_3);    // Rising edge on P0_0, P0_1, P0_2 and P0_3. 
#endif
    
    // Enable P0 interrupts.
    IEN1 |= IEN1_P0IE;

    // Enable global interrupt by setting the IEN0.EA=1.
    EA = 1;

    /* Now everything is set up and the rest of program will run through the ISR */
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