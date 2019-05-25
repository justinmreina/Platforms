//******************************************************************************
//! @file       cc112x_easy_link_rx_sniff_mode.c
//! @brief      This program sets up an easy link between two trxEB's with
//!             CC112x EM's connected.
//!             The program can take any recomended register settings exported
//!             from SmartRF Studio 7 without any modification with exeption
//!             from the assumtions decribed below.
//
//              Notes: The following asumptions must be fulfilled for
//              the program to work:
//
//                  1.  GPIO2 has to be set up with GPIO2_CFG = 0x06
//                      PKT_SYNC_RXTX for correct interupt
//                  2.  Packet engine has to be set up with status bytes enabled
//                      PKT_CFG1.APPEND_STATUS = 1
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
#include "cc112x_easy_link_reg_config.h"
#include "math.h"
#include "string.h"

/*******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0
#define RX_FIFO_ERROR       0x11

#define GPIO3               0x04
#define GPIO2               0x08
#define GPIO0               0x80

/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    uint16  addr;
    uint8   data;
    uint8   keepMask;
} customRegisterSetting_t, calculatedRegisterSetting_t;

/*******************************************************************************
* DEFINES
*/
#define ISR_ACTION_REQUIRED 1
#define ISR_IDLE            0
#define RX_FIFO_ERROR       0x11

#define GPIO3               0x04
#define GPIO2               0x08
#define GPIO0               0x80

/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter = 0;
static uint8 REG_WOR_EVENT0_MSB;
static uint8 REG_WOR_EVENT0_LSB;
static uint8 REG_WOR_CFG1;
static uint8 REG_WOR_RES;
static char warningMessage[32];


/*******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void runRX(void);
static void radioRxISR(void);
static void updateLcd(void);
static long getBitRate(void);
static uint8 getPreambleLength(void);
static void customRegisterConfig(customRegisterSetting_t* customRegisterSettings,
                                 uint8 length);
static void manualCalibration(void);
static void calibrateRCOsc(void);
static void generateRegisters(void);

/*******************************************************************************
*   @fn         main
*
*   @brief      Runs the main routine
*
*   @param      none
*
*   @return     none
*/
void main(void) {

    // Initialize MCU and peripherals
    initMCU();

    // Write radio registers
    registerConfig();
    
    /***************************Sniff Mode changes******************************
    The following array contains the changes needed to the settings exported
    from SmartRF Studio to run the Smart RX Sniff Mode receiver.
   
    Note on Keep Bits:
        If the Keep Bits are set to all zero then the Data will be written
        to the specific Register, overwriting any existing register value.
        If the Keep Bits are non-zero then that Register will be first be
        read and a logical AND operation will be performed with the Keep
        Bits. The result of this operation will then be logically OR:ed with
        the Data. This means that all bits in the Keep Bits field will be
        preserved, making it possible to keep specific SmartRF exported
        Register fields and flags intact.

    ===========================================================================
    |   Register                  | Data                          | Keep Bits |
    ==========================================================================*/
    customRegisterSetting_t customRegisterSettings[] = {
      {CC112X_IOCFG2                    ,0x13                           ,0x00},
      {CC112X_IOCFG0                    ,0x06                           ,0x00}, 
      {CC112X_PREAMBLE_CFG0             ,0x2A                           ,0x00},
      {CC112X_AGC_CFG3                  ,0x11                           ,0x00}, 
      {CC112X_AGC_CFG1                  ,0xA0                           ,0x00},
      {CC112X_AGC_CFG0                  ,0x90                           ,0x00},   
      {CC112X_SETTLING_CFG              ,0x03                           ,0x00},   
      {CC112X_RFEND_CFG1                ,0x0F                           ,0xC0},
      {CC112X_RFEND_CFG0                ,0x09                           ,0x00},
      {CC112X_PKT_LEN                   ,0x7D                           ,0x00},
      {CC112X_AGC_CS_THR                ,0xF7                           ,0x00},
      {CC112X_FIFO_CFG                  ,0x80                           ,0x00},
      {CC112X_PKT_CFG1                  ,0x05                           ,0xF3},
      {CC112X_PKT_CFG0                  ,0x20                           ,0x9F},
      {CC112X_MDMCFG1                   ,0x00                           ,0x7F},
      {CC112X_SYNC_CFG0                 ,0x17                           ,0xF7},
      {CC112X_WOR_CFG0                  ,0x20                           ,0x00},
    };
    /*========================================================================*/
    
    //Write custom register settings to radio
    customRegisterConfig(customRegisterSettings,
                        (sizeof(customRegisterSettings) /
                         sizeof(customRegisterSetting_t)));
    
    //Generates Event0 and WOR_CFG0 automatically. 
    //Normally this should be extracted from SmartRF Studio and hard coded.
    generateRegisters();
    
    //Put the generated registers in an array before writing the registers.
    //The settings that are variable are calculated for modularity purposes in 
    //this example. Normally they should be extracted from SmartRF Studio and 
    //hard coded.
    /*==========================================================================
    |   Register                  | Data                          | Keep Bits |
    ==========================================================================*/
    calculatedRegisterSetting_t calculatedRegisterSettings[] = {
      {CC112X_WOR_CFG1                  ,REG_WOR_CFG1                   ,0x00},
      {CC112X_WOR_EVENT0_MSB            ,REG_WOR_EVENT0_MSB             ,0x00},
      {CC112X_WOR_EVENT0_LSB            ,REG_WOR_EVENT0_LSB             ,0x00},
    };
    /*========================================================================*/
    //Write the calculated register settings
    customRegisterConfig(calculatedRegisterSettings,
                        (sizeof(calculatedRegisterSettings) /
                         sizeof(calculatedRegisterSetting_t)));
    
    /**********************End of changes for Sniff Mode***********************/
 
    
    //Enter runRX, never coming back
    runRX();
}

/*******************************************************************************
*   @fn         runRX
*
*   @brief      Puts radio in RX and waits for packets. Function assumes
*               that status bytes are appended in the RX_FIFO
*               Update packet counter and display for each packet received.
*
*   @param      none
*
*   @return     none
*/
static void runRX(void) {

    uint8 rxBuffer[128] = {0};
    uint8 rxBytes;
    uint8 marcState;

    // Connect ISR function to GPIO2
    ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &radioRxISR);

    // Interrupt on rising edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_RISING_EDGE);

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO2);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

    // Update LCD
    updateLcd();
    
    // Calibrate radio
    trxSpiCmdStrobe(CC112X_SCAL);
    
    manualCalibration();

    // Wait for calibration to be done (radio back in IDLE state)
    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
    } while (marcState != 0x41);

    // Calibrate the RCOSC
    calibrateRCOsc();

    // Infinite loop
    while(TRUE) {
      
        // Set radio in RX
        trxSpiCmdStrobe(CC112X_SWOR);

        // Wait for packet to be received
        while(packetSemaphore != ISR_ACTION_REQUIRED);

        // Reset packet semaphore
        packetSemaphore = ISR_IDLE;

        // Read number of bytes in RX FIFO
        cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);

            // Check that we have bytes in FIFO
            if(rxBytes != 0) {

                // Read MARCSTATE to check for RX FIFO error
                cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);

                // Mask out MARCSTATE bits and check if we have a RX FIFO error
                if((marcState & 0x1F) == RX_FIFO_ERROR) {

                    // Flush RX FIFO
                    trxSpiCmdStrobe(CC112X_SFRX);
                } else {

                    // Read n bytes from RX FIFO
                    cc112xSpiReadRxFifo(rxBuffer, rxBytes);

                    // Check CRC ok (CRC_OK: bit7 in second status byte)
                    // This assumes status bytes are appended in RX_FIFO
                    // (PKT_CFG1.APPEND_STATUS = 1)
                    // If CRC is disabled the CRC_OK field will read 1
                    if(rxBuffer[rxBytes - 1] & 0x80) {
                        // Update packet counter
                        packetCounter++;
                    }
                }
            }
            
            // Update LCD
            updateLcd();
    }
}


/*******************************************************************************
*   @fn         radioRxISR
*
*   @brief      ISR for packet handling in RX. Sets packet semaphore
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


/*******************************************************************************
*   @fn         initMCU
*
*   @brief      Initialize MCU and board peripherals
*
*   @param      none
*
*   @return     none
*/
static void initMCU(void) {

    // Init clocks and I/O
    bspInit(BSP_SYS_CLK_8MHZ);

    // Init LEDs
    bspLedInit();

    // Init buttons
    bspKeyInit(BSP_KEY_MODE_POLL);

    // Initialize SPI interface to LCD (shared with SPI flash)
    bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);

    // Init LCD
    lcdInit();

    // Instantiate transceiver RF SPI interface to SCLK ~ 4 MHz
    // Input parameter is clockDivider
    // SCLK frequency = SMCLK/clockDivider
    trxRfSpiInterfaceInit(2);

    // Enable global interrupt
    _BIS_SR(GIE);
}


/*******************************************************************************
*   @fn         registerConfig
*
*   @brief      Write register settings as given by SmartRF Studio found in
*               cc112x_easy_link_reg_config.h
*
*   @param      none
*
*   @return     none
*/
static void registerConfig(void) {

    uint8 writeByte;

    // Reset radio
    trxSpiCmdStrobe(CC112X_SRES);

    // Write registers to radio
    for(uint16 i = 0;
        i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {
        writeByte = preferredSettings[i].data;
        cc112xSpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
    }
}

/*******************************************************************************
* @fn           customRegisterConfig
*
* @brief        Write register settings to run the SmartPreamble transmitter
*
* @param        none
*
* @return       none
*/
static void customRegisterConfig(customRegisterSetting_t* settings,
                                 uint8 length) {
    uint8 readByte;
    uint8 writeByte;

    for(uint16 i = 0; i < length; i++) {

        // Respect the Keep Mask
        if( settings[i].keepMask != 0 ) {
            cc112xSpiReadReg(settings[i].addr, &readByte, 1);
            writeByte = (readByte & settings[i].keepMask) | settings[i].data;
        } else {
            writeByte = settings[i].data;
        }
        cc112xSpiWriteReg(settings[i].addr, &writeByte, 1);
    }
}

/*******************************************************************************
*   @fn         updateLcd
*
*   @brief      updates LCD buffer and sends buffer to LCD module
*
*   @param      none
*
*   @return     none
*/
static void updateLcd(void) {

    // Update LDC buffer and send to screen.
    lcdBufferClear(0);
    lcdBufferPrintString(0, "Easy Link Sniff Mode", 0, eLcdPage0);
    lcdBufferSetHLine(0, 0, LCD_COLS-1, 7);
    lcdBufferPrintString(0, "Received ok:", 0, eLcdPage3);
    lcdBufferPrintInt(0, packetCounter, 70, eLcdPage4);
    lcdBufferPrintString(0, "Packet RX ", 0, eLcdPage7);
    lcdBufferSetHLine(0, 0, LCD_COLS-1, 55);
    lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
    lcdSendBuffer(0);
}

/*******************************************************************************
*   @fn         generateRegisters
*
*   @brief      Generates EVENT0 and WOR_CFG1
*
*   @return     none
*/
static void generateRegisters(void) {
  
    //Generate Event0
    uint32 EVENT0;
    double tmp;
    float preambleLength;
    preambleLength = getPreambleLength();
    uint32 bitRate;
    bitRate = getBitRate();
    
    
    //Calculate length of preamble bytes
    tmp =  ( (preambleLength * 8) - 4) / bitRate;

    // Muliplying with 100/100.1 to account for RC oscillator tolerance (0.1%)
    tmp = (100/100.1)*tmp;

    // Multiply with the RC oscillator frequency to get floating point EVENT0
    tmp *= 32000;

    // Round down to be sure to get the EVENT0 integer value
    EVENT0 = (uint16)floor(tmp);

    // Separate to MSB and LSB for easy EVENT0 register updates
    REG_WOR_EVENT0_MSB = (EVENT0 >> 8);
    REG_WOR_EVENT0_LSB = (EVENT0 & 0x00FF);
    
    //Generate WOR_CFG1
    //Settings in WOR_CFG1 that don't need to be calculated:
    //WOR_MODE(3-5) = 1
    //EVENT1(0-2) = 0
    REG_WOR_CFG1 = 0x08;
    
    //Calculate WOR_RES
    if (EVENT0 < 1638) { 
      REG_WOR_RES = 0;
    } else if (EVENT0 < 52429) { 
      REG_WOR_RES = 1;
    } else if (EVENT0 < 1677722) { 
      REG_WOR_RES = 2;
    } else if (EVENT0 < 53687091) { 
      REG_WOR_RES = 3;
    } else {
      REG_WOR_RES = 0;
    }
    
    REG_WOR_CFG1 |= (0xC&REG_WOR_RES);
    
    //Generate warning message if preamble is to short for Event0 minimum sleep time.
    if((preambleLength < 32 && bitRate >= 500000)      ||
       (preambleLength < 8 && bitRate >= 100000)       ||
         (preambleLength < 5 && bitRate >= 50000)      ||
           (preambleLength < 4 && bitRate >= 38400)) {   
             strcpy(warningMessage, "Preamble length too short!");
           }
}

/*******************************************************************************
*   @fn         calibrateRcOsc
*
*   @brief      Calibrates the RC oscillator used for the eWOR timer. When this
*               function is called, WOR_CFG0.RC_PD must be 0
*
*   @param      none
*
*   @return     none
*/
static void calibrateRCOsc(void) {

    uint8 temp;

    // Read current register value
    cc112xSpiReadReg(CC112X_WOR_CFG0, &temp,1);

    // Mask register bit fields and write new values
    temp = (temp & 0xF9) | (0x02 << 1);

    // Write new register value
    cc112xSpiWriteReg(CC112X_WOR_CFG0, &temp,1);

    // Strobe IDLE to calibrate the RCOSC
    trxSpiCmdStrobe(CC112X_SIDLE);

    // Disable RC calibration
    temp = (temp & 0xF9) | (0x00 << 1);
    cc112xSpiWriteReg(CC112X_WOR_CFG0, &temp, 1);
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
  bitratetemp = ((pow(2,20)+drateM)*pow(2,drateE))/(pow(2,39))*40000000;
  bitrate = (uint32)roundf(bitratetemp);
  return bitrate;
}

/******************************************************************************
* @fn          getPreambleLength
*
*@brief        Returns bitrate based on current register settings.
*
*
* @param       none
*
*
* @return      bitrate - the current bitrate
*/
static uint8 getPreambleLength(void) {
  uint8 preambleLength;
  uint8 readByte;
  // Calculate preamble time
  cc112xSpiReadReg(CC112X_PREAMBLE_CFG1, &readByte, 1);
  preambleLength = readByte;
  preambleLength >>= 2;
  if(preambleLength == 0) preambleLength = 0;
  else if(preambleLength == 1) preambleLength = 0;
  else if(preambleLength == 2) preambleLength = 1;
  else if(preambleLength == 3) preambleLength = 1;
  else if(preambleLength == 4) preambleLength = 2;
  else if(preambleLength == 5) preambleLength = 3;
  else if(preambleLength == 6) preambleLength = 4;
  else if(preambleLength == 7) preambleLength = 5;
  else if(preambleLength == 8) preambleLength = 6;
  else if(preambleLength == 9) preambleLength = 7;
  else if(preambleLength == 10) preambleLength = 8;
  else if(preambleLength == 11) preambleLength = 12;
  else if(preambleLength == 12) preambleLength = 24;
  else if(preambleLength == 13) preambleLength = 30;
  else if(preambleLength == 14) preambleLength = 0;
  else if(preambleLength == 15) preambleLength = 0;
  else preambleLength = 0;
  
return preambleLength;
}

/*******************************************************************************
*   @fn         manualCalibration
*
*   @brief      Calibrates radio according to CC112x errata
*
*   @param      none
*
*   @return     none
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

    // 3) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with
    //    high VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

    // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
    writeByte = 0x00;
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

    // 6) Continue with mid VCDAC (original VCDAC_START):
    writeByte = original_fs_cal2;
    cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

    // 7) Calibrate and wait for calibration to be done
    //   (radio back in IDLE state)
    trxSpiCmdStrobe(CC112X_SCAL);

    do {
        cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
    } while (marcstate != 0x41);

    // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained
    //    with mid VCDAC_START value
    cc112xSpiReadReg(CC112X_FS_VCO2,
                     &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4,
                     &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
    cc112xSpiReadReg(CC112X_FS_CHP,
                     &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

    // 9) Write back highest FS_VCO2 and corresponding FS_VCO
    //    and FS_CHP result
    if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
        calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
        writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    } else {
        writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
        cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
        writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
        cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
    }
}
