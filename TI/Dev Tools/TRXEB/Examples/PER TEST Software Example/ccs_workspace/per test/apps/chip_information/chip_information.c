//*****************************************************************************
//! @file        chip_information.c
//  
//! @brief     This file holds the chipInfoApp function which gather and 
//                displays information about the EM connected to the TrxEB.
//                The following information is displayed:
//                - Device name
//                - PG version
//                - Crystal frequency
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
#include "lcd_dogm128_6.h"
#include "trx_rf_spi.h"
#include "chip_detect.h"
#include "freq_xosc_detect.h"
#include "hal_timer_32k.h"
#include "menu_system.h"
#include "menu_driver.h"
#include "chip_information.h"
#include "bsp.h"
#include "bsp_key.h"
/******************************************************************************
* LOCAL VARIABLES
*/
static radioChipType_t pRadioChipType;

uint16 id;
uint8  ver;
uint8  xoscFreq;

/******************************************************************************
* STATIC FUNCTIONS
*/
menu_t chipInfoMenu;

// menu item for chipInfoMenu
static const menuItem_t chipInfoMenuItems[] =
{
  {0x00,"<-"," Return to menu",0,0,0,0,0}
};

// menu to display chip information
menu_t chipInfoMenu =
{
  (menuItem_t*)chipInfoMenuItems,    // pItems          
  0,                                 // pParentMenu     
  0,                                 // pMenuGraphics   
  "Chip Information",                // pTextHeader     
  0,                                 // pTextMenuItems  
  1,                                 // nMenuItems      
  0,                                 // nCurrentItem    
  -1,                                // nSelectedItem   
  0,                                 // nScreen         
  0x7E                               // reservedAreas   
};

/******************************************************************************
 * @fn          chipInfoApp
 *
 * @brief       Detects chip partnummer, version number and crystal frequency
 *              and displays infomation on LCD.
 *                
 * @input       *pVoid - pointer to void. Not used 
 *
 * @return      0 - success
 ******************************************************************************/
uint8 chipInfoApp(void **pVoid)
{
  // Detect if a supported radio is present
  trxDetectChipType(&pRadioChipType);
  
  // Detect frequency of crystal on connected EM
  trxDetectRfCrystalFrequency(&pRadioChipType);
 
  // Get chip inf from struct
  id = pRadioChipType.deviceName;
  ver = pRadioChipType.ver;
  xoscFreq = pRadioChipType.xoscFreq;
  
  // Clear reserved area of menu to display chip info
  menuClearReservedArea(&chipInfoMenu);
  
  if(!(id | ver))
  {
    // Issue message to user to insert radio EM
    lcdBufferClear(0);
    lcdBufferPrintString(0,"Test could not",18,eLcdPage2);
    lcdBufferPrintString(0,"detect a supported",6,eLcdPage3);
    lcdBufferPrintString(0,"radio",36,eLcdPage4);
    lcdSendBuffer(0);
    // Make the message visible for max 2 seconds
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    // clear potential button pushes
    bspKeyPushed(BSP_KEY_ALL);
    
    return 1;
  }
  else
  {
    lcdBufferPrintString(0,"Device:",1,eLcdPage2);
    lcdBufferPrintString(0,"CC",86,eLcdPage2);
    if(id&0xF000)
    {
      lcdBufferPrintInt(0,((id&0xF000)>>12),98,eLcdPage2);
      lcdBufferPrintInt(0,((id&0x0F00)>>8),104,eLcdPage2);
      lcdBufferPrintInt(0,((id&0x00F0)>>4),110,eLcdPage2);
      lcdBufferPrintInt(0,((id&0x000F)),116,eLcdPage2);
    }
    else
    {
      lcdBufferPrintInt(0,((id&0x0F00)>>8),98,eLcdPage2);
      lcdBufferPrintInt(0,((id&0x00F0)>>4),104,eLcdPage2);
      lcdBufferPrintInt(0,((id&0x000F)),110,eLcdPage2);
      lcdBufferPrintString(0,"L",116,eLcdPage2); 
    }
    lcdBufferPrintString(0,"Version:",1,eLcdPage3);
    // Set version if CC112x or CC120x detected
    if((id == CHIP_TYPE_CC1120) ||(id == CHIP_TYPE_CC1121) 
       ||(id == CHIP_TYPE_CC1125) ||(id ==  CHIP_TYPE_CC1175)
         ||(id == CHIP_TYPE_CC1200) ||(id ==  CHIP_TYPE_CC1201)
           ||(id ==  CHIP_TYPE_CC2500) )
    {
      lcdBufferPrintInt(0,((ver&0xF0)>>4),105,eLcdPage3);
      lcdBufferPrintString(0,".",111,eLcdPage3);
      lcdBufferPrintInt(0,((ver&0x0F))   ,116,eLcdPage3);
    }
    else
    {
      lcdBufferPrintString(0,"n/a",104,eLcdPage3);
    }
   
    lcdBufferPrintString(0,"XOSC freq:",1,eLcdPage4);
    lcdBufferPrintInt(0,xoscFreq,91,eLcdPage4);
    lcdBufferPrintString(0,"MHz",104,eLcdPage4);  
 
    return 0;
  }
}
