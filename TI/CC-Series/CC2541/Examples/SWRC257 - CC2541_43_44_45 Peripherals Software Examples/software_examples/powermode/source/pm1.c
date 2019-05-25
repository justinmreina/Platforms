/***********************************************************************************
  Filename:     pm1.c

  Description:  Example describing how to go into Power Mode 1

  Comments:     This code example shows how to correctly enter Power Mode 1 and
                wake up using an External Interrupt. The example uses Button 1 (S1)
                on SmartRF05EB (Button S1 on the CC2544Dongle) to generate the 
                External Interrupt. For test purpose, LED 1 (Green LED2 on the CC2544Dongle) 
                is used to show when the SoC enters and exits PM1. When the LED 1 
                (Green LED2 on the CC2544Dongle) is set, the SoC will stay in Active 
                Mode for some time before entering PM1 again, when the LED is cleared.

                Settings:
                  Interrupt:                        External Interrupt
                  System clock oscillator:          HS XOSC oscillator
                  Clock speed:                      32 Mhz

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <power_mode.h>
#include <hal_types.h>
#include <hal_wait.h>
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


/***********************************************************************************
* LOCAL VARIABLES
*/


/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn          button_isr
*
* @brief       Interrupt Service Routine for P0_1, which is connected to
*              button S1 on SmartRF05EB. The ISR only clear the interrupt flags
*              and is used to wake up from powermode
*
* @param       void
*
* @return      void
*/

#pragma vector = P0INT_VECTOR
__interrupt void button_isr(void)
{
    /*
     * Note that the order in which the following flags are cleared is important.
     * For level triggered interrupts (port interrupts) one has to clear the module
     * interrupt flag prior to clearing the CPU interrupt flags.
     */

#if (chip==2544)
    // Clear interrupt status flag for P0_1.
    P0IFG  = ~BIT0;     // Clear interrupt flag by R/W0 method, see datasheet.
#else    
    // Clear interrupt status flag for P0_1.
    P0IFG  = ~BIT1;     // Clear interrupt flag by R/W0 method, see datasheet.
#endif

    // Clear CPU interrupt flag for P0 (IRCON.P0IF).
    P0IF = 0;
}


/***********************************************************************************
* @fn          main
*
* @brief       Entering Power Mode 1 (PM1) using External Interrupt.
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
     * For idle mode both the HS RCOSC and HS XOSC can be used.
     */

    // Change the system clock source to HS XOSC and set the clock speed to 32 MHz.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKCON_CLKSPD_32M;

    // Wait until system clock source has changed to HS XOSC (CLKCON.OSC = 0).
    while(CLKCONSTA & CLKCON_OSC);
    
    // Wait until the LS RCOSC is calibrated.
    halMcuWaitMs(2);     // given 32 MHz system clock
    
    
    /***************************************************************************
     * Setup I/O
     */
    
#if (chip==2541 || chip==2543 || chip==2545) 
    // Initialize P1_0 for SRF05EB LED1.
    P1SEL &= ~BIT0;           // Function as General Purpose I/O.
    P1_0 = 1;                 // LED1 on.
    P1DIR |= BIT0;            // Output.

    // Initialize P0_1 for SRF05EB S1 button.
    P0SEL &= ~BIT1;           // Function as General Purpose I/O
    P0DIR &= ~BIT1;           // Input
    P0INP |= BIT1;            // Select input type as 3-state, S1 has external pulldown.
#elif (chip==2544) 
    // Initialize P0_2 for CC2544Dongle Green LED D2. 
    P0SEL1 &= ~P0SEL1_SELP0_2;  // Function as General Purpose I/O.
    P0_2 = 1;                   // LED1 on.
    PDIR |= PDIR_DIRP0_2;       // Output.

    // Initialize P0_0 for CC2544Dongle S1 button.
    P0SEL0 &= ~P0SEL0_SELP0_0;  // Function as General Purpose I/O.
    PDIR &= ~PDIR_DIRP0_0;      // Input.
#endif
    
    /***************************************************************************
     * Setup interrupt
     *
     * Setup + enable the interrupt source(s) which is/are intended to wake-up
     * the SoC from PM1. Any SoC peripheral interrupt will wake up the SoC from
     * PM1.
     */

#if (chip==2541 || chip==2543 || chip==2545) 
    // Clear CPU interrupt flag for P0 and interrupt status flags for pin 1
    P0IF = 0;
    P0IFG  = ~BIT1;     
    
    // Set individual interrupt enable bit in the peripherals SFR.
    P0IEN |= BIT1;    // P0_1 (Button S1)
#elif (chip==2544)
    // Clear CPU interrupt flag for P0 and interrupt status flags for pin 1
    P0IF = 0;
    P0IFG  = ~BIT0;     
    
    // Set individual interrupt enable bit in the peripherals SFR.
    P0IEN |= BIT0;    // P0_1 (Button S1).
#endif
    
#if (chip==2541)
    // Set interrupt on rising edge on P0_[7:0].
    PICTL &= ~PICTL_P0ICON;
#elif(chip==2543 || chip==2545)
    // Set interrupt on rising edge on P0_[3:0].
    PICTL &= ~PICTL_P0ICONL;
#elif (chip==2544)
    // The CC2544Dongle has button s1 (active low) connected to P0.0.
    // Set interrupt on rising edge on P0.0.
    PICTL &= ~PICTL_P0ICON_0;
#endif

    // Enable P0 interrupts by setting [IEN1.P0IE=1].
    P0IE = 1;

    // Enable global interrupt by setting the [IEN0.EA=1].
    EA = 1;


    /***************************************************************************
     * Setup powermode
     */

    // Set [SLEEPCMD.MODE] to PM1.
    SLEEPCMD = (SLEEPCMD & ~SLEEPCMD_MODE) | SLEEPCMD_MODE_PM1;
    

    /***************************************************************************
     * Test-loop
     * 
     * This while-loop is used for testing by looping the process of entering
     * and exiting powermode.  (test purpose)
     */

    do
    {
#if (chip==2541 || chip==2543 || chip==2545) 
        // Clear SRF05EB LED1.
        P1_0 = 0;
#elif (chip==2544)
        // Clear CC2544Dongle green LED2.
        P0_2 = 0;
#endif
        
        // Enter powermode
        // Sets PCON.IDLE with a 2-byte boundary assembly instruction.
        // NOTE: Cache must be enabled (see CM in FCTL).
        // This method gives the least current consumption.
        EnterSleepModeProcessInterruptsOnWakeup();

       /* 
        * Wake up from powermode
        * After waking up, the system clock source is HS RCOSC, but since
        * the system clock source was HS XOSC before entering PM1, the CC254x
        * will automatically switch the system clock source over to HS XOSC
        * when it becomes ready. We therefore only need to poll until this
        * system clock source has indeed changed to HS XOSC.
        */
        while(CLKCONSTA & CLKCON_OSC);

        // Program is now in Active Mode.
#if (chip==2541 || chip==2543 || chip==2545) 
        // Set SRF05EB LED1.
        P1_0 = 1;
#elif (chip==2544)
        // Set CC2544Dongle green LED2.
        P0_2 = 1;
#endif

        // Wait some time in Active Mode.
        halMcuWaitMs(700);     // given 32 MHz system clock

    } while(1);
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