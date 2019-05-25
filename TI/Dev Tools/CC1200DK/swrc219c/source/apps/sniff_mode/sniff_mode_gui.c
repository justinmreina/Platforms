//*****************************************************************************
//! @file    sniff_mode_gui.c
//
//! @brief  Implementation of the menu system used in the sniff mode test.
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

#include "hal_types.h"
#include "menu_system.h"
#include "menu_driver.h"
#include "sniff_mode.h"
#include "chip_detect.h"
#include "lcd_dogm128_6.h"
   
/******************************************************************************
 * Local Functions
 */
/******************************************************************************
 * LOCAL VARIABLES AND CONSTANTS - needed for GUI information
 */

/******************************************************************************
 * MENUS
 */

menu_t sniffModeMainMenu;
menu_t sniffModeFrequencyMenu;


static const menuItem_t sniffModeMainMenuItems[] =
{
  {M_DISABLED,0,"    Select Device",0,0,0,0,0},
  {0x00,"1","Master",0,0,0,&sniffMasterStartApp,0},
  {0x00,"2","Slave ",0,0,0,&sniffSlaveStartApp ,0},
};

menu_t sniffModeMainMenu = 
{
  (menuItem_t*)sniffModeMainMenuItems,  /* pItems          */
  0,                                    /* pParentMenu     */
  0,                                    /* pMenuGraphics   */
  "RX Sniff Test",                      /* pTextHeader     */
  "2",                                  /* pTextMenuItems  */
  3,                                    /* nMenuItems      */
  1,                                    /* nCurrentItem    */
  -1,                                   /* nSelectedItem   */
  0,                                    /* nScreen         */
  0xE0                                  /* reservedAreas   */
};

static const menuItem_t sniffModeFrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","169 MHz",0,&sniffModeMainMenu,0,&sniffModeFreqConfig,(void **)0},
  {0x00,"2","434 MHz",0,&sniffModeMainMenu,0,&sniffModeFreqConfig,(void **)1}, 
  {0x00,"3","868 MHz",0,&sniffModeMainMenu,0,&sniffModeFreqConfig,(void **)2},
  {0x00,"4","915 MHz",0,&sniffModeMainMenu,0,&sniffModeFreqConfig,(void **)3},
  {0x00,"5","955 MHz",0,&sniffModeMainMenu,0,&sniffModeFreqConfig,(void **)4}  
};
menu_t sniffModeFrequencyMenu =
{
  (menuItem_t*)sniffModeFrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "5",                                          /* pTextMenuItems */
  6,                                            /* nMenuItems     */
  3,                                            /* nCurrentItem   */
  3,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};

/******************************************************************************
 * Functions
 */
void drawInfo(void)
{
    menuClearReservedArea(&sniffModeMainMenu);
    lcdBufferSetHLine(0,0,LCD_COLS-1,LCD_ROWS-17);
    lcdBufferPrintString(0,"Carrier Sense Thrshld",1,eLcdPage6);
    lcdBufferPrintString(0,"set high for DEMO use",1,eLcdPage7);
    lcdBufferInvertPage(0,0,LCD_COLS-1,eLcdPage6);
    lcdBufferInvertPage(0,0,LCD_COLS-1,eLcdPage7);
    lcdSendBuffer(0);
}