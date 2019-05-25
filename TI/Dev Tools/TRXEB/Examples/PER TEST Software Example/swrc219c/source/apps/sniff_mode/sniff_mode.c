//*****************************************************************************
//! @file   sniff_mode.c
//
//! @brief  Implementation file for the trxeb sniff_mode app.
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
#include "lcd_dogm128_6.h"
#include "chip_detect.h"
#include "hal_timer_32k.h"
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "menu_driver.h"
#include "sniff_mode.h"
#include "cc112x_spi.h"
#include "cc120x_spi.h"
#include "bsp.h"
#include "bsp_key.h"
#include "cc112x_sniff_mode_api.h"
#include "cc120x_sniff_mode_api.h"
/******************************************************************************
* LOCAL FUNCTIONS
*/
//Functions to be tied to the specific API's
void (*sniffModeRegConfig)(uint16_t);
void (*manualCalibration)(void);
uint8 (*freqConfig)(uint16 index);
uint8 (*masterStartApp)(void);
uint8 (*slaveStartApp)(void);

/******************************************************************************
* LOCAL VARIABLES
*/
static radioChipType_t sniffRadioChipType;

/******************************************************************************
* @fn          sniffMasterStartApp
*
* @brief       
*
* input parameters
*
* @param       pDummy  - pointer to pointer to void. Not used
*
* output parameters
*
* @return      SNIFF_RETURN_SUCCESS
*/
uint8 sniffMasterStartApp(void **pDummy)
{ 
  return masterStartApp();
}

/******************************************************************************
* @fn          sniffSlaveStartApp
*
* @brief       
*
* input parameters
*
* @param       pDummy  - pointer to pointer to void. Not used
*
* output parameters
*
* @return      SNIFF_RETURN_SUCCESS
*/
uint8 sniffSlaveStartApp(void **pDummy)
{ 
  return slaveStartApp();
}

/******************************************************************************
* @fn          sniffInitApp
*
* @brief       
*
* input parameters
*              
* @param        pDummy - manual chip selection for cc1190, casted to void**
*
* output parameters
*
* @return      SNIFF_RETURN_SUCCESS/SNIFF_RETURN_FAILURE
*/
uint8 sniffInitApp(void** pDummy)
{
  // Detect if a supported radio is present  
  trxDetectChipType(&sniffRadioChipType);
  //If detected radio is not cc1101 go directly initiation 
  if(sniffRadioChipType.deviceName == CHIP_TYPE_CC1125
       || sniffRadioChipType.deviceName == CHIP_TYPE_CC1120             
           || sniffRadioChipType.deviceName == CHIP_TYPE_CC1121)
  {
    //Bind functions to the right API
    sniffModeRegConfig = &cc112x_sniffModeRegConfig;
    manualCalibration = &cc112x_manualCalibration;
    freqConfig = &cc112x_FreqConfig;
    masterStartApp = &cc112x_masterStartApp;
    slaveStartApp = &cc112x_slaveStartApp;

    // Configure radio registers
    sniffModeRegConfig(sniffRadioChipType.deviceName);
    // manual calibration according to errata
    manualCalibration();
    //Return success to the menu
    return SNIFF_RETURN_SUCCESS; 
  }
  else if((sniffRadioChipType.deviceName == CHIP_TYPE_CC1200)
          || (sniffRadioChipType.deviceName == CHIP_TYPE_CC1201))
  {
    //Bind functions to the right API
    sniffModeRegConfig = &cc120x_sniffModeRegConfig;
    freqConfig = &cc120x_FreqConfig;
    masterStartApp = &cc120x_masterStartApp;
    slaveStartApp = &cc120x_slaveStartApp;

    // Configure radio registers
    sniffModeRegConfig(sniffRadioChipType.deviceName);

    //Return success to the menu
    return SNIFF_RETURN_SUCCESS; 
  }
  else
  {
    //Not supported radio
    // Issue message to user to insert radio EM 
    lcdBufferClear(0);
    lcdBufferPrintString(0,"Sniff test could not",6,eLcdPage1);
    lcdBufferPrintString(0,"detect a supported",6,eLcdPage2);
    lcdBufferPrintString(0,"radio.",36,eLcdPage3);
    lcdBufferPrintString(0," ",36,eLcdPage4);
    lcdBufferPrintString(0,"Use CC112x PG 2.x.",6,eLcdPage5);
    lcdBufferPrintString(0,"Or CC1200.",6,eLcdPage6);
    lcdSendBuffer(0);
    
    // Make the message visible for max 3 seconds 
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    
    // clear potential button pushes 
    bspKeyPushed(BSP_KEY_ALL);
    
    return SNIFF_RETURN_FAILURE;
  }
}

/******************************************************************************
* @fn          sniffModeFreqConfig
*
* @brief       Writes frequency word depending on user input
*
* input         pFreqIndex - frequency band index casted to void**
*              
* output       none
*
* @return      void
*/
uint8 sniffModeFreqConfig(void** pFreqIndex)
{
  uint16 index = (uint16)*pFreqIndex;
  
  return freqConfig(index);
}