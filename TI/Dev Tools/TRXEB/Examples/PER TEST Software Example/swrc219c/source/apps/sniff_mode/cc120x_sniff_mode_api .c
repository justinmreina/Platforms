//*****************************************************************************
//! @file      cc11xL_per_test_api.h
//  
//! @brief :   This header file declares api-like functions that the per test
//             will call if a CC11xL was detected.
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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


#include <msp430.h>
#include "lcd_dogm128_6.h"
#include "chip_detect.h"
#include "hal_timer_32k.h"
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "menu_driver.h"
#include "sniff_mode.h"
#include "cc112x_spi.h"
#include "cc120x_spi.h"
#include "bsp.h"
#include "bsp_key.h"
#include "cc120x_sniff_mode_api.h"
/******************************************************************************
 * TYPEDEFS
 */
typedef struct
{
  uint16  addr;
  uint8   data;
} radioSetting_t;

/******************************************************************************
* LOCAL FUNCTIONS
*/
static void  cc120x_radioRXISR(void);
static void  cc120x_trxRxIdle(void);
static void  cc120x_trxIdleWor(void);
static void  cc120x_calibrateRCOsc(void);

/******************************************************************************
 * LOCAL VARIABLES
 */
static radioChipType_t sniffRadioChipType;
static uint8 writeByte;
static radioChipType_t sniffRadioChipType;

static uint8 writeByte;
static uint8 buttonPressed;
static uint8 comArray[20];

static uint32 pkt;
static uint32 pktMiss;
static uint32 pktExpected;
static uint8 packetStatus;

static int8 rssi;
static uint8 rssi_readout;

static uint8 cc120xRssiOffset = 96;

static const registerSetting_t simpleLinkTestSniffCC120x[]= 
{
  {CC120X_IOCFG2, 		0x13},
  {CC120X_IOCFG0, 		0x06},
  {CC120X_DEVIATION_M,       0xD1},
  {CC120X_MODCFG_DEV_E,      0x00},
  {CC120X_DCFILT_CFG,        0x5D},
  {CC120X_PREAMBLE_CFG0,     0x8A},
  {CC120X_IQIC,              0xCB},
  {CC120X_CHAN_BW,           0xA6},
  {CC120X_MDMCFG1,           0x40},
  {CC120X_MDMCFG0,           0x05},
  {CC120X_SYMBOL_RATE2,      0x3F},
  {CC120X_SYMBOL_RATE1,      0x75},
  {CC120X_SYMBOL_RATE0,      0x10},
  {CC120X_AGC_REF,           0x20},
  {CC120X_AGC_CS_THR,        0xFF},
  {CC120X_AGC_CFG1,          0x40},
  {CC120X_AGC_CFG0,          0x83},
  {CC120X_FIFO_CFG,          0x00},
  {CC120X_SETTLING_CFG,      0x03},
  {CC120X_FS_CFG,            0x12},
  {CC120X_WOR_CFG0,          0x20},
  {CC120X_WOR_EVENT0_MSB,    0x03},
  {CC120X_WOR_EVENT0_LSB,    0xA5},
  {CC120X_PKT_CFG2,          0x00},
  {CC120X_PKT_CFG0,          0x20},
  {CC120X_RFEND_CFG0,        0x09},
  {CC120X_PA_CFG1,           0x3F},
  {CC120X_PKT_LEN,           0xFF},
  {CC120X_IF_MIX_CFG,        0x1C},
  {CC120X_FREQOFF_CFG,       0x22},
  {CC120X_MDMCFG2,           0x0C},
  {CC120X_FREQ2,             0x56},
  {CC120X_FREQ1,             0xCC},
  {CC120X_FREQ0,             0xCC},
  {CC120X_IF_ADC1,           0xEE},
  {CC120X_IF_ADC0,           0x10},
  {CC120X_FS_DIG1,           0x07},
  {CC120X_FS_DIG0,           0xAF},
  {CC120X_FS_CAL1,           0x40},
  {CC120X_FS_CAL0,           0x0E},
  {CC120X_FS_DIVTWO,         0x03},
  {CC120X_FS_DSM0,           0x33},
  {CC120X_FS_DVC0,           0x17},
  {CC120X_FS_PFD,            0x00},
  {CC120X_FS_PRE,            0x6E},
  {CC120X_FS_REG_DIV_CML,    0x1C},
  {CC120X_FS_SPARE,          0xAC},
  {CC120X_FS_VCO0,           0xB5},
  {CC120X_XOSC5,             0x0E},
};

static uint8 freqSettings40Mhz[5][3] = 
{ 
  {0x54, 0xC1, 0x89},  // 169.5125 MHz 
  {0x56, 0xCC, 0xCC},  // 433 MHz  
  {0x56, 0xCC, 0xCC},  // 868 MHz 
  {0x5B, 0x80, 0x00},  // 915 MHz
  {0x5F, 0x80, 0x00}   // 955 MHz
};

//Band select setting for LO divider
static uint8 cc120xFsCfgs[5] = 
{
  0x0A, // 169 MHz 
  0x04, // 434 MHz  
  0x02, // 868 MHz 
  0x02, // 915 MHz 
  0x02, // 955 MHz   
};

// GPIO output for master and slave. For debug purpose
uint8 cc120x_gpioConfigMaster[] = 
{
  0x0C, // MARC_ 2PIN_STATUS[1]
  0x26, // MARC_2PIN_STATUS[0]
  0x0B, // PQT_REACHED
  0x06, // PKT_SYNC_RXTX
};

uint8 cc120x_gpioConfigSlave[] = 
{
  0x38, // WOR_EVENT1
  0x37, // WOR_EVENT0
  0x10, // CARRIER_SENSE_VALID
  0x06, // PKT_SYNC_RXTX        This signal is needed for the radioRXISR(). Do not change.
};
/******************************************************************************
 * @fn          cc120x_sniffModeRegConfig
 *
 * @brief       Writes packet and modem settings to registers
 *
 * input parameters
 *              
 * output parameters
 *
 * @return      void
*/
void cc120x_sniffModeRegConfig(uint16_t deviceName) {
  uint8 writeByte;
  
  // Reset radio
  trxSpiCmdStrobe(CC120X_SRES);
  
  // Write registers to radio
  for(uint16 i = 0; i < (sizeof(simpleLinkTestSniffCC120x)/sizeof(radioSetting_t)); i++) {
    writeByte =  simpleLinkTestSniffCC120x[i].data;
    cc120xSpiWriteReg( simpleLinkTestSniffCC120x[i].addr, &writeByte, 1);
  }  
    // Put radio in powerdown to save power
    trxSpiCmdStrobe(CC120X_SPWD);
}

/******************************************************************************
* @fn          cc120x_FreqConfig
*
* @brief       Writes frequency word depending on user input
*
* input        index
*              
* output       none
*
* @return      void
*/
uint8 cc120x_FreqConfig(uint16 index) {
  // Set frequency
  cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[index],1);
  cc120xSpiWriteReg(CC120X_FREQ2,freqSettings40Mhz[index],3);
  
  // Put radio in powerdown to save power
  trxSpiCmdStrobe(CC120X_SPWD);
  
  //Insert Carrier Sense threshold warning in Sniff Test Menu
  drawInfo();
  
  return SNIFF_RETURN_SUCCESS;
}

/******************************************************************************
* @fn          cc120x_masterStartApp
*
* @brief       
*
* input parameters
*
* @param       pDummy  - pointer to pointer to void. Not used
*
* output parameters
*
* @return      SNIFF_RETURN_SUCCESS
*/
uint8 cc120x_masterStartApp(void)
{ 
  static uint8 marcState;
  // Set first packet number
  pkt = 1;
  
  // Set up GPIO pins. For debug
  cc112xSpiWriteReg(CC120X_IOCFG3,&cc120x_gpioConfigMaster[0],4);
  
  //Display while configuring radios*
  lcdBufferClear(0);
  lcdBufferPrintString(0,"    RX Sniff Test    ",0,eLcdPage0);
  lcdBufferSetHLine(0,0,LCD_COLS-1,eLcdPage7);
  lcdBufferPrintString(0,"     Radio in TX     ",0,eLcdPage2);
  lcdBufferPrintString(0,"                 ",0,eLcdPage3);
  lcdBufferPrintString(0,"  Press right button  ",0,eLcdPage4);
  lcdBufferPrintString(0,"    to send packet  ",0,eLcdPage5);
  lcdBufferPrintString(0," 1 Abort Master Mode ",0,eLcdPage7);
  lcdBufferSetHLine(0,0,LCD_COLS-1,55);
  lcdBufferInvertPage(0,0,LCD_COLS,eLcdPage7);
  lcdSendBuffer(0);
  
  // Calibrate radio
  trxSpiCmdStrobe(CC120X_SCAL);
  
  // Wait for calibration to be done (radio back in IDLE state)
  do {
    cc120xSpiReadReg(CC120X_MARCSTATE, &marcState, 1);
  } while (marcState != 0x41);
  // Put MCU to sleep
  __low_power_mode_3();
  while(1)
  {
    if(buttonPressed = bspKeyPushed(BSP_KEY_ALL))
    {
      if(buttonPressed == BSP_KEY_LEFT)
      {
        // Left button pressed. Abort function
        // Put radio in powerdown to save power
        trxSpiCmdStrobe(CC120X_SPWD);
        //Insert Carrier Sense threshold warning in Sniff Test Menu
        drawInfo();
        
        return SNIFF_RETURN_FAILURE;
      }
      else if (buttonPressed == BSP_KEY_RIGHT)
      {        
        //Right button pressed, send packet
        lcdBufferClear(0);
        // build packet
        comArray[0] = PKTLEN;   // length field
        comArray[1] = 0x00;     // address field
        comArray[2] = pkt>>24;  // payload
        comArray[3] = pkt>>16;
        comArray[4] = pkt>>8;
        comArray[5] = pkt;
        // Update LCD
        lcdBufferPrintString(0,"    RX Sniff Test    ",0,eLcdPage0);
        lcdBufferSetHLine(0,0,LCD_COLS-1,eLcdPage7);
        lcdBufferPrintString(0,"Sent Pkt number:",0,eLcdPage3);
        lcdBufferPrintInt(0,pkt,70,eLcdPage4);
        lcdBufferPrintString(0," 1 Abort Master Mode ",0,eLcdPage7);
        lcdBufferSetHLine(0,0,LCD_COLS-1,55);
        lcdBufferInvertPage(0,0,LCD_COLS,eLcdPage7);
        lcdSendBuffer(0);
        // Update packet counter
        pkt++;
        // Strobe IDLE and fill TX FIFO
        trxSpiCmdStrobe(CC120X_SIDLE);
        // wait for radio to enter IDLE state
        while((trxSpiCmdStrobe(CC112X_SNOP)& 0xF0) != 0x00);
        cc112xSpiWriteTxFifo(comArray,PKTLEN+1);
        // Send packet
        trxSpiCmdStrobe(CC120X_STX);
        // Wait for radio to finish sending packet
        while((trxSpiCmdStrobe(CC120X_SNOP)& 0xF0) != 0x00);
        // Put radio in powerdown to save power
        trxSpiCmdStrobe(CC120X_SPWD);
        //Put MCU to sleep
        __low_power_mode_3(); 
      }
    }
  }
}
/******************************************************************************
* @fn          cc120x_sniffSlaveStartApp
*
* @brief       
*
* input parameters
*
* @param       pDummy  - pointer to pointer to void. Not used
*
* output parameters
*
* @return      SNIFF_RETURN_SUCCESS
*/
uint8 cc120x_slaveStartApp(void)
{ 
  uint8 marcState;
  //Be sure we are in IDLE
  trxSpiCmdStrobe(CC112X_SIDLE);
  
  // Reset packet status
  packetStatus = ISR_IDLE;
  //Intit packet counters
  pkt =0;
  pktMiss = 0;
  pktExpected = 1;
  
  // Calibrate radio
  trxSpiCmdStrobe(CC120X_SCAL);
  
  // Wait for calibration to be done (radio back in IDLE state)
  do {
    cc120xSpiReadReg(CC120X_MARCSTATE, &marcState, 1);
  } while (marcState != 0x41);
  
  // Calibrate the RCOSC
  cc120x_calibrateRCOsc();
  
  // Set up GPIO pins. For debug 
  //IOCFG0 has to be 0x06 for radioRXISR interrupt. Other pins are free to change 
  cc112xSpiWriteReg(CC120X_IOCFG3,&cc120x_gpioConfigSlave[0],4);
  // Connect ISR function to interrupt from GDO0 (P1.7)   
  trxIsrConnect(&cc120x_radioRXISR);   
  
  // Update LCD with status
  lcdBufferClear(0);
  lcdBufferPrintString(0,"    RX Sniff Test    ",0,eLcdPage0);
  lcdBufferSetHLine(0,0,LCD_COLS-1,eLcdPage7);
  lcdBufferPrintString(0,"     Radio in RX      ",0,eLcdPage3);
  lcdBufferPrintString(0,"  Waiting for packet  ",0,eLcdPage4);
  lcdBufferPrintString(0," 1 Abort Slave Mode ",0,eLcdPage7);
  lcdBufferSetHLine(0,0,LCD_COLS-1,55);
  lcdBufferInvertPage(0,0,LCD_COLS,eLcdPage7);
  lcdSendBuffer(0);
    
  while(1)
  {
    if(packetStatus == ISR_ACTION_REQUIRED)
    {
      packetStatus = ISR_IDLE;
      // Update display
      lcdBufferClear(0);
      lcdBufferPrintString(0,"    RX Sniff Test    ",0,eLcdPage0);
      lcdBufferSetHLine(0,0,LCD_COLS-1,eLcdPage7);
      lcdBufferPrintString(0,"Rcv'd Pkt nr:",0,eLcdPage2);
      lcdBufferPrintInt(0,pkt,90,eLcdPage2);
      lcdBufferPrintString(0,"Missed Pkt's:",0,eLcdPage3);
      lcdBufferPrintInt(0,pktMiss,90,eLcdPage3); 
      lcdBufferPrintString(0,"RSSI:",0,eLcdPage5);
      lcdBufferPrintInt(0,rssi,40,eLcdPage5); 
      lcdBufferPrintString(0," 1 Abort Slave Mode ",0,eLcdPage7);
      lcdBufferSetHLine(0,0,LCD_COLS-1,55);
      lcdBufferInvertPage(0,0,LCD_COLS,eLcdPage7);
      lcdSendBuffer(0);
    }
    // Re-enter WOR cycle
    cc120x_trxIdleWor();
    
    // Put MCU to sleep
    __low_power_mode_3();
    
    // If left button pushed by user, abort test and exit to menu
    if(BSP_KEY_LEFT == bspKeyPushed(BSP_KEY_ALL))
    {
      cc120x_trxRxIdle();
      //Insert Carrier Sense threshold warning in Sniff Test Menu
      drawInfo();
      // Put radio in powerdown to save power
      trxSpiCmdStrobe(CC120X_SPWD);
      break;
    }   
  }
  return SNIFF_RETURN_SUCCESS;
}
/***********************************************************************************
* @fn          cc120x_radioRXISR
*
* @brief       ISR for packet handling in RX
*
* @param       none
*
* @return      none
*/
static void cc120x_radioRXISR(void)
{
  uint8 rxBytes;
  packetStatus = ISR_ACTION_REQUIRED;
  // Read NUM_RXBYTES for bytes in FIFO  
  cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
  
  if(rxBytes == PKTLEN+3) // PAYLOAD + LENGHT BYTE + 2 STATUS BYTES
  { 
    // Read RX FIFO
    cc112xSpiReadRxFifo(comArray,rxBytes);
    // Check CRC ok and read packet if CRC ok. (CRC_OK: bit7 in second status byte)
    if(comArray[rxBytes-1] | 0x80)
    {
      pkt = (((uint32)comArray[2])<<24)|(((uint32)comArray[3])<<16)|(((uint32)comArray[4])<<8)|(comArray[5]);
    }
  }
  // Check if we missed any packets
  if(pkt != pktExpected )
  {
    if(pkt > pktExpected)
    {
      pktMiss += (pkt-pktExpected);
    }
    else
    {
      pktMiss =0;
    }          
  }
  pktExpected = pkt+1;
  // Read RSSI
  cc112xSpiReadReg(CC112X_RSSI1,&rssi_readout,1);
  // Convert to decimal value from 2's complement rssi_readout. 
  rssi = (int16)((int8)rssi_readout) - cc120xRssiOffset;
  
  cc120x_trxRxIdle();
}
/***************************************************************************************************
* @fn          cc120x_trxRxIdle
*
* @brief       Radio state is switched from RX to IDLE
*              Note: assumes chip is ready
*
* @param       none
*
* @return      none
**************************************************************************************************/ 
void cc120x_trxRxIdle(void)
{
  // Disable pin interrupt 
  trxDisableInt();
  // Strobe IDLE 
  trxSpiCmdStrobe(CC112X_SIDLE);
  // Wait until chip is in IDLE 
  while(trxSpiCmdStrobe(CC112X_SNOP)& 0xF0);
  // Flush the Receive FIFO 
  trxSpiCmdStrobe(CC112X_SFRX);
  // Clear pin interrupt flag 
  trxClearIntFlag();
}
/***************************************************************************************************
* @fn          cc120x_trxIdleWor
*
* @brief       Radio state is switched from Idle to WOR  
*              Note: assumes chip is ready
*
* @param       none
*
* @return      none
**************************************************************************************************/ 
void cc120x_trxIdleWor(void)
{
  trxClearIntFlag();
  // Reset WOR otherwise we might get stuck in powerdown
  trxSpiCmdStrobe(CC112X_SWORRST);
  trxSpiCmdStrobe(CC112X_SWOR);
  
  // Maybe switch 
  trxEnableInt();
}
/*******************************************************************************
* @fn          cc120x_calibrateRcOsc
*
* @brief       Calibrates the RC oscillator used for the eWOR timer. When this
*              function is called, WOR_CFG0.RC_PD must be 0
*
* @param       none
*
* @return      none
*/
static void cc120x_calibrateRCOsc(void) {

  uint8 temp; 

  // Read current register value
  cc120xSpiReadReg(CC120X_WOR_CFG0, &temp,1);

  // Mask register bit fields and write new values
  temp = (temp & 0xF9) | (0x02 << 1);

  // Write new register value
  cc120xSpiWriteReg(CC120X_WOR_CFG0, &temp,1);

  // Strobe IDLE to calibrate the RCOSC
  trxSpiCmdStrobe(CC120X_SIDLE);

  // Disable RC calibration
  temp = (temp & 0xF9) | (0x00 << 1);
  cc120xSpiWriteReg(CC120X_WOR_CFG0, &temp, 1); 
}