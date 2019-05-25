//*****************************************************************************
//! @file   per_test.h
//!  
//! @brief: This header file denotes the interface between the GUI, CORE
//          and API parts of the Packet Error Rate(PER) tester for trxeb. 
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

#ifndef PER_TEST_H
#define PER_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
#include "chip_detect.h"
#include "hal_lcd_trxeb.h"
#include "menu_system.h"
 
/******************************************************************************
* CONSTANTS
*/ 

#define PER_CONFIGURATION_PAYLOAD          7 /* #config bytes that must be sent from master -> slave */
#define PER_MAX_AVG_WINDOW_SIZE           64
#define PER_RSSI_RINGBUFFER_INIT           0
#define PER_SLAVE_STATUS_LEN              16
#define PER_MASTER_STATUS_LEN             11
#define LINK_1_WAY 0
#define LINK_2_WAY 1

#define EASY_MODE 0
#define EXPERT_MODE 1 
#define CONFIGURATOR_MODE 2
#define LINK_BYPASS_MODE 3
#define CC1101_DEFAULT_PACKETLENGTH 20
#define CC11xL_DEFAULT_PACKETLENGTH 20
#define CC112X_DEFAULT_PACKETLENGTH 3
#define CC120X_DEFAULT_PACKETLENGTH 3  

#define MASTER_DEVICE 1
#define SLAVE_DEVICE 0

#define PER_SETTINGS_PACKET_LEN            9
#define PER_MAX_DATA                      64 

/* Values used to signal state of RXISR */
#define SYNC_FOUND                        BIT0
#define PACKET_RECEIVED                   BIT7

#define PER_TRUE   1
#define PER_FALSE  0

#define RSSI_LOW                          -127
#define TIMER_32K_CLK_FREQ               32768

#define PER_RETURN_SUCCESS      0
#define PER_RETURN_FAILURE      1
#define PER_DEVICE_LINKED       1
#define PER_DEVICE_NOT_LINKED   0

#define PER_DEVICE_LINK_BYPASS  2  
  
#define SMARTRF_CONFIGURATION_0 0
#define SMARTRF_CONFIGURATION_1 1
#define SMARTRF_CONFIGURATION_2 2
#define SMARTRF_CONFIGURATION_3 3
#define DEFAULT_RSSI_AVERAGE_WIN_SIZE 16
/******************************************************************************
 * TYPEDEFS
 */
 

/* Definition of perSettings_t
 * Description: Stores data needed to execute test 
 * Fields:
 * - payloadLength            : Number of bytes transmitted - length byte
 * - packetRate               : The rate at which packets are sent
 * - txPower                  : The preferred TX output power
 * - smartRfConfiguration     : Determines what SmartRF register settings that will be used 
 * - linkTopology             : Select if one-way or a two-way retransmit test will be executed
 * - frequencyBand            : The frequency band selected
 *                              0: 169 MHz, 1: 416 MHz, 2: 868 MHz, 3: 915 MHz
 * - address                  : Address of the nodes. Will be the same for both nodes. > 0 implies filtering.
 *                              The address will be obtained by sampling RSSI and light sensor, XOR both + 1
 * - totalNumPackets          : Total number of packets to receive
 * - rssiWindowSize           : Selected windowing size for RSSI = {0,2,4,8,16,32,64}  
 * - sensitivity              : Sensitivity the chosen radio configuration gives.
 * - nAllowedRetransmits      : Two-way PER test: If the number of re-transmits exceeds this number, the packet
 *                              is considered lost. A re-transmit will happen if the prevoius sent packet
 *                              wasn't ACK'ed by the slave. 
 * - deviceMode               : deviceMode = 0 for slave mode and deviceMode = 1 for master mode.
 * - masterSlaveLinked        : Equals PER_DEVICE_LINKED if linking master and slave proved successfull. PER_DEVICE_NOT_LINKED otherwise.
 *
 */
typedef struct
{
  uint8  payloadLength;  
  uint16 packetRate;
  uint8  txPower;
  uint8  smartRfConfiguration;
  uint8  linkTopology;
  uint8  frequencyBand;
  uint8  address; 
  uint16 totalNumPackets;
  uint8  rssiWindowSize;
  int16   sensitivity;
  uint8  nAllowedRetransmits;
  uint8  deviceMode;
  uint8  masterSlaveLinked;
  uint8  testRunning;
} perSettings_t;

/* Definition of struct perStatistics 
 * Description: Stores data collected by the test
 * Fields:
 *  - rssiBuffer              : Ring buffer used to average RSSI values
 *  - rssiBufferCounter       : Head of buffer
 *  - sumRssi                 : Sum of buffer of chosen window size
 *  - avgRSSI                 : sumRssi/chosen window size
 *  - testProgressNominator   : number of packets that should have been received 
 *  - testProgressDenominator : Total number of packets to receicve(subject to reallocation) 
 *  - numberOfGoodPackets     : Instantaneous number of good packets received 
 *  - numberOfLostPackets     : Instantaneous number of lost packets
 *  - currentPer              : numberOfLostPackets/testProgressNominator
 *  - graphBuffer             : Buffer where LCD graphic is stored
 *  - sensitivity             : Sensitivity for current RF settings(subject to reallocation) 
 *  - remainingLinkMargin     : sensitivity - avgRSSI
 *  - progress                : testProgressNominator/testProgressDenominator
 *  - nSyncsFound             : Number of sync detections during the test. 
 *  - nRetransmits            : Used in two-way PER test to indicate how many times a packet has been
 *                              retransmitted since an ACK wasn't received on the previous packet. 
 */
typedef struct
{
  int8     rssiBuffer[PER_MAX_AVG_WINDOW_SIZE];
  uint8    rssiBufferCounter;
  int16    sumRSSI;
  int8     avgRSSI;
  uint16   testProgressNominator;    
  uint16   testProgressDenominator;
  uint16   numberOfGoodPackets;      
  uint16   numberOfLostPackets;      
  float    currentPer;               
  char     graphBuffer[LCD_BYTES];   
  int8     remainingLinkMargin;      
  int16    sensitivity;             
  float    progress;               
  uint32   nSyncsFound;   
  uint32   nRetransmits;             
  
}perStatistics_t;



/* Function pointer types for RF API */
typedef void  (*VFPTR_U8)(uint8 a);
typedef void  (*VFPTR_ISR_FUNC_PTR)(ISR_FUNC_PTR a);
typedef void  (*VFPTR_U8PTR)(uint8 *a);
typedef void  (*VFPTR_U8PTR_U8)(uint8 *a, uint8 b);
typedef uint8 (*U8FPTR_V)(void);
typedef int8  (*I8FPTR_U8)(uint8 a);
typedef int8  (*I8FPTR_V)(void);
typedef float (*FLOATFPTR_U8)(uint8 a);


/* RF API struct that will be assigned at startup */
typedef struct
{
  VFPTR               perTrxRegConfig;        /* Radio specific configuration of registers   */
  VFPTR_ISR_FUNC_PTR  perConnectTrxIsr;       /* Connects the provided perTrxIsr to P1.7 */
  VFPTR               perTrxIsr;              /* Radio specific ISR that sets packetSemaphore, assigned to port/pin */
  VFPTR_U8            perSetTxPower;          /* Sets a specfic TX power */
  I8FPTR_U8           perGetGuiTxPower;       /* Gets the GUI TX power */
  VFPTR               perEnterRX;             /* Put radio in RX */
  VFPTR_U8PTR         perSendPacket;          /* Sends a packet according to variable length transmission. 
                                               * The time until transmission begins and what state the radio returns to after
                                               * TX is dependant on the radio settings.
                                               */
  VFPTR               perEnterIdle;           /* Puts radio in IDLE  */                 
  VFPTR               perEnterSleep;          /* Puts radio in SLEEP */
  VFPTR_U8PTR_U8      perWriteTxFifo;         /* Writes the TX FIFO  */
  U8FPTR_V            perSampleSyncPin;       /* Samples the Sync pin given that radio is RX */
  FLOATFPTR_U8        perGetDataRate;         /* Function that provides a data rate given smart rf configuration */
  I8FPTR_V            perGetRssi;              /* Reads RSSI from register. Returns decimal representation corrected for offset */
  
}perRfApi_t;

/* Definition of trxData_t struct
 * Description: Global instance of struct used for receiving data packets <= 61 payload bytes.  
 *              Will be utilized in ccxxxx_per_test_api.c. It's global so that data space is saved among
 *              the different radios supported and access is quicker. No mutex is implemented.
 *              If using the recived data actively, it should be copied while interrupts are disabled.
 * Fields:
 * - data      : [length(1B):payload(<=61B):FCS(2B)]
 * - rssi      : RSSI value of the last CORRECTLY received packet
 * - lqi       : LQI of last CORRECTLY received packet
 * - length    : Length of the last CORRECTLY received packet's payload
 * - addr      : The address contained in the last CORRECTLY received packet. The byte following 
 *               the length byte is always considered to be the address byte. 
 * 
 */
typedef struct
{
  uint8 data[PER_MAX_DATA]; 
  int8  rssi;
  uint8 lqi;
  uint8 length;
  uint8 addr;
}rxData_t;

/******************************************************************************
 * GLOBALS
 */ 

/* CORE */
extern volatile uint8 packetSemaphore; /* Set by Pacet ISR, unset by program, defined in per_test.c */
extern volatile uint16 timer32kValue; /* Set by Master's Sync ISR when 2 - way PER test is executed  */
extern rxData_t *rxData;



/* CORE -> GUI  : Results */
extern perStatistics_t perStatistics;

/* CORE -> GUI : Configuration */
extern char slaveStatus[PER_SLAVE_STATUS_LEN];
extern char slaveConfigureStatus[PER_MASTER_STATUS_LEN];
extern const char slaveConfigureStatus0[PER_MASTER_STATUS_LEN];
extern perRfApi_t perRfApi;

/* GUI -> CORE : Configuration */
extern perSettings_t perSettings;
extern uint8         perDeviceMode;
extern menu_t        perSlaveMenu;
extern menu_t        perAbstractHeadMenu;
extern menu_t        perCC1101ChipSelectMenu;
extern menu_t        perCC1120ChipSelectMenu;

/* Head of PER Menu system*/
extern menu_t perHeadMenu;
extern radioChipType_t perRadioChipType;
/******************************************************************************
 * PROTOTYPES
 */
 
/* Interrupt functions */
void trxIsrConnect(void (*pfnIntHandler)(void));
void trxDisableInt(void);
void trxEnableInt(void);
void trxClearIntFlag(void);
uint8 trxSampleSyncPin(void);

/* GUI -> CORE : Configuration */

uint8 perInitApp(void** pChipSelect);
uint8 perChipSelectApp(void** pDummy);
uint8 perMasterConfigureDevicesApp(void **pDummy);
uint8 perMasterConfigureDevicesLinkBypassApp(void **pDummy);
uint8 perSlaveStartApp(void** pDummy);
uint8 perSlaveStartLinkBypassApp(void** pDummy);
uint8 perMasterStartTestApp(void **pDummy);
void perResetMenuVariables(void);

uint8 perSlaveStartValueLineApp(void** pDummy);
uint8 perMasterConfigureDevicesValueLineApp(void **pDummy);

/* CORE -> GUI : Configuration */
void cpyStatusString(char *pStringTo, const char *pStringFrom, uint8 length);

#ifdef  __cplusplus
}
#endif

#endif //PER_TEST_H