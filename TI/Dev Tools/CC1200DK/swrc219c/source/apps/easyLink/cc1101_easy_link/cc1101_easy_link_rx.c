//*****************************************************************************
//! @file        cc1101_easy_link.c
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

/*****************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "lcd_dogm128_6.h"
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "easyLink.h"
#include "cc1101_spi.h"
#include "cc1101_easy_link_reg_config.h"
#include "stdlib.h"
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

#define PKTLEN              30
/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void registerConfig(void);
void cc1101RunRX(void);
static void radioRxTxISR(void);
static void updateLcd(void);

/******************************************************************************
 * @fn          cc1101RunRX
 *
 * @brief       puts radio in RX and waits for packets. Update packet counter
 *              and display for each packet received.
 *                
 * @param       none
 *
 * @return      none
 */
void cc1101RunRX(void)
{
  uint8 rxBuffer[64] = {0};
  uint8 rxBytes;
  uint8 rxBytesVerify;
  
    // Write radio registers
  registerConfig();
  
  // Connect ISR function to GPIO2, interrupt on falling edge
  trxIsrConnect(&radioRxTxISR);
  
  // enable interrupt from GPIO_0
  trxEnableInt();
     
  // update LCD
  updateLcd();
    
  // set radio in RX
  trxSpiCmdStrobe(CC1101_SRX);

  // reset packet counter
  packetCounter = 0;
  
  // Loop until left button is pushed (exits application)
  while(BSP_KEY_LEFT != bspKeyPushed(BSP_KEY_ALL)){
    
    // wait for packet received interrupt 
    if(packetSemaphore == ISR_ACTION_REQUIRED)
    {
        cc1101SpiReadReg(CC1101_RXBYTES,&rxBytesVerify,1);
        
        do
        {
          rxBytes = rxBytesVerify;
          cc1101SpiReadReg(CC1101_RXBYTES,&rxBytesVerify,1);
        }
        while(rxBytes != rxBytesVerify);
        
        cc1101SpiReadRxFifo(rxBuffer,(rxBytes));
        
        // check CRC ok (CRC_OK: bit7 in second status byte)
        if(rxBuffer[rxBytes-1] & 0x80)
        {
          // update packet counter
          packetCounter++;
        }
      
      // update LCD
      updateLcd();
      // reset packet semaphore
      packetSemaphore = ISR_IDLE;
      
      // set radio back in RX
      trxSpiCmdStrobe(CC1101_SRX);
      
    }
  }
    // Reset packet counter
  packetCounter = 0;
  // Put radio to sleep and exit application
  trxSpiCmdStrobe(CC1101_SPWD);
}
/*******************************************************************************
* @fn          radioRxTxISR
*
* @brief       ISR for packet handling in RX. Sets packet semaphore, puts radio
*              in idle state and clears isr flag.
*
* @param       none
*
* @return      none
*/
static void radioRxTxISR(void) {

  // set packet semaphore
  packetSemaphore = ISR_ACTION_REQUIRED;
  // clear isr flag
  trxClearIntFlag();
}
/*******************************************************************************
* @fn          registerConfig
*
* @brief       Write register settings as given by SmartRF Studio
*
* @param       none
*
* @return      none
*/
static void registerConfig(void) {
  uint8 writeByte;
  
  // reset radio
  trxSpiCmdStrobe(CC1101_SRES);
  // write registers to radio
  for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
    writeByte =  preferredSettings[i].data;
    cc1101SpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
}
/******************************************************************************
 * @fn          updateLcd
 *
 * @brief       updates LCD buffer and sends bufer to LCD.
 *                
 * @param       none
 *
 * @return      none
 */
static void updateLcd(void)
{       
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