/***********************************************************************************
  Filename:     sleep_int.c

  Description:  Sets up Sleep Timer and Sleep Timer interrupt.

  Comments:     This code example shows how to setup the Sleep Timer for using
                it as the source for waking up from a Power Mode. In the example
                the system goes into Power Mode 1 after setting up the Sleep
                Timer and Sleep Timer interrupt. The Sleep Timer is set up to
                generate interrupt every second. When awaken, the ISR toggles
                LED1.

                The clock source for the Sleep Timer is the 32.753 kHz LS RCOSC.

  Note:         Power Mode 2 is typically used when sleep time exceeds 3 ms.
                The sleep timer cannot wake up the device from a PM3 state, 
                then a external interrupt is required. 
                
                The CC2544 does not support PM2 and PM3!

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <power_mode.h>
#include <hal_types.h>
#include <hal_wait.h>
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
* LOCAL VARIABLES
*/
// Union for storing 24 bit sleep timer value. 
typedef union {
    unsigned long value;
    unsigned char byte[4];
} union_32bit;

static union_32bit sleep_timer;


/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn          sleep_isr
*
* @brief       Interrupt service routine for the sleep timer which wakes the
*              system from Power Mode. When awake the flags are cleared and
*              LED1 is toggled.
*
* @param       void
*
* @return      void
*/


#pragma vector = ST_VECTOR
__interrupt void sleep_isr(void)
{
    /* Note that the order in which the following flags are cleared is important.
       For pulse or egde triggered interrupts one has to clear the CPU interrupt
       flag prior to clearing the module interrupt flag. */

#if (chip==2541 || chip==2543 || chip==2545)
    // Toggle SRF05EB LED1.
    P1_0 ^= 1;
#elif (chip==2544)
    // Toggle CC2544Dongle green LED2.
    P0_2 ^= 1;
#endif

    // Clear [IRCON.STIF] (Sleep Timer CPU interrupt flag).
    STIF = 0;
}


/***********************************************************************************
* @fn          main
*
* @brief       Setup Sleep Timer and Sleep Timer interrupt. Goes in and out of
*              Power Mode 1. 
*
* @param       void
*
* @return      0
*/

int main(void)
{
    /***************************************************************************
     * Setup clock & frequency
     *
     * The system clock source used is the HS XOSC at 32 MHz speed.
     */

    // Change the system clock source to HS XOSC and set the clock speed to 32 MHz.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKCON_CLKSPD_32M;

    // Wait until system clock source has changed to HS XOSC (CLKCON.OSC = 0).
    while(CLKCONSTA & CLKCON_OSC);
    
    // We need to wait approx. 2 ms until the 32 kHz RCOSC is calibrated.
    halMcuWaitMs(2);   // Given 32 MHz system clock source.
  
    
    /***************************************************************************
     * Setup I/O
     */    
    
#if (chip==2541 || chip==2543 || chip==2545)
    // Initialize P1_0 for SRF05EB LED1.
    P1SEL &= ~BIT0;           // Function as General Purpose I/O.
    P1_0 = 1;                 // LED1 on.
    P1DIR |= BIT0;            // Output.
#elif (chip==2544) 
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED1 on.
    PDIR |= PDIR_DIRP0_2;       // Output.
#endif

    
    /***************************************************************************
     * Setup Sleep Timer interrupt
     */

    // Clear [IRCON.STIF] (Sleep Timer CPU interrupt flag).
    STIF = 0;
    
    // Set the individual, interrupt enable bit [IEN0.STIE=1].
    STIE = 1;

    // Enable global interrupt by setting the [IEN0.EA=1].
    EA = 1;
    
    
    /***************************************************************************
     * Setup Power Mode in PM1.
     * Power Mode 2 is typically used when sleep time exceeds 3 ms.
     * The CC2544 does not support PM2 and PM3!
     */
    
    // Set power mode, PM1.
    SLEEPCMD = (SLEEPCMD & ~SLEEPCMD_MODE) | SLEEPCMD_MODE_PM1;

    
    /***************************************************************************
     * Setup Sleep Timer
     * 
     * The Sleep Timer is running on the 32 kHz RC oscillator. The CC2545 has
     * the possibility of using an external 32 kHz crystal, the LS XOSC can be 
     * selected by clearing the CLKCONCMD.OSC32K bit. This initiates a clock 
     * source change, CLKCONSTA.OSC32K reflects the current setting. Note this
     * must be done while the system clock source is HS RCOSC.
     */
       
    // To ensure an updated value is read, wait for a positive transition on the
    // 32 kHz clock by polling the SLEEPSTA.CLK32K bit.
    while(!(SLEEPSTA & SLEEPSTA_CLK32K)); // Wait for positive flank on sleep timer. 
    
    // Read the sleep timer current value. 
    sleep_timer.byte[0] = ST0;    // ST0 must be read first.
    sleep_timer.byte[1] = ST1;                  
    sleep_timer.byte[2] = ST2;
    
    while(1)
    {
        // Increment the Sleep Timer value with 32753 cycles to create a new
        // compare match in approx. 1 second.
        sleep_timer.value += 32753;
        
        // When loading a new compare value for the Sleep Timer,
        // the ST2 and ST1 registers must be loaded before ST0. 
        ST2 = sleep_timer.byte[2];
        ST1 = sleep_timer.byte[1];
            
        // Wait until load ready, before writing to STO.
        while( !(STLOAD & STLOAD_LDRDY) );
        ST0 = sleep_timer.byte[0];
      
        // Force the device to enter the power mode set by SLEEPCMD.MODE.
        EnterSleepModeProcessInterruptsOnWakeup();
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