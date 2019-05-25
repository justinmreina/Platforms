//*****************************************************************************
//! @file        cc120x_easy_link_rx.c
//
//! @brief     This program sets up an easy link between two trxEB's with
//                CC120x EM's connected. 
//                The program can take any recomended register settings exported
//                from SmartRF Studio 7 without any modification with exeption 
//                from the assumtions decribed below.
//  
//                The following asumptions must be fulfilled for the program
//                to work:
//                
//                1. GPIO2 has to be set up with GPIO2_CFG = 0x06
//                   PKT_SYNC_RXTX for correct interupt
//                2. Packet engine has to be set up with status bytes enabled 
//                   PKT_CFG1.APPEND_STATUS = 1
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


/*****************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "lcd_dogm128_6.h" 
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "easyLink.h"
#include "cc120x_spi.h"
#include "cc120x_easy_link_reg_config.h"
#include "bsp.h"
#include "bsp_key.h"
/******************************************************************************
 * CONSTANTS
 */ 

/******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0

#define RX_FIFO_ERROR       0x11

/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter = 0;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void registerConfig(void);
void cc120xRunRX(void);
static void radioRxTxISR(void);
static void updateLcd(void);

/******************************************************************************
 * @fn          runRX
 *
 * @brief       puts radio in RX and waits for packets. Function assumes
 *              that status bytes are appended in the RX_FIFO
 *              Update packet counter and display for each packet received.
 *                
 * @param       none
 *
 * @return      none
 */
void cc120xRunRX(void){
  
  uint8 rxBuffer[128] = {0};
  uint8 rxBytes;
  uint8 marcStatus;
  
    // Write radio registers
  registerConfig();
  
  // Connect ISR function to GPIO0, interrupt on falling edge
  trxIsrConnect(&radioRxTxISR);
  
  // Enable interrupt from GPIO_0
  trxEnableInt();
   
  // Update LCD
  updateLcd();

  // Set radio in RX
  trxSpiCmdStrobe(CC120X_SRX);

  // Loop until left button is pushed (exits application)
  while(BSP_KEY_LEFT != bspKeyPushed(BSP_KEY_ALL)){
    
    // Wait for packet received interrupt 
    if(packetSemaphore == ISR_ACTION_REQUIRED){
      
      // Read number of bytes in rx fifo
      cc120xSpiReadReg(CC120X_NUM_RXBYTES, &rxBytes, 1);
      
      // Check that we have bytes in fifo
      if(rxBytes != 0){
        
        // Read marcstate to check for RX FIFO error
        cc120xSpiReadReg(CC120X_MARCSTATE, &marcStatus, 1);
        
        // Mask out marcstate bits and check if we have a RX FIFO error
        if((marcStatus & 0x1F) == RX_FIFO_ERROR){
          
          // Flush RX Fifo
          trxSpiCmdStrobe(CC120X_SFRX);
        }
        else{
        
          // Read n bytes from rx fifo
          cc120xSpiReadRxFifo(rxBuffer, rxBytes);  
          
          // Check CRC ok (CRC_OK: bit7 in second status byte)
          // This assumes status bytes are appended in RX_FIFO
          // (PKT_CFG1.APPEND_STATUS = 1.)
          // If CRC is disabled the CRC_OK field will read 1
          if(rxBuffer[rxBytes-1] & 0x80){
            
            // Update packet counter
            packetCounter++;
          }
        }
      }
      
      // Update LCD
      updateLcd();
      
      // Reset packet semaphore
      packetSemaphore = ISR_IDLE;
      
      // Set radio back in RX
      trxSpiCmdStrobe(CC120X_SRX);
      
    }
  }
  // Reset packet counter
  packetCounter = 0;
  // Put radio to sleep and exit application
  trxSpiCmdStrobe(CC120X_SPWD);
}
/*******************************************************************************
* @fn          radioRxTxISR
*
* @brief       ISR for packet handling in RX. Sets packet semaphore 
*              and clears isr flag.
*
* @param       none
*
* @return      none
*/
static void radioRxTxISR(void){

  // Set packet semaphore
  packetSemaphore = ISR_ACTION_REQUIRED;  
  
  // Clear isr flag
  trxClearIntFlag();
}

/*******************************************************************************
* @fn          registerConfig
*
* @brief       Write register settings as given by SmartRF Studio found in
*              cc120x_easy_link_reg_config.h
*
* @param       none
*
* @return      none
*/
static void registerConfig(void){
  
  uint8 writeByte;
  
  // Reset radio
  trxSpiCmdStrobe(CC120X_SRES);
  
  // Write registers to radio
  for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
    writeByte =  preferredSettings[i].data;
    cc120xSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
}
/******************************************************************************
 * @fn          updateLcd
 *
 * @brief       updates LCD buffer and sends bufer to LCD module.
 *                
 * @param       none
 *
 * @return      none
 */
static void updateLcd(void){
  
      // Update LDC buffer and send to screen.
      lcdBufferClear(0);
      lcdBufferPrintString(0, "    EasyLink Test    ", 0, eLcdPage0);
      lcdBufferSetHLine(0, 0, LCD_COLS-1, eLcdPage7); 
      lcdBufferPrintString(0, "Received ok:", 0, eLcdPage3);
      lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
      lcdBufferPrintString(0, "      Packet RX        ", 0, eLcdPage7);
      lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
      lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
      lcdSendBuffer(0);
}