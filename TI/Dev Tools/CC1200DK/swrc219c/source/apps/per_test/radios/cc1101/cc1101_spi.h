//*****************************************************************************
//! @file cc1101_spi.h  
//    
//! @brief  header file that defines a minimum set of neccessary functions
//          to communicate with CC1101 over SPI as well as the regsister 
//          mapping.    
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

#ifndef CC1101_SPI_H
#define CC1101_SPI_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "trx_rf_spi.h"

/******************************************************************************
 * CONSTANTS
 */

/* Register adresses */
#define CC1101_IOCFG2      0x00      /*  IOCFG2   - GDO2 output pin configuration  */
#define CC1101_IOCFG1      0x01      /*  IOCFG1   - GDO1 output pin configuration  */
#define CC1101_IOCFG0      0x02      /*  IOCFG1   - GDO0 output pin configuration  */
#define CC1101_FIFOTHR     0x03      /*  FIFOTHR  - RX FIFO and TX FIFO thresholds */
#define CC1101_SYNC1       0x04      /*  SYNC1    - Sync word, high byte */
#define CC1101_SYNC0       0x05      /*  SYNC0    - Sync word, low byte */
#define CC1101_PKTLEN      0x06      /*  PKTLEN   - Packet length */
#define CC1101_PKTCTRL1    0x07      /*  PKTCTRL1 - Packet automation control */
#define CC1101_PKTCTRL0    0x08      /*  PKTCTRL0 - Packet automation control */
#define CC1101_ADDR        0x09      /*  ADDR     - Device address */
#define CC1101_CHANNR      0x0A      /*  CHANNR   - Channel number */
#define CC1101_FSCTRL1     0x0B      /*  FSCTRL1  - Frequency synthesizer control */
#define CC1101_FSCTRL0     0x0C      /*  FSCTRL0  - Frequency synthesizer control */
#define CC1101_FREQ2       0x0D      /*  FREQ2    - Frequency control word, high byte */
#define CC1101_FREQ1       0x0E      /*  FREQ1    - Frequency control word, middle byte */
#define CC1101_FREQ0       0x0F      /*  FREQ0    - Frequency control word, low byte */
#define CC1101_MDMCFG4     0x10      /*  MDMCFG4  - Modem configuration */
#define CC1101_MDMCFG3     0x11      /*  MDMCFG3  - Modem configuration */
#define CC1101_MDMCFG2     0x12      /*  MDMCFG2  - Modem configuration */
#define CC1101_MDMCFG1     0x13      /*  MDMCFG1  - Modem configuration */
#define CC1101_MDMCFG0     0x14      /*  MDMCFG0  - Modem configuration */
#define CC1101_DEVIATN     0x15      /*  DEVIATN  - Modem deviation setting */
#define CC1101_MCSM2       0x16      /*  MCSM2    - Main Radio Control State Machine configuration */
#define CC1101_MCSM1       0x17      /*  MCSM1    - Main Radio Control State Machine configuration */
#define CC1101_MCSM0       0x18      /*  MCSM0    - Main Radio Control State Machine configuration */
#define CC1101_FOCCFG      0x19      /*  FOCCFG   - Frequency Offset Compensation configuration */
#define CC1101_BSCFG       0x1A      /*  BSCFG    - Bit Synchronization configuration */
#define CC1101_AGCCTRL2    0x1B      /*  AGCCTRL2 - AGC control */
#define CC1101_AGCCTRL1    0x1C      /*  AGCCTRL1 - AGC control */
#define CC1101_AGCCTRL0    0x1D      /*  AGCCTRL0 - AGC control */
#define CC1101_WOREVT1     0x1E      /*  WOREVT1  - High byte Event0 timeout */
#define CC1101_WOREVT0     0x1F      /*  WOREVT0  - Low byte Event0 timeout */
#define CC1101_WORCTRL     0x20      /*  WORCTRL  - Wake On Radio control */
#define CC1101_FREND1      0x21      /*  FREND1   - Front end RX configuration */
#define CC1101_FREND0      0x22      /*  FREDN0   - Front end TX configuration */
#define CC1101_FSCAL3      0x23      /*  FSCAL3   - Frequency synthesizer calibration */
#define CC1101_FSCAL2      0x24      /*  FSCAL2   - Frequency synthesizer calibration */
#define CC1101_FSCAL1      0x25      /*  FSCAL1   - Frequency synthesizer calibration */
#define CC1101_FSCAL0      0x26      /*  FSCAL0   - Frequency synthesizer calibration */
#define CC1101_RCCTRL1     0x27      /*  RCCTRL1  - RC oscillator configuration */
#define CC1101_RCCTRL0     0x28      /*  RCCTRL0  - RC oscillator configuration */
#define CC1101_FSTEST      0x29      /*  FSTEST   - Frequency synthesizer calibration control */
#define CC1101_PTEST       0x2A      /*  PTEST    - Production test */
#define CC1101_AGCTEST     0x2B      /*  AGCTEST  - AGC test */
#define CC1101_TEST2       0x2C      /*  TEST2    - Various test settings */
#define CC1101_TEST1       0x2D      /*  TEST1    - Various test settings */
#define CC1101_TEST0       0x2E      /*  TEST0    - Various test settings */


/* status registers */
#define CC1101_PARTNUM     0x30      /*  PARTNUM    - Chip ID */
#define CC1101_VERSION     0x31      /*  VERSION    - Chip ID */
#define CC1101_FREQEST     0x32      /*  FREQEST    – Frequency Offset Estimate from demodulator */
#define CC1101_LQI         0x33      /*  LQI        – Demodulator estimate for Link Quality */
#define CC1101_RSSI        0x34      /*  RSSI       – Received signal strength indication */
#define CC1101_MARCSTATE   0x35      /*  MARCSTATE  – Main Radio Control State Machine state */
#define CC1101_WORTIME1    0x36      /*  WORTIME1   – High byte of WOR time */
#define CC1101_WORTIME0    0x37      /*  WORTIME0   – Low byte of WOR time */
#define CC1101_PKTSTATUS   0x38      /*  PKTSTATUS  – Current GDOx status and packet status */
#define CC1101_VCO_VC_DAC  0x39      /*  VCO_VC_DAC – Current setting from PLL calibration module */
#define CC1101_TXBYTES     0x3A      /*  TXBYTES    – Underflow and number of bytes */
#define CC1101_RXBYTES     0x3B      /*  RXBYTES    – Overflow and number of bytes */

/* burst write registers */
#define CC1101_PA_TABLE0   0x3E      /*  PA_TABLE0 - PA control settings table */
#define CC1101_FIFO        0x3F      /*  TXFIFO  - Transmit FIFO */

/* Other register bit fields */

#define CC1101_LQI_CRC_OK_BM            0x80
#define CC1101_LQI_EST_BM               0x7F


/* Command strobe registers */
#define CC1101_SRES                     0x30      /*  SRES    - Reset chip. */
#define CC1101_SFSTXON                  0x31      /*  SFSTXON - Enable and calibrate frequency synthesizer. */
#define CC1101_SXOFF                    0x32      /*  SXOFF   - Turn off crystal oscillator. */
#define CC1101_SCAL                     0x33      /*  SCAL    - Calibrate frequency synthesizer and turn it off. */
#define CC1101_SRX                      0x34      /*  SRX     - Enable RX. Perform calibration if enabled. */
#define CC1101_STX                      0x35      /*  STX     - Enable TX. If in RX state, only enable TX if CCA passes. */
#define CC1101_SIDLE                    0x36      /*  SIDLE   - Exit RX / TX, turn off frequency synthesizer. */
#define CC1101_SWOR                     0x38      /*  SWOR    - Start automatic RX polling sequence (Wake-on-Radio) */
#define CC1101_SPWD                     0x39      /*  SPWD    - Enter power down mode when CSn goes high. */
#define CC1101_SFRX                     0x3A      /*  SFRX    - Flush the RX FIFO buffer. */
#define CC1101_SFTX                     0x3B      /*  SFTX    - Flush the TX FIFO buffer. */
#define CC1101_SWORRST                  0x3C      /*  SWORRST - Reset real time clock. */
#define CC1101_SNOP                     0x3D      /*  SNOP    - No operation. Returns status byte. */
    

/* Chip states returned in status byte */
#define CC1101_STATE_IDLE               0x00
#define CC1101_STATE_RX                 0x10
#define CC1101_STATE_TX                 0x20
#define CC1101_STATE_FSTXON             0x30
#define CC1101_STATE_CALIBRATE          0x40
#define CC1101_STATE_SETTLING           0x50
#define CC1101_STATE_RXFIFO_ERROR       0x60
#define CC1101_STATE_TXFIFO_ERROR       0x70



/******************************************************************************
 * TYPEDEFS
 */

/* Number of burst registers from SmartRF Studio */
typedef uint8 CC1101_BURST_REGISTERS[47];

/******************************************************************************
 * PROTPTYPES
 */ 

/* Basic radio SPI functions */
rfStatus_t cc1101SpiWriteReg(uint8 addr,uint8 *pData, uint8 len);    
rfStatus_t cc1101GetTxStatus(void);
rfStatus_t cc1101GetRxStatus(void);                                                                        
rfStatus_t cc1101SpiReadReg(uint8 addr, uint8 *pData, uint8 len);  
rfStatus_t cc1101SpiWriteTxFifo(uint8 *pData, uint8 len);
rfStatus_t cc1101SpiReadRxFifo(uint8 *pData, uint8 len);  
         


#ifdef  __cplusplus
}
#endif

#endif// CC1101_SPI_H