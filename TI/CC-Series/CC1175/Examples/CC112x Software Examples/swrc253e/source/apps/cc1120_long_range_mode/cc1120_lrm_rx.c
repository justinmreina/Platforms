//******************************************************************************
//! @file       cc1120_lrm_rx.c
//! @brief      This file contains the runRX function and the ISR used by this
//              routine. Details are explained below in the function header. It
//              also contains functions for updating the LCD and help functions
//              used to make the runRX code more readable.
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
#define PACKET_SIZE             3
#define TIMEOUT                 322 // The transmitter sends with a 
                                    // period of 322 ms


/*******************************************************************************
* VARIABLES
*/
extern MENU_ITEM pMenuTable[];
static uint8  packetSemaphore;
static uint16 packetCounter = 0;
static uint16 packetsReceivedOK = 0;
static uint16 packetsLost = 0;
static uint8 graphMarkerPos;
static char graphBuffer[LCD_BYTES];  
static uint8 rssi2compl,rssiValid;
static uint8 rssiOffset = 102;
static int8 rssiConverted;
static uint16 timeout;
static uint8 received = FALSE;
static uint16 counter = 0;


/*******************************************************************************
* PROTOTYPES
*/
static void radioRxISR(void);
static void waitMs(uint16 msec);
static void waitUs(uint16 msec);
static void startTimerB(void);
static void updateRssiGraph(char *pBuffer,int8 rssiValue, uint8 received);
static void initRssiGraph(char *pBuffer);
static void regSettingsPacket1(void);
static void regSettingsPacket2(void);
static void updateRssi(void);
static void updateLcd(int32 packetCounter);
static void updateLcdRx(int32 packetsReceivedOK,
                        int32 packetsLost,
                        int32 rssiConverted,
                        int32 avgRssi);


/*******************************************************************************
*   @fn         runRX
*
*   @brief      The RX code will run the LRM algorithm described one the 
*               Sub-1GHz Wiki: 
*               http://processors.wiki.ti.com/index.php/Category:Sub-1GHz
*               For the 868 MHz band frequency offset compensation is 
*               implemented and packet1 is used to compensate for freq. errors 
*               while packet2 is the "real" packet. If packet2 is not received 
*               within a given time, the radio goes back to search for another 
*               packet1. Before starting the test, the RSSI is scanned and 
*               output on the LCD
*
*   @param      none
*
*   @return     none
*/
 void runRX(void) {
  uint8 rxBuffer[5] = {0};
  uint8 writeByte;
  uint8 readByte;
  uint8 freqOffset1;
  uint8 freqOffset0;
  uint8 key;
  
  while (TRUE) {

    // Configure register settings
    registerConfig((uint8)(*pMenuTable[FREQ_BAND].pValue));
  
    // Write custom register settings to radio (force fixed packet length in RX)
    cc112xSpiReadReg(CC112X_PKT_CFG0, &readByte, 1);
    writeByte = (readByte & 0x9F) | 0x00;
    cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
    
    // Connect ISR function to GPIO2
    ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &radioRxISR);

    // Calibrate radio according to errata
    manualCalibration();
  
    // Clear counter variables before running test
    packetsReceivedOK = 0;
    packetsLost = 0;
    timeout = TIMEOUT;
  
    // Clear the buffer used when printing to the LCD and init the RSSI graph
    lcdBufferClear(graphBuffer);
    initRssiGraph(graphBuffer);
  
    // Freq. offset compensation is only used on the 868 MHz band
    if ((uint8)(*pMenuTable[FREQ_BAND].pValue) == FREQ_868) {
    
      // Set registers and interrupts needed to search for packet1
      regSettingsPacket1();
    
      // Strobe SRX and wait for the radio to enter RX mode before starting the
      // timer
      trxSpiCmdStrobe(CC112X_SRX);
      
      do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &readByte, 1);
      } while (readByte != 0x6D);
    
      startTimerB();
      
      do {
        if (packetSemaphore != ISR_IDLE) { // Packet received?
          packetSemaphore = ISR_IDLE;
          
          // Enter IDLE (only sync is needed to do freq. offset compensation)
          trxSpiCmdStrobe(CC112X_SIDLE);
          trxSpiCmdStrobe(CC112X_SFRX);
          
          // Freq. offset compensation
          cc112xSpiReadReg(CC112X_FREQOFF_EST1, &freqOffset1, 1);
          writeByte = freqOffset1;
          cc112xSpiWriteReg(CC112X_FREQOFF1, &writeByte, 1);
          cc112xSpiReadReg(CC112X_FREQOFF_EST0, &freqOffset0, 1);
          writeByte = freqOffset0;
          cc112xSpiWriteReg(CC112X_FREQOFF0, &writeByte, 1);

          // Set registers and interrupts needed to search for packet2
          regSettingsPacket2();

          trxSpiCmdStrobe(CC112X_SRX);
          
          waitMs(160); // Sync should have been received within this time. This 
                       // number will change with data rate, packet format, and 
                       // code execution time between packet1 and packet2 in TX
         
          if (P1IN & GPIO2) { // If sync is found
            while (packetSemaphore == ISR_IDLE); // Wait for packet to be 
                                                 // received
            packetSemaphore = ISR_IDLE;
          
            cc112xSpiReadRxFifo(rxBuffer, PACKET_SIZE + 2);

            if(rxBuffer[PACKET_SIZE + 1] & 0x80) { // CRC OK?

              packetsReceivedOK++;
              updateRssi();
              received = TRUE;
              counter = timeout/2; // Avoid having the timeout happen at the 
                                   // same tiem as a packet is received
            }  
          } else { // Packet2 not received
            trxSpiCmdStrobe(CC112X_SIDLE);
            trxSpiCmdStrobe(CC112X_SFRX);
          }

          // Go back to search for packet1
          regSettingsPacket1();

          trxSpiCmdStrobe(CC112X_SRX);  
        } 
      } while (bspKeyPushed(BSP_KEY_ALL) == 0);

      // Test aborted
      trxSpiCmdStrobe(CC112X_SIDLE);
      trxSpiCmdStrobe(CC112X_SFRX);
      TB0CTL = 0x0200; // Stop mode: Timer is halted
      while (bspKeyPushed(BSP_KEY_ALL) == 0);
    } else { // 470 MHz, Freq. offset compesation is not used

      regSettingsPacket2();

      trxSpiCmdStrobe(CC112X_SRX);
      do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &readByte, 1);
      } while (readByte != 0x6D);
    
      startTimerB();

      do {
        if (packetSemaphore != ISR_IDLE) {
          packetSemaphore = ISR_IDLE;

          cc112xSpiReadRxFifo(rxBuffer, PACKET_SIZE + 2);

          if(rxBuffer[PACKET_SIZE + 1] & 0x80) { // CRC OK?

            packetsReceivedOK++;
            updateRssi();
            received = TRUE;
            counter = timeout/2; // Avoid having the timeout happen at the 
                                   // same tiem as a packet is received
          }  
          trxSpiCmdStrobe(CC112X_SRX);
        } 
      } while (bspKeyPushed(BSP_KEY_ALL) == 0);

      // Test aborted
      trxSpiCmdStrobe(CC112X_SIDLE);
      trxSpiCmdStrobe(CC112X_SFRX);
      TB0CTL = 0x0200; // Stop mode: Timer is halted
      while (bspKeyPushed(BSP_KEY_ALL) == 0);
    }
  }
}


/*******************************************************************************
* @fn          timerB0ISR
*
* @brief       Timer B generates an interrupt every 2 ms
*
* @param       none
*
* @return      none
*/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void timerB0ISR(void) {
  
  if (++counter == timeout) {
    timeout = TIMEOUT/2;
    if (!received) {
      cc112xSpiReadReg(CC112X_RSSI0, &rssiValid, 1);
      if(rssiValid & 0x01) {
        cc112xSpiReadReg(CC112X_RSSI1, &rssi2compl, 1);
        rssiConverted = (int8)rssi2compl - rssiOffset;
      }
      packetsLost++;
    }
    updateRssiGraph(graphBuffer, rssiConverted, received);   
    counter = 0;
    received = FALSE;
  }
}


/*******************************************************************************
* @fn          startTimerB
*
* @brief       Init and start TimerB
*
* @param       none
*
* @return      none
*/

static void startTimerB(void) {
  TB0CTL = TBSSEL_2 + MC_1 + TBCLR; // SMCLK, Up mode, clear TBR
  TB0CCTL0 = CCIE;                  // CCR0 interrupt enabled
  TB0CCR0 = 16000;                  // Int. every 16000 SMCLK cycles (2 ms)
}


/*******************************************************************************
*   @fn         updateRssi
*
*   @brief      Reads the RSSI register if RSSI_VALID is asserted and do 
*               necessary conversions. Calculates the average RSSI based on the 
*               last 10 packets
*
*   @param      none
*
*   @return     none
*/
static void updateRssi(void) {
  static uint8 index = 0;

  cc112xSpiReadReg(CC112X_RSSI0, &rssiValid, 1);

  if(rssiValid & 0x01) {
    cc112xSpiReadReg(CC112X_RSSI1, &rssi2compl, 1);
    rssiConverted = (int8)rssi2compl - rssiOffset;
  }
}


/*******************************************************************************
*   @fn         regSettingsPacket1
*
*   @brief      - Set register settings necessary to search for packet1 (packet 
*                 used for frequency offset compensation)
*                 - Fixed packet length: 1 byte
*                 - Sync word:           0x2633D9CC (SYNC_1)
*                 - RX Filter BW:        12.5 kHz
*               - Clear FREQOFF registers
*               - Rising edge interrupt on GPIO2 (PKT_SYNC_RXTX)
*
*   @param      none
*
*   @return     none
*/
static void regSettingsPacket1(void) {
  uint8 writeByte;

  // Set fixed packet length: 1 byte 
  writeByte = 0x01; cc112xSpiWriteReg(CC112X_PKT_LEN,  &writeByte, 1);

  // SYNC_1
  writeByte = 0x26; cc112xSpiWriteReg(CC112X_SYNC3, &writeByte, 1);
  writeByte = 0x33; cc112xSpiWriteReg(CC112X_SYNC2, &writeByte, 1);
  writeByte = 0xD9; cc112xSpiWriteReg(CC112X_SYNC1, &writeByte, 1);
  writeByte = 0xCC; cc112xSpiWriteReg(CC112X_SYNC0, &writeByte, 1);

  // Set RX Filter BW (12.5 kHz
  writeByte = 0x10; cc112xSpiWriteReg(CC112X_CHAN_BW,  &writeByte, 1);

  // Interrupt on rising edge
  ioPinIntDisable(IO_PIN_PORT_1, GPIO2);
  ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_RISING_EDGE);
  ioPinIntClear(IO_PIN_PORT_1, GPIO2);
  packetSemaphore = ISR_IDLE;
  ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

  // Clear FREQOFF register in case prev. compensation has been done on a
  // faulty packet
  writeByte = 0; cc112xSpiWriteReg(CC112X_FREQOFF1, &writeByte, 1);
  writeByte = 0; cc112xSpiWriteReg(CC112X_FREQOFF0, &writeByte, 1);
}


/*******************************************************************************
*   @fn        regSettingsPacket2
*
*   @brief     - Set register settings necessary to search for packet2 (packet 
*                used for frequency offset compensation)
*                - Fixed packet length: 3 byte
*                - Sync word:           0x930B51DE (SYNC_2)
*                - RX Filter BW:        7.8 kHz
*               - Falling edge interrupt on GPIO2 (PKT_SYNC_RXTX)
*
*   @param      none
*
*   @return     none
*/
static void regSettingsPacket2(void) {
  uint8 writeByte;

  // Set fixed packet length: 3 byte 
  writeByte = PACKET_SIZE; cc112xSpiWriteReg(CC112X_PKT_LEN, &writeByte, 1);

  // SYNC_2
  writeByte = 0x93; cc112xSpiWriteReg(CC112X_SYNC3, &writeByte, 1);
  writeByte = 0x0B; cc112xSpiWriteReg(CC112X_SYNC2, &writeByte, 1);
  writeByte = 0x51; cc112xSpiWriteReg(CC112X_SYNC1, &writeByte, 1);
  writeByte = 0xDE; cc112xSpiWriteReg(CC112X_SYNC0, &writeByte, 1);

  // Set RX Filter BW 7.8 kHz
  writeByte = 0x50; cc112xSpiWriteReg(CC112X_CHAN_BW,  &writeByte, 1);

  // Interrupt on falling edge
  ioPinIntDisable(IO_PIN_PORT_1, GPIO2);
  ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_FALLING_EDGE);
  ioPinIntClear(IO_PIN_PORT_1, GPIO2);
  packetSemaphore = ISR_IDLE;
  ioPinIntEnable(IO_PIN_PORT_1, GPIO2);
}


/*******************************************************************************
*   @fn         initRssiGraph
*
*   @brief      Writes out the axes etc. for the typical graph domain
*               First pixel (first col., first row) is pixel (0,0)
*
*
*   @param      pBuffer
*                 The target buffer
*
*   @return     none
*/
static void initRssiGraph(char *pBuffer) {
  char *pBuf = lcdDefaultBuffer; // Default buffer to point to
  if(pBuffer) pBuf = pBuffer;    // Point to the specified buffer

  graphMarkerPos = 29;

  // Top left of graph area: (28,  16)
  // Bottom right:           (127, 55)
  
  // Write static Packet Info text above graph area
  lcdBufferPrintString( pBuf, "Received:", 28, eLcdPage0);
  lcdBufferPrintString( pBuf, "Lost:",     28, eLcdPage1);

  // Write the surrounding axis box
  lcdBufferSetVLine(pBuf, 28,  16,  55); // Left y axis
  lcdBufferSetVLine(pBuf, 127, 16,  55); // Right y axis
  lcdBufferSetHLine(pBuf, 29,  126, 16); // Top x axis
  lcdBufferSetHLine(pBuf, 29,  126, 55); // Bottom x axis

  // Write y axis labels and markers
  lcdBufferPrintString(pBuf, " -22", 0,  eLcdPage2); // Page 2 label
  lcdBufferSetHLine(   pBuf, 26,     27, 19       ); // Page 2 marker
  lcdBufferPrintString(pBuf, " -46", 0,  eLcdPage3); // Page 3 label
  lcdBufferSetHLine(   pBuf, 26,     27, 27       ); // Page 3 marker
  lcdBufferPrintString(pBuf, " -70", 0,  eLcdPage4); // Page 4 label 
  lcdBufferSetHLine(   pBuf, 26,     27, 35       ); // Page 4 marker
  lcdBufferPrintString(pBuf, " -94", 0,  eLcdPage5); // Page 5 label 
  lcdBufferSetHLine(   pBuf, 26,     27, 43       ); // Page 5 marker
  lcdBufferPrintString(pBuf, "-118", 0,  eLcdPage6); // Page 6 label 
  lcdBufferSetHLine(   pBuf, 26,     27, 51       ); // Page 6 marker

  // Write static "RSSI" text below graph area
  lcdBufferPrintString(pBuf, "RSSI:     dBm", 30, eLcdPage7);
  return;
}


/*******************************************************************************
*   @fn         updateRssiGraph
*
*   @brief      Updates destination buffer with argument values
*
*   @param      pBuffer
*                 Destination buffer
*   @param      rssi
*                 Received RSSI value
*               received
*                 TRUE or FALSE, depeding packet received or not
*
* return        none
*/
static void updateRssiGraph(char *pBuffer,int8 rssiValue, uint8 received) {
  char *pBuf = lcdDefaultBuffer; // pBuf pointer defaults to the 
                                 // static halLcdBuffer
  if(pBuffer) pBuf = pBuffer;    // If pBuffer is set, pBuf points to it
  uint8 rssiPixel;               // Pixel row

  // Initial checking of column counter
  if(graphMarkerPos >= 127)
    graphMarkerPos = 29; // First x-pos of graph area
  
  // Write packets received OK and packets lost
  lcdBufferPrintString( pBuf, "Received:",              28, eLcdPage0);
  lcdBufferPrintInt(    pBuf, (int32)packetsReceivedOK, 85, eLcdPage0);
  lcdBufferPrintString( pBuf, "Lost:",                  28, eLcdPage1);
  lcdBufferPrintInt(    pBuf, (int32)packetsLost,       85, eLcdPage1);

  // Write RSSI
  lcdBufferPrintString(pBuf, "    ",    60, eLcdPage7); // Clearing old text and 
                                                        // adding unit
  lcdBufferPrintInt(   pBuf, rssiValue, 60, eLcdPage7); // Printing RSSI value 
                                                        // below graph

  // Clearing marker line
  lcdBufferClearVLine(pBuf, graphMarkerPos, 17, 54);

  
  // Drawing RSSI pixel
  rssiPixel = (((-1) * rssiValue + 36) / 3); // Convert RSSI value to a 
                                             // pixel row
  if (rssiPixel < 16) // Make sure line is not drawn outside boundaries
    rssiPixel = 16;
  
  if (received)
    lcdBufferSetPx(pBuf, graphMarkerPos, rssiPixel);
  else
    lcdBufferSetVLine(pBuf, graphMarkerPos, rssiPixel, 54);

  // Drawing marker line
  lcdBufferSetVLine(pBuf, (++graphMarkerPos), 17, 54);  
  lcdSendBuffer(graphBuffer);
}


/*******************************************************************************
*   @fn         waitUs
*
*   @brief      Busy wait function. Waits the specified number of microseconds.
*               Use assumptions about number of clock cycles needed for the
*               various instructions. The duration of one cycle depends on MCLK.
*               In this HAL it is set to 8 MHz, thus 8 cycles per us.
*
*               NB! This function is highly dependent on architecture
*               and compiler!
*
*   @param      uint16 uSec
*                 Number of microseconds delay
*
*   @return     none
*/
#pragma optimize=none
static void waitUs(uint16 uSec) { // 5 cycles for calling

  // The least we can wait is 3 usec:
  // ~1 usec for call, 1 for first compare and 1 for return
  while(uSec > 3) {  // 2 cycles for compare
                     // 2 cycles for jump
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      NOP();         // 1 cycle for nop
      uSec -= 2;     // 1 cycle for optimized decrement
  }
}                    // 4 cycles for returning


/*******************************************************************************
*   @fn         waitMs
*
*   @brief      Busy wait function. Waits the specified number of milliseconds.
*               Use assumptions about number of clock cycles needed for the
*               various instructions.
*
*               NB! This function is highly dependent on architecture and
*               compiler!
*
*   @param      uint16 mSec
*                 Number of milliseconds delay
*
*   @return     none
*/
#pragma optimize=none
static void waitMs(uint16 mSec) {
  while(mSec-- > 0) {
    waitUs(1000);
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
static void radioRxISR(void) {

  // Set packet semaphore
  packetSemaphore = ISR_ACTION_REQUIRED;

  // Clear ISR flag
  ioPinIntClear(IO_PIN_PORT_1, GPIO2);
}
