//******************************************************************************
//! @file      cc1101_cc1190_per_test_api.c
//  
//! @brief    Implemenation file for api-like functions that the per test
//            will call if a CC1101_CC1190 combo was selected.
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


/******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "hal_timer_32k.h"
#include "trx_rf_spi.h"
#include "trx_rf_int.h"
#include "cc1101_per_test_api.h"
#include "cc1101_spi.h"
#include "per_test.h"

#include "cc1101_cc1190_per_test_api.h"

#include "cc1101_cc1190_915MHz_reg_settings.c"
/******************************************************************************
 * LOCAL FUNCTIONS
 */

static void cc1101cc1190RxIdle(void);
static void cc1101cc1190IdleRx(void);
static int8 cc1101cc1190Convert8BitRssi(uint8 rawRssi);
void perCC1101CC1190Tx(void);
void perCC1101CC1190Rx(void);

/* cc1190 pa and lna functions */
void perCC1101CC1190PaEnable(void);
void perCC1101CC1190PaDisable(void);
void perCC1101CC1190LnaEnable(void);
void perCC1101CC1190LnaDisable(void);
void perCC1101CC1190HgmEnable(void);
void perCC1101CC1190HgmDisable(void);

/******************************************************************************
 * LOCAL VARIABLES
 */

/* Variable is STATE_RX when in RX. If not in RX it is STATE_TX or STATE_IDLE. 
 * The use of this variable is only to avoid an RX interrupt when sending a 
 * packet. This facilitates a reduction in power consumption.
 */
static rfStatus_t cc1101cc1190RadioTxRx;

/******************************************************************************
 * CONSTANTS
 */ 


//#define CC1101_CONFIGURATION_REGISTERS 47

/* The following must be added to base and selected parameters */
#define PER_CC1101_MCSM1 0x0C /* CCA disabled, RX after packet, Idle after TX */
#define PER_CC1101_MCSM2 0x07 /* RX Time disabled - no termination */
#define PER_CC1101_MCSM0 0x18 /* Calibrate from IDLE to RX/TX, PO_TIMEOUT set to rec. settings */

/* No CCA, No RX timeout, Calibration from IDLE->RX/TX, PO_TIMEOUT, RX after pakt, 
 * IDLE after TX
 */
static uint8 CC1101_MCSMs[3]=
{
  PER_CC1101_MCSM2,
  PER_CC1101_MCSM1,
  PER_CC1101_MCSM0
};

static const int8 cc1101cc1190RssiOffset  = 81;

/* Register values for FIFOTHR in TX and RX state*/
static uint8 cc1101cc1190FifothrTx = 0xC7;
static uint8 cc1101cc1190FifothrRx = 0x47;
/* Register values for TEST1 in TX and RX state*/
static uint8 cc1101cc1190Test1Tx = 0x2D;
static uint8 cc1101cc1190Test1Rx = 0x35;


/* Radio configurations exported from SmartRF Studio*/
 
/* Deviation = 4.943848 */
/* Base frequency = 869.524750 */
/* Carrier frequency = 869.524750 */
/* Channel number = 0 */
/* Carrier frequency = 869.524750 */
/* Modulated = true */
/* Modulation format = GFSK */
/* Manchester enable = false */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Preamble count = 4 */
/* Channel spacing = 199.813843 */
/* Carrier frequency = 869.524750 */
/* Data rate = 1.20056 */
/* RX filter BW = 210.937500 */
/* Data format = Normal mode */
/* Length config = Variable packet length mode. Packet length configured by the first byte after sync word */
/* CRC enable = true */
/* Packet length = 255 */
/* Device address = 0 */
/* Address config = No address check */
/* CRC autoflush = false */
/* PA ramping = false */
/* TX power = 0 */

static const registerSetting_t lowDataRateCC1101CC1190RfSettings[] = {
    {CC1101_TEST0    ,0x09}, /*TEST0         Various Test Settings                            */
    {CC1101_TEST1    ,0x35}, /*TEST1         Various Test Settings                            */
    {CC1101_TEST2    ,0x81}, /*TEST2         Various Test Settings                            */
    {CC1101_AGCTEST  ,0x3F}, /*AGCTEST       AGC Test                                         */
    {CC1101_PTEST    ,0x7F}, /*PTEST         Production Test                                  */
    {CC1101_FSTEST   ,0x59}, /*FSTEST        Frequency Synthesizer Calibration Control        */
    {CC1101_RCCTRL0  ,0x00}, /*RCCTRL0       RC Oscillator Configuration                      */
    {CC1101_RCCTRL1  ,0x41}, /*RCCTRL1       RC Oscillator Configuration                      */
    {CC1101_FSCAL0   ,0x1F}, /*FSCAL0        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL1   ,0x00}, /*FSCAL1        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL2   ,0x2A}, /*FSCAL2        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL3   ,0xE9}, /*FSCAL3        Frequency Synthesizer Calibration                */
    {CC1101_FREND0   ,0x10}, /*FREND0        Front End TX Configuration                       */
    {CC1101_FREND1   ,0x56}, /*FREND1        Front End RX Configuration                       */
    {CC1101_WORCTRL  ,0xFB}, /*WORCTRL       Wake On Radio Control                            */
    {CC1101_WOREVT0  ,0x6B}, /*WOREVT0       Low Byte Event0 Timeout                          */
    {CC1101_WOREVT1  ,0x87}, /*WOREVT1       High Byte Event0 Timeout                         */
    {CC1101_AGCCTRL0 ,0x91}, /*AGCCTRL0      AGC Control                                      */
    {CC1101_AGCCTRL1 ,0x40}, /*AGCCTRL1      AGC Control                                      */
    {CC1101_AGCCTRL2 ,0x03}, /*AGCCTRL2      AGC Control                                      */
    {CC1101_BSCFG    ,0x6C}, /*BSCFG         Bit Synchronization Configuration                */
    {CC1101_FOCCFG   ,0x16}, /*FOCCFG        Frequency Offset Compensation Configuration      */
    {CC1101_MCSM0    ,0x18}, /*MCSM0         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM1    ,0x30}, /*MCSM1          Main Radio Control State Machine Configuration   */
    {CC1101_MCSM2    ,0x07 }, /*MCSM2         Main Radio Control State Machine Configuration   */
    {CC1101_DEVIATN  ,0x14}, /*DEVIATN       Modem Deviation Setting                          */
    {CC1101_MDMCFG0  ,0xE5}, /*MDMCFG0       Modem Configuration                              */
    {CC1101_MDMCFG1  ,0x22}, /*MDMCFG1       Modem Configuration                              */
    {CC1101_MDMCFG2  ,0x13}, /*MDMCFG2       Modem Configuration                              */
    {CC1101_MDMCFG3  ,0x75}, /*MDMCFG3       Modem Configuration                              */
    {CC1101_MDMCFG4  ,0x85}, /*MDMCFG4       Modem Configuration                              */
    {CC1101_FREQ0    ,0x62}, /*FREQ0         Frequency Control Word, Low Byte                 */
    {CC1101_FREQ1    ,0x34}, /*FREQ1         Frequency Control Word, Middle Byte              */
    {CC1101_FREQ2    ,0x20}, /*FREQ2         Frequency Control Word, High Byte                */
    {CC1101_FSCTRL0  ,0x00}, /*FSCTRL0       Frequency Synthesizer Control                    */
    {CC1101_FSCTRL1  ,0x06}, /*FSCTRL1       Frequency Synthesizer Control                    */
    {CC1101_CHANNR   ,0x00}, /*CHANNR        Channel Number                                   */
    {CC1101_ADDR     ,0x00}, /*ADDR          Device Address                                   */
    {CC1101_PKTCTRL0 ,0x05}, /*PKTCTRL0      Packet Automation Control                        */
    {CC1101_PKTCTRL1 ,0x04}, /*PKTCTRL1      Packet Automation Control                        */
    {CC1101_PKTLEN   ,0xFF}, /*PKTLEN        Packet Length                                    */
    {CC1101_SYNC0    ,0x91}, /*SYNC0         Sync Word, Low Byte                              */
    {CC1101_SYNC1    ,0xD3}, /*SYNC1         Sync Word, High Byte                             */
    {CC1101_FIFOTHR  ,0x07}, /*FIFOTHR       RX FIFO and TX FIFO Thresholds                   */
    {CC1101_IOCFG0   ,0x06}, /*IOCFG0        GDO0 Output Pin Configuration                    */
    {CC1101_IOCFG1   ,0x2E}, /*IOCFG1        GDO1 Output Pin Configuration                    */
    {CC1101_IOCFG2   ,0x29}  /*IOCFG2        GDO2 Output Pin Configuration                    */
};

/* Deviation = 14.831543 */
/* Base frequency = 869.524750 */
/* Carrier frequency = 869.524750 */
/* Channel number = 0 */
/* Carrier frequency = 869.524750 */
/* Modulated = true */
/* Modulation format = GFSK */
/* Manchester enable = false */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Preamble count = 4 */
/* Channel spacing = 199.813843 */
/* Carrier frequency = 869.524750 */
/* Data rate = 1.20056 */
/* RX filter BW = 210.937500 */
/* Data format = Normal mode */
/* Length config = Variable packet length mode. Packet length configured by the first byte after sync word */
/* CRC enable = true */
/* Packet length = 255 */
/* Device address = 0 */
/* Address config = No address check */
/* CRC autoflush = false */
/* PA ramping = false */
/* TX power = 0 */
/* RF settings CC112X for simple PER test*/
static const registerSetting_t lowDataRateSensCC1101CC1190RfSettings[] = {
    {CC1101_TEST0    ,0x09}, /*TEST0         Various Test Settings                            */
    {CC1101_TEST1    ,0x35}, /*TEST1         Various Test Settings                            */
    {CC1101_TEST2    ,0x81}, /*TEST2         Various Test Settings                            */
    {CC1101_AGCTEST  ,0x3F}, /*AGCTEST       AGC Test                                         */
    {CC1101_PTEST    ,0x7F}, /*PTEST         Production Test                                  */
    {CC1101_FSTEST   ,0x59}, /*FSTEST        Frequency Synthesizer Calibration Control        */
    {CC1101_RCCTRL0  ,0x00}, /*RCCTRL0       RC Oscillator Configuration                      */
    {CC1101_RCCTRL1  ,0x41}, /*RCCTRL1       RC Oscillator Configuration                      */
    {CC1101_FSCAL0   ,0x1F}, /*FSCAL0        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL1   ,0x00}, /*FSCAL1        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL2   ,0x2A}, /*FSCAL2        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL3   ,0xE9}, /*FSCAL3        Frequency Synthesizer Calibration                */
    {CC1101_FREND0   ,0x10}, /*FREND0        Front End TX Configuration                       */
    {CC1101_FREND1   ,0x56}, /*FREND1        Front End RX Configuration                       */
    {CC1101_WORCTRL  ,0xFB}, /*WORCTRL       Wake On Radio Control                            */
    {CC1101_WOREVT0  ,0x6B}, /*WOREVT0       Low Byte Event0 Timeout                          */
    {CC1101_WOREVT1  ,0x87}, /*WOREVT1       High Byte Event0 Timeout                         */
    {CC1101_AGCCTRL0 ,0x91}, /*AGCCTRL0      AGC Control                                      */
    {CC1101_AGCCTRL1 ,0x40}, /*AGCCTRL1      AGC Control                                      */
    {CC1101_AGCCTRL2 ,0x03}, /*AGCCTRL2      AGC Control                                      */
    {CC1101_BSCFG    ,0x6C}, /*BSCFG         Bit Synchronization Configuration                */
    {CC1101_FOCCFG   ,0x16}, /*FOCCFG        Frequency Offset Compensation Configuration      */
    {CC1101_MCSM0    ,0x18}, /*MCSM0         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM1    ,0x30}, /*MCSM1         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM2    ,0x07}, /*MCSM2         Main Radio Control State Machine Configuration   */
    {CC1101_DEVIATN  ,0x31}, /*DEVIATN       Modem Deviation Setting                          */
    {CC1101_MDMCFG0  ,0xE5}, /*MDMCFG0       Modem Configuration                              */
    {CC1101_MDMCFG1  ,0x22}, /*MDMCFG1       Modem Configuration                              */
    {CC1101_MDMCFG2  ,0x13}, /*MDMCFG2       Modem Configuration                              */
    {CC1101_MDMCFG3  ,0x75}, /*MDMCFG3       Modem Configuration                              */
    {CC1101_MDMCFG4  ,0x85}, /*MDMCFG4       Modem Configuration                              */
    {CC1101_FREQ0    ,0x62}, /*FREQ0         Frequency Control Word, Low Byte                 */
    {CC1101_FREQ1    ,0x34}, /*FREQ1         Frequency Control Word, Middle Byte              */
    {CC1101_FREQ2    ,0x20}, /*FREQ2         Frequency Control Word, High Byte                */
    {CC1101_FSCTRL0  ,0x00}, /*FSCTRL0       Frequency Synthesizer Control                    */
    {CC1101_FSCTRL1  ,0x06}, /*FSCTRL1       Frequency Synthesizer Control                    */
    {CC1101_CHANNR   ,0x00}, /*CHANNR        Channel Number                                   */
    {CC1101_ADDR     ,0x00}, /*ADDR          Device Address                                   */
    {CC1101_PKTCTRL0 ,0x05}, /*PKTCTRL0      Packet Automation Control                        */
    {CC1101_PKTCTRL1 ,0x04}, /*PKTCTRL1      Packet Automation Control                        */
    {CC1101_PKTLEN   ,0xFF}, /*PKTLEN        Packet Length                                    */
    {CC1101_SYNC0    ,0x91}, /*SYNC0         Sync Word, Low Byte                              */
    {CC1101_SYNC1    ,0xD3}, /*SYNC1         Sync Word, High Byte                             */
    {CC1101_FIFOTHR  ,0x07}, /*FIFOTHR       RX FIFO and TX FIFO Thresholds                   */
    {CC1101_IOCFG0   ,0x06}, /*IOCFG0        GDO0 Output Pin Configuration                    */
    {CC1101_IOCFG1   ,0x2E}, /*IOCFG1        GDO1 Output Pin Configuration                    */
    {CC1101_IOCFG2   ,0x29}  /*IOCFG2        GDO2 Output Pin Configuration                    */
};



/* Deviation = 24.719238 */
/* Base frequency = 869.524750 */
/* Carrier frequency = 869.524750 */
/* Channel number = 0 */
/* Carrier frequency = 869.524750 */
/* Modulated = true */
/* Modulation format = GFSK */
/* Manchester enable = false */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Preamble count = 4 */
/* Channel spacing = 199.813843 */
/* Carrier frequency = 869.524750 */
/* Data rate = 4.80223 */
/* RX filter BW = 210.937500 */
/* Data format = Normal mode */
/* Length config = Variable packet length mode. Packet length configured by the first byte after sync word */
/* CRC enable = true */
/* Packet length = 255 */
/* Device address = 0 */
/* Address config = No address check */
/* CRC autoflush = false */
/* PA ramping = false */
/* TX power = -10 */
/* RF settings CC112X for simple PER test*/
static const registerSetting_t mediumDataRateCC1101CC1190RfSettings[] = {
    {CC1101_TEST0    ,0x09}, /*TEST0         Various Test Settings                            */
    {CC1101_TEST1    ,0x35}, /*TEST1         Various Test Settings                            */
    {CC1101_TEST2    ,0x81}, /*TEST2         Various Test Settings                            */
    {CC1101_AGCTEST  ,0x3F}, /*AGCTEST       AGC Test                                         */
    {CC1101_PTEST    ,0x7F}, /*PTEST         Production Test                                  */
    {CC1101_FSTEST   ,0x59}, /*FSTEST        Frequency Synthesizer Calibration Control        */
    {CC1101_RCCTRL0  ,0x00}, /*RCCTRL0       RC Oscillator Configuration                      */
    {CC1101_RCCTRL1  ,0x41}, /*RCCTRL1       RC Oscillator Configuration                      */
    {CC1101_FSCAL0   ,0x1F}, /*FSCAL0        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL1   ,0x00}, /*FSCAL1        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL2   ,0x2A}, /*FSCAL2        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL3   ,0xE9}, /*FSCAL3        Frequency Synthesizer Calibration                */
    {CC1101_FREND0   ,0x10}, /*FREND0        Front End TX Configuration                       */
    {CC1101_FREND1   ,0x56}, /*FREND1        Front End RX Configuration                       */
    {CC1101_WORCTRL  ,0xFB}, /*WORCTRL       Wake On Radio Control                            */
    {CC1101_WOREVT0  ,0x6B}, /*WOREVT0       Low Byte Event0 Timeout                          */
    {CC1101_WOREVT1  ,0x87}, /*WOREVT1       High Byte Event0 Timeout                         */
    {CC1101_AGCCTRL0 ,0x91}, /*AGCCTRL0      AGC Control                                      */
    {CC1101_AGCCTRL1 ,0x40}, /*AGCCTRL1      AGC Control                                      */
    {CC1101_AGCCTRL2 ,0x43}, /*AGCCTRL2      AGC Control                                      */
    {CC1101_BSCFG    ,0x6C}, /*BSCFG         Bit Synchronization Configuration                */
    {CC1101_FOCCFG   ,0x16}, /*FOCCFG        Frequency Offset Compensation Configuration      */
    {CC1101_MCSM0    ,0x18}, /*MCSM0         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM1    ,0x30}, /*MCSM1         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM2    ,0x07}, /*MCSM2         Main Radio Control State Machine Configuration   */
    {CC1101_DEVIATN  ,0x37}, /*DEVIATN       Modem Deviation Setting                          */
    {CC1101_MDMCFG0  ,0xE5}, /*MDMCFG0       Modem Configuration                              */
    {CC1101_MDMCFG1  ,0x22}, /*MDMCFG1       Modem Configuration                              */
    {CC1101_MDMCFG2  ,0x13}, /*MDMCFG2       Modem Configuration                              */
    {CC1101_MDMCFG3  ,0x75}, /*MDMCFG3       Modem Configuration                              */
    {CC1101_MDMCFG4  ,0x87}, /*MDMCFG4       Modem Configuration                              */
    {CC1101_FREQ0    ,0x62}, /*FREQ0         Frequency Control Word, Low Byte                 */
    {CC1101_FREQ1    ,0x34}, /*FREQ1         Frequency Control Word, Middle Byte              */
    {CC1101_FREQ2    ,0x20}, /*FREQ2         Frequency Control Word, High Byte                */
    {CC1101_FSCTRL0  ,0x00}, /*FSCTRL0       Frequency Synthesizer Control                    */
    {CC1101_FSCTRL1  ,0x06}, /*FSCTRL1       Frequency Synthesizer Control                    */
    {CC1101_CHANNR   ,0x00}, /*CHANNR        Channel Number                                   */
    {CC1101_ADDR     ,0x00}, /*ADDR          Device Address                                   */
    {CC1101_PKTCTRL0 ,0x05}, /*PKTCTRL0      Packet Automation Control                        */
    {CC1101_PKTCTRL1 ,0x04}, /*PKTCTRL1      Packet Automation Control                        */
    {CC1101_PKTLEN   ,0xFF}, /*PKTLEN        Packet Length                                    */
    {CC1101_SYNC0    ,0x91}, /*SYNC0         Sync Word, Low Byte                              */
    {CC1101_SYNC1    ,0xD3}, /*SYNC1         Sync Word, High Byte                             */
    {CC1101_FIFOTHR  ,0x07}, /*FIFOTHR       RX FIFO and TX FIFO Thresholds                   */
    {CC1101_IOCFG0   ,0x06}, /*IOCFG0        GDO0 Output Pin Configuration                    */
    {CC1101_IOCFG1   ,0x2E}, /*IOCFG1        GDO1 Output Pin Configuration                    */
    {CC1101_IOCFG2   ,0x29}  /*IOCFG2        GDO2 Output Pin Configuration                    */
};

/* Deviation = 19.775391 */
/* Base frequency = 869.524750 */
/* Carrier frequency = 869.524750 */
/* Channel number = 0 */
/* Carrier frequency = 869.524750 */
/* Modulated = true */
/* Modulation format = GFSK */
/* Manchester enable = false */
/* Sync word qualifier mode = 30/32 sync word bits detected */
/* Preamble count = 4 */
/* Channel spacing = 199.813843 */
/* Carrier frequency = 869.524750 */
/* Data rate = 38.4178 */
/* RX filter BW = 210.937500 */
/* Data format = Normal mode */
/* Length config = Variable packet length mode. Packet length configured by the first byte after sync word */
/* CRC enable = true */
/* Packet length = 255 */
/* Device address = 0 */
/* Address config = No address check */
/* CRC autoflush = false */
/* PA ramping = false */
/* TX power = -10 */
/* RF settings CC112X for simple PER test*/

static const registerSetting_t highDataRateCC1101CC1190RfSettings[] = {
    {CC1101_TEST0    ,0x09}, /*TEST0         Various Test Settings                            */
    {CC1101_TEST1    ,0x35}, /*TEST1         Various Test Settings                            */
    {CC1101_TEST2    ,0x81}, /*TEST2         Various Test Settings                            */
    {CC1101_AGCTEST  ,0x3F}, /*AGCTEST       AGC Test                                         */
    {CC1101_PTEST    ,0x7F}, /*PTEST         Production Test                                  */
    {CC1101_FSTEST   ,0x59}, /*FSTEST        Frequency Synthesizer Calibration Control        */
    {CC1101_RCCTRL0  ,0x00}, /*RCCTRL0       RC Oscillator Configuration                      */
    {CC1101_RCCTRL1  ,0x41}, /*RCCTRL1       RC Oscillator Configuration                      */
    {CC1101_FSCAL0   ,0x1F}, /*FSCAL0        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL1   ,0x00}, /*FSCAL1        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL2   ,0x2A}, /*FSCAL2        Frequency Synthesizer Calibration                */
    {CC1101_FSCAL3   ,0xE9}, /*FSCAL3        Frequency Synthesizer Calibration                */
    {CC1101_FREND0   ,0x10}, /*FREND0        Front End TX Configuration                       */
    {CC1101_FREND1   ,0x56}, /*FREND1        Front End RX Configuration                       */
    {CC1101_WORCTRL  ,0xFB}, /*WORCTRL       Wake On Radio Control                            */
    {CC1101_WOREVT0  ,0x6B}, /*WOREVT0       Low Byte Event0 Timeout                          */
    {CC1101_WOREVT1  ,0x87}, /*WOREVT1       High Byte Event0 Timeout                         */
    {CC1101_AGCCTRL0 ,0x91}, /*AGCCTRL0      AGC Control                                      */
    {CC1101_AGCCTRL1 ,0x40}, /*AGCCTRL1      AGC Control                                      */
    {CC1101_AGCCTRL2 ,0x43}, /*AGCCTRL2      AGC Control                                      */
    {CC1101_BSCFG    ,0x6C}, /*BSCFG         Bit Synchronization Configuration                */
    {CC1101_FOCCFG   ,0x16}, /*FOCCFG        Frequency Offset Compensation Configuration      */
    {CC1101_MCSM0    ,0x18}, /*MCSM0         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM1    ,0x30}, /*MCSM1         Main Radio Control State Machine Configuration   */
    {CC1101_MCSM2    ,0x07}, /*MCSM2         Main Radio Control State Machine Configuration   */
    {CC1101_DEVIATN  ,0x34}, /*DEVIATN       Modem Deviation Setting                          */
    {CC1101_MDMCFG0  ,0xE5}, /*MDMCFG0       Modem Configuration                              */
    {CC1101_MDMCFG1  ,0x22}, /*MDMCFG1       Modem Configuration                              */
    {CC1101_MDMCFG2  ,0x13}, /*MDMCFG2       Modem Configuration                              */
    {CC1101_MDMCFG3  ,0x75}, /*MDMCFG3       Modem Configuration                              */
    {CC1101_MDMCFG4  ,0x8A}, /*MDMCFG4       Modem Configuration                              */
    {CC1101_FREQ0    ,0x62}, /*FREQ0         Frequency Control Word, Low Byte                 */
    {CC1101_FREQ1    ,0x34}, /*FREQ1         Frequency Control Word, Middle Byte              */
    {CC1101_FREQ2    ,0x20}, /*FREQ2         Frequency Control Word, High Byte                */
    {CC1101_FSCTRL0  ,0x00}, /*FSCTRL0       Frequency Synthesizer Control                    */
    {CC1101_FSCTRL1  ,0x06}, /*FSCTRL1       Frequency Synthesizer Control                    */
    {CC1101_CHANNR   ,0x00}, /*CHANNR        Channel Number                                   */
    {CC1101_ADDR     ,0x00}, /*ADDR          Device Address                                   */
    {CC1101_PKTCTRL0 ,0x05}, /*PKTCTRL0      Packet Automation Control                        */
    {CC1101_PKTCTRL1 ,0x04}, /*PKTCTRL1      Packet Automation Control                        */
    {CC1101_PKTLEN   ,0xFF}, /*PKTLEN        Packet Length                                    */
    {CC1101_SYNC0    ,0x91}, /*SYNC0         Sync Word, Low Byte                              */
    {CC1101_SYNC1    ,0xD3}, /*SYNC1         Sync Word, High Byte                             */
    {CC1101_FIFOTHR  ,0x07}, /*FIFOTHR       RX FIFO and TX FIFO Thresholds                   */
    {CC1101_IOCFG0   ,0x06}, /*IOCFG0        GDO0 Output Pin Configuration                    */
    {CC1101_IOCFG1   ,0x2E}, /*IOCFG1        GDO1 Output Pin Configuration                    */
    {CC1101_IOCFG2   ,0x29}  /*IOCFG2        GDO2 Output Pin Configuration                    */
};

static const uint8 per869MHzPowerTable[]=   
{
  0x03, /* -xx dBm - min */
  0x3C /*  20 dBm - max */
};

//add per915MHzPowerTable
static const uint8 per915MHzPowerTable[]=
{
  0x03, /* -xx dBm - min*/
  0xCA  /*  26 dBm - max*/
};

static uint8 CC1101_FREQs[2][3] =
{
  {0x20,0x34,0x62}, /* 869 MHz */
  {0x23,0x31,0x3B}  /* 915 MHz */
};

static const int8 paPowerGuiValues[2] = 
{
  20, /* - index 0 */
  26 /* - index 1 */
};

/* Sensitivity table for CC1190 869MHz */
static const int16 perSensitivityTable[]=
{
  -115, // 1.2 kbps
  -118, // 1.2 kBps SENSITIVITY 
  -103, // 4.8 kbps
  -107  // 38.4 kbps
};
static const int16 perSensitivityTable915MHz[]=
{
  -112, // 9.6 kbps
  -108, // 50 kBps  
  -104, // 115 kbps
  -98   // 300 kbps
};

static const float dataRate869MHz[4] =
{
    1.20,  /* <=> SMARTRF_CONFIGURATION_0 */
    1.20,  /* <=> SMARTRF_CONFIGURATION_1 */
    4.80,  /* <=> SMARTRF_CONFIGURATION_2 */
   38.42   /* <=> SMARTRF_CONFIGURATION_3 */
};
static const float dataRate915MHz[] =
{
    9.60,  /* <=> SMARTRF_CONFIGURATION_0 */  
   50.00,  /* <=> SMARTRF_CONFIGURATION_1 */
  115.00,  /* <=> SMARTRF_CONFIGURATION_2 */
  300.00   /* <=> SMARTRF_CONFIGURATION_3 */
};

/******************************************************************************
 * FUNCTIONS
 */




/******************************************************************************
* @fn          perCC1101CC1190GetGuiTxPower
*
* @brief       Returns the TX power in [dBm] used by the menu system. 
*              Implemented by LUT.
*
* input parameters
*
* @param       index - index to GUI TX power LUT
*                  
* output parameters
*
* @return      TX power [dBm]
*/
int8 perCC1101CC1190GetGuiTxPower(uint8 index)
{
  return paPowerGuiValues[index];
}
/******************************************************************************
* @fn          perCC1101CC1190GetDataRate
*
* @brief       Returns the data rate corresponding to the selected
*              Smart RF configuration
*
* input parameters
*
* @param       index - index to data rate table 
*                  
* output parameters
*
* @return      data rate
*/
float perCC1101CC1190GetDataRate(uint8 index)
{
    if(perSettings.frequencyBand == 0)
        return dataRate869MHz[index];
    else 
        return dataRate915MHz[index];
    
} 

/******************************************************************************
* @fn          perCC1101CC1190SetOutputPower
*
* @brief       Configures the output power of CC1101 to 20 dBm

*
* input parameters
*
* @param       index - index to power table <=> wanted output level
*                  
* output parameters
*
* @return      none
*/
void perCC1101CC1190SetOutputPower(uint8 index)
{
  
  uint8 level;
    switch(perSettings.frequencyBand)
    {
    case 0:
      level = per869MHzPowerTable[index];
      break;
    case 1:
       level = per915MHzPowerTable[index];
       break;
    default:
       level = per869MHzPowerTable[index];
       break;
    }
      
  cc1101SpiWriteReg(CC1101_PA_TABLE0,&level,1);
  return;
}

/******************************************************************************
 * @fn          perCC1101CC1190RegConfig
 *
 * @brief       Configures the CC1101 radio with the selected SmartRF Studio
 *              paramters and test properties or the base configuration 
 *              with no address check. Assumes that the radio is in IDLE.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void perCC1101CC1190RegConfig(void)
{
   // make switch statement to set ref settings between 869 and 915 
    
  /* initialize radio registers given the selected perSettings */
  uint8 data;
  
  /* Log that radio is in IDLE state */
  cc1101cc1190RxIdle();
  cc1101cc1190RadioTxRx = CC1101_STATE_IDLE;
  

 if(perSettings.frequencyBand == 0)
 {
     /* Extract what radio configuration to use */
      if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
      {
        switch(perSettings.smartRfConfiguration)
        {
        case 0:
          for(uint16 i = 0; i < (sizeof lowDataRateCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = lowDataRateCC1101CC1190RfSettings[i].data;
            cc1101SpiWriteReg(lowDataRateCC1101CC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable[0];
          break;
        case 1:
          for(uint16 i = 0; i < (sizeof lowDataRateSensCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = lowDataRateSensCC1101CC1190RfSettings[i].data;
            cc1101SpiWriteReg(lowDataRateSensCC1101CC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable[1];
          break;
        case 2:
          for(uint16 i = 0; i < (sizeof mediumDataRateCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = mediumDataRateCC1101CC1190RfSettings[i].data;
            cc1101SpiWriteReg(mediumDataRateCC1101CC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable[2];
          break;
        case 3:
          for(uint16 i = 0; i < (sizeof highDataRateCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = highDataRateCC1101CC1190RfSettings[i].data;
            cc1101SpiWriteReg(highDataRateCC1101CC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable[3];
          break;
        default:
          for(uint16 i = 0; i < (sizeof highDataRateCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = highDataRateCC1101CC1190RfSettings[i].data;
            cc1101SpiWriteReg(highDataRateCC1101CC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable[3];
          break;
        }
        /* add support for configuratos mode */
      }
      else
      {
        for(uint16 i = 0; i < (sizeof mediumDataRateCC1101CC1190RfSettings/sizeof(registerSetting_t));i++)
        {
          data = mediumDataRateCC1101CC1190RfSettings[i].data;
          cc1101SpiWriteReg(mediumDataRateCC1101CC1190RfSettings[i].addr,&data,1);
        }
        perSettings.sensitivity = perSensitivityTable[2];
      }
 }
 else
 {
          /* Extract what radio configuration to use */
      if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
      {
        switch(perSettings.smartRfConfiguration)
        {
        case 0:
          for(uint16 i = 0; i < (sizeof lowDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = lowDataRate915MHzCC1190RfSettings[i].data;
            cc1101SpiWriteReg(lowDataRate915MHzCC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable915MHz[0];
          break;
        case 1:
          for(uint16 i = 0; i < (sizeof mediumDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = mediumDataRate915MHzCC1190RfSettings[i].data;
            cc1101SpiWriteReg(mediumDataRate915MHzCC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable915MHz[1];
          break;
        case 2:
          for(uint16 i = 0; i < (sizeof highDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = highDataRate915MHzCC1190RfSettings[i].data;
            cc1101SpiWriteReg(highDataRate915MHzCC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable915MHz[2];
          break;
        case 3:
          for(uint16 i = 0; i < (sizeof veryHighDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = veryHighDataRate915MHzCC1190RfSettings[i].data;
            cc1101SpiWriteReg(veryHighDataRate915MHzCC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable915MHz[3];
          break;
        default:
          for(uint16 i = 0; i < (sizeof highDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
          {
            data = highDataRate915MHzCC1190RfSettings[i].data;
            cc1101SpiWriteReg(highDataRate915MHzCC1190RfSettings[i].addr,&data,1);
          }
          perSettings.sensitivity = perSensitivityTable915MHz[3];
          break;
        }
        /* add support for configuratos mode */
      }
      else
      {
        for(uint16 i = 0; i < (sizeof highDataRate915MHzCC1190RfSettings/sizeof(registerSetting_t));i++)
        {
          data = highDataRate915MHzCC1190RfSettings[i].data;
          cc1101SpiWriteReg(highDataRate915MHzCC1190RfSettings[i].addr,&data,1);
        }
        perSettings.sensitivity = perSensitivityTable915MHz[2];
      }
 }
 
  /* Differences from recommended studio values and values needed */
  cc1101SpiWriteReg(CC1101_MCSM2, CC1101_MCSMs,3);
  
  /* Correct for chosen frequency band */
  cc1101SpiWriteReg(CC1101_FREQ2, CC1101_FREQs[perSettings.frequencyBand],3);

  
  if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
  {
    /* PKTLEN set to user specified packet length: HW length filtering */
    cc1101SpiWriteReg(CC1101_PKTLEN, &(perSettings.payloadLength),1);
    /* Turn on HW Address filtering */
    data = 0x05;
    cc1101SpiWriteReg(CC1101_PKTCTRL1,&data,1);
    /* Set address */
    cc1101SpiWriteReg(CC1101_ADDR,&perSettings.address,1);
    /* Two way uses different MCSM1 settings */
    if((perSettings.linkTopology == LINK_2_WAY))
    {
      if(perSettings.deviceMode == SLAVE_DEVICE)
      {
      	/* RX after RX, IDLE after TX --> Calibration prioritized */ 
      	/* RX->TX forced by command strobe */
        data = 0x0C;
        cc1101SpiWriteReg(CC1101_MCSM1,&data,1);
      }
      else
      {
        data = 0x03;
        cc1101SpiWriteReg(CC1101_MCSM1,&data,1);
      }
    }
  }
  else
  {
    /* length of configuration packet + filter byte */
    uint8 pktlen = PER_SETTINGS_PACKET_LEN; 
    cc1101SpiWriteReg(CC1101_PKTLEN, &pktlen,1);
  }
}

/******************************************************************************
 * @fn          perCC1101CC1190SendPacket
 *
 * @brief       Sends the contents that pData points to. pData has the 
 *              following structure:
 *
 *              txArray[0] = length byte
 *              txArray[n] = payload[n]
 *              | n<[sizeOf(RXFIFO)-2], variable packet length is assumed.
 * 
 *              The radio state after completing TX is dependant on the 
 *              MCSM1 register setting. This function enables SYNC interrupt. 
 *              This means that an interrupt will go off when a packet 
 *              has been sent, i.e sync signal transitions from high to low.
 *              MSP will be in low power mode until packet has been sent given
 *              that no other interrupts go off.
 *             
 *              The One-Way PER test disables the sync pin interrupt when TX
 *              finishes, while the Two-Way PER test doesn't to enable quick
 *              reception of Slave ACK.
 *
 *              Note: Assumes chip is ready
 *
 * input parameters
 *             
 * @param       *pData - pointer to data array that starts with length byte
 *                       and followed by payload.
 * output parameters
 *
 * @return      none
 */
void perCC1101CC1190SendPacket(uint8 *pData)
{
  uint8 len = *pData;
  
  /* Enable PA and disable LNA on CC1190. Change FIFOTHR and TEST1 register 
   * values for TX operation.
   */
  perCC1101CC1190Tx(); //insert freq dependent reg settings
  
  /* Will only try to transmit if the whole packet can fit i RXFIFO 
   * and we're not currently sending a packet.
   */
  if(!(len > (PER_MAX_DATA-2)) && (cc1101cc1190RadioTxRx != CC1101_STATE_TX) )
  {
    cc1101SpiWriteTxFifo(pData,(len+1));
    /* Indicate state to the ISR and issue the TX strobe */
    trxEnableInt();
    cc1101cc1190RadioTxRx = CC1101_STATE_TX; 
    trxSpiCmdStrobe(CC1101_STX);
    /* Wait until packet is sent before doing anything else */
    __low_power_mode_3();
    while(cc1101cc1190RadioTxRx == CC1101_STATE_TX);
    if(perSettings.linkTopology == LINK_1_WAY)
    {
      /* Back in Idle*/
      trxDisableInt();
    }
  }
  return;
}

/******************************************************************************
 * @fn          perCC1101CC1190EnterRx
 *
 * @brief       Enters RX. Function is used to abstract the cc1101cc1190RadioTxRx
 *              functionality away from the lower-level radio interface.
 *              Note: assumes chip is ready
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void perCC1101CC1190EnterRx(void)
{
   /* Enable PA and disable LNA on CC1190. Change FIFOTHR and TEST1 register 
   * values for TX operation.
   */
  perCC1101CC1190Rx();
    
  cc1101cc1190IdleRx();
  cc1101cc1190RadioTxRx = CC1101_STATE_RX;
}

/******************************************************************************
 * @fn          perCC1101CC1190EnterSleep
 *
 * @brief       Enters Sleep. Function is used to abstract the cc1101cc1190RadioTxRx
 *              functionality away from the lower-level radio interface.
 *              Note: assumes chip is ready
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void perCC1101CC1190EnterSleep(void)
{
  cc1101cc1190RxIdle();
  trxSpiCmdStrobe(CC1101_SPWD);
  /* Only important to differ between RX/TX and IDLE */
  cc1101cc1190RadioTxRx = CC1101_STATE_IDLE;
}

/******************************************************************************
 * @fn          perCC1101CC1190EnterIdle
 *
 * @brief       Enters IDLE from ANY state. Function is used to abstract the 
 *              cc1101cc1190RadioTxRx functionality away from the lower-level radio 
 *              interface.
 *
 * input parameters
 *            
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void perCC1101CC1190EnterIdle(void)
{
  /* wait until chip is ready */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  cc1101cc1190RxIdle();
  cc1101cc1190RadioTxRx = CC1101_STATE_IDLE;
}

/******************************************************************************
 * @fn          cc1101cc1190RxISR
 *
 * @brief       ISR that's called when sync signal goes low. 
 *              In RX State: Filters incoming data. The global rxData pointer
 *              always points to this functions static rxData_tmp(struct of
 *              same kind). The validnes of rxData fields is indicated by the
 *              the global flag packetSemaphore.
 *              In TX State: Nothing is done except it facilitates power 
 *              consumption reduction when TX since the program doesn't need
 *              to wait until TX is done before re-enabling sync pin interrupt.
 *              cc1101cc1190RadioTxRx is also set to CC1101_STATE_IDLE to be consistent with
 *              program.
 * 
 * input parameters
 *             
 * @param       none
 *
 * output parameters 
 *
 * @return      none
 */
void perCC1101CC1190RxTxISR(void)
{
  uint8 rxBytes,rxBytesVerify,rxLength,rssiIndex,lqiIndex;
  /* This variable stores the data locally. Access is given to per_test by 
   * assigning this instance to the global rxData pointer
   */
  static rxData_t rxData_tmp;
  
  rxData = &rxData_tmp;
  /* Checking if the chip is in RX state: */
  if(cc1101cc1190RadioTxRx != CC1101_STATE_RX)
  {
    /* Transmission finished */
    if((perSettings.deviceMode == MASTER_DEVICE) && (perSettings.linkTopology == LINK_2_WAY) && (perSettings.masterSlaveLinked == PER_DEVICE_LINKED))
    {
      /* RX after TX only applicable when master in 2-way test */
      cc1101cc1190RadioTxRx=CC1101_STATE_RX;
    }
    else
    {
      /* IDLE after TX in all other cases */
      cc1101cc1190RadioTxRx  = CC1101_STATE_IDLE;
    }
    return;
  }
  
  packetSemaphore |= SYNC_FOUND;
  /* Only relevant for 1-way PER test. In case of receiver not finding sync, 
   * the MSP will sample the RSSI value right after the instant where the packet
   * was supposed to be received. By setting the 32k timer at this point, the 
   * sample instant will be very close to the end of the wanted packet. The RSSI
   * value will hence hold lot of the signal power from the packet.
   */
  if(((perSettings.masterSlaveLinked == PER_DEVICE_LINKED)||(perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS)) && (perSettings.deviceMode == MASTER_DEVICE) && (perSettings.testRunning == PER_TRUE))
  {
  	if(perSettings.linkTopology == LINK_1_WAY)
  	{
  	  /* Read timer value and set the perSettings.packetRate valu(adjustment for temperature drift */
      halTimer32kSetIntFrequency(perSettings.packetRate);
      halTimer32kIntEnable();
    }
    else
    {
    	/* LINK_2_WAY */ 
    	
    	/* Timeout interrupt configuring is handled by the 2-way Per test */
      timer32kValue = halTimer32kReadTimerValue();
    	halTimer32kAbort();
    }
  }
  /* The RXBYTES register must read the same value twice
   * in a row to guarantee an accurate value
   */
  cc1101SpiReadReg(CC1101_RXBYTES,&rxBytesVerify,1);
  do
  {
    rxBytes = rxBytesVerify;
    cc1101SpiReadReg(CC1101_RXBYTES,&rxBytesVerify,1);
  }
  while(rxBytes != rxBytesVerify);
  
  /* Checking if the FIFO is empty */
  if(rxBytes == PER_FALSE)
  {
    /* The packet was removed by HW due to addr or length filtering -> Do nothing */
    /* Report that a sync was detected */ 
    rxData_tmp.rssi = perCC1101Read8BitRssi();
    return;
  }
  else
  {
    /* The RX FIFO is not empty, process contents */    
    cc1101SpiReadRxFifo(&rxLength, 1);  
    /* Check that the packet length just read + FCS(2B) + length byte match the RXBYTES */
    /* If these are not equal:
     * - RXFIFO overflow: Received packets not processed while beeing in RX. 
     */
    if(rxBytes != (rxLength+3))
    {
      /* This is a fault FIFO condition -> clean FIFO and register a sync detection */
      /* IDLE -> FLUSH RX FIFO -> RX */
      cc1101cc1190RxIdle();
      cc1101cc1190IdleRx();
      /* Report that a sync was detected */
      rxData_tmp.rssi = perCC1101Read8BitRssi();
      return;
    }
    else
    {
      /* We don't have a FIFO error condition -> get packet */
      
      /* Length Field */
      rxData_tmp.data[0] = rxLength;
      rssiIndex = rxLength+1;
      lqiIndex  = rssiIndex +1;
      
      /* Payload(ADDR + DATA + FCS) */
      cc1101SpiReadRxFifo(&rxData_tmp.data[1], lqiIndex);
      
      /* The whole packet has been read from the FIFO.
       * Check if the CRC is correct and that the packet length is as expected.
       * If not correct: report sync found and do not update RSSI or LQI.
       */
      if( (!(rxData_tmp.data[lqiIndex] & CC1101_LQI_CRC_OK_BM)) || (perSettings.payloadLength != rxLength ))
      {
        rxData_tmp.rssi = perCC1101Read8BitRssi();
        return;
      }
      /* A complete error-free packet has arrived  */
      rxData_tmp.length  = rxLength;
      rxData_tmp.lqi     = rxData_tmp.data[lqiIndex] & CC1101_LQI_EST_BM;
      rxData_tmp.addr    = rxData_tmp.data[1]; /* May not be the address, dependant on if this is used or not */
     
      /* Convert RSSI value from 2's complement to decimal value accounting for offset value */
      rxBytes = rxData_tmp.data[rssiIndex];
      rxData_tmp.rssi = cc1101cc1190Convert8BitRssi(rxBytes);
      /* Signal a good packet is received */
      packetSemaphore |= PACKET_RECEIVED;
      return;
    }
  } 
}
/*******************************************************************************
 * @fn          perCC1101CC1190Read8BitRssi
 *
 * @brief       Reads RSSI value from register, converts the dBm value to
 *              decimal and adjusts it according to RSSI offset
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      decimal RSSI value corrected for RSSI offset
 */ 
int8 perCC1101CC1190Read8BitRssi(void)
{
  uint8 rssi2compl,rssi2compl_1;
  int16 rssiConverted;
  
  /* Read RSSI from MSB register */
  cc1101SpiReadReg(CC1101_RSSI, &rssi2compl,1);
  do
  {
    rssi2compl_1 = rssi2compl;
    cc1101SpiReadReg(CC1101_RSSI,&rssi2compl,1);
  }
  while(rssi2compl_1 != rssi2compl);
  
  rssiConverted = cc1101cc1190Convert8BitRssi(rssi2compl);
  return rssiConverted;
}

static int8 cc1101cc1190Convert8BitRssi(uint8 rawRssi)
{
  int16 rssiConverted;
  
  if(rawRssi >= 128)
  {
    rssiConverted = (int16)(((int16)(rawRssi-256)/2) - cc1101cc1190RssiOffset);
  }
  else
  {
    rssiConverted = (int16)((rawRssi/2) - cc1101cc1190RssiOffset);
  }
  /* Restricting to 8 bit signed number range */
  if(rssiConverted < -128)
  {
    rssiConverted = -128;
  }
  return (int8)rssiConverted;
} 

/*******************************************************************************
 * @fn          cc1101cc1190RxIdle
 *
 * @brief       Radio state is switched from RX to IDLE
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      none
 */ 
static void cc1101cc1190RxIdle(void)
{
  /* Disable pin interrupt */
  trxDisableInt();
  /* Strobe IDLE */
  trxSpiCmdStrobe(CC1101_SIDLE); 
  /* Wait until chip is in IDLE */
  while(trxSpiCmdStrobe(CC1101_SNOP) & 0xF0);
  /* Flush the Receive FIFO */
  trxSpiCmdStrobe(CC1101_SFRX);
  /* Clear pin interrupt flag */
  trxClearIntFlag();
}

/*******************************************************************************
 * @fn          cc1101cc1190IdleRx
 *
 * @brief       Radio state is switched from Idle to RX. Function assumes that
 *              radio is in IDLE when called. 
 * 
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      none
 */ 
static void cc1101cc1190IdleRx(void)
{
  trxClearIntFlag();
  trxSpiCmdStrobe(CC1101_SRX);
  trxEnableInt();
}

/*******************************************************************************
 * @fn          perCC1101CC1190WriteTxFifo
 *
 * @brief       Means for PER test to write the TX FIFO
 * 
 * input parameters
 *
 * @param       *pData  - pointer to data array that will be written to TX Fifo
 * @param       len     - number of bytes in that will be written to TX Fifo
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190WriteTxFifo(uint8 *pData, uint8 len)
{
	cc1101SpiWriteTxFifo(pData,len);
}

/*******************************************************************************
 * @fn                  perCC1101CC1190Tx
 *
 * @brief               Enables PA on CC1190 and disables LNA
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190Tx(void)
{
    if(perSettings.frequencyBand == 0)
    {
     /*FOR 868MHz only*/
     /* Change FIFOTHR and TEST1 register on CC1101 for TX operation with CC1190*/
     cc1101SpiWriteReg(CC1101_FIFOTHR,&cc1101cc1190FifothrTx,1);
     cc1101SpiWriteReg(CC1101_TEST1,&cc1101cc1190Test1Tx,1);
    }
    //perCC1101CC1190HgmEnable();
    perCC1101CC1190LnaDisable();
    perCC1101CC1190PaEnable();
    
}
/*******************************************************************************
 * @fn                  perCC1101CC1190Rx
 *
 * @brief               Enables LNA on CC1190 and disables PA
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190Rx(void)
{
    if(perSettings.frequencyBand == 0)
    {
     /* Change FIFOTHR and TEST1 register on CC1101 for TX operation with CC1190*/
     cc1101SpiWriteReg(CC1101_FIFOTHR,&cc1101cc1190FifothrRx,1);
     cc1101SpiWriteReg(CC1101_TEST1,&cc1101cc1190Test1Rx,1);
    }
    //perCC1101CC1190HgmEnable();
    perCC1101CC1190PaDisable();
    perCC1101CC1190LnaEnable();
    
}
/*******************************************************************************
 * @fn                  perCC1101CC1190PaEnable
 *
 * @brief               Enables PA on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190PaEnable(void)
{
    TRXEM_CC1190_PORT_SEL &= ~(TRXEM_CC1190_PA);  // Set pin to zero for I/O function
    TRXEM_CC1190_PORT_DIR |=  (TRXEM_CC1190_PA);  // Set pin direction to output
    TRXEM_CC1190_PORT_OUT |=  (TRXEM_CC1190_PA);  // Set output pin high
}
/*******************************************************************************
 * @fn                  perCC1101CC1190LnaEnable
 *
 * @brief               Enables LNA on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190LnaEnable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_LNA);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_LNA);
    TRXEM_PORT_OUT |=  (TRXEM_CC1190_LNA);
}
/*******************************************************************************
 * @fn                  perCC1101CC1190PaDisable
 *
 * @brief               Disables PA on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190PaDisable(void)
{
    TRXEM_CC1190_PORT_SEL &= ~(TRXEM_CC1190_PA);  // Set pin to zero for I/O function
    TRXEM_CC1190_PORT_DIR |=  (TRXEM_CC1190_PA);  // Set pin direction to output
    TRXEM_CC1190_PORT_OUT &= ~(TRXEM_CC1190_PA);  // Set output pin low
}
/*******************************************************************************
 * @fn                  perCC1101CC1190LnaDisable
 *
 * @brief               Disables LNA on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190LnaDisable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_LNA);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_LNA);
    TRXEM_PORT_OUT &= ~(TRXEM_CC1190_LNA);    
}
/*******************************************************************************
 * @fn                  perCC1101CC1190HgmEnable
 *
 * @brief               Enables HGM on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190HgmEnable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_HGM);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_HGM);
    TRXEM_PORT_OUT |=  (TRXEM_CC1190_HGM);
}
/*******************************************************************************
 * @fn                  perCC1101CC1190HgmDisable
 *
 * @brief               Disables HGM on CC1190
 * 
 * input parameters
 *
 * @param       
 *
 * output parameters
 *
 * @return      none
 */ 
void perCC1101CC1190HgmDisable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_HGM);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_HGM);
    TRXEM_PORT_OUT &= ~(TRXEM_CC1190_HGM);
}