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

#include "cc112x_sniff_mode_api.h"
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
static void  cc112x_worRssiInit(void);
static void  cc112x_radioRXISR(void);
static void  cc112x_trxRxIdle(void);
static void  cc112x_trxIdleWor(void);
static void  cc112x_calibrateRCOsc(void);

/******************************************************************************
* DEFINES
*/
// defines used for the manual calibration
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
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

static uint8 cc112xRssiOffset = 96;

static const radioSetting_t simpleLinkTestSniff[] = 
{
  {CC112X_IOCFG3,            0x0F},// CCA Status
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            38},//MARC[0]
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_PREAMBLE_CFG1,     0x18},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x08},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_AGC_REF,           0x20},
  {CC112X_AGC_CS_THR,        0x02},
  {CC112X_AGC_CFG1,          0xA0},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_WOR_CFG0,          0x20},
  {CC112X_WOR_EVENT0_MSB,    0x02},
  {CC112X_WOR_EVENT0_LSB,    0xEA},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_RFEND_CFG0,        0x09},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC2,             0x00},
};

static const registerSetting_t simpleLinkTestSniffCC1125[]= 
{
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x08},
  {CC112X_DEVIATION_M,       0xA3},
  {CC112X_MODCFG_DEV_E,      0x0A},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x33},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x10},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,      0x3F},
  {CC112X_SYMBOL_RATE1,      0x75},
  {CC112X_SYMBOL_RATE0,      0x10},
  {CC112X_AGC_REF,           0x20},
  {CC112X_AGC_CS_THR,        0x0C},
  {CC112X_AGC_CFG1,          0xA0},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_WOR_CFG0,          0x20},
  {CC112X_WOR_EVENT0_MSB,    0x02},
  {CC112X_WOR_EVENT0_LSB,    0xEA},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_RFEND_CFG0,        0x09},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x56},
  {CC112X_FREQ1,             0xEC},
  {CC112X_FREQ0,             0x28},
  {CC112X_IF_ADC0,           0x05},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC3,             0xC7},
};

//Register Settings for different frequency bands.
static uint8 freqSettings[5][3] = 
{ 
  {0x69,0xF1,0xFF}, // 169.5125 MHz
  {0x6C,0x80,0x00}, // 434 MHz
  {0x6C,0x80,0x00}, // 868 MHz   
  {0x72,0x60,0x00}, // 915 MHz
  {0x77,0x60,0x00}  // 955 MHz
};
// For CC1125 with 40 MHz TSXO
static uint8 freqSettings40Mhz[5][3] = 
{ 
  {0x54, 0xC1, 0x89},  // 169.5125 MHz 
  {0x56, 0xCC, 0xCC},  // 433 MHz  
  {0x56, 0xCC, 0xCC},  // 868 MHz 
  {0x5B, 0x80, 0x00},  // 915 MHz
  {0x5F, 0x80, 0x00}   // 955 MHz
};
//Band select setting for LO divider
static uint8 cc112xFsCfgs[5] = 
{
  0x0A, // 169 MHz 
  0x04, // 434 MHz  
  0x02, // 868 MHz 
  0x02, // 915 MHz 
  0x02, // 955 MHz   
};


uint8 gpioConfigMaster[] = 
{
  0x0C, // MARC_ 2PIN_STATUS[1]
  0x26, // MARC_2PIN_STATUS[0]
  0x0B, // PQT_REACHED
  0x06, // PKT_SYNC_RXTX
};

// GPIO output for master and slave. For debug purpose
uint8 gpioConfigSlave[] = 
{
  0x38, // WOR_EVENT1
  0x37, // WOR_EVENT0
  0x10, // CARRIER_SENSE_VALID
  0x06, // PKT_SYNC_RXTX        This signal is needed for the radioRXISR(). Do not change.
};

/******************************************************************************
* @fn          cc112x_sniffModeRegConfig
*
* @brief       Writes packet and modem settings to registers
*
* input parameters
*              
* output parameters
*
* @return      void
*/
void cc112x_sniffModeRegConfig(uint16_t deviceName)
{
  // reset radio
  trxSpiCmdStrobe(CC112X_SRES);
  if(deviceName == CHIP_TYPE_CC1125)
  {
    //Write register settings based on TC01 to chip
    for(uint16 i = 0; i < (sizeof simpleLinkTestSniffCC1125/sizeof(radioSetting_t));i++)
    {
      writeByte  = (uint8)simpleLinkTestSniffCC1125[i].data;
      cc112xSpiWriteReg(simpleLinkTestSniffCC1125[i].addr,&writeByte,1);
    }    
  }
  else
  {
    //Write register settings based on TC01 to chip
    for(uint16 i = 0; i < (sizeof simpleLinkTestSniff/sizeof(radioSetting_t));i++)
    {
      writeByte  = (uint8)simpleLinkTestSniff[i].data;
      cc112xSpiWriteReg(simpleLinkTestSniff[i].addr,&writeByte,1);
    }
  }   
  // Put radio in powerdown to save power
  trxSpiCmdStrobe(CC112X_SPWD);
  
}
/******************************************************************************
* @fn          cc112x_FreqConfig
*
* @brief       Writes frequency word depending on user input
*
* input        index
*              
* output       none
*
* @return      void
*/
uint8 cc112x_FreqConfig(uint16 index) {
  // Set frequency
  cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[index],1);
  if(sniffRadioChipType.deviceName == CHIP_TYPE_CC1125)
  {
    cc112xSpiWriteReg(CC112X_FREQ2,freqSettings40Mhz[index],3);
  }
  else
  {
    cc112xSpiWriteReg(CC112X_FREQ2,freqSettings[index],3);
  }
  
  // Put radio in powerdown to save power
  trxSpiCmdStrobe(CC112X_SPWD);
  
  //Insert Carrier Sense threshold warning in Sniff Test Menu
  drawInfo();
  
  return SNIFF_RETURN_SUCCESS;
}

/******************************************************************************
* @fn          cc112x_masterStartApp
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
uint8 cc112x_masterStartApp(void)
{ 
  
  // Set first packet number
  pkt = 1;
  
  // Set up GPIO pins. For debug
  cc112xSpiWriteReg(CC112X_IOCFG3,&gpioConfigMaster[0],4);
  
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
        trxSpiCmdStrobe(CC112X_SPWD);
        //Insert Carrier Sense threshold warning in Sniff Test Menu
        drawInfo();
        
        return SNIFF_RETURN_FAILURE;
      }
      else if (buttonPressed == BSP_KEY_RIGHT)
      {
        cc112x_manualCalibration();
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
        trxSpiCmdStrobe(CC112X_SIDLE);
        // wait for radio to enter IDLE state
        while((trxSpiCmdStrobe(CC112X_SNOP)& 0xF0) != 0x00);
        cc112xSpiWriteTxFifo(comArray,PKTLEN+1);
        // Send packet
        trxSpiCmdStrobe(CC112X_STX);
        // Wait for radio to finish sending packet
        while((trxSpiCmdStrobe(CC112X_SNOP)& 0xF0) != 0x00);
        // Put radio in powerdown to save power
        trxSpiCmdStrobe(CC112X_SPWD);
        //Put MCU to sleep
        __low_power_mode_3(); 
      }
    }
  }
}
/******************************************************************************
* @fn          cc112x_sniffSlaveStartApp
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
uint8 cc112x_slaveStartApp(void)
{ 
  //Be sure we are in IDLE
  trxSpiCmdStrobe(CC112X_SIDLE);
  
  // Reset packet status
  packetStatus = ISR_IDLE;
  //Init packet counters
  pkt =0;
  pktMiss = 0;
  pktExpected = 1;
  
  // Set up GPIO pins. For debug 
  //IOCFG0 has to be 0x06 for radioRXISR interrupt. Other pins are free to change 
  cc112xSpiWriteReg(CC112X_IOCFG3,&gpioConfigSlave[0],4);
  // Connect ISR function to interrupt from GDO0 (P1.7)   
  trxIsrConnect(&cc112x_radioRXISR);   
  
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
  
  cc112x_manualCalibration();
  // calibrate RC Osc
  cc112x_calibrateRCOsc();
  
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
    cc112x_trxIdleWor();
    
    // Put MCU to sleep
    __low_power_mode_3();
    
    // If left button pushed by user, abort test and exit to menu
    if(BSP_KEY_LEFT == bspKeyPushed(BSP_KEY_ALL))
    {
      cc112x_trxRxIdle();
      //Insert Carrier Sense threshold warning in Sniff Test Menu
      drawInfo();
      // Put radio in powerdown to save power
      trxSpiCmdStrobe(CC112X_SPWD);
      break;
    }   
  }
  return SNIFF_RETURN_SUCCESS;
}

/***********************************************************************************
* @fn          cc112x_radioRXISR
*
* @brief       ISR for packet handling in RX
*
* @param       none
*
* @return      none
*/
static void cc112x_radioRXISR(void)
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
  rssi = (int16)((int8)rssi_readout) - cc112xRssiOffset;
  
  cc112x_trxRxIdle();
}
/***************************************************************************************************
* @fn          cc112x_trxRxIdle
*
* @brief       Radio state is switched from RX to IDLE
*              Note: assumes chip is ready
*
* @param       none
*
* @return      none
**************************************************************************************************/ 
void cc112x_trxRxIdle(void)
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
* @fn          cc112x_trxIdleWor
*
* @brief       Radio state is switched from Idle to WOR  
*              Note: assumes chip is ready
*
* @param       none
*
* @return      none
**************************************************************************************************/ 
void cc112x_trxIdleWor(void)
{
  trxClearIntFlag();
  // Reset WOR otherwise we might get stuck in powerdown
  trxSpiCmdStrobe(CC112X_SWORRST);
  trxSpiCmdStrobe(CC112X_SWOR);
  
  // Maybe switch 
  trxEnableInt();
}
/******************************************************************************
* @fn          cc112x_calibrateRcOsc
*
* @brief       calibrates the RC Oclillator used for the cc112x wake on radio 
*              functionality.
*                
* @param       none
*
* @return      none
*/
static void cc112x_calibrateRCOsc(void)
{
  uint8 writeByte; 
  // read current register value
  cc112xSpiReadReg(CC112X_WOR_CFG0,&writeByte,1);
  // mask register bitfields and write new values
  writeByte = (writeByte & 0xF9) | (0x02 << 1);
  //write new register value
  cc112xSpiWriteReg(CC112X_WOR_CFG0,&writeByte,1);
  // strobe idle to calibrate RC osc
  trxSpiCmdStrobe(CC112X_SIDLE);
  //disable RC calibration
  writeByte = (writeByte & 0xF9) | (0x00 << 1);
  cc112xSpiWriteReg(CC112X_WOR_CFG0, & writeByte, 1); 
}
/******************************************************************************
* @fn          cc112x_manualCalibration
*
* @brief       calibrates radio according to CC112x errata
*                
* @param       none
*
* @return      none
*/
void cc112x_manualCalibration(void) 
{
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