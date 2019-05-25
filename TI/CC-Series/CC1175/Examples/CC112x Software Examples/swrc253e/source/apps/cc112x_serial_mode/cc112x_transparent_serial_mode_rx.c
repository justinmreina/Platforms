//*****************************************************************************
//! @file       cc112x_transparent_serial_mode_rx.c
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
#include "stdbool.h"

/******************************************************************************
* DEFINES
*/
#define CRC_ENABLE          TRUE
#define ISR_ACTION_REQUIRED TRUE
#define ISR_IDLE            FALSE
#define CRC_VALID           FALSE
#define WHITENING           TRUE
#define CRC_BYTES           2
#define PKTLEN              0 //Packet length. Zero for variable packet length                            
#define SERIAL_DATA         P1IN
#define BYTES_BEFORE_SYNC   4
#define GPIO0PIN            0x80        
#define GPIO2PIN            0x08
#define GPIO3PIN            0x04
#define CRC_INIT            0xFFFF
#define CRC16_POLY          0x8005
#define MSB                 0x80

//Enable interrupts
#define ENABLEGPIO3()     st( P1IE |= GPIO3PIN; )
#define ENABLEGPIO2()     st( P1IE |= GPIO2PIN; )

//Disable interrupts
#define DISABLEGPIO3()    st( P1IE &= ~GPIO3PIN; )
#define DISABLEGPIO2()    st( P1IE &= ~GPIO2PIN; )

//Timers
#define TIMER_INT_ENABLE()  st( TA0CCTL0 = CCIE; )
#define TIMER_INT_DISABLE() st( TA0CCTL0 &= ~CCIE; )
#define TIMER_SET(x)        st( TA0CCR0 = x; )
#define TIMER_RESET();      st( TA0CCR0 = 0; )
#define TIMER_CLR_FLAG();   st( TA0CCTL0 &= ~CCIFG; )

//Clear flags
#define CLRGPIO3()        st( P1IFG &= ~GPIO3PIN; )
#define CLRGPIO2()        st( P1IFG &= ~GPIO2PIN; )
#define CLRGPIO0()        st( P1IFG &= ~GPIO0PIN; )                

//Check interrupt flags
#define GPIO3FG (P1IFG &GPIO3PIN)
#define GPIO2FG (P1IFG &GPIO2PIN)
#define GPIO0FG (P1IFG &GPIO0PIN)

//Defines DATALINE and CS_VALID to the right pins
#define DATALINE (SERIAL_DATA &0x08)
#define CS_VALID (SERIAL_DATA &0x04)

/******************************************************************************
* GLOBAL VARIABLES
*/
//Delays
static uint16 preambledelay = 0;
static uint16 halfbittime = 0;
static uint16 bittime = 0;
static uint16 usdelay = 0;

//Packets
static uint16 erroneousPackets = 0;
static uint32 packetCounter = 0;
static uint16 syncReceived = 0;
uint8 rxBuffer[250] = {0};
uint8 bufferIndex;

//Changed by interrupt
static volatile uint8  packetSemaphore;
static volatile uint8 carrierSenseDetected;
static uint16 dataBit;

//RSSI
static int8 rssi_value = 0;

//Constants for sync word and the PN9 generator
static uint8 syncWord[4] = {0xDE, 0x51, 0x0B, 0x93};
static uint16 whitenoise = 0x01FF;

/******************************************************************************
* STATIC FUNCTIONS
*/
static void initRX(void);
static void initMCU(void);
static int dataEdge(void);
void delay_us(uint16 usec);
static void updateLcd(void);
static long getBitRate(void);
static void readPacket(void);
static int8 Read8BitRssi(void);
static uint16 whitening(void);
static uint8 syncSearch(void);
static void packetError(void);
static void registerConfig(void);
static void packetReceived(void);
static void transparentMode(void);
static uint8 getSystemClock(void);
static void runTransparentRX(void);
static void manualCalibration(void);
static void timingcalculations(void);
static uint16 calcCRC(uint8 *rxBuffer, uint16 crcReg);

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
  
  //Initialize MCU and peripherals
  initMCU();
  
  //Write radio registers
  registerConfig();
  
  //Calibrate radio according to errata
  manualCalibration();
  
  //Setup ports, timers, calculate delay and set the radio in RX mode.
  initRX();
  
  //Enter runTX, never coming back
  runTransparentRX();
}

/******************************************************************************
* @fn          runTransparentRX
*
* @brief       Puts radio in RX and waits for packets. Waits for interrupt from
*              carrier sense before searching for synch word. When correct 
*              sync word is detected, the function reads the packets and 
*              performs a CRC check. Updates packet counter and display for 
*              each packet received with correct CRC.
*
*              For each given data rate, the timer needs to be adjusted to
*              sample at correct data rate. The timer value for a complete bit 
*              period can be calculated by (clock frequency)/(data rate).
*              Some adjustments may be nessesary. Initially det timer should
*              be set up to half bit period, to sample in the middle of the
*              bits. The timer should be reset to complete bit period in each
*              timer interrupt. It might be nessesary to synchronize the timer
*              to avoid drifting and packet errors. This is implemented in the
*              interrupt function serialData(), wich is called every eighth 
*              byte. 
*                
* @param       none
*
* @return      none
*/
static void runTransparentRX() {
  //Reset packetSemaphore
  packetSemaphore = ISR_IDLE;
  
  //Start the RX loop
  while(1) {
    
    //Enable interrupt from GPIO_3
    ENABLEGPIO3();

    //Disable interrupt from GPIO_2
    DISABLEGPIO2();
    
    //If carrier sense is detected, search for sync word and receive packet
    if(carrierSenseDetected == ISR_ACTION_REQUIRED) {
      
      //Disable interrupt from GPIO_3
      DISABLEGPIO3();
      
      //Enable timerA interrupt
      TIMER_INT_ENABLE();
      
      //Delay a bit into the preamble.
      delay_us(preambledelay);
      
      //Wait for edge on the serial data
      while(!dataEdge());
      
      //Sample in the middle of each bit. 
      //Uses the calculated halfbittime and compensates for code delay.
      TIMER_SET(halfbittime-50);
      
      //Search for sync word. If the sync word is not found within the first
      //8 bytes, wait for new packet.
      if(syncSearch()) {
        
        rssi_value = Read8BitRssi();
        //Read packet
        readPacket();
        
        //Disable interrupt on data
        DISABLEGPIO2();
        
        //Disable timer interrupt
        TIMER_INT_DISABLE();
        
        //Reset timer
        TIMER_RESET();
        
        // Clear timer interrupt flag
        TIMER_CLR_FLAG();
        
        //Whiten the data if defined
        if(WHITENING) {
          for(int i = 1; i < bufferIndex; i++)
            rxBuffer[i] ^= whitening();
          //Revert the whitenoise variable to the original value
          whitenoise = 0x01FF;
        }
        
        //Calculate CRC if CRC_ENABLE is true
        if(CRC_ENABLE) {
          //The calculated checksum is 0 if correct      
          if(calcCRC(rxBuffer, CRC_INIT) == CRC_VALID) {
            packetReceived();
          } else {
              packetError();
          }
          
        } else { 
        //If crc is not enabled increase packetcounter and update LCD.
          packetReceived();
        }
      }
      //Carrier Sense reset
      carrierSenseDetected = ISR_IDLE;
    }
  }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
  if (GPIO3FG) { 
    // Set carrierSenseDetected
    carrierSenseDetected = ISR_ACTION_REQUIRED;  
    CLRGPIO3(); 
    DISABLEGPIO3();
  } else if (GPIO2FG) { 
    //Disable timer interrupt
    TIMER_INT_DISABLE();
    //Reset timer
    TIMER_RESET();
    //Clear timer interrupt flag
    TIMER_CLR_FLAG();
    //Set the timer again and compensate for this function
    TIMER_SET(halfbittime-80);
    //Enable timer interrupt
    TIMER_INT_ENABLE();
    //Disable interrupt and clear flag on GPIO_2  
    CLRGPIO2();
    DISABLEGPIO2();
  } else if (GPIO0FG) {
    CLRGPIO0();
  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
  dataBit = DATALINE;
  // Set timer to bit period
  TIMER_SET(bittime);
  //Set flag
  packetSemaphore = ISR_ACTION_REQUIRED;
}

/******************************************************************************
* @fn          dataEdge
*
*@brief        Detects an edge on the serial data
*
*
* @param       none
*
*
* @return      none
*/
static int dataEdge(void) {
  if(DATALINE)
    while(DATALINE);
  if(!DATALINE)
    while(!DATALINE);
  return 1;
}

/******************************************************************************
* @fn          readPacket
*
*@brief        Reads a packet when the length is known
*
*
* @param       none
*
*
* @return      none
*/
static void readPacket(void) {
  
  uint8 dataByte = 0;
  uint8 lengthByte = 0;
  
  //If the packet length is known, receive packet without reading the length byte
  if(PKTLEN) {
  bufferIndex = 0;
  for (int j = 0; j < (PKTLEN + CRC_BYTES); j++) {
    //Is carrier sense still valid?
    if(!CS_VALID) break;
    //Synchronize timer to get correct datarate
    if(!(bufferIndex%BYTES_BEFORE_SYNC)){
      //Clear interrupt flag on GPIO_2
      CLRGPIO2();
      //Enable interrupt from GPIO_2
      ENABLEGPIO2();
    }
    for (int i = 0; i < 8; i++) {                           
      while (!packetSemaphore);
      packetSemaphore = ISR_IDLE;  
      
      //Read one data bit
      dataByte = dataByte << 1;
      if(dataBit == 0)
        dataByte &= 0xFE;
      else
        dataByte |= 0x01;
    }
    rxBuffer[bufferIndex++] = dataByte;
  }
  //If packet length is unknown, read lengthbyte and receive packet
  } else {
    bufferIndex = 1;
    // Read length byte
    for (int i = 0; i < 8; i++) {
      //Is carrier sense still valid?
      if(!CS_VALID) break;
      while (!packetSemaphore);
      packetSemaphore = ISR_IDLE;
      //Read one length bit
      lengthByte = lengthByte << 1;
      if(dataBit == 0)
        lengthByte &= 0xFE;
      else
        lengthByte |= 0x01;
    }
    //Whiten the lengthbyte if WHITENING is defined
    if(WHITENING)
      lengthByte ^= whitening();
    // Read the rest of the payload + optionally the 2 CRC bytes
    for (int j = 0; j < (lengthByte + CRC_BYTES); j++) {
      //Synchronize timer to get correct datarate
      if(!(bufferIndex%BYTES_BEFORE_SYNC)) {
        //Clear interrupt flag on GPIO_2
        CLRGPIO2();
        //Enable interrupt from GPIO_2
        ENABLEGPIO2();
      }
      for (int i = 0; i < 8; i++) {
        while (!packetSemaphore);
        packetSemaphore = ISR_IDLE;
        
        //Read one data bit
        dataByte = dataByte << 1;
        if(dataBit == 0)
          dataByte &= 0xFE;
        else
          dataByte |= 0x01;
      }
      rxBuffer[bufferIndex++] = dataByte;
    }
    rxBuffer[0] = lengthByte;
  }
}

/******************************************************************************
* @fn          transparentMode
*
*@brief        Writes all the register settings necessary for transparent mode
*
*
* @param       none
*
*
* @return      none
*/
static void transparentMode(void) {
  uint8 writeByte;
  // Carrier sense on GPIO3
  writeByte = 0x11;
  cc112xSpiWriteReg(CC112X_IOCFG3, &writeByte, 1);
  // Serial data on GPIO2
  writeByte = 0x09;
  cc112xSpiWriteReg(CC112X_IOCFG2, &writeByte, 1);
  // FIFO_EN = 0;
  writeByte = 0x06;
  cc112xSpiWriteReg(CC112X_MDMCFG1, &writeByte, 1);
  // Transparent mode enable 
  writeByte = 0x65;
  cc112xSpiWriteReg(CC112X_MDMCFG0, &writeByte, 1);
  // Carrier sense threshold -90 dBm = 0x0C
  // Carrier sense threshold -123 dBm = 0xED
  // Carrier sense threshold -80 dBm = 0x16
  // Carrier sense threshold -70 dBm = 0x20
  writeByte = 0x20;
  
  cc112xSpiWriteReg(CC112X_AGC_CS_THR, &writeByte, 1);
  // No gain freeze, AGC_WIN_SIZE 8 samples and AGC_SETTLE_WAIT 24 samples
  writeByte = 0x00;
  cc112xSpiWriteReg(CC112X_AGC_CFG1, &writeByte, 1);
  // Transparent mode
  writeByte = 0x07;
  cc112xSpiWriteReg(CC112X_PKT_CFG2, &writeByte, 1);
}

/******************************************************************************
* @fn          initRX
*
*@brief        Setup pin directions, systemclock, transparent mode specific registers,
*              timer A, calculates delays and puts the radio in RX 
*
*
* @param       none
*
*
* @return      none
*/
static void initRX(void) {
  
  //Set direction on P1: Carrier sense on GPIO_3 and serial data on GPIO_2
  P1DIR &= ~0x0C;
  P1SEL &= ~0x0C;
  P1REN &= ~0x0C;  
  
  //Used for debugging
  P1DIR |= 0x13;
  P1DS |= 0x13;
  
  //Set transparent serial mode specific registers
  transparentMode();
  
  //Calculate timings and delays
  timingcalculations();
  
  // Update LCD
  updateLcd();  
  
  // Set up timer A
  TA0CTL = TASSEL_2 + MC_1;//SMCLK clock source + Up mode
  
  //Set radio in RX mode
  trxSpiCmdStrobe(CC112X_SRX);  
}

/******************************************************************************
* @fn          TimingCalculations
*
*@brief        Calculates all the timings; How long each bit is, based 
*              on bitrate and clockspeed and the usdelay.
*
* @param       none
*
*
* @return      bittime, halfbittime and usdelay
*/
static void timingcalculations(void) {
  bittime = (int) roundf((getSystemClock()*1000000)/getBitRate());
  halfbittime = (int) roundf(bittime/2);
  usdelay = (int)roundf((bittime/getSystemClock()) / 4);
  preambledelay = (usdelay*80)+100;
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
  uint32 drateM;
  uint32 drateE;
  uint8 readByte2;
  uint8 readByte1;
  uint8 readByte0;
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
* @fn          syncSearch
*
* @brief       This function search for a 4 bytes synch word.
*                
* @param       uint8 sync3, uint8 sync2, uint8 sync1, uint8 sync0
*              The sync word that is being searched for. 
*              sync3 is the most significant byte.
*
* @return      none
*/
static uint8 syncSearch(void) {    
  uint32 syncBitsReceived = 0;
  uint8 syncBytesReceived = 0;
  uint8 sync0Received, sync1Received, sync2Received, sync3Received;
  uint8 SyncNOK;
  
  do {
    //Is CS still valid?
    if(!CS_VALID) break;
    
    sync3Received = sync3Received << 1;
    if((sync2Received & MSB) == 0)
      sync3Received &= 0xFE;
    else
      sync3Received |= 0x01;
    
    sync2Received = sync2Received << 1;
    if((sync1Received & MSB) == 0)
      sync2Received &= 0xFE;
    else
      sync2Received |= 0x01;
    
    sync1Received = sync1Received << 1;
    if((sync0Received & MSB) == 0)
      sync1Received &= 0xFE;
    else
      sync1Received |= 0x01;
    
    while (!packetSemaphore);
    
    packetSemaphore = ISR_IDLE;
    P1OUT ^=BIT7;
    sync0Received = sync0Received << 1;
    if(dataBit == 0)
      sync0Received &= 0xFE;
    else
      sync0Received |= 0x01;
    if(syncBytesReceived < 4) {
      SyncNOK = 1;
    } else {
      SyncNOK = ((sync3Received != syncWord[3]) || 
                 (sync2Received != syncWord[2]) || 
                   (sync1Received != syncWord[1]) || 
                     (sync0Received != syncWord[0]));
    }
    //Count received bits
    syncBitsReceived++;
    if(!(syncBitsReceived%8)) {
      //Count received bytes
      syncBytesReceived++;
    }
    // If sync word is not found within the first 8 bytes, stop searching
    if(syncBytesReceived > 16) {
      TA0CCTL0 &= ~CCIE;  // Disable timer interrupt
      TA0CCR0 = 0;
      TA0CCTL0 &= ~CCIFG;
      return 0;
    }
  } while(SyncNOK && CS_VALID);
  if(!SyncNOK) {
    syncReceived++;
    return 1;
  }
  return 0;
}

/******************************************************************************
* @fn          packetReceived
*
* @brief       Reads RSSI, increase packet counter and updates LCD.
*
* @param       none
*
* @return      none
*/
static void packetReceived(void) {
  packetCounter++;
  updateLcd();
}

/******************************************************************************
* @fn          packetError
*
* @brief       Increase packet error counter and updates LCD.
*
* @param       none
*
* @return      none
*/
static void packetError(void) {
  erroneousPackets++;
  updateLcd();
}

/******************************************************************************
* @fn          calcCRC
*
* @brief       Calculates a checksum over the payload, included the seq. number
*              and length byte.
*
* @param       none
*
* @return      checksum
*/
static uint16 calcCRC(uint8 *rxBuffer, uint16 crcReg) {
uint8 crcData;

for(int i = 0; i < (bufferIndex); i++) {
  crcData = rxBuffer[i];
  for (int i = 0; i < 8; i++) {
    if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
      crcReg = (crcReg << 1) ^ CRC16_POLY;
    else
      crcReg = (crcReg << 1);
    crcData <<= 1;
  }
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

/*******************************************************************************
* @fn          whitening
*
* @brief       Whitens one byte each time it is run
*
* @param       none
*
* @return      none
*/
static uint16 whitening(void) {
  uint16 x1, x2;
  static uint16 xor = 0;
  uint16 whitenoiseNew;
  //Sets the new value to the one calculated last time the function was run.
  //This is to ensure that the original value gets returned the first time.
  whitenoiseNew = whitenoise;
  for(int i=0; i<8; i++) {
    x1 = whitenoise &0x01;
    x2 = (whitenoise &0x20)>>5;
    xor = (x1 ^ x2)<<8;
    whitenoise = whitenoise>>1;
    if(xor&0x100)
      whitenoise |= 0x100;
    else
      whitenoise &= ~0x100;
  }
  return whitenoiseNew;
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
  //Update LDC buffer and send to screen.
  lcdBufferClear(0);
  lcdBufferPrintString(0, "  Serial Mode Test  ", 0, eLcdPage0);
  lcdBufferSetHLine(0, 0, LCD_COLS-1, eLcdPage7); 
  lcdBufferPrintString(0, "Received ok:", 0, eLcdPage1);
  lcdBufferPrintInt(0, packetCounter, 80, eLcdPage1);
  lcdBufferPrintString(0, "Bitrate:", 0, eLcdPage2);
  lcdBufferPrintInt(0, getBitRate(), 80, eLcdPage2);
  lcdBufferPrintString(0, "RSSI:", 0, eLcdPage3);
  lcdBufferPrintInt(0, rssi_value, 80, eLcdPage3);
  lcdBufferPrintString(0, "Err. Packets:", 0, eLcdPage6);
  lcdBufferPrintInt(0, erroneousPackets, 80, eLcdPage6);
  lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
  lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
  lcdSendBuffer(0);
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

/***********************************************************************************
* @fn          Read8BitRssi
*
*@brief        Reads out the last RSSI value and converts it to dB.
*
*
* @param       none
*
*
* @return      rssiConverted - 8 bit rssi value
*/
int8 Read8BitRssi(void) {
  uint8 rssi2compl,rssiValid;
  int16 rssiConverted;
  
  cc112xSpiReadReg(CC112X_RSSI0, &rssiValid,1);
  if(rssiValid & 0x01)
  {
    /* Read RSSI from MSB register */
    cc112xSpiReadReg(CC112X_RSSI1, &rssi2compl,1);
    rssiConverted = (int16)((int8)rssi2compl) - 102;
    return rssiConverted;
  }
  /* keep last value since new value is not valid */
  return 0;
}

/***********************************************************************************
* @fn          delay_us
*
*@brief        Delays the specified number of microseconds
*
*
* @param       uint16 usec - number of microseconds delay
*
*
* @return      none
*/
#pragma optimize=none
void delay_us(uint16 usec) // 5 cycles for calling
{
  // The least we can wait is 3 usec:
  // ~1 usec for call, 1 for first compare and 1 for return
  //usec += 2;
  while(usec >= 3)       // 2 cycles for compare
  {                // 2 cycles for jump
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    NOP();       // 1 cycles for nop
    usec -= 2;        // 1 cycles for optimized decrement
  }
}                         // 4 cycles for returning