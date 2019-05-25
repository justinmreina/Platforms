//******************************************************************************
//! @file       cc1120_lrm_tx.c
//! @brief      This file contains the runTX function and the ISR used by this
//              routine. Details are explained below in the function header
//!
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
#include "cc1120_lrm_config.h"


/*******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED     1
#define ISR_IDLE                0

#define GPIO2                   0x08


/*******************************************************************************
* VARIABLES
*/
static uint8  packetSemaphore;
static uint16 packetCounter = 0;
extern MENU_ITEM pMenuTable[];
    

/*******************************************************************************
* PROTPTYPES
*/
static void radioTxISR(void);
static void updateLcdTx(int32 packetCounter);


/*******************************************************************************
*   @fn         runTX
*
*   @brief      Run TX routine and never return. Two packets are sent for each 
*               iteration; packet1 and packet2. Packet1 uses SYNC_1 and contains
*               a dummy byte. This packet is used by the receiver  to do 
*               frequency offset compensation. The second packet uses SYNC_2 and
*               is the "real" packet. For this example, this packet contains a
*               two byte long sequence number and a dummy byte. The LCD is 
*               updated with number of sent packets for every iteration of the 
*               loop. If a key is pushed the test is stopped, the packet counter
*               cleared and a key needs to be pushed again to restart the test
*
*   @param      none
*
*   @return    none
*/
void runTX(void) {
  
  uint8 writeByte;
  uint8 readByte;
  
  // Initialize packet buffer
  uint8 txBuffer[4] = {0};
    
  // Configure register settings
  registerConfig((uint8)(*pMenuTable[FREQ_BAND].pValue));
  
  // Write custom register settings to radio (force fixed packet length in TX)
  cc112xSpiReadReg(CC112X_PKT_CFG0, &readByte, 1);
  writeByte = (readByte & 0x9F) | 0x00;
  cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
  
  // Connect ISR function to GPIO2
  ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &radioTxISR);

  // Interrupt on falling edge
  ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_FALLING_EDGE);

  // Clear ISR flag
  ioPinIntClear(IO_PIN_PORT_1, GPIO2);

  // Enable interrupt
  ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

  // Calibrate radio according to errata
  manualCalibration();

  // Infinite loop
  while(TRUE) {
    do {

      txBuffer[0] = 0x55; // Dummy byte in packet 1
      txBuffer[1] = (uint8_t)(++packetCounter >> 8); // Seq. Number in packet 2
      txBuffer[2] = (uint8_t)(packetCounter); 
      txBuffer[3] = 0x55;

      // Write both packets to TX FIFO
      cc112xSpiWriteTxFifo(txBuffer, sizeof(txBuffer));

      // Reg. Config for packet 1
      writeByte = 0x01; cc112xSpiWriteReg(CC112X_PKT_LEN,  &writeByte, 1);

      // SYNC_1
      writeByte = 0x26; cc112xSpiWriteReg(CC112X_SYNC3, &writeByte, 1);
      writeByte = 0x33; cc112xSpiWriteReg(CC112X_SYNC2, &writeByte, 1);
      writeByte = 0xD9; cc112xSpiWriteReg(CC112X_SYNC1, &writeByte, 1);
      writeByte = 0xCC; cc112xSpiWriteReg(CC112X_SYNC0, &writeByte, 1);
    
      // Strobe TX to send packet 1
      //---------------------------------------------------------
      // 0xAA 0xAA 0xAA |  0x26 0x33 0xD9 0xCC | 0x55 | CRC CRC |
      //---------------------------------------------------------
      trxSpiCmdStrobe(CC112X_STX);

      // Wait for interrupt that packet has been sent.
      // (Assumes the GPIO connected to the radioRxTxISR function is
      // set to GPIOx_CFG = 0x06)
      while(packetSemaphore != ISR_ACTION_REQUIRED);

      // Clear semaphore flag
      packetSemaphore = ISR_IDLE;
    
      // Reg. Config for packet 2
      writeByte = 0x03; cc112xSpiWriteReg(CC112X_PKT_LEN,  &writeByte, 1);
       
      // SYNC_2
      writeByte = 0x93; cc112xSpiWriteReg(CC112X_SYNC3, &writeByte, 1);
      writeByte = 0x0B; cc112xSpiWriteReg(CC112X_SYNC2, &writeByte, 1);
      writeByte = 0x51; cc112xSpiWriteReg(CC112X_SYNC1, &writeByte, 1);
      writeByte = 0xDE; cc112xSpiWriteReg(CC112X_SYNC0, &writeByte, 1);

      // Strobe TX to send packet 2
      //--------------------------------------------------------------------
      // 0xAA 0xAA 0xAA |  0x93 0x0B 0x51 0xDE | Seq. Seq. 0x55  | CRC CRC |
      //--------------------------------------------------------------------
      trxSpiCmdStrobe(CC112X_STX);

      // Wait for interrupt that packet has been sent.
      // (Assumes the GPIO connected to the radioRxTxISR function is
      // set to GPIOx_CFG = 0x06)
      while(packetSemaphore != ISR_ACTION_REQUIRED);

      // Clear semaphore flag
      packetSemaphore = ISR_IDLE;
    
      updateLcdTx((int32)packetCounter);
    
    } while (bspKeyPushed(BSP_KEY_ALL) == 0);

    // Test has been stopped. Wait for key push to restart
    packetCounter = 0;
    lcdBufferPrintString(0, "Transmitting stopped", 0,            eLcdPage0);
    lcdBufferPrintString(0, "Press key to start",   0,            eLcdPage1);
    lcdBufferSetHLine(   0, 0,                      LCD_COLS - 1, 15       );
    lcdSendBuffer(0);
    while (bspKeyPushed(BSP_KEY_ALL) == 0);
  }
}


/*******************************************************************************
*   @fn         radioTxISR
*
*   @brief      ISR for packet handling in TX. Sets packet semaphore
*               and clears ISR flag
*
*   @param      none
*
*   @return     none
*/
static void radioTxISR(void) {

  // Set packet semaphore
  packetSemaphore = ISR_ACTION_REQUIRED;

  // Clear ISR flag
  ioPinIntClear(IO_PIN_PORT_1, GPIO2);
}


/*******************************************************************************
*   @fn         updateLcdTx
*
*   @brief      Updates the LCD every time a packet is sent
*
*   @param      packetCounter 
*
*   @return     none
*/
static void updateLcdTx(int32 packetCounter) {
  lcdBufferClear(0);
  lcdBufferPrintString(0, "Transmitting",         0,            eLcdPage0);
  lcdBufferPrintString(0, "Press key to stop",    0,            eLcdPage1);
  lcdBufferSetHLine(   0, 0,                      LCD_COLS - 1, 15        );
  lcdBufferPrintString(0, "Packets sent:",        0,            eLcdPage3);
  lcdBufferPrintInt(   0, packetCounter,          80,           eLcdPage3);
  lcdSendBuffer(0);
}