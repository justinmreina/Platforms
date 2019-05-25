//*****************************************************************************
//! @file   main.c
//
//! @brief  This file implements the startup of the board and and a menu
//             menu driver.
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

#include  <msp430.h>
#include "trx_rf_spi.h"
#include "chip_detect.h"
#include "freq_xosc_detect.h"
#include "per_test.h"
#include "main_graphics.c"
#include "menu_system.h"
#include "menu_driver.h"
#include "sniff_mode.h"
#include "chip_information.h"
#include "easyLink.h"
   
#include "lcd_dogm128_6.h"
#include "bsp.h"
#include "bsp_key.h"
#include "bsp_led.h"
/******************************************************************************
* GLOBAL VARIABLES
*/
extern uint8  mclkFrequency;

char version[] = "v4.3";    // Version number (versions from 4.0 includes CC120x support)

/******************************************************************************
* LOCAL FUNCTIONS
*/
static uint8 contrastApp(void **pVoid);

/******************************************************************************
* The Main Menu
*/

menu_t mainMenu;
menu_t contrastMenu;

menuItem_t mainMenuItems[] =
{
  {0x00,"1","PER Test"                   ,0,&perAbstractHeadMenu    ,0,&perChipSelectApp    ,0},
  {0x00,"2","EasyLink Test"              ,0,&easyLinkMainMenu       ,0,&initEasyLink      ,0},
  {0x00,"3","RX Sniff Test"              ,0,&sniffModeFrequencyMenu ,0,&sniffInitApp        ,0},
  {0x00,"4","Chip Information"           ,0,&chipInfoMenu           ,0,&chipInfoApp          ,0},
  {0x00,"5","Set Contrast"               ,0,&contrastMenu,0,0,0},
  {M_DISABLED,0,0,0,0,0,0,0},
  {M_DISABLED|M_STRING|M_RIGHT,0,0,version,0,0,0,0}
};

static const menuItem_t contrastMenuItems[] =
{
  {0x00,"1","High",0,0,0   ,&contrastApp,(void**)20},
  {0x00,"2","Medium",0,0,0,&contrastApp,(void**)25},
  {0x00,"3","Low",0,0,0  ,&contrastApp,(void**)30}
};

menu_t contrastMenu =
{
  (menuItem_t*)contrastMenuItems,   /* pItems          */
  &mainMenu,                        /* pParentMenu     */
  0,                                /* pMenuGraphics   */
  "Select Contrast",                /* pTextHeader     */
  "3",                              /* pTextMenuItems  */
  3,                                /* nMenuItems      */
  0,                                /* nCurrentItem    */
  0,                                /* nSelectedItem   */
  0,                                /* nScreen         */
  0                                 /* reservedAreas   */
};

menu_t mainMenu =
{
  (menuItem_t*)mainMenuItems,   /* pItems          */
  0,                            /* pParentMenu     */
  0,                            /* pMenuGraphics   */
  "Main Menu",                  /* pTextHeader     */
  "5",                          /* pTextMenuItems  */
  7,                            /* nMenuItems      */
  0,                            /* nCurrentItem    */
  -1,                           /* nSelectedItem   */
  0,                            /* nScreen         */
  0                             /* reservedAreas   */
};

/******************************************************************************
 * @fn          main
 *
 * @brief       Main handles all applications attached to the menu system
 *
 * input parameters
 *
 * output parameters
 *
 *@return
 */
void main( void )
{
   
  // Init clocks and I/O 
  bspInit(BSP_SYS_CLK_16MHZ);
  
  // Init leds 
  bspLedInit(); 

  // Init Buttons
  bspKeyInit(BSP_KEY_MODE_POLL);
  
  // Initialize SPI interface to LCD (shared with SPI flash)
  bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);  

  /* Init Buttons */
  bspKeyInit(BSP_KEY_MODE_ISR);
  bspKeyIntEnable(BSP_KEY_ALL);
  /* Init LCD and issue a welcome */
  lcdInit();
  lcdClear();
  // Instantiate tranceiver RF spi interface to SCLK ~ 4 MHz */
  //input clockDivider - SMCLK/clockDivider gives SCLK frequency
  trxRfSpiInterfaceInit(0x10);
  
  /* Welcome Screen Part */
  lcdSendBuffer(trxebWelcomeScreen);
  lcdBufferPrintString(lcdDefaultBuffer,"TEXAS",60,eLcdPage6);
  lcdBufferPrintString(lcdDefaultBuffer,"INSTRUMENTS",60,eLcdPage7);
  lcdSendBufferPart(lcdDefaultBuffer, 60,127,eLcdPage6, eLcdPage7);
  /* MCU will stay in sleep until button is pressed */
  __low_power_mode_3();
  bspKeyPushed(BSP_KEY_ALL);
  //Clear screen
  lcdBufferClear(0);

  /* Menu Driver */
  menu_t *pCurrentMenu = &mainMenu;
  uint8 menuButtonsPressed;
  menuDisplay(pCurrentMenu);
  __low_power_mode_3();
  while(1)
  {
    menuButtonsPressed = bspKeyPushed(BSP_KEY_ALL);
    switch(menuButtonsPressed)
    {
      case BSP_KEY_LEFT:
        pCurrentMenu = menuBack(pCurrentMenu);
        break;
      case BSP_KEY_RIGHT:
        pCurrentMenu = menuEnter(pCurrentMenu);
        break;
      case BSP_KEY_DOWN:
        menuDown(pCurrentMenu);
        break;
      case BSP_KEY_UP:
        menuUp(pCurrentMenu);
        break;
      default:
        break;
    }
    menuDisplay(pCurrentMenu);
    /* Enter low power mode while menu driver waits on user input */
    __low_power_mode_3();
  }
}


static uint8 contrastApp(void **pVoid)
{
  uint16 c = (uint16)*pVoid;
  lcdSetContrast(c&0x00FF);
  return 0;
}