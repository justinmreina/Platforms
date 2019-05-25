//*****************************************************************************
//! @file        cc1101_easy_link_tx.c
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
void cc1101RunTX(void);
static void createPacket(uint8 txBuffer[]);
static void radioRxTxISR(void);
static void updateLcd(void);

/******************************************************************************
 * @fn          cc1101RunTX
 *
 * @brief       sends one packet on button push. Updates packet counter and
 *              display for each packet sent.
 *                
 * @param       none
 *
 * @return      none
 */
void cc1101RunTX(void)
{
    // Reset button pushed variable
  uint8  buttonPushed = 0; 
  // Initialize packet buffer of size PKTLEN + 1
  uint8 txBuffer[PKTLEN+1] = {0};

  // connect ISR function to GPIO0, interrupt on falling edge
  trxIsrConnect(&radioRxTxISR);
  
  // enable interrupt from GPIO_0
  trxEnableInt();
    
  // update LCD
  updateLcd();
  
  
  // Loop until left button is pushed
  while(buttonPushed != BSP_KEY_LEFT ){
    
    // wait for button push
    if(bspKeyPushed(BSP_KEY_ALL))
    { 
      //continiously sent packets until button is pressed
      do
      {
        // update packet counter
        packetCounter++;
        
        // create a random packet with PKTLEN + 2 byte packet counter + n x random bytes
        createPacket(txBuffer);
      
      // write packet to tx fifo
      cc1101SpiWriteTxFifo(txBuffer,sizeof(txBuffer));
      
      // strobe TX to send packet
      trxSpiCmdStrobe(CC1101_STX);
      
        // wait for interrupt that packet has been sent. 
        // (Assumes the GPIO connected to the radioRxTxISR function is set 
        // to GPIOx_CFG = 0x06)
        while(!packetSemaphore);
        
        // clear semaphore flag
        packetSemaphore = ISR_IDLE;
        
        // update LCD
        updateLcd();
         
        // Check for button pushed
        buttonPushed = bspKeyPushed(BSP_KEY_ALL);
        
      }while(!buttonPushed);
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
#ifdef PA_TABLE
  uint8 paTable[] = PA_TABLE;
#endif
  
  // reset radio
  trxSpiCmdStrobe(CC1101_SRES);
  // write registers to radio
  for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
    writeByte =  preferredSettings[i].data;
    cc1101SpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
  }
#ifdef PA_TABLE
  // write PA_TABLE
  cc1101SpiWriteReg(CC1101_PA_TABLE0,paTable, sizeof(paTable));
#endif
}
/******************************************************************************
 * @fn          createPacket
 *
 * @brief       This function is called before a packet is transmitted. It fills
 *              the txBuffer with a packet consisting of a length byte, two
 *              bytes packet counter and n random bytes.
 *
 *              The packet format is as follows:
 *              |--------------------------------------------------------------|
 *              |           |           |           |         |       |        |
 *              | pktLength | pktCount1 | pktCount0 | rndData |.......| rndData|
 *              |           |           |           |         |       |        |
 *              |--------------------------------------------------------------|
 *               txBuffer[0] txBuffer[1] txBuffer[2]  ......... txBuffer[PKTLEN]
 *                
 * @param       pointer to start of txBuffer
 *
 * @return      none
 */
static void createPacket(uint8 txBuffer[])
{
  
  txBuffer[0] = PKTLEN;                     // Length byte
  txBuffer[1] = (uint8) (packetCounter >> 8); // MSB of packetCounter
  txBuffer[2] = (uint8) packetCounter;      // LSB of packetCounter
  
  // fill rest of buffer with random bytes
  for(uint8 i =3; i< (PKTLEN+1); i++)
  {
    txBuffer[i] = (uint8)rand();
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
      lcdBufferPrintString(0, "Sent packets:", 0, eLcdPage3);
      lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
      lcdBufferPrintString(0, "      Packet TX        " , 0, eLcdPage7);
      lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
      lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
      lcdSendBuffer(0);

}