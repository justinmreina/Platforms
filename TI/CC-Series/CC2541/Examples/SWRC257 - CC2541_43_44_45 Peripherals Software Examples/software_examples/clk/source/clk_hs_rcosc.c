/***********************************************************************************
  Filename:     clk_hs_rcosc.c

  Description:  Set system clock source to High Speed RC oscillator (HS RCOSC)
                and set system clock speed = 2 Mhz.

                Initially, the HS RCOSC is not calibrated.
                To do so, the system clock source must be changed to the HS XOSC.
                Once the system clock source is HS XOSC then the chip will start
                calibrating both the HS RCOSC and the low power LS RCOSC. The
                LS RCOSC calibration takes about 2 ms. The calibration of LS
                RCOSC can be disabled prior to the clock change, by setting
                [SLEEPCMD.OSC32K_CALDIS = 1]. LS RSOSC is calibrated to
                32.753 kHz.
        
                The following is not possible before calibration is completed:
                 - Changing system clock source.
                 - Entering PM {1-3}.
        
***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
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
* @fn          main
*
* @brief       Set system clock source to HS RCOSC @ 2 MHz
*
* @param       void
*
* @return      void
*/

void main(void)
{
    // Change the system clock source to HS XOSC and set the clock speed to 32 MHz.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKCON_CLKSPD_32M;

    // Wait until system clock source has changed to HS XOSC (CLKCONSTA.OSC = 0).
    while(CLKCONSTA & CLKCON_OSC);
    
    // Wait until the LS RCOSC is calibrated.
    halMcuWaitMs(2);     // given 32 MHz system clock.

    // Change system clock source to HS RCOSC and set clock speed = 2 MHz.
    CLKCONCMD = (CLKCONCMD & ~CLKCON_CLKSPD) | CLKCON_OSC | CLKCON_CLKSPD_2M;

    // Wait until the system clock source has indeed changed to HS RCOSC.
    while ( !(CLKCONSTA & CLKCON_OSC) );
   
    /* Loop forever */
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