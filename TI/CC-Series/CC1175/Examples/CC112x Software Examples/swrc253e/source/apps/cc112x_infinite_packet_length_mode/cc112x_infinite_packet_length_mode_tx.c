//******************************************************************************
//! @file       cc112x_infinite_packet_length_mode_tx.c
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
#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"
#include "cc112x_infinite_packet_length_mode_reg_config.h"


/*******************************************************************************
* DEFINES
*/
#define PACKET_LENGTH               500     // Must be a number between
//                                          // 256 and 2^16
#define FIFO_SIZE                   128
#define CRC_OK                      0x80
#define AVAILABLE_BYTES_IN_TX_FIFO  122     // # of bytes one can write to the
                                            // TX_FIFO when a falling edge occur
                                            // on IOCFGx = 0x02 and
                                            // FIFO_THR = 120
#define BYTES_IN_TX_FIFO            (FIFO_SIZE - AVAILABLE_BYTES_IN_TX_FIFO)
#define INFINITE                    0
#define FIXED                       1
#define MAX_VARIABLE_LENGTH         255
#define INFINITE_PACKET_LENGTH_MODE 0x40
#define FIXED_PACKET_LENGTH_MODE    0x00

#define GPIO3                       0x04
#define GPIO2                       0x08
#define GPIO0                       0x80


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8 txBuffer[PACKET_LENGTH + 2];   // Buffer used to hold the packet
                                            // to be sent
static uint8 packetSent = FALSE;            // Flag set when packet is sent
static uint16 packetCounter = 0;            // Counter keeping track of
                                            // packets sent
static uint16 packetLength = PACKET_LENGTH; // Length byte inserted in two first
                                            // bytes of the TX FIFO
static uint32 bytesLeft;                    // Keeping track of bytes left to
                                            // write to the TX FIFO
static uint8 fixedPacketLength;
static uint8 *pBufferIndex;                 // Pointer to current position in
                                            // the txBuffer
static uint8 iterations;                    // For packets greater than 128
                                            // bytes, this variable is used to
                                            // keep track of how many time the
                                            // TX FIFO should be re-filled to
                                            // its limit
static uint8 writeRemainingDataFlag;        // When this flag is set, the
                                            // TX FIFO should not be filled
                                            // entirely
static uint8 pktFormat = INFINITE;


/*******************************************************************************
* STATIC FUNCTIONS
*/
static void initMCU(void);
static void registerConfig(void);
static void manualCalibration(void);
static void updateLcd(void);
static void packetSentISR(void);
static void txFifoBelowThresholdISR(void);
static void printWelcomeMessage(void);
static void waitMs(uint16 msec);
static void waitUs(uint16 msec);


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

    // Write radio registers (typical settings from SmartRF Studio)
    registerConfig();

    // Application specific registers
    // FIFO_THR = 120
    // GPIO0 = TXFIFO_THR
    // GPIO2 = PKT_SYNC_RXTX
    writeByte = INFINITE_PACKET_LENGTH_MODE;
    cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
    writeByte = 0x78; cc112xSpiWriteReg(CC112X_FIFO_CFG, &writeByte, 1);
    writeByte = 0x02; cc112xSpiWriteReg(CC112X_IOCFG0,   &writeByte, 1);
    writeByte = 0x06; cc112xSpiWriteReg(CC112X_IOCFG2,   &writeByte, 1);

    bspLedSet(BSP_LED_ALL);

    // Calibrate the radio according to the errata note
    manualCalibration();

    // Connect ISR function to GPIO0
    ioPinIntRegister(IO_PIN_PORT_1, GPIO0, &txFifoBelowThresholdISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO0, IO_PIN_FALLING_EDGE);

    // Clear interrupt
    ioPinIntClear(IO_PIN_PORT_1, GPIO0);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO0);

    // Connect ISR function to GPIO2
    ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &packetSentISR);

    // Interrupt on falling edge
    ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_FALLING_EDGE);

    // Clear interrupt
    ioPinIntClear(IO_PIN_PORT_1, GPIO2);

    // Enable interrupt
    ioPinIntEnable(IO_PIN_PORT_1, GPIO2);

    // Create data packet
    txBuffer[0] = (uint8)(packetLength >> 8);
    txBuffer[1] = (uint8)(packetLength & 0x00FF);
    for (uint16 i = 2; i < PACKET_LENGTH + 2; i++)
        txBuffer[i] = rand();

    printWelcomeMessage();

    // Infinite loop
    while(TRUE) {

        // Wait for button push
        while(!bspKeyPushed(BSP_KEY_ALL));

        // Transmit 1000 packets
        for (uint16 i = 0; i < 1000; i++) {

            // Set infinite packet length mode
            writeByte = INFINITE_PACKET_LENGTH_MODE;
            cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
            pktFormat = INFINITE;

            // Update variables
            writeRemainingDataFlag = FALSE;
            bytesLeft = packetLength + 2;
            pBufferIndex = txBuffer + FIFO_SIZE;
            fixedPacketLength = bytesLeft % (MAX_VARIABLE_LENGTH + 1);

            // Configure the PKT_LEN register
            writeByte = fixedPacketLength;
            cc112xSpiWriteReg(CC112X_PKT_LEN, &writeByte, 1);
            
            // Fill up the TX FIFO
            cc112xSpiWriteTxFifo(txBuffer, FIFO_SIZE);

            bytesLeft -= FIFO_SIZE;
            
            iterations = (bytesLeft / AVAILABLE_BYTES_IN_TX_FIFO);

            // Enter TX mode
            trxSpiCmdStrobe(CC112X_STX);

            // Clear isr flag on GPIO0
            ioPinIntClear(IO_PIN_PORT_1, GPIO0);

            // Enable interrupt on GPIO0
            ioPinIntEnable(IO_PIN_PORT_1, GPIO0);

            // Wait for packet to be sent
            while (!packetSent);
            packetSent = FALSE;

            // Update LCD
            updateLcd();

            waitMs(300);
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
    lcdBufferPrintString(0, "Press a button", 0, eLcdPage2);
    lcdBufferPrintString(0, "to start", 0, eLcdPage3);
    lcdBufferPrintString(0, "Packet TX", 0, eLcdPage7);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
    lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
    lcdSendBuffer(0);
}


/*******************************************************************************
* @fn          packetSentISR
*
* @brief       Function running every time a packet has been sent
*
* @param       none
*
* @return      none
*/
static void packetSentISR(void) {

    packetSent = TRUE;

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO2);
}


/*******************************************************************************
*   @fn         txFifoBelowThresholdISR
*
*   @brief      Function running every time the TX FIFO is drained below
*               127 - FIFO_THR = 127 - 120 = 7
*
*   @param      none
*
*   @return     none
*/
static void txFifoBelowThresholdISR(void) {

    uint8 writeByte;

    if (writeRemainingDataFlag) { // Less than 120 bytes to write to the TX FIFO

        cc112xSpiWriteTxFifo(pBufferIndex, bytesLeft);  // Write remaining bytes
                                                        // to the TX FIFO
        // Disable interrupt on GPIO0
        ioPinIntDisable(IO_PIN_PORT_1, GPIO0);

    } else { // Fill up the TX FIFO
        cc112xSpiWriteTxFifo(pBufferIndex, AVAILABLE_BYTES_IN_TX_FIFO);

        // Change to fixed packet length mode when there is less than 256 bytes
        // left to transmit
        if ((bytesLeft < (MAX_VARIABLE_LENGTH + 1 - BYTES_IN_TX_FIFO))
            && (pktFormat == INFINITE)) {

            writeByte = FIXED_PACKET_LENGTH_MODE;
            cc112xSpiWriteReg(CC112X_PKT_CFG0, &writeByte, 1);
            pktFormat = FIXED;
        }

        // Update the variables keeping track of how many more bytes should be
        // written to the TX FIFO and where in txBuffer data should 
        // be taken from
        pBufferIndex += AVAILABLE_BYTES_IN_TX_FIFO;
       
        bytesLeft -= AVAILABLE_BYTES_IN_TX_FIFO;
        
        if (!(--iterations))
            writeRemainingDataFlag = TRUE;
    }

    // Clear ISR flag
    ioPinIntClear(IO_PIN_PORT_1, GPIO0);
}


/*******************************************************************************
*   @fn         updateLcd
*
*   @brief      Updates the LCD with # of packets sent
*
*   @param      none
*
*   @return     none
*/
static void updateLcd(void) {
    lcdBufferClear(0);
    lcdBufferPrintString(0, "Inf. Pkt. Length Mode", 0, eLcdPage0);
    lcdBufferSetHLine(0, 0, LCD_COLS - 1, eLcdPage7);
    lcdBufferPrintString(0, "Sent packets:", 0, eLcdPage2);
    lcdBufferPrintInt(0, (int32)(++packetCounter), 80, eLcdPage2);
    lcdBufferPrintString(0, "Packet TX", 0, eLcdPage7);
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
*   @param      uint16 uSec - number of microseconds delay
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
}                      // 4 cycles for returning


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
*   @param      uint16 mSec - number of milliseconds delay
*
*   @return     none
*/
#pragma optimize=none
static void waitMs(uint16 mSec) {
    while(mSec-- > 0) {
        waitUs(1000);
    }
}