//*****************************************************************************
//! @file        cc112x_synchronous_serial_mode_rx.c
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
#define SERIAL_DATA         (P1IN & 0x08)


#define CRC16_POLY          0x8005
#define CRC_INIT            0xFFFF

#define FIXED_LENGTH        0 // 0 if variable packet length, else packetlength

/******************************************************************************
* LOCAL VARIABLES
*/
static uint32 packetCounter = 0;
static uint8 erroneousPackets = 0;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
static void runSerialRX(void);
static void updateLcd(void);
static uint16 calcCRC(uint8 crcData, uint16 crcReg);


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
  
  // Enter runRX, never coming back
  runSerialRX();
  
}
/******************************************************************************
* @fn          runSerialRX
*
* @brief       Puts radio in RX and waits for packets. The function reads the
*              packets and performs a CRC check. Updates packet counter and 
*              display for each packet received with correct CRC.
*                
* @param       none
*
* @return      none
*/
static void runSerialRX(void){
 
  // Set direction on P1.1
  P1DIR &= ~0x08;
  P1SEL &= ~0x08;
  P1REN &= ~0x08;
  
  uint8 writeByte;
  
  //Set serial mode specific registers
  writeByte = 0x08;
  cc112xSpiWriteReg(CC112X_IOCFG3, &writeByte, 1);
  writeByte = 0x09;
  cc112xSpiWriteReg(CC112X_IOCFG2, &writeByte, 1);
  writeByte = 0x30;
  cc112xSpiWriteReg(CC112X_IOCFG0, &writeByte, 1);
  writeByte = 0x0F;
  cc112xSpiWriteReg(CC112X_SYNC_CFG1, &writeByte, 1);
  writeByte = 0x00;
  cc112xSpiWriteReg(CC112X_PREAMBLE_CFG1, &writeByte, 1);
  writeByte = 0x06;
  cc112xSpiWriteReg(CC112X_MDMCFG1, &writeByte, 1);
  writeByte = 0x0A;
  cc112xSpiWriteReg(CC112X_MDMCFG0, &writeByte, 1);
  writeByte = 0x0A;
  cc112xSpiWriteReg(CC112X_AGC_CFG1, &writeByte, 1);
  writeByte = 0x05;
  cc112xSpiWriteReg(CC112X_PKT_CFG2, &writeByte, 1);
  writeByte = 0x00;
  cc112xSpiWriteReg(CC112X_PKT_CFG1, &writeByte, 1);
  writeByte = 0x08;
  cc112xSpiWriteReg(CC112X_SERIAL_STATUS, &writeByte, 1);
  
  uint8 i, j;
  uint8 dataByte = 0;
  uint8 lengthByte;
  uint8 crcBytes; 
  uint16 checksum;
  uint8 bufferIndex;
  
  uint8 rxBuffer[128] = {0};
  crcBytes = 2;
  
  // Update LCD  
  updateLcd();  
  // Calibrate radio according to errata
  manualCalibration();
 
  while(1){
  trxSpiCmdStrobe(CC112X_SRX);
  if(FIXED_LENGTH){
    bufferIndex = 0;
    lengthByte = FIXED_LENGTH;
    for (j = 0; j < (lengthByte + crcBytes); j++) {   
      for (i = 0; i < 8; i++) {                           
        while (!SERIAL_CLK);
        dataByte = dataByte << 1;
        if(SERIAL_DATA == 0)
          dataByte &= 0xFE;
        else
          dataByte |= 0x01;
        while (SERIAL_CLK);
      }
      rxBuffer[bufferIndex++] = dataByte;
    }
  }

  else {
    bufferIndex = 1;
    // Read length byte
    for (i = 0; i < 8; i++) {
      while (!SERIAL_CLK);
      lengthByte = lengthByte << 1;
      if(SERIAL_DATA == 0)
        lengthByte &= 0xFE;
      else
        lengthByte |= 0x01;
      while (SERIAL_CLK);
    }  
         
    // Read the rest of the payload + optionally the 2 CRC bytes
    for (j = 0; j < (lengthByte + crcBytes); j++) {        
      for (i = 0; i < 8; i++) {                            
        while (!SERIAL_CLK);
        dataByte = dataByte << 1;
        if(SERIAL_DATA == 0)
          dataByte &= 0xFE;
        else
          dataByte |= 0x01;
        while (SERIAL_CLK);
      }
      rxBuffer[bufferIndex++] = dataByte;
    }
    rxBuffer[0] = lengthByte;
  }
  trxSpiCmdStrobe(CC112X_SIDLE);  
  if (crcBytes) {
    //Calculate CRC
    checksum = CRC_INIT;
    for(i = 0; i < (bufferIndex); i++)
      checksum = calcCRC(rxBuffer[i], checksum);    
    if(checksum == 0)
      packetCounter++;
    else
      erroneousPackets++;
  }
  else
      packetCounter++;
  updateLcd();
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
  bspInit(BSP_SYS_CLK_25MHZ);
  
  // Initialize SPI interface to LCD (shared with SPI flash)
  bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);  
  
  // Init LCD
  lcdInit();

  // Instantiate tranceiver RF spi interface to SCLK ~ 4 MHz */
  //input clockDivider - SMCLK/clockDivider gives SCLK frequency
  trxRfSpiInterfaceInit(0x08);

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
  lcdBufferPrintString(0, "Received ok:", 0, eLcdPage3);
  lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
  lcdBufferPrintString(0, "      Serial RX        ", 0, eLcdPage7);
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