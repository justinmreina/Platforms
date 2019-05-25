//*****************************************************************************
//! @file       easyLink.c
//  
//! @brief  
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

/*****************************************************************************
* INCLUDES
*/

#include  <msp430.h>
#include "lcd_dogm128_6.h"
#include "easyLink.h"
#include "hal_timer_32k.h"
#include "menu_system.h"
#include "menu_driver.h"
#include "bsp.h"
#include "bsp_key.h"
/*****************************************************************************
 * TYPEDEFS
 */  
typedef void  (*VFPTR_U8PTR)(uint8 *a);
typedef void  (*VFPTR_ISR_FUNC_PTR)(ISR_FUNC_PTR a);
  
/******************************************************************************
* LOCAL VARIABLES
*/
radioChipType_t pRadioChipType;

/******************************************************************************
* STATIC FUNCTIONS
*/

VFPTR               easyLinkRunRX;
VFPTR               easyLinkRunTX;

/******************************************************************************
 * @fn          initEasyLink
 *
 * @brief      Function assigns functions for the easyLink test depending on
 *             what type of radio connected to the board. Currently supporting
 *             CC1101, CC1120, CC1121 and CC1125
 *                
 * @param        pDummy - pointer to pointer to void. no value used
 *
 * @return      0 -SUCCESS 1- FAILURE
 */
uint8 initEasyLink(void** pDummy)
{
  // detect chip type connected to EB
  trxDetectChipType(&pRadioChipType);
  
  // assign functions depending on radio detected
  if(pRadioChipType.deviceName == CHIP_TYPE_CC1101)
  {
    easyLinkRunTX = cc1101RunTX;
    easyLinkRunRX = cc1101RunRX;
  }
  else if(pRadioChipType.deviceName == CHIP_TYPE_CC110L)
  {
    easyLinkRunTX = cc110LRunTX;
    easyLinkRunRX = cc110LRunRX;
  }
  else if(pRadioChipType.deviceName == CHIP_TYPE_CC113L)
  {
    easyLinkRunRX = cc113LRunRX;
  }
  else if(pRadioChipType.deviceName == CHIP_TYPE_CC115L)
  {
    easyLinkRunTX = cc115LRunTX;
  }
  else if((pRadioChipType.deviceName == CHIP_TYPE_CC1121)||
            (pRadioChipType.deviceName == CHIP_TYPE_CC1120)||
              (pRadioChipType.deviceName == CHIP_TYPE_CC1125))
  {
    easyLinkRunTX = cc112xRunTX;
    easyLinkRunRX = cc112xRunRX;
  }
  else if((pRadioChipType.deviceName == CHIP_TYPE_CC1200)||
            (pRadioChipType.deviceName == CHIP_TYPE_CC1201))
  {
    easyLinkRunTX = cc120xRunTX;
    easyLinkRunRX = cc120xRunRX;
  }
  else
  {
    // Issue message to user to insert radio EM
    lcdBufferClear(0);
    lcdBufferPrintString(0,"Test could not",18,eLcdPage2);
    lcdBufferPrintString(0,"detect a supported",6,eLcdPage3);
    lcdBufferPrintString(0," radio",36,eLcdPage4);
    lcdSendBuffer(0);
    // Make the message visible for max 2 seconds
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    // clear potential button pushes
    bspKeyPushed(BSP_KEY_ALL);
    
    return 1;
  }
  
  // Reseting the variable GUI menu fields 
  easyLinkResetMenuVariables();
  
  return 0;
}
/******************************************************************************
 * @fn          easyLinkMaster
 *
 * @brief       function called from the easyLinkMainMenu.
 *              Configures radio and sends a packets  
 *              until aborted by user.
 *              
 *                
 * @param       pDummy - pointer to pointer to void. no value used
 *
 * @return     0 - SUCCESS
 */
uint8 easyLinkMaster(void** pDummy)
{
  easyLinkRunTX();
  
  return 1;
}
/******************************************************************************
 * @fn          easyLinkSlave
 *
 * @brief       function called from the easyLinkMainMenu.
 *              
 *              
 *                
 * @param       pDummy - pointer to pointer to void. no value used
 *
 * @return     0 - SUCCESS
 */
uint8 easyLinkSlave(void** pDummy)
{
  easyLinkRunRX();
  
  return 1;
}
