//*****************************************************************************
//! @file       als_trxeb.c
//! @brief      SmartRF TrxEB device driver implementation for SFH5711 ambient
//!             light sensor.
//!
//! Revised     $Date: 2013-12-12 10:20:32 +0100 (Wed, 12 Dec 2013) $
//! Revision    $Revision: 9000 $
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/
#ifndef ALS_EXCLUDE


/**************************************************************************//**
* @addtogroup als_api
* @{
******************************************************************************/


/******************************************************************************
* INCLUDES
*/
#include "bsp.h"
#include "als_sfh5711.h"


/******************************************************************************
* DEFINES
*/
//
// Using ADC channel A2, connected to pin 6.2
//
#define ALS_ADC_CHANNEL         ADC12INCH_2

#define ALS_DIR                 P6DIR
#define ALS_SEL                 P6SEL
#define ALS_OUT                 P6OUT
#define ALS_REN                 P6REN


/******************************************************************************
* LOCAL VARIABLES AND FUNCTION PROTOTYPES
*/


/******************************************************************************
* FUNCTIONS
*/
/**************************************************************************//**
* @brief    This function initializes the ALS. The sensor is powered up and the
*           onboard ADC is configured.
*
*
* @return   None
******************************************************************************/
void alsInit(void)
{
    //
    // Configure ALS power pin as GPIO output high
    //
    ALS_SEL &= ~(BSP_ALS_PWR);
    ALS_DIR |= (BSP_ALS_PWR);
    ALS_OUT |= (BSP_ALS_PWR);

    //
    // Set ALS_OUT pin to peripheral function
    //
    ALS_SEL |= (BSP_ALS_OUT);

    //
    // Configure ADC12
    // Disable conversion (to enable configuration of ADC12)
    // SHT=64 cycles, enable 2.5-V internal reference, turn ADC12 on
    // Use sampling timer, source clock = ACLK, single/channel conversion
    // Turn off temperature sensor, 12 bit resolution, data in unsigend format
    // Input channel is A2 (end of sequence). Use AVcc and AVss as references
    //
    ADC12CTL0 &= ~(ADC12ENC);
    ADC12CTL0 = ADC12SHT02 + ADC12ON;
    ADC12CTL1 = ADC12SHP + ADC12SSEL_1 + ADC12CONSEQ_0;
    ADC12CTL2 = ADC12TCOFF + ADC12RES_2;
    ADC12MCTL0 = ALS_ADC_CHANNEL + ADC12EOS + ADC12SREF_0;

    //
    // Allow the light sensor to settle before sampling any data (~11ms)
    //
    switch(bspSysClockSpeedGet())
    {
    case BSP_SYS_CLK_1MHZ:
        __delay_cycles(11000);
        break;
    case BSP_SYS_CLK_4MHZ:
        __delay_cycles(44000);
        break;
    case BSP_SYS_CLK_8MHZ:
        __delay_cycles(88000UL);
        break;
    case BSP_SYS_CLK_12MHZ:
        __delay_cycles(132000UL);
        break;
    case BSP_SYS_CLK_16MHZ:
        __delay_cycles(176000UL);
        break;
    case BSP_SYS_CLK_20MHZ:
        __delay_cycles(220000UL);
        break;
    case BSP_SYS_CLK_25MHZ:
        __delay_cycles(275000UL);
        break;
    }
}


/**************************************************************************//**
* @brief    This function uninitializes the ALS. This function assumes that
*           the ALS power pin has already been configured as output using,
*           for example, alsInit().
*
* @return   None
******************************************************************************/
void alsUninit(void)
{
    //
    // Disable ADC12
    //
    ADC12CTL0 &= ~(ADC12ON);

    //
    // Power down the light sensor (output low, then input pulldown)
    //
    ALS_OUT &= ~(BSP_ALS_PWR);
    ALS_REN |= (BSP_ALS_PWR);
    ALS_DIR &= ~(BSP_ALS_PWR);
}


/**************************************************************************//**
* @brief    This function triggers and returns ADC conversion from the ALS
*           output. A 12-bit ADC conversion results in a value of [0, 4095].
*
* @return   Returns the value read from the light sensor
******************************************************************************/
uint16_t alsRead(void)
{
    //
    // Clear pending memory register 0 flag
    //
    ADC12IFG &= ~(BIT0);

    //
    // Enable and start conversion
    //
    ADC12CTL0 |= (ADC12ENC | ADC12SC);

    //
    // Wait for conversion to finish
    //
    while(!(ADC12IFG & BIT0))
    {
    }

    //
    // Stop/disable conversion
    //
    ADC12CTL0 &= ~(ADC12ENC | ADC12SC);

    //
    // Return converted value
    //
    return((uint16_t)ADC12MEM0);
}


/**************************************************************************//**
* Close the Doxygen group.
* @}
******************************************************************************/
#endif // #ifndef ALS_EXCLUDE
