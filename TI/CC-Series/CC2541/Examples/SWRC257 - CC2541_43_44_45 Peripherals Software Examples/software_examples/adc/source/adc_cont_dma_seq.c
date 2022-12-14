/***********************************************************************************
  File name:    adc_cont_dma_seq.c

  Description:  This example shows how the DMA can be used to store the results
                from an ADC conversion. It performs single-ended ADC sequence in
                continous mode with DMA transfer on end of ADC conversion event.

                The ADC is set up with the following configuration:
                - Continuous mode
                - Single-ended sequence conversion (P0_0 - P0_1)
                - Reference Voltage is VDD on the AVDD pin
                - 9 bit resolution (128 dec rate)

                The DMA is set up with the following configuration:
                - ADCL is source Address
                - Fixed length transfer
                - Byte Size
                - Block Mode non-repetitive
                - Increment source and destination address
                - High Priority memory access
                - Interrupt disabled

                The DMA channels are configured to transfer the ADC conversion
                result to an allocated ADC result table. The DMA triggers are
                the corresponding end of conversion for the ADC channel. When
                the sequence is completed, the ADC is disabled.

                The system clock source is set to the HS XOSC, with no prescaling
                as recommended in the section "ADC Conversion timing" in the
                datasheet.

                PIN0_0 can be connected to the SRF05EB's potmeter by connecting
                PIN 17 on the P18 Debug Connector, with a wire, to PIN 12 on the
                P20 Debug Connector.

                The ADC result is always represented in two's complement form,
                as stated in section "ADC Conversion Result" in the datasheet.

  Note:         The DMA transfer must be given sufficient memory access priority
                so that the transfer can complete before the next result from the
                ADC conversion is ready.

                The DMA can only access the XDATA space, and this restriction
                applies also for the DMA descriptors.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include <hal_types.h>
#include <dma.h>
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

// DMA descriptor n -> DMA channel n.
static __xdata DMA_DESC dma_channel[2];

// ADC result table.
static __xdata uint16 adc_data[8];


/***********************************************************************************
* LOCAL FUNCTIONS
*/

// Prototype for local functions.
void dma_channel_init (DMA_DESC __xdata *dma_p,
                       uint16 __xdata *dest_adr,
                       uint8 lenl,
                       uint8 trig);

/*******************************************************************************
* @fn          main
*
* @brief       Single-ended ADC sequence in continous mode with DMA transfer on
*              end of ADC conversion event.
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
    while (CLKCONSTA & CLKCON_OSC);   //Wait until clock source has changed

    /* Note the 32 kHz RCOSC starts calibrating, if not disabled. */

  
    /****************************************************************************
    * I/O-Port configuration
    * PIN0_0 and P0_1 is configured as ADC input pins.
    */  
    
    APCFG = APCFG_APCFG1 | APCFG_APCFG0;
  
  
    /****************************************************************************
    * DMA configuration:
    * Set up DMA channel 1 and 2 for transfer from ADCL and ADCH to
    * the ADC result table. The ADC will generate the DMA trigger
    * on end of conversion.
    */

    // Set the DMA channel 0 configuration address registers.
    DMA0CFGL = ((uint16)&dma_channel[0]) & 0x00FF;
    DMA0CFGH = ((uint16)&dma_channel[0] >> 8);

    // Set the DMA channel 1 configuration address registers.
    DMA1CFGL = ((uint16)&dma_channel[1]) & 0x00FF;
    DMA1CFGH = ((uint16)&dma_channel[1] >> 8);

    // Configure DMA channel 0.
    dma_channel_init(&dma_channel[0], &adc_data[0], 2, DMA_TRIG_ADC_CH0);

    // Configure DMA channel 1.
    dma_channel_init(&dma_channel[1], &adc_data[1], 2, DMA_TRIG_ADC_CH1);

    // Arm DMA channels, takes 18 clock cycles.
    DMAARM = (DMA_CHANNEL_0 | DMA_CHANNEL_1);
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs


    /****************************************************************************
    * ADC configuration :
    *  - Continous mode
    *  - Single-ended sequence conversion
    *  - Reference Voltage is VDD on the AVDD pin
    *  - 9 bit resolution (128 dec rate)
    */

    // Set [ADCCON2.SREF/SDIV/SCH] bits according to ADC configuration.
    ADCCON2 = ADCCON2_SREF_AVDD | ADCCON2_SDIV_128 | ADCCON2_SCH_AIN1;

    // Set [ADCCON1.STSEL] to continues conversion.
    ADCCON1 = (ADCCON1 & ~ADCCON1_STSEL) | ADCCON1_STSEL_FULL_SPEED;

  
    /* ADC conversion :
    * The ADC will run in continous mode until disabled.
    * Await all transfers being completed.
    */
  
    while( !(DMAIRQ & (DMAIRQ_DMAIF1 | DMAIRQ_DMAIF0)) );

    // Stop continuous mode.
    ADCCON1 |= ADCCON1_ST;
  
    // Right align the 9 bits result data, might need a brakepoint to update debugger.
    // The results are in two's complement form and the value is not converted to signed.
    adc_data[0] = (adc_data[0] >> 7) & 0x01FF;
    adc_data[1] = (adc_data[1] >> 7) & 0x01FF;

    // Loop forever.
    while (1);
}


/***********************************************************************************
* @fn          dma_channel_init
*
* @brief       Initialize DMA channel to
*               - ADCL is source Address
*               - Fixed length transfer
*               - Byte Size
*               - Block Mode non-repetitive
*               - Increment source and destination address
*               - High Priority memory access
*               - Interrupt disabled
*
*
* @param       dma_desc *dma_p     : Pointer to DMA descriptor
*              uint8    *dest_adr  : Destination address
*              uint8    lenl       : Transfer count
*              uint8    trig       : Trigger source
*
* @return      0
*/

void dma_channel_init ( DMA_DESC __xdata *dma_p,
                        uint16 __xdata *dest_adr,
                        uint8 lenl,
                        uint8 trig )
{
    // Setup DMA configuration.-
    dma_p->SRCADDRH  = (uint16)(&X_ADCL) >> 8;
    dma_p->SRCADDRL  = (uint16)(&X_ADCL) & 0x00FF;
    dma_p->DESTADDRH = ((uint16)dest_adr) >> 8;
    dma_p->DESTADDRL = ((uint16)dest_adr);
    dma_p->VLEN      = DMA_VLEN_FIXED;
    dma_p->LENH      = 0;
    dma_p->LENL      = lenl;                   // Tranfer Count
    dma_p->WORDSIZE  = DMA_WORDSIZE_BYTE;
    dma_p->TMODE     = DMA_TMODE_BLOCK;
    dma_p->TRIG      = trig;                   // Channel trigger
    dma_p->SRCINC    = DMA_SRCINC_1;
    dma_p->DESTINC   = DMA_DESTINC_1;
    dma_p->IRQMASK   = DMA_IRQMASK_DISABLE;
    dma_p->M8        = DMA_M8_USE_7_BITS;
    dma_p->PRIORITY  = DMA_PRI_HIGH;
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
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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