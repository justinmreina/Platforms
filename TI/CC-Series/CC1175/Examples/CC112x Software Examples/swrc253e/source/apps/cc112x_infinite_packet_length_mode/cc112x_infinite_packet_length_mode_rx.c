//******************************************************************************
//! @file       cc112x_infinite_packet_length_mode_rx.c
//! @brief      This examples shows how on can transmit packets with length
//              byte greater than 255 and and less than/or equal to
//              PACKET_LENGTH. The example runs with all typical settings
//              from SmartRF Studio.
//              The packets are on the following format:
//              ------------------------------------------------------------
//              preamble | sync | 2 length bytes | payload | 2 bytes CRC |
//              ------------------------------------------------------------
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
//#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"
#include "cc112x_infinite_packet_length_mode_reg_config.h"


/*******************************************************************************
* DEFINES
*/
#define PACKET_LENGTH               500     // Max packet length excepted
#define CRC_OK                      0x80
#define BYTES_IN_RX_FIFO            121     // # of bytes one can read from the
                                            // RX_FIFO when a rising edge occur
                                            // on IOCFGx = 0x00 and
                                            // FIFO_THR = 120
#define INFINITE                    0
#define FIXED                       1

#define MAX_VARIABLE_LENGTH         255
#define INFINITE_PACKET_LENGTH_MODE 0x40
#define FIXED_PACKET_LENGTH_MODE    0x00
#define RX_START                    0
#define RX_WAIT                     1

#define GPIO3                       0x04
#define GPIO2                       0x08
#define GPIO0                       0x80


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8 rxBuffer[PACKET_LENGTH + 4];   // Buffer used to hold the received
                                            // packet
static uint8 packetReceived = FALSE;        // Flag set when packet is received
static uint16 packetCounter = 0;            // Counter keeping track of packets
                                            // with CRC OK
static uint16 packetLength;                 // Two first bytes received after
                                            // the sync word
static uint32 bytesLeft;                    // Keeping track of bytes left to
                                            // read from the RX FIFO
static uint8 fixedPacketLength;
static uint8 *pBufferIndex;                 // Pointer to current position in
                                            // the rxBuffer
static uint8 pktFormat = INFINITE;
static uint8 state = RX_START;


/*******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
static void updateLcd(void);
static void syncReceivedISR(void);
static void packetReceivedISR(void);
static void rxFifoAboveThresholdISR(void);
static void printWelcomeMessage(void);


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

    uint8 writeByte;

    // Initialize MCU and peripherals
    initMCU();

    // Write radio registers (preferred settings from SmartRF Studio)
    registerConfig();

    // Application specific registers
    // FIFO_THR = 120
    // GPIO0 = RXFIFO_THR
    // GPIO2 = PKT_SYNC_RXTX
    // GPIO3 = PKT_SYNC_RXTX
    writeByte = INFINITE_PACKET_LENGTH_MODE;
    cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
    writeByte = 0x78; cc112xSpiWriteReg(CC112X_FIFO_CFG, &writeByte, 1);
    writeByte = 0x00; cc112xSpiWriteReg(CC112X_IOCFG0,   &writeByte, 1);
    writeByte = 0x06; cc112xSpiWriteReg(CC112X_IOCFG2,   &writeByte, 1);
    writeByte = 0x06; cc112xSpiWriteReg(CC112X_IOCFG3,   &writeByte, 1);

    // Calibrate the radio according to the errata note
    manualCalibration();

    // Connect ISR function to GPIO0
    ioPinIntRegister(IO_PIN_PORT_1, GPIO0, &rxFifoAboveThresholdISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO0, IO_PIN_RISING_EDGE);

    // Clear interrupt
    ioPinIntClear(IO_PIN_PORT_1, GPIO0);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO0);

    // Connect ISR function to GPIO2
    ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &syncReceivedISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_RISING_EDGE);

    // Clear interrupt
    ioPinIntClear(IO_PIN_PORT_1, GPIO2);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

    // Set up interrupt on GPIO3 (PKT_SYNC_RXTX)
    ioPinIntRegister(IO_PIN_PORT_1, GPIO3, &packetReceivedISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO3, IO_PIN_FALLING_EDGE);

    printWelcomeMessage();

    while (TRUE) {
        switch (state) {

            //------------------------------------------------------------------
            case RX_START:
            //------------------------------------------------------------------
            trxSpiCmdStrobe(CC112X_SRX);
            pBufferIndex = rxBuffer;

            // Disable interrupt on GPIO3
            ioPinIntDisable(IO_PIN_PORT_1, GPIO3);

            state = RX_WAIT;

            //------------------------------------------------------------------
            case RX_WAIT:
            //------------------------------------------------------------------
            if (packetReceived) {
                packetReceived = FALSE;

                // Check CRC and update LCD if CRC OK
                if ((rxBuffer[packetLength + 3]) & CRC_OK)
                    updateLcd();

                // Change to infinite packet length mode
                pktFormat = INFINITE;
                writeByte = INFINITE_PACKET_LENGTH_MODE;
                cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);

                state = RX_START;
            }
            break;

            //------------------------------------------------------------------
            default:
            //------------------------------------------------------------------

            break;
        }
    }
}


/*******************************************************************************
*   @fn             printWelcomeMessage
*
*   @brief          Function displaying a welkome message at start-up
*
*
*   @param          none
*
*   @return         none
*/
static void printWelcomeMessage(void) {
    lcdBufferClear(0);
    lcdBufferPrintString(0, "Inf. Pkt. Length Mode", 0, eLcdPage0);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, eLcdPage7);
    lcdBufferPrintString(0, "Waiting to receive", 0, eLcdPage2);
    lcdBufferPrintString(0, "packets", 0, eLcdPage3);
    lcdBufferPrintString(0, "Packet RX", 0, eLcdPage7);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
    lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
    lcdSendBuffer(0);
}


/*******************************************************************************
*   @fn         syncReceivedISR
*
*   @brief      Function running every time a sync word has been received
*
*   @param      none
*
*   @return     none
*/
static void syncReceivedISR(void) {

    uint8 numRxBytes;
    uint8 writeByte;

    // After the sync word is received one needs to wait for the two
    // length bytes to be put in the RX FIFO
    do {
        cc112xSpiReadReg(CC112X_NUM_RXBYTES, &numRxBytes, 1);
    } while (numRxBytes < 2);

    // Read the length byte and store it in the packetLength variable
    cc112xSpiReadRxFifo(rxBuffer, 2);
    pBufferIndex += 2;
    packetLength =  (uint16)(((uint16)(rxBuffer[0] << 8)) | rxBuffer[1]);

    // Make sure that the packet length is in the correct range, update
    // variables and enable interrupt on falling edge of GPIO3 (PKT_SYNC_RXTX)
    if ((packetLength > MAX_VARIABLE_LENGTH) 
        && (packetLength <= PACKET_LENGTH)) {
        bytesLeft = packetLength + 2;
        fixedPacketLength = bytesLeft  % (MAX_VARIABLE_LENGTH + 1);
        writeByte = fixedPacketLength;
        cc112xSpiWriteReg(CC112X_PKT_LEN, &writeByte, 1);

        // Clear interrupt flag and enable GPIO3
        ioPinIntClear(IO_PIN_PORT_1, GPIO3);
        ioPinIntEnable(IO_PIN_PORT_1, GPIO3);

    } else { // Exit RX and flush RX FIFO due to length byte being out of range
        trxSpiCmdStrobe(CC112X_SIDLE);
        trxSpiCmdStrobe(CC112X_SFRX);
        state = RX_START;
    }
  
    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO2);
}


/*******************************************************************************
*   @fn         packetReceivedISR
*
*   @brief      Function running every time a packet has been received
*
*   @param      none
*
*   @return     none
*/
static void packetReceivedISR(void) {
    cc112xSpiReadRxFifo(pBufferIndex, bytesLeft);
    
    packetReceived = TRUE;

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO3);
}


/*******************************************************************************
*   @fn         rxFifoAboveThresholdISR
*
*   @brief      Function running every time the RX FIFO is filled above
*               threshold (FIFO_THR = 120)
*
*   @param      none
*
*   @return     none
*/
static void rxFifoAboveThresholdISR(void) {

    uint8 writeByte;
    
    if (!packetReceived) {

        // Change to fixed packet length mode when there is less than 256
        // bytes left to receive
        if (((bytesLeft - BYTES_IN_RX_FIFO) < (MAX_VARIABLE_LENGTH + 1)) && (pktFormat == INFINITE)) {
            writeByte = FIXED_PACKET_LENGTH_MODE;
            cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
            pktFormat = FIXED;
        }

        // Read BYTS_IN_RX_FIFO bytes from the RX FIFO and update the variables
        // keeping track of how many more bytes need to be read and where in
        // rxBuffer they should be storeds
        cc112xSpiReadRxFifo(pBufferIndex, BYTES_IN_RX_FIFO);
        bytesLeft -= BYTES_IN_RX_FIFO;
        pBufferIndex += BYTES_IN_RX_FIFO;

        // Clear ISR flag
        ioPinIntClear(IO_PIN_PORT_1, GPIO0);
    }
}


/*******************************************************************************
*   @fn         updateLcd
*
*   @brief      Updates the LCS every time a packet has been received
*               with CRC OK
*
*   @param      none
*
*   @return     none
*/
static void updateLcd(void) {
    lcdBufferClear(0);
    lcdBufferPrintString(0, "Inf. Pkt. Length Mode", 0, eLcdPage0);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, eLcdPage7);
    lcdBufferPrintString(0, "Received packets:", 0, eLcdPage3);
    lcdBufferPrintInt(0, (int32)(++packetCounter), 102, eLcdPage3);
    lcdBufferPrintString(0, "Packet RX", 0, eLcdPage7);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
    lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
    lcdSendBuffer(0);
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
*               cc112x_infinite_packet_length_mode_reg_config.h
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