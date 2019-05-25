//**********************************************************************************
//! @file     hal_create_random.c
//
//! @brief   creates a random byte using the ADC connected to the light sensor
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

/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
   
/******************************************************************************
 * CONSTANTS
 */

/**************** Functions ***************************************************/


/******************************************************************************
 * @fn     halCreateRandomByte
 *   
 * @brief  creates a "random" byte by xor-ing three reads of the lower ADC byte
 *         that is not 0xFF or 0x00.
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      random byte
 */
uint8 halCreateRandomByte(void) {
    uint16 savedADC12MEM0 = 0;
    
    /* Sequence of channels, once per ACLK */
    ADC12CTL0 = ADC12SHT02 + ADC12REF2_5V + ADC12REFON + ADC12ON + ADC12MSC;   /* enable 2.5 V internal reference, turn ADC12 on                                */
    ADC12CTL1 = ADC12SHP + ADC12SSEL_3 + ADC12CONSEQ_0;                        /* source clock = ACLK, single channel/single conv                               */
    ADC12CTL2 = ADC12TCOFF + ADC12RES_2;                                       /* turn off temp sensor, 12 bit resolution, data in unsigned format              */
                                                                                                                                                               
    ADC12MCTL0 =  ADC12INCH_2 + ADC12SREF_0 + ADC12EOS;                         /* temporary: use AVcc and AVss as references */
    
    ADC12IFG &= ~(BIT1+BIT0);               /* Clear any pending flags */                                                                 
    ADC12CTL0 |=  ADC12ENC | ADC12SC ;      /* start conversion        */
    /*ADC12IE |= BIT2; */                   /* enable interrupt        */
    ADC12CTL0 |= ADC12REFON;                /* Turn on ADC12 reference */

    /* Delay to stabilize ADC12 reference assuming the fastest MCLK of 16 MHz.*/
    /* 35 us = 1 / 16 MHz * 560                                               */
    __delay_cycles(560);
    asm(" nop");

    /*ADC12IE |= BIT0; */                       /* Enable interrupt */
    ADC12CTL0 |=  ADC12ENC | ADC12SC;

    /* Read the ADC value 3 times and xor the values*/
    __delay_cycles(1000000);
    asm(" nop");
    savedADC12MEM0 = ADC12MEM0;              /* Store the first sampled data */
    __delay_cycles(1000000);
    asm(" nop");
    savedADC12MEM0 ^= ADC12MEM0;             /* XOR last read value with new one */
    __delay_cycles(1000000);
    asm(" nop");
    savedADC12MEM0 ^= ADC12MEM0;             /* XOR last read value with new one */
    
    ADC12CTL0 &= ~(ADC12ON);                /* Turn off ADC12 */

    if(((savedADC12MEM0 & 0x00FF)==0x00FF) || ((savedADC12MEM0 & 0x00FF)==0x0000))
    {
    	/* Defaulting the "random" value */
    	return 0x9D;
    }
    else
    {
      return (uint8) (0x00FF&savedADC12MEM0);
    }
}