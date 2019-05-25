/***********************************************************************************
  Filename:     powermode3.c

  Description:  This software example shows how to properly enter Power Mode 3
                and then exit upon Port 0 Interrupt. The SRF05EB LED1 is cleared
                before entering Power Mode 3, and then set by the Port 0 ISR.
                Hence, the LED1 is ON in Active Mode and OFF in Power Mode 3.

  Note:         PM2 and PM3 is not supported on the CC2544. 

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <power_mode.h>
#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/***********************************************************************************
* CONSTANTS
*/
// Wait time in Active mode.
#define ACT_MODE_TIME  50000


/***********************************************************************************
* LOCAL VARIABLES
*/
// Variable for active mode duration.
static uint32 __xdata activeModeCnt = 0;


/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn          setup_port_interrupt
*
* @brief       Function which sets up the Port 0 Interrupt for Power Mode 3 usage.
*
* @param       void
*
* @return      void
*/
void setup_port_interrupt(void)
{
    // Clear Port 0 Interrupt flags.
    P0IF = 0;
    P0IFG = 0x00;

    // Interrupt enable on all pins on port 0.
    P0IEN = 0xFF;   

    // Enable CPU Interrupt for Port 0 (IEN1.P0IE = 1).
    P0IE = 1;

    // Enable Global Interrupt by setting the (IEN0.EA = 1).
    EA = 1;
}


/***********************************************************************************
* @fn          port0_isr
*
* @brief       Port 0 Interrupt Service Routine, which executes when SRF05EB S1
*              (P0_1) is pressed.
*
* @param       void
*
* @return      void
*/
#pragma vector = P0INT_VECTOR
__interrupt void port0_isr(void)
{
    // Note that the order in which the following flags are cleared is important.

    // Clear Port 0 Interrupt Flags.
    P0IFG = 0x00;

    // Clear CPU Interrupt Flag for P0 (IRCON.P0IF = 0).
    P0IF = 0;

    // Set LED1 to indicate Active Mode.
    P1_0 = 1;
}


/***********************************************************************************
* @fn          main
*
* @brief       Enter Power Mode, exit Power Mode 3 using Port 0 Interrupt.
*
* @param       none
*
* @return      0
*/

void main(void)
{
    /***************************************************************************
     * Setup I/O
     */
    // Initialize P1_0 for SRF05EB LED1.
    P1SEL &= ~BIT0;           // Function as General Purpose I/O.
    P1_0 = 1;                 // LED1 on.
    P1DIR |= BIT0;            // Output.

    // Initialize P0_1 for SRF05EB S1 button.
    P0SEL &= ~BIT1;           // Function as General Purpose I/O.
    P0DIR &= ~BIT1;           // Input.
    P0INP |= BIT1;            // 3-state input.


    /***************************************************************************
     * Setup interrupt
     *
     * Setup and enable Port 0 Interrupt, which shall wake-up the SoC from
     * Power Mode 3.
     */
    setup_port_interrupt();
    
    
    /***********************************************************************
     * Setup powermode
     */
    // Set [SLEEPCMD.MODE] to PM3.
    SLEEPCMD = (SLEEPCMD & ~SLEEPCMD_MODE) | SLEEPCMD_MODE_PM3;


    /* Main Loop, enter/exit Power Mode 3. */
    while(1)
    {
        // Wait some time in Active Mode, and clear LED1 before
        // entering Power Mode 3.
        for(activeModeCnt = 0; activeModeCnt < ACT_MODE_TIME; activeModeCnt++);
        P1_0 = 0;
        
        // Enter powermode
        // Sets PCON.IDLE with a 2-byte boundary assembly instruction.
        // NOTE: Cache must be enabled (see CM in FCTL).
        // This method gives the least current consumption.
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