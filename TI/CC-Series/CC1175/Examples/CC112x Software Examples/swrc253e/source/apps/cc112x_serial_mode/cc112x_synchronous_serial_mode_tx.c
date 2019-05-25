//*****************************************************************************
//! @file        cc112x_synchronous_serial_mode_tx.c
//! @brief      
//!
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
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"
#include "bsp.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "stdlib.h"
#include "cc112x_serial_mode_reg_config.h"
#include "math.h"

/******************************************************************************
* CONSTANTS
*/ 

/******************************************************************************
* DEFINES
*/
#define SERIAL_CLK          (P1IN & 0x04)
#define SERIAL_DATA         P1OUT

#define PKTLEN              126 // Length byte and seq. number included in payload
#define FIXED_LENGTH        FALSE // TRUE if fixed packet length, else FALSE
#define CRC_ENABLE          TRUE
#define BUFFER_SIZE         PKTLEN + 2 // 2 bytes crc

#define CRC16_POLY          0x8005
#define CRC_INIT            0xFFFF

/******************************************************************************
* LOCAL VARIABLES
*/
static uint32 packetCounter = 0;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
static void runSerialTX(void);
static void updateLcd(void);
static uint16 calcCRC(uint8 crcData, uint16 crcReg);
static void createPacket(uint8 txBuffer[]);


/******************************************************************************
* @fn          main
*
* @brief       Runs the main routine
*                
* @param       none
*
* @return      none
*/
void main(void){
  
  // Initialize MCU and peripherals
  initMCU();
  
  // Write radio registers
  registerConfig();
  
  // Calibrate radio according to errata
  manualCalibration();
   
  // Enter runRX, never coming back
  runSerialTX();
  
}
/******************************************************************************
* @fn          runSerialTX
*
* @brief       Continuously sends packets on button push until button is pushed
*              again. Updates packet counter and display for each packet sent.
* @param       none
*
* @return      none
*/

static void runSerialTX(void){
  
    // Set direction on P1.1
    P1DIR |= 0x80;
    P1DIR &= ~0x04;
    P1SEL &= ~0x84;
    P1DS |= 0x80;
    
    uint8 writeByte;
  
    //Set serial mode specific registers
    writeByte = 0x08;
    cc112xSpiWriteReg(CC112X_IOCFG3, &writeByte, 1);
    writeByte = 0x09;
    cc112xSpiWriteReg(CC112X_IOCFG2, &writeByte, 1);
    writeByte = 0x30;
    cc112xSpiWriteReg(CC112X_IOCFG0, &writeByte, 1);
    writeByte = 0x08;
    cc112xSpiWriteReg(CC112X_SYNC_CFG1, &writeByte, 1);
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_PREAMBLE_CFG1, &writeByte, 1);
    writeByte = 0x06;
    cc112xSpiWriteReg(CC112X_MDMCFG1, &writeByte, 1);
    writeByte = 0x0A;
    cc112xSpiWriteReg(CC112X_MDMCFG0, &writeByte, 1);
    writeByte = 0xA9;
    cc112xSpiWriteReg(CC112X_AGC_CFG1, &writeByte, 1);
    writeByte = 0x05;
    cc112xSpiWriteReg(CC112X_PKT_CFG2, &writeByte, 1);
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_PKT_CFG1, &writeByte, 1);
    writeByte = 0x08;
    cc112xSpiWriteReg(CC112X_SERIAL_STATUS, &writeByte, 1);
    
    uint8 i;
    uint16 checksum;
    uint8 dataByte = 0;
    uint8 firstByte;
    uint8 bufferIndex = 0;
    uint8 txBuffer[BUFFER_SIZE] = {0};
    
    updateLcd();
    
    while(1){
      if(bspKeyPushed(BSP_KEY_ALL)){
      //Continiously sent packets until button is pressed
      do{
        createPacket(txBuffer);
           if (CRC_ENABLE) {
            // Generate CRC
            checksum = CRC_INIT;   //Init value for CRC calculation
            for(i = 0; i < (BUFFER_SIZE - 2); i++) 
              checksum = calcCRC(txBuffer[i], checksum);
            txBuffer[BUFFER_SIZE - 2] = checksum >> 8;     // CRC1
            txBuffer[BUFFER_SIZE - 1] = checksum & 0x00FF; // CRC0
          }

          firstByte = txBuffer[0];
          // MSB of the first byte needs to ready on the data line before strobing TX
          if (((firstByte & 0x80) >> 7) == 0)
            SERIAL_DATA &= ~BIT7;
          else
            SERIAL_DATA |= BIT7;
          firstByte = firstByte << 1;

          // Enter TX mode
          trxSpiCmdStrobe(CC112X_STX);

          // Transmit the first byte
          for (i = 1; i < 8; i++) {
            while (!SERIAL_CLK);
              if (((firstByte & 0x80) >> 7) == 0)
                SERIAL_DATA &= ~BIT7;
              else
                SERIAL_DATA |= BIT7;
              firstByte = firstByte << 1;
              while (SERIAL_CLK);
          }
    
          // Tranmit the rest of the packet                               
          for (bufferIndex = 1; bufferIndex < BUFFER_SIZE; bufferIndex++) {
            dataByte = txBuffer[bufferIndex];
            for (i = 0; i < 8; i++) {
              while (!SERIAL_CLK);
              if (((dataByte & 0x80) >> 7) == 0)
                SERIAL_DATA &= ~BIT7;
              else
                SERIAL_DATA |= BIT7;
              dataByte = dataByte << 1;
              while (SERIAL_CLK);
            }
          }
          while (!SERIAL_CLK);
          while (SERIAL_CLK);
          
         //transmit 13 dummybits
          /*SERIAL_DATA &=~BIT7;
          for(i = 0; i<1; i++){
            while (!SERIAL_CLK);
            while(SERIAL_CLK);
          }*/

      //Enter IDLE mode
      trxSpiCmdStrobe(CC112X_SIDLE);
      packetCounter++;
      updateLcd();
    }while(!bspKeyPushed(BSP_KEY_ALL));
   }
 } 
}


/******************************************************************************
* @fn          calcCRC
*
* @brief       Calculates a checksum over the payload, included the seq. number
*              and length byte.
*
* @param       uint8 crcData: The data to perform the CRC-16 operation on
*              uint16 crcReg: The current value of the CRC register. For the 
*                             first byte the value CRC16_INIT should be 
*                             supplied. For each additional byte the value 
*                             returned for the last invocation should be 
*                             supplied
*
* @return      crcReg
*/
static uint16 calcCRC(uint8 crcData, uint16 crcReg)
{ uint8 i;
  for (i = 0; i < 8; i++) {
  if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
    crcReg = (crcReg << 1) ^ CRC16_POLY;
  else
    crcReg = (crcReg << 1);
    crcData <<= 1;
  }

  return crcReg;
}
/******************************************************************************
 * @fn          createPacket
 *
 * @brief       This function is called before a packet is transmitted. It fills
 *              the txBuffer with a packet consisting of a length byte, two
 *              bytes packet counter and n random bytes. If the packet length is
 *              fixed, the length byte is excluded.
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
    uint8 i;
    uint8 index;
  
    if(FIXED_LENGTH){
      txBuffer[0] = (uint8) packetCounter >> 8; // MSB of packetCounter
      txBuffer[1] = (uint8) packetCounter;      // LSB of packetCounter
      index = 2;
    }else{
      txBuffer[0] = (PKTLEN - 1);               // Length byte
      txBuffer[1] = (uint8) packetCounter >> 8; // MSB of packetCounter
      txBuffer[2] = (uint8) packetCounter;      // LSB of packetCounter
      index = 3;
    }
    
    // Fill rest of buffer with random bytes
    for(i =index; i< (PKTLEN); i++)
    {
      txBuffer[i] = (uint8)rand();
    }
}
/******************************************************************************
 * @fn          initMCU
 *
 * @brief       Initialize MCU and board peripherals
 *                
 * @param       input, output parameters
 *
 * @return      describe return value, if any
 */
static void initMCU(void){
  
  // Init clocks and I/O 
  bspInit(BSP_SYS_CLK_8MHZ);
  
  // Init leds 
  bspLedInit(); 

  // Init Buttons
  bspKeyInit(BSP_KEY_MODE_POLL);
  
  // Initialize SPI interface to LCD (shared with SPI flash)
  bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);  
  
  // Init LCD
  lcdInit();

  // Instantiate tranceiver RF spi interface to SCLK ~ 4 MHz */
  //input clockDivider - SMCLK/clockDivider gives SCLK frequency
  trxRfSpiInterfaceInit(0x04);

  // Enable global interrupt
  _BIS_SR(GIE);
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
  for(uint16 i = 0; i < (sizeof  preferredSettings/sizeof(registerSetting_t)); i++) {
    writeByte =  preferredSettings[i].data;
    cc112xSpiWriteReg( preferredSettings[i].addr, &writeByte, 1);
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
  lcdBufferPrintString(0, "    Serial Mode Test    ", 0, eLcdPage0);
  lcdBufferSetHLine(0, 0, LCD_COLS-1, eLcdPage7); 
  lcdBufferPrintString(0, "Packets sent:", 0, eLcdPage3);
  lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
  lcdBufferPrintString(0, "      Serial TX        ", 0, eLcdPage7);
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
static void manualCalibration(void){
  
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