//*****************************************************************************
//! @file         cc112x_easy_link_tx.c
//  
//! @brief        This program sets up an easy link between two trxEB's with
//                CC112x EM's connected. 
//                The program can take any recomended register settings exported
//                from SmartRF Studio 7 without any modification with exeption 
//                from the assumtions decribed below.
//
//                The following asumptions must be fulfilled for the program
//                to work:
//                
//                1. GPIO2 has to be set up with GPIOx_CFG = 0x06
//                   PKT_SYNC_RXTX for correct interupt
//                2. Packet engine has to be set up with status bytes enabled 
//                   PKT_CFG1.APPEND_STATUS = 1
//
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
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "easyLink.h"
#include "cc112x_spi.h"
#include "stdlib.h"
#include "cc112x_easy_link_reg_config.h"
#include "lcd_dogm128_6.h"
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

#define PKTLEN              30 // Packet length has to be within fifo limits ( 1 - 127 bytes)
/******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter = 0;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
void cc112xRunTX(void);
static void createPacket(uint8 randBuffer[]);
static void radioRxTxISR(void);
static void updateLcd(void);

/******************************************************************************
 * @fn          runTX
 *
 * @brief       Continuously sends packets on button push until button is pushed
 *              again. After the radio has gone into TX the function waits for 
 *              interrupt that packet has been sent. Updates packet counter and
 *              display for each packet sent.
 *                
 * @param       none
 *
 * @return      none
 */
void cc112xRunTX(void){
    // Reset button pushed variable
  uint8  buttonPushed = 0; 
  // Initialize packet buffer of size PKTLEN + 1
  uint8 txBuffer[PKTLEN+1] = {0};

    // Write radio registers
  registerConfig();
  
  // Connect ISR function to GPIO0, interrupt on falling edge
  trxIsrConnect(&radioRxTxISR);
  
  // Enable interrupt from GPIO_0
  trxEnableInt();
    
  // Update LCD
  updateLcd();
  
  // Calibrate radio according to errata
  manualCalibration();

  // Loop until left button is pushed
  while(buttonPushed != BSP_KEY_LEFT ){
    
    // Wait for button push
    if(bspKeyPushed(BSP_KEY_ALL)){
      
      //Continiously sent packets until button is pressed
      do{
        
        // Update packet counter
        packetCounter++;
        
        // Create a random packet with PKTLEN + 2 byte packet counter + n x random bytes
        createPacket(txBuffer);
        
        // Write packet to tx fifo
        cc112xSpiWriteTxFifo(txBuffer,sizeof(txBuffer));
        
        // Strobe TX to send packet
        trxSpiCmdStrobe(CC112X_STX);
        
        // Wait for interrupt that packet has been sent. 
        // (Assumes the GPIO connected to the radioRxTxISR function is set 
        // to GPIOx_CFG = 0x06)
        while(!packetSemaphore) {
          uint8 readByte;
          //Read datarate from registers
          cc112xSpiReadReg(CC112X_MARCSTATE, &readByte, 1);
          if(readByte);
        };
        
        // Clear semaphore flag
        packetSemaphore = ISR_IDLE;
        
        // Update LCD
        updateLcd();
        
        // Check for button pushed
        buttonPushed = bspKeyPushed(BSP_KEY_ALL);
        
      }while(!buttonPushed);
    }
  }
  // Reset packet counter
  packetCounter = 0;
  // Put radio to sleep and exit application
  trxSpiCmdStrobe(CC112X_SPWD);  
}
/*******************************************************************************
* @fn          radioRxTxISR
*
* @brief       ISR for packet handling in TX. Sets packet semaphore 
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
*              cc112x_easy_link_reg_config.h
*
* @param       none
*
* @return      none
*/
static void registerConfig(void){
  
  uint8 writeByte;
  
  // Reset radio
  trxSpiCmdStrobe(CC112X_SRES);
  
  // Write registers to radio
  if(pRadioChipType.deviceName == CHIP_TYPE_CC1125){
    for(uint16 i = 0; i < (sizeof  cc1125PreferredSettings/sizeof(registerSetting_t)); i++) {
      writeByte =  cc1125PreferredSettings[i].data;
      cc112xSpiWriteReg( cc1125PreferredSettings[i].addr, &writeByte, 1);
    }
  }
  else{
    for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
      writeByte =  preferredSettings[i].data;
      cc112xSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
    }
  }
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
static void createPacket(uint8 txBuffer[]){
  
  txBuffer[0] = PKTLEN;                     // Length byte
  txBuffer[1] = (uint8) (packetCounter >> 8); // MSB of packetCounter
  txBuffer[2] = (uint8) packetCounter;      // LSB of packetCounter
  
  // Fill rest of buffer with random bytes
  for(uint8 i =3; i< (PKTLEN+1); i++)
  {
    txBuffer[i] = (uint8)rand();
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
      lcdBufferPrintString(0, "Sent packets:", 0, eLcdPage3);
      lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
      lcdBufferPrintString(0, "      Packet TX        " , 0, eLcdPage7);
      lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
      lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
      lcdSendBuffer(0);
}
/******************************************************************************
 * @fn          manualCalibration
 *
 * @brief       calibrates radio according to CC112x errata
 *                
 * @param       none
 *
 * @return      none
 */
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
static void manualCalibration(void) {
  
    uint8 original_fs_cal2;
    uint8 calResults_for_vcdac_start_high[3];
    uint8 calResults_for_vcdac_start_mid[3];
    uint8 marcstate;
    uint8 writeByte;
    
    // 1) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    
    // 2) Start with high VCDAC (original VCDAC_START + 2):
    cc112xSpiReadReg(CC112X_FS_CAL2, &original_fs_cal2, 1);
    writeByte = original_fs_cal2 + VCDAC_START_OFFSET;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);
    
    // 3) Calibrate and wait for calibration to be done (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);
    
    do 
    {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);
    
    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with high VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);
    
    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    
    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);
    
    // 7) Calibrate and wait for calibration to be done (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);
    
    do 
    {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);
    
    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with mid VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);
    
    // 9) Write back highest FS_VCO2 and corresponding FS_VCO and FS_CHP result
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) 
    {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
    else 
    {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
}