//*****************************************************************************
//! @file     sniff_mode.h
//  
//! @brief  
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
#ifndef SNIFF_MODE_H
#define SNIFF_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
#include "chip_detect.h"
#include "hal_lcd_trxeb.h"
#include "menu_system.h"
#include "stdint.h"
 
/******************************************************************************
* CONSTANTS
*/ 
#define SNIFF_RETURN_SUCCESS      0
#define SNIFF_RETURN_FAILURE      1
  
#define PKTLEN              5
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0
/******************************************************************************
 * TYPEDEFS
 */
 
/******************************************************************************
 * GLOBALS
 */ 
extern uint8 sniffInitApp(void **pDummy);
extern uint8 sniffMasterStartApp(void **pDummy);
extern uint8 sniffSlaveStartApp(void **pDummy);
extern uint8 sniffModeFreqConfig(void **pDummy);

extern menu_t        sniffModeMainMenu;
extern menu_t        sniffModeFrequencyMenu;

extern void drawInfo(void);



/******************************************************************************
 * PROTOTYPES
 */


#ifdef  __cplusplus
}
#endif

#endif //PER_TEST_H