//*****************************************************************************
//! @file      cc1101_per_test_api.h
//  
//! @brief    This header file declares api-like functions that the per test
//            will call if a CC1101 was detected.
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

#ifndef CC1101_PER_TEST_API_H
#define CC1101_PER_TEST_API_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "per_test.h"

/******************************************************************************
 * PROTOTYPES
 */
 
/* Functions that will be connected to the PER RF api*/
void perCC1101RegConfig(void);
void perCC1101SendPacket(uint8 *pData);
void perCC1101EnterRx(void);
void perCC1101EnterIdle(void);
void perCC1101EnterSleep(void);
void perCC1101RxTxISR(void);
void perCC1101SetOutputPower(uint8 index);
int8 perCC1101GetGuiTxPower(uint8 index);
void perCC1101WriteTxFifo(uint8 *pData, uint8 len);
float perCC1101GetDataRate(uint8 index);

int8 perCC1101Read8BitRssi(void);

#ifdef  __cplusplus
}
#endif 

#endif