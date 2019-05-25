//*****************************************************************************
//! @file       cc112x_transparent_serial_mode_tx.c
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
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"


#include "stdlib.h"
#include "cc112x_serial_mode_reg_config.h"
#include "math.h"
#include "stdbool.h"

/******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED TRUE
#define ISR_IDLE            FALSE
#define WHITENING           TRUE
#define FIXED_LENGTH        FALSE
#define CRC_ENABLE          TRUE
#define CRC_INIT            0xFFFF
#define CRC16_POLY          0x8005
#define MSB                 0x80
#define PREAMBLE_LENGTH     8
#define PKTLEN              125
#define BUFFER_SIZE         PKTLEN + PREAMBLE_LENGTH + 6
#define SERIAL_DATA         P1OUT

/******************************************************************************
* GLOBAL VARIABLES
*/
static uint32 delay;
static uint8 firstByte;
static unsigned long bitRate;
static uint32 packetCounter = 0;
static volatile uint8  packetSemaphore;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void initTX(void);
static void initMCU(void);
static void updateLcd(void);
static long getBitRate(void);
static void registerConfig(void);
static uint8 getSystemClock(void);
static void runTransparentTX(void);
static void manualCalibration(void);
static void whitenData(uint8 *txBuffer);
static void createPacket(uint8 txBuffer[]);
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
void main(void) {
  // Initialize MCU and peripherals
  initMCU();
  
  // Write radio registers
  registerConfig();
  
  // Calibrate radio according to errata
  manualCalibration();
  
  //Initialize TX
  initTX();
  
  // Enter runTX, never coming back
  runTransparentTX();
}

/******************************************************************************
* @fn          runTransparentTX
*
* @brief       Continuously sends packets on button push until button is pushed
*              again. Updates packet counter and display for each packet sent.
*
*              For each given data rate, the timer needs to be adjusted to
*              sample at correct data rate. The timer value for a complete bit 
*              period can be calculated by (clock frequency)/(data rate).
*              Some adjustments may be nessesary. The timer should be reset to 
*              complete bit period in each timer interrupt. It might be 
*              nessesary to synchronize the timer to avoid drifting and packet 
*              errors.
*
* @param       none
*
* @return      none
*/
static void runTransparentTX() {
    
    uint8 i;
    uint16 checksum;
    uint8 dataByte = 0;
    uint8 bufferIndex = 0;
    uint8 txBuffer[BUFFER_SIZE] = {0}; 
    
    while(1) {
      
      //If any buttons are pushed
      if(bspKeyPushed(BSP_KEY_ALL)){
       
        //Continiously send packets until button is pressed
        do{
          createPacket(txBuffer);
          
          if (CRC_ENABLE) {
            
            //Generate CRC
            checksum = CRC_INIT;   //Init value for CRC calculation
            for(i = PREAMBLE_LENGTH+4; i < (BUFFER_SIZE - 2); i++) 
                checksum = calcCRC(txBuffer[i], checksum);
            txBuffer[BUFFER_SIZE - 2] = (checksum >> 8);     // CRC1
            txBuffer[BUFFER_SIZE - 1] = (checksum & 0x00FF); // CRC0
          }
          
          //Whiten the data if defined
          if(WHITENING)
            whitenData(txBuffer);
          
          firstByte = txBuffer[0];
          //MSB of the first byte needs to ready on the data line 
          //before strobing TX
          if (((firstByte & MSB) >> 7) == 0)
            SERIAL_DATA &= ~BIT7;
          else
            SERIAL_DATA |= BIT7;
          firstByte = firstByte << 1;
  
          //Enter TX mode          
          trxSpiCmdStrobe(CC112X_STX);
          
          //Set timer to delay the bittime
          TA1CCR0 = delay;
          
          //Transmit the first byte
          for (i = 1; i < 8; i++) {
            //Enable timer interrupt
            TA1CCTL0 |= CCIE; 
            if (((firstByte & MSB) >> 7) == 0){
              while(!packetSemaphore);
              SERIAL_DATA &= ~BIT7;
            }else{
              while(!packetSemaphore);
              SERIAL_DATA |= BIT7;
            }
            //Disable timer interrupt
            TA1CCTL0 &= ~CCIE; 
            firstByte = firstByte << 1;
            packetSemaphore = ISR_IDLE;
            //Clear timer interrupt flag
            TA1CCTL0 &= ~CCIFG; 
          }
      
          //Transmit the rest of the packet                               
          for (bufferIndex = 1; bufferIndex < BUFFER_SIZE; bufferIndex++) {
            dataByte = txBuffer[bufferIndex];
            for (i = 0; i < 8; i++) {
              TA1CCTL0 |= CCIE;
              if (((dataByte & MSB) >> 7) == 0){
                while(!packetSemaphore);
                SERIAL_DATA &= ~BIT7;
              }else{
                while(!packetSemaphore);
                SERIAL_DATA |= BIT7;
              }
              //Disable timer interrupt
              TA1CCTL0 &= ~CCIE; 
              dataByte = dataByte << 1;
              //Reset semaphore
              packetSemaphore = ISR_IDLE;
              //Clear interrupt flag
              TA1CCTL0 &= ~CCIFG; 
            }
          }
          //Disable timer interrupt
          TA1CCTL0 &= ~CCIE;  
          //Reset timer
          TA1CCR0 = 0;   
          //Clear timer interrupt flag
          TA1CCTL0 &= ~CCIFG; 
          
          //Enter IDLE mode
          trxSpiCmdStrobe(CC112X_SIDLE);
          __delay_cycles(250000);
          packetCounter++;
          updateLcd();
          
        } while(!bspKeyPushed(BSP_KEY_ALL));
      }
    }   
}
  
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A5_ISR(void) { 
  packetSemaphore = ISR_ACTION_REQUIRED;  
}

/******************************************************************************
* @fn         initTX
*
* @brief       Set pin directions, write transparent mode registers and setup timers.
*                
* @param       none
*
* @return      none
*/
static void initTX(void) {
    uint8 writeByte;
    //Set direction on P1.1
    P1DIR |= 0x82;
    P1DS |= 0x82;
    //FIFO_EN = 0    
    writeByte = 0x06;
    cc112xSpiWriteReg(CC112X_MDMCFG1, &writeByte, 1);
    //Transparent mode enable + 4x interpolation factor
    writeByte = 0x65; 
    cc112xSpiWriteReg(CC112X_MDMCFG0, &writeByte, 1);
    // Transparent mode
    writeByte = 0x07;
    cc112xSpiWriteReg(CC112X_PKT_CFG2, &writeByte, 1);
    // Transparent mode TX
    writeByte = 0x08;
    cc112xSpiWriteReg(CC112X_SERIAL_STATUS, &writeByte, 1);

    //Set up timer and calculate delay
    TA1CTL = TASSEL_2 + MC_1; //SMCLK + Up mode
    bitRate = getBitRate();
    delay = (int)roundf((getSystemClock()*1000000)/bitRate);
    updateLcd();
}

/******************************************************************************
* @fn          createPacket
*
* @brief       This function is called before a packet is transmitted. It fills
*              the txBuffer with a packet consisting of a 4 byte preamble, 
*              4 byte sync word, length byte, two bytes packet counter and
*              n random bytes. If fixed packet length is used, the length byte 
*              is excluded.
*                
* @param       none
*
* @return      none
*/
static void createPacket(uint8 txBuffer[]) {
    uint8 i;
    uint8 index;
    uint8 test;
  
    //Create preamble
    for(i=0; i<PREAMBLE_LENGTH; i++) {
      txBuffer[i] = 0xAA;
    }
    
    //Insert syncword from registers
    cc112xSpiReadReg(CC112X_SYNC3, &test, 1);
    cc112xSpiReadReg(CC112X_SYNC3, &txBuffer[PREAMBLE_LENGTH], 1);
    cc112xSpiReadReg(CC112X_SYNC2, &txBuffer[PREAMBLE_LENGTH+1], 1);
    cc112xSpiReadReg(CC112X_SYNC1, &txBuffer[PREAMBLE_LENGTH+2], 1);
    cc112xSpiReadReg(CC112X_SYNC0, &txBuffer[PREAMBLE_LENGTH+3], 1);
    
    //If fixed packet length insert the packet number
    if(FIXED_LENGTH) {
      txBuffer[PREAMBLE_LENGTH+4] = (uint8) packetCounter >> 8; // MSB of packetCounter
      txBuffer[PREAMBLE_LENGTH+5] = (uint8) packetCounter;      // LSB of packetCounter
      index = PREAMBLE_LENGTH+6;
    } else {
      txBuffer[PREAMBLE_LENGTH+4] = (uint8) (PKTLEN - 1);               // Length byte
      txBuffer[PREAMBLE_LENGTH+5] = (uint8) (packetCounter >> 8); // MSB of packetCounter
      txBuffer[PREAMBLE_LENGTH+6] = (uint8) packetCounter;      // LSB of packetCounter
      index = PREAMBLE_LENGTH+7;
    }
    
    //Fill rest of buffer with random bytes
    for(i=index; i< (PKTLEN+PREAMBLE_LENGTH+5); i++) {
      txBuffer[i] = (uint8)rand();
    }
}
  
/*******************************************************************************
* @fn          whitenData
*
* @brief       Whitens the txBuffer
*
* @param       none
*
* @return      none
*/
static void whitenData(uint8 *txBuffer) {
  uint8 i, j;
  static uint16 whitenoiseNew = 0;
  static uint16 x1, x2;
  static uint16 xor = 0;
  uint16 whitenoise = 0x01FF;
  //Sets the new value to the one calculated last time the function was run.
  //This is to ensure that the original value gets returned the first time.
  for(i = PREAMBLE_LENGTH+4; i < BUFFER_SIZE; i++) {  
    whitenoiseNew = whitenoise;
    for(j=0; j<8; j++) {
      x1 = whitenoise &0x01;
      x2 = (whitenoise &0x20)>>5;
      xor = (x1 ^ x2)<<8;
      whitenoise = whitenoise>>1;
      if(xor&0x100)
        whitenoise |= 0x100;
      else
        whitenoise &= ~0x100;
    }
    txBuffer[i] ^= whitenoiseNew;
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
static uint16 calcCRC(uint8 crcData, uint16 crcReg) {
  uint8 i;
  for (i = 0; i < 8; i++) {
    if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
      crcReg = (crcReg << 1) ^ CRC16_POLY;
    else
      crcReg = (crcReg << 1);
    crcData <<= 1;
  }
  return crcReg;
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
static void registerConfig(void) {
  
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
static void updateLcd(void) {
  //Update LDC buffer.
  lcdBufferClear(0);
  lcdBufferPrintString(0, "  Serial Mode Test  ", 0, eLcdPage0);
  lcdBufferSetHLine(0, 0, LCD_COLS-1, eLcdPage7); 
  lcdBufferPrintString(0, "Packets sent:", 0, eLcdPage2);
  lcdBufferPrintInt(0, packetCounter, 70, eLcdPage3);
  lcdBufferPrintString(0, "Bitrate:", 0, eLcdPage5);
  lcdBufferPrintInt(0, bitRate, 70, eLcdPage5);
  lcdBufferPrintString(0, "   Transparent TX     ", 0, eLcdPage7);
  lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
  lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
  lcdSendBuffer(0);
}

/******************************************************************************
* @fn          getBitRate
*
*@brief        Returns bitrate based on current register settings.
*
*
* @param       none
*
*
* @return      bitrate - the current bitrate
*/
static long getBitRate(void) {
  uint8 readByte2;
  uint8 readByte1;
  uint8 readByte0;
  uint32 drateM;
  uint32 drateE;
  float bitratetemp;
  uint32 bitrate;
  //Read datarate from registers
  cc112xSpiReadReg(CC112X_SYMBOL_RATE2, &readByte2, 1);
  cc112xSpiReadReg(CC112X_SYMBOL_RATE1, &readByte1, 1);
  cc112xSpiReadReg(CC112X_SYMBOL_RATE0, &readByte0, 1);
  drateE = (readByte2&0xF0)>>4;
  drateM = readByte2&0x0F;
  drateM = drateM<<16;
  drateM |= (uint16)readByte1<<8;
  drateM |= readByte0;
  bitratetemp = ((pow(2,20)+drateM)*pow(2,drateE))/(pow(2,39))*32000000;
  bitrate = (uint32)roundf(bitratetemp);
  return bitrate;
}

/******************************************************************************
* @fn          getSystemClock
*
*@brief        Returns current clock speed.
*
*
* @param       none
*
*
* @return      clk - current clock speed
*/
static uint8 getSystemClock(void) {
  int clk;
  switch (bspSysClockSpeedGet()) {
  case BSP_SYS_CLK_1MHZ:
    clk = 1;
    break;
  case BSP_SYS_CLK_4MHZ:
    clk = 4;
    break;
  case BSP_SYS_CLK_8MHZ:
    clk = 8;
    break;
  case BSP_SYS_CLK_12MHZ:
    clk = 12;
    break;
  case BSP_SYS_CLK_16MHZ:
    clk = 16;
    break;
  case BSP_SYS_CLK_20MHZ:
    clk = 20;
    break;
  case BSP_SYS_CLK_25MHZ:
    clk = 25;
    break;
  default:
    clk = 25;
    break;
  }
  return clk;
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