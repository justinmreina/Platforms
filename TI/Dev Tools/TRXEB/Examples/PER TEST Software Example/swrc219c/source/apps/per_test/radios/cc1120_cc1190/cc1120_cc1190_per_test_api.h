//*****************************************************************************
//! @file      cc1120_cc1190_per_test_api.h
//  
//! @brief    This header file declares api-like functions that the per test
//            will call if a cc1120_CC1190 combo was selected.
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

#ifndef CC1120_CC1190_PER_TEST_API_H
#define CC1120_CC1190_PER_TEST_API_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include "msp430.h"
#include "hal_types.h"
#include "hal_defs.h"
#include "per_test.h"
/******************************************************************************
 * CONSTANTS
 */
    
#define TRXEM_PORT_SEL         P3SEL
#define TRXEM_PORT_OUT         P3OUT
#define TRXEM_PORT_DIR         P3DIR
#define TRXEM_PORT_IN          P3IN
    
#define TRXEM_CC1190_PORT_SEL  P1SEL
#define TRXEM_CC1190_PORT_OUT  P1OUT
#define TRXEM_CC1190_PORT_DIR  P1DIR
#define TRXEM_CC1190_PORT_IN   P1IN
    
#define TRXEM_CC1190_PA        BIT4
#define TRXEM_CC1190_LNA       BIT4
#define TRXEM_CC1190_HGM       BIT5

    
/******************************************************************************
 * PROTOTYPES
 */
 
/* Functions that will be connected to the PER RF api*/
void perCC1120CC1190RegConfig(void);
void perCC1120CC1190SendPacket(uint8 *pData);
void perCC1120CC1190EnterRx(void);
void perCC1120CC1190EnterIdle(void);
void perCC1120CC1190EnterSleep(void);
void perCC1120CC1190RxTxISR(void);
void perCC1120CC1190SetOutputPower(uint8 index);
void perCC1120CC1190WriteTxFifo(uint8 *pData, uint8 len);

float perCC1120CC1190GetDataRate(uint8 index);
int8 perCC1120CC1190GetGuiTxPower(uint8 index);
int8 perCC1120CC1190Read8BitRssi(void);

#ifdef  __cplusplus
}
#endif 


#endif