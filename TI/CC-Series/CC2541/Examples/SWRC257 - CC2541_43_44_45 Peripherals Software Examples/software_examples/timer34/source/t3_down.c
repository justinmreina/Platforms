/*******************************************************************************
  Filename:    t3_down.c

  Description: Runs Timer 3 in down mode. Initially, LED1 (green LED on CC2544Dongle) 
               is turned on. When the Timer has counted down to 0x00, an interrupt 
               occurs, it turns LED1 (green LED on CC2544Dongle) off and LED3 
               (Red LED on CC2544Dongle) on. The LEDs are toggled by the interrupt, 
               not the peripheral I/O from the Timer.

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

// Variable that terminates the program when set to zero by t3_isr().
static uint8 running;


/*******************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* @fn      t3_isr
*
* @brief   Interrupt handler for Timer 3 overflow interrupts. LED1 is turned
*          off and LED3 is turned on. Interrupts from Timer 3 are level
*          triggered, so the module interrupt flag is cleared
*          prior to the CPU interrupt flag.
*
* @param   void
*
* @return  void
*
*******************************************************************************/
#pragma vector = T3_VECTOR
__interrupt void t3_isr(void)
{
    // Clears the module interrupt flag.
    T3OVFIF = 0;

#if (chip==2541)
    P1_0 = 0;   // Turn off SRF05EB LED1.
    P1_4 = 1;   // Turn on SRF05EB LED3.
#elif (chip==2543)
    P1_0 = 0;   // Turn off SRF05EB LED1.
    P1_1 = 1;   // Turn on SRF05EB LED3.
#elif (chip==2545)
    P1_0 = 0;   // Turn off SRF05EB LED1.
    P2_1 = 1;   // Turn on SRF05EB LED3.
#elif (chip==2544)
    P0_2 = 0;   // Turn off green LED1.
    P0_1 = 1;   // Turn on red LED2.
#endif

    // Makes the program terminate by setting the running variable to 0.
    running = 0;

    // Clears the CPU interrupt flag.
    T3IF = 0;
}

/*******************************************************************************
* @fn          main
*
* @brief       LED1, LED3 and Timer 3 are all initialized. LED1 is turned on.
*              The rest of the program is executed by an interrupt from Timer 3.
*              This interrupt occurs when the Timer has counted down from T3CC0
*              to 0x00, and it turns off LED1 and turns on LED3. The LEDs are in
*              no way directly related to the Timer, they are just used to
*              indicate what is happening.
*
* @param       void
*
* @return      0
*******************************************************************************/
int main(void)
{
    /***************************************************************************
     * Setup clock & frequency
     * 
     * Configures the Timer tick speed setting, resulting in a Timer tick
     * frequency of 250 kHz.
     */
    CLKCONCMD = (CLKCONCMD & ~CLKCON_TICKSPD) | CLKCON_TICKSPD_250K;
  
  
    /***************************************************************************
     * Setup I/O
     * 
     * Select P1_0 and P1_1 direction to output
     */
#if (chip==2541)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~(BIT4 | BIT0);   
    // Set LED1 and LED3 as output.
    P1DIR |= (BIT0 | BIT4);
    P1_0 = 1;   // Turn on SRF05EB LED1.
    P1_4 = 0;   // Turn off SRF05EB LED3.   
#elif (chip==2543)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~(BIT1 | BIT0);   
    // Set LED1 and LED3 as output.
    P1DIR |= (BIT0 | BIT1);
    P1_0 = 1;   // Turn on SRF05EB LED1.
    P1_1 = 0;   // Turn off SRF05EB LED3.
#elif (chip==2545)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~BIT0; 
    P2SEL &= ~BIT1; 
    // Set LED1 and LED3 as output.
    P1DIR |= BIT0;
    P2DIR |= BIT1;
    P1_0 = 1;   // Turn on SRF05EB LED1.
    P2_1 = 0;   // Turn off SRF05EB LED3.
#elif (chip==2544)
    // LED1 (GREEN) and LED2 (RED) as GPIO.
    P0SEL0 &= ~P0SEL0_SELP0_1; 
    P0SEL1 &= ~P0SEL1_SELP0_2;
    // Set LED1 (GREEN) and LED2 (RED) as output.
    PDIR |= (PDIR_DIRP0_1 | PDIR_DIRP0_2);
    P0_2 = 1;   // Turn on green LED1.
    P0_1 = 0;   // Turn off red LED2.     
#endif
  
  
    /***************************************************************************
     * Setup interrupt
     *
     * Enables global interrupts (IEN0.EA = 1) and interrupts from Timer 3
     * (IEN1.T3IE = 1).
     */
    EA = 1;
    T3IE = 1;
  
    
    /***************************************************************************
     * Timer 3 setup 
     *
     * Timer 3 channel 0 compare value, sets the initial value which the Timer
     * is to count down from.
     */
    T3CC0 = 0xF0;

    /* Timer 3 control (T3CTL) configuration:
     * - Prescaler divider value: 128.
     * - Interrupts enabled.
     * - Down mode.
     * and start the Timer.
     */
    T3CTL = T3CTL_DIV_128 | T3CTL_OVFIM | T3CTL_MODE_DOWN | T3CTL_START;

  // Loop until the running variable is set to 0 by t3_isr().
  running = 1;
  while (running);

  return 0;
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