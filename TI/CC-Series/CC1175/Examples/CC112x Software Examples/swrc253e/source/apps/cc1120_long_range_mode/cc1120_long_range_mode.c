//******************************************************************************
//! @file       cc1120_long_range_mode.c
//! @brief      This code example demonstrate the Long Range Mode 
//              described on the Sub-1GHz Wiki: 
//              http://processors.wiki.ti.com/index.php/Category:Sub-1GHz
//              Through a menu system the user can select frequency band and if 
//              the device should operate as slave or master              
//
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//      Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//      Neither the name of Texas Instruments Incorporated nor the names of
//      its contributors may be used to endorse or promote products derived
//      from this software without specific prior written permission.
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
//*****************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "lcd_dogm128_6.h"
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"
#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"
#include "cc1120_long_range_mode_reg_config.h"
#include "cc1120_long_range_mode.h"
#include "cc1120_lrm_tx.h"
#include "cc1120_lrm_rx.h"


/*******************************************************************************
*  VARIABLES
*/
static uint8 menuIndex;
//uint8 updateMenu = TRUE;
MENU_DATA menuData;
MENU_ITEM pMenuTable[] = {
  // *pMenuName       type,   *pValue,            min, max, text
  {" Freq Band     ", TEXT,   &menuData.freqBand, 0,   1,   &ppFreqBand[0][0],},
  {" Select Mode   ", TEXT,   &menuData.mode,     0,   1,   &ppMode[0][0],    },
  {" Start Test    ", ACTION, NULL,               0,   0,   NULL,             }
};


/*******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void handleMenu(uint8 action);
static void displayMenu(void);


/*******************************************************************************
*   @fn         main
*
*   @brief      Runs the main routine
*
*   @param      none
*
*   @return     none
*/
void main(void) {
  uint8 key;
  
  initMCU();

  displayMenu();
  
  // Loop forever, handling user inputs (key presses) 
  while (TRUE) {
    do {
      key = bspKeyPushed(BSP_KEY_ALL);
    } while (!key);
      
    handleMenu(key);
    displayMenu();   
  }
}


/*******************************************************************************
*   @fn         handleMenu
*
*   @brief      Updates variable in pMenuTable accoring to key input
*
*   @param      action
*                 Last key pushed
*
*   @return     none
*/
void handleMenu(uint8 action) {
  MENU_ITEM current;

  // Fetch a copy of the current menu item
  current = pMenuTable[menuIndex];

  switch (action) {
    
    // Move up if possible
    case BSP_KEY_UP:
      if (menuIndex > 0) {
        menuIndex--;
      }
      break;

    // Move down if possible
    case BSP_KEY_DOWN:
      if (menuIndex < (MENU_ITEM_COUNT - 1)) {
        menuIndex++;
      }
      break;

    // Decrease the item value if possible
    case BSP_KEY_LEFT:
      if (current.type != ACTION) {
        if (current.minValue < *current.pValue) {
          (*current.pValue)--;  
        }
      }
      break;

    // Increase the item value if possible, or perform action
    case BSP_KEY_RIGHT:
      if (current.type == ACTION) {
        if (*pMenuTable[SELECT_MODE].pValue == RX_MODE) {
          runRX();
        } else {
          runTX();
        }
      } else { // TEXT
        if (*current.pValue < current.maxValue) {
          (*current.pValue)++;
        }
      }
      break;
  }
}


/*******************************************************************************
*   @fn         displayMenu
*
*   @brief      Display the menu and the selected parameter on the LCD
*
*   @param      none
*
*   @return     none
*/
void displayMenu(void) {
  uint8 i;
  MENU_ITEM  current;

  // Fetch a copy of the current menu item
  current = pMenuTable[menuIndex];

  // Update the parameter value
  if (current.type == TEXT) {
    
    const char *pcStr = &current.pText[(*current.pValue)*(VALUE_TEXT_SIZE + 1)];
       
    // Write text string to LCD
    lcdBufferClear(0);
    lcdBufferPrintString(0, current.pMenuName, 0,            eLcdPage0);
    lcdBufferSetHLine(   0, 0,                 LCD_COLS - 1, 7        );
    lcdBufferPrintString(0, pcStr,             0,            eLcdPage2);
      
    // Diplay arrows indicating how to navigate in the menu
    switch (menuIndex) {
      case 0: // Dovn
        lcdBufferSetVLine(0, 95, 32, 42);
        lcdBufferSetHLine(0, 94, 96, 41);
        lcdBufferSetHLine(0, 93, 97, 40);
        break;
    
      case MENU_ITEM_COUNT: // UP
        lcdBufferSetVLine(0, 95, 12, 22);
        lcdBufferSetHLine(0, 94, 96, 13);
        lcdBufferSetHLine(0, 93, 97, 14);
        break;
    
      default: // UP and DOWN
        lcdBufferSetVLine(0, 95, 12, 22);
        lcdBufferSetHLine(0, 94, 96, 13);
        lcdBufferSetHLine(0, 93, 97, 14);
        
        lcdBufferSetVLine(0, 95, 32, 42);
        lcdBufferSetHLine(0, 94, 96, 41);
        lcdBufferSetHLine(0, 93, 97, 40);
        break;
    }
      
    if (*current.pValue < current.maxValue) { // RIGHT
      lcdBufferHArrow(0, 100, 110, 27); 
    } else if (*current.pValue > current.minValue) { // LEFT
      lcdBufferHArrow(0, 90,  80,  27);
    } else { // RIGHT and LEFT
      lcdBufferHArrow(0, 100, 110, 27);
      lcdBufferHArrow(0, 90,  80,  27);
    }
    lcdSendBuffer(0);
  } else { // ACTION
    lcdBufferClear(0);
    lcdBufferPrintString(0, current.pMenuName,   0,          eLcdPage0);
    lcdBufferSetHLine(   0, 0,                   LCD_COLS-1, 7        );
    lcdBufferPrintString(0, "Start by pressing", 0,          eLcdPage3);
    lcdBufferPrintString(0, "right key",         0,          eLcdPage4);
    lcdSendBuffer(0);
  }
}


/*******************************************************************************
*   @fn         initMCU
*
*   @brief      Initialize MCU and board peripherals
*
*   @param      none
*
*   @return     none
*/
static void initMCU(void) {
  
  // Init clocks and I/O
  bspInit(BSP_SYS_CLK_8MHZ);

  // Init LEDs
  bspLedInit();

  // Init buttons
  bspKeyInit(BSP_KEY_MODE_POLL);

  // Initialize SPI interface to LCD (shared with SPI flash)
  bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);

  // Init LCD
  lcdInit();

  // Instantiate transceiver RF SPI interface to SCLK ~ 4 MHz
  // Input parameter is clockDivider
  // SCLK frequency = SMCLK/clockDivider
  trxRfSpiInterfaceInit(2);

  // Enable global interrupt
  _BIS_SR(GIE);
}
