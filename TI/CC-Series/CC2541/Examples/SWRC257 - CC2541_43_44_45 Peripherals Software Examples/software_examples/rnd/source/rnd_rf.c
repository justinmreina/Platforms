/*******************************************************************************
  Filename:     rnd_rf.c

  Description:  This example generates several true-random numbers based on 
                radio noise measurements. The radio is enabled in RX mode without
                sync, and the radio ADC seeds the Random Number Generator.

                The random numbers are stored in the array "rndArray", which can
                be viewed by the debugger.


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
#define ARRAY_SIZE 8

// Link Layer Engine commands
#define CMD_DEMOD_TEST    0x02
#define CMD_SHUTDOWN      0x01

/*******************************************************************************
* LOCAL VARIABLES
*/


/*******************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* @fn          main
*
* @brief       Sets up the clock as recommended in the datasheet. Puts the radio
*              in RX without sync, and uses the radio ADC value as seed and
*              generates a random number.
*
* @param       void
*
* @return      void
*******************************************************************************/
void main(void)
{
    static uint8 rndArray[ARRAY_SIZE];
  
    /****************************************************************************
     * Clock setup
     */
  
    // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    // Wait until clock source has changed
    while (CLKCONSTA & CLKCON_OSC);
  
    // Note the 32 kHz RCOSC starts calibrating, if not disabled.
  
    /****************************************************************************
     * Radio setup
     */
  
    // Enable Link Layer Engine.
    LLECTRL |= BIT0;
  
    // Set minimum LNA gain.
    LNAGAIN = 0x00;
  
    // Set lowest possible frequency to avoid signals in ISM band.
    FREQCTRL = 0x00;
  
    // Enable radio in Rx without sync search.
    while (RFST != 0);
    RFST = CMD_DEMOD_TEST;
  
    // Wait for modem to be running.
    while (RFRND == 0);
  
    // Seed RNG.
    RNDL = RFRND;
    RNDL = RFRND;
  
    uint8 j;
    for (j = 0; j < ARRAY_SIZE; j++)
    {
        // Read 8 random bytes into CRC generation.
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
        RNDH = RFRND;
    
        // Read out LSB of CRC state
        // Storing the random numbers, debug to see the value.
        rndArray[j] = RNDL;
  }
  
    // Shut down radio.
    while (RFST != 0);
    RFST = CMD_SHUTDOWN;
  
    // Disable Link Layer Engine.
    LLECTRL &= ~BIT0;
  
    // End function with infinite loop (for debugging purposes). 
    while(1);
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