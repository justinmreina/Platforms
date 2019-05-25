/***********************************************************************************
  Filename:     clk_hs_xosc.c

  Description:  Change system clock source to High Speed Crystal Oscillator (HS XOSC)
                and set clock speed to highest clock speed (32 Mhz).

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


/***********************************************************************************
* LOCAL VARIABLES
*/


/***********************************************************************************
* LOCAL FUNCTIONS
*/



/***********************************************************************************
* @fn          main
*
* @brief       Set clock to HS XOSC at 32 Mhz
*
* @param       void
*
* @return      void
*/

void main(void)
{
    /* Select HS XOSC as system clock source and set the clockspeed to 32 Mhz.
       Once the clock source change has been initiated, the clock source should
       not be changed/updated again until the current clock change has finished.
  
       The 32 kHz RCOSC starts to calibrate if not disabled prior to the clock
       change. The calibration takes approx. 2 ms. */

    // Disable the 32 kHz RCOSC calibration
    SLEEPCMD |= OSC32K_CALDIS;
  
    // Change the system clock source to HS XOSC and set the clock speed to 32 MHz.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKCON_CLKSPD_32M;

    // Wait until system clock source has changed to HS XOSC (CLKCONSTA.OSC = 0).
    while(CLKCONSTA & CLKCON_OSC);
    
    /* If calibration is not disabled, the 32 kHz RCOSC starts to calibrate. */
    
    /* Main loop */
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
