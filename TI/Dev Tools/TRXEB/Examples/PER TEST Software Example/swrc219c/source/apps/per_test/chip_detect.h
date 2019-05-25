//*****************************************************************************
//!  @file chip_detect.h	
//    
//!  @brief  header file for radio chip detection
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

#ifndef CHIP_DETECT_H
#define CHIP_DETECT_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/******************************************************************************
 * CONSTANTS
 */ 

/* Chip type constants */
#define CHIP_TYPE_CC1101                0x1101

#define CHIP_TYPE_CC110L                0x0110
#define CHIP_TYPE_CC113L                0x0113 
#define CHIP_TYPE_CC115L                0x0115
  
#define CHIP_TYPE_CC1101_CC1190         0x0190  // CC1101 + CC1190 = 0190
#define CHIP_TYPE_CC1120_CC1190         0x2090  // CC1120 + CC1190 = 2090
#define CHIP_TYPE_CC1120                0x1120
#define CHIP_TYPE_CC1121                0X1121
#define CHIP_TYPE_CC1125                0x1125
#define CHIP_TYPE_CC1175                0x1175
#define CHIP_TYPE_CC1200                0x1200
#define CHIP_TYPE_CC1201                0x1201
#define CHIP_TYPE_CC2500                0x2500
#define CHIP_TYPE_NONE                  0x0000

/* Manual chip select constants*/
#define CC1101_SELECTED                 0x1101
#define CC1120_SELECTED                 0x1120
#define CC1101_CC1190_SELECTED          0x0190
#define CC1120_CC1190_SELECTED          0x2090
/******************************************************************************
 * Prototypes
 */ 

typedef struct
{
	uint16 deviceName;
	uint16 id;
	uint8  ver;
	uint8  xoscFreq;
  
}radioChipType_t;


/******************************************************************************
 * PROTOTYPES
 */
 
/* Will populate the radioChipType struct when called */
uint8 trxDetectChipType(radioChipType_t *pRadioChipType);

#ifdef  __cplusplus
}
#endif

#endif //CHIP_DETECT_H