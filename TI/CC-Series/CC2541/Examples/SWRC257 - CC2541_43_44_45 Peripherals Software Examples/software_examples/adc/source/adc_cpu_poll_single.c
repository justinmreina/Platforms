/***********************************************************************************
  Filename:     adc_cpu_poll_single.c

  Description:  This example performs single-ended, single channel CPU controlled
                ADC conversion.

                The ADC is set up for the following configuration:
                - Single-ended and single-channel conversion on PIN0_0
                - Reference Voltage is VDD on the AVDD pin
                - 12 bit resolution (512 dec rate)

                The ADC conversion will be CPU triggered, and the CPU will
                poll for end of ADC conversion.

                The system clock source is set to the HS XOSC, with no prescaling
                as recommended in the section "ADC Conversion timing" in the
                datasheet.

                PIN0_0 can be connected to the SRF05EB's potmeter by connecting
                PIN 17 on the P18 Debug Connector, with a wire, to PIN 12 on the
                P20 Debug Connector.

                The conversion result can be seen in the "adc_result" variable.

  Note:         The ADC result is always represented in two's complement form,
                as stated in section "ADC Conversion Result" in the datasheet.

***********************************************************************************/


/***********************************************************************************
* INCLUDES
*/
#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file.
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


/***********************************************************************************
* LOCAL VARIABLES
*/

static int16  adc_result;


/***********************************************************************************
* LOCAL FUNCTIONS
*/


/***********************************************************************************
* @fn          main
*
* @brief       This example performs single-ended, single channel CPU controlled
*              ADC conversion.
*
* @param       none
*
* @return      void
*/

void main(void)
{
    /****************************************************************************
    * Clock setup
    * See basic software example "clk_xosc_cc254x"
    */
  
    // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    while (CLKCONSTA & CLKCON_OSC);   // Wait until clock source has changed
  
    /* Note the 32 kHz RCOSC starts calibrating, if not disabled. */
  
  
    /****************************************************************************
    * I/O-Port configuration
    * PIN0_7 is configured to an ADC input pin.
    */

    // Set [APCFG.APCFG0 = 1].
    APCFG |= APCFG_APCFG0;

  
    /****************************************************************************
    * ADC configuration:
    *  - [ADCCON1.ST] triggered
    *  - 12 bit resolution
    *  - Single-ended
    *  - Single-channel, due to only 1 pin is selected in the APCFG register
    *  - Reference voltage is VDD on AVDD pin
    */

    // Set [ADCCON1.STSEL] according to ADC configuration.
    ADCCON1 = (ADCCON1 & ~ADCCON1_STSEL) | ADCCON1_STSEL_ST;

    // Set [ADCCON2.SREF/SDIV/SCH] according to ADC configuration.
    ADCCON2 = ADCCON2_SREF_AVDD | ADCCON2_SDIV_512 | ADCCON2_SCH_AIN0;

  
    /****************************************************************************
    * ADC conversion :
    * The ADC conversion is triggered by setting [ADCCON1.ST = 1].
    * The CPU will then poll [ADCCON1.EOC] until the conversion is completed.
    */

    // Set [ADCCON1.ST] and await completion (ADCCON1.EOC = 1). 
    ADCCON1 |= ADCCON1_ST;
    while( !(ADCCON1 & ADCCON1_EOC));

    /* Store the ADC result from the ADCH/L register to the adc_result variable.
    * The conversion result resides in the MSB section of the combined ADCH and
    * ADCL registers.
    */
    adc_result = (ADCL >> 4);
    adc_result |= (ADCH << 4);

    // End function with infinite loop (for debugging purposes). 
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