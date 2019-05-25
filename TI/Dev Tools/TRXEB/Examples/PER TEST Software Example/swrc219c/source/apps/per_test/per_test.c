//*****************************************************************************
//! @file   per_test.c
//
//! @brief  Implementation file for the trxeb PER tester.
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

#include <msp430.h>
#include "lcd_dogm128_6.h"
#include "chip_detect.h"
#include "hal_timer_32k.h"
#include "hal_create_random.h"
#include "per_test.h"
#include "trx_rf_spi.h"
#include "cc1101_per_test_api.h"
#include "cc110L_per_test_api.h" 
#include "cc113L_per_test_api.h" 
#include "cc115L_per_test_api.h" 
#include "cc112x_per_test_api.h"
#include "cc120x_per_test_api.h"
#include "cc1101_cc1190_per_test_api.h"
#include "cc1120_cc1190_per_test_api.h"
#include "menu_driver.h"
#include "freq_xosc_detect.h"

#include "io_pin_int.h"
#include "bsp.h"
#include "bsp_key.h"

/******************************************************************************
* CONSTANTS
*/ 

#define N_CONFIG_ACKS       3 // NB: if this is changed, the time constant used in loops further down must be adjusted.
#define ISR_ACTION_REQUIRED 1
#define NO_ACTION           0
#define SMARTRF_BASE_REGISTER_SETTINGS 1
#define MIN_OUTPUT_POWER_INDEX 0
#define MAX_OUTPUT_POWER_INDEX 1


/* Strings used for Slave mode status update */
const char slaveStatus0[PER_SLAVE_STATUS_LEN] = " Connecting...  ";
const char slaveStatus1[PER_SLAVE_STATUS_LEN] = "   Linked       ";
const char slaveStatus2[PER_SLAVE_STATUS_LEN] = "Test Running... ";

/******************************************************************************
* LOCAL FUNCTIONS
*/

/* Radio API that the PER test uses */
perRfApi_t perRfApi;

/* PER core functions */
static void per32kTimerISR(void);
static void perSlaveOneWayTest(void);
static void perMasterOneWayTest(void);
static void perSlaveTwoWayTest(void);
static void perMasterTwoWayTest(void);
static void perMasterRefreshResults(void);
static void perMasterInitTestStatisticsAndVariables(void);
static void perMasterUpdateRssiMembers(int8 rssi);    
static uint8 perMasterProcessButtonInput(void);
static void perMasterInitRssiGraph(char *pBuffer);
static void perMasterUpdateRssiGraph(char *pBuffer,int8 rssiValue, char packetReceived);
static uint16 perComputePacketRate(void);
static void perMasterFindPacketSync(void);


/******************************************************************************
* LOCAL VARIABLES
*/ 

/* RSSI graph variables */
static uint8 graphMarkerPos;      /* Holding graph marker's x-pos */           

/* Array that holds data to be transmitted
* - txArray[0] = length byte
*   txArray[n] = payload[n],
*   where addr is the first part of the payload if applicable.
*   Max contents is SIZEOF(RXFIFO) - 2
*/
static uint8 txArray[PER_MAX_DATA-2];

/* 32k timer interrupt flag */
static volatile uint8 perTimerState;

static uint8 syncPinWasHigh;

/* variables controlling the results screen */
static uint8 updateScreen,screenUpdateCounter, screenToggle, screenNumber;

/* Variable to indicate if the PER test is finished or not*/
static uint8 perTestFinished;

/******************************************************************************
* GLOBAL VARIABLES
*/ 

/* Status strings to GUI part */
char slaveStatus[PER_SLAVE_STATUS_LEN];   
char slaveConfigureStatus[PER_MASTER_STATUS_LEN];



/* Used to hold RX data accross different radios. No Mutex is implemented */
rxData_t *rxData;

/* Flag to set by RX ISR routine and PER CORE */
volatile uint8 packetSemaphore;
volatile uint16 timer32kValue;

/* perSettings configuration struct. It controls the PER core */
perSettings_t perSettings;

/* perStatistics stuct that holds collected data from test */
perStatistics_t perStatistics;

/* Detected radio */
radioChipType_t perRadioChipType;

/******************************************************************************
* @fn          perConfigureSlaveApp
*
* @brief       Executed from menu system(App notation). It uses the global 
*              perSettings struct to configure the Slave accordingly. 
*              The slave must ACK that it received the configuration.
* 
*              When the slave is configured, it will start operate in the PER 
*              test mode selected(one-way or two-way with retransmit)
*                
* input parameters
*             
* @param       pointer to pointer to void, value not used
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/
uint8 perMasterConfigureDevicesApp(void **pDummy)
{ 
  /**************************** RF Initialization  ****************************
  * - Set device mode 
  * - Wake up radio.
  * - Start configuration process by initializing radio with "base" settings.
  *   The "base" setting is a common set of radio register parameters at both
  *   slave and the master.
  * - Set minimum Tx power since devices will be close when configurating.
  * - Create random address to be used in test.
  * - Connect the TRX ISR.
  * - Make packet with the neccessary perSettings fields.
  */
  perSettings.packetRate =  perComputePacketRate();
  perSettings.deviceMode = MASTER_DEVICE;
  perSettings.masterSlaveLinked = PER_DEVICE_NOT_LINKED;
  perRfApi.perEnterIdle();
  packetSemaphore = 0;
  uint8 smartRfConfigurationBak = perSettings.smartRfConfiguration;
  uint8 payloadlengthBak = perSettings.payloadLength;
  perSettings.payloadLength = PER_CONFIGURATION_PAYLOAD;
  perSettings.smartRfConfiguration = SMARTRF_BASE_REGISTER_SETTINGS ;
  perSettings.address = 0;
  
  perRfApi.perTrxRegConfig();
  perSettings.smartRfConfiguration = smartRfConfigurationBak;
  perSettings.address = halCreateRandomByte();
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  
  txArray[0] = PER_CONFIGURATION_PAYLOAD;
  txArray[1] = payloadlengthBak;
  txArray[2] = (perSettings.packetRate>>8); /* MSB of packetRate*/
  txArray[3] = perSettings.packetRate;      /* LSB of packetRate*/
  txArray[4] = perSettings.txPower;
  txArray[5] = perSettings.smartRfConfiguration;
  txArray[6] = perSettings.linkTopology;
  txArray[7] = perSettings.address;
  
  /************************** Configuration of Slave **************************
  * (1) Send the configuration packet with lowest power available.
  * (2) Wait on ACK from Slave. Slave will send 2*N_CONFIG_ACKS acks whitin a 
  *     time window of 1 s: N_CONFIG_ACKS with lowest power available and
  *     N_CONFIG_ACKS ACKs with highest power available. The Master needs to
  *     detect at least one of them to state a success. The 32KHz timer is 
  *     used for this purpose.
  * (3) If Master didn't detect an ACK it will try linking with maximum 
  *     available output power. Step (2) is then repeated, and error message is 
  *     reported if linking fails.
  */
  uint8 powerIndex = MIN_OUTPUT_POWER_INDEX;
  while((perSettings.masterSlaveLinked == PER_DEVICE_NOT_LINKED) && (powerIndex<=MAX_OUTPUT_POWER_INDEX)){ 
    perRfApi.perSetTxPower(powerIndex);
    perRfApi.perSendPacket(txArray);
    perTimerState = NO_ACTION;
    perRfApi.perEnterRX();
    halTimer32kIntConnect(&per32kTimerISR);
    halTimer32kSetIntFrequency(2);
    halTimer32kIntEnable();
    __low_power_mode_3();
    
    /* Receive the ACK packets within a period of 1 second, the Slave will have time to acknowledge packets
    * with both minimum and maximum output power.
    */
    while(perTimerState != ISR_ACTION_REQUIRED)
    {
      /* Button interrupt or RX ISR interrupt occurred. Save the value of packetSemaphore
      * in the latter case. Radio will enter IDLE and then RX after each packet received.
      */
      if(packetSemaphore>0)
      {
        /* NOTE: Following 2 lines account for CC112X PG10 errata and is compatible with cc1101 - Should be removed when PG2.0 of CC112X is present */
        /* TXOFF and RXOFF modes are used...*/
        perRfApi.perEnterIdle();
        perRfApi.perEnterRX();
      }
      __low_power_mode_3();
    }
    /* Aborting timer and entering IDLE */
    halTimer32kAbort();
    perRfApi.perEnterIdle();
    perTimerState = NO_ACTION;
    /* The folowing events will break the loop: ACK is received or linking failed with both low and high output power */
    if(packetSemaphore & PACKET_RECEIVED)
    {
      perSettings.masterSlaveLinked = PER_DEVICE_LINKED;
    }
    else
    {  
      perSettings.masterSlaveLinked = PER_DEVICE_NOT_LINKED;
      powerIndex++;
    }
  }
  
  
  
  /***************************** Success Check ********************************
  * - Check if the PACKET_RECEIVED is set in masterSlaveLinked, this will
  *   indicate that configuration is ok. Status of configuration process is 
  *   either way reported to screen. The radio register configuration of the
  *   Master node will happen when PER test is started to saves some power.
  */
  
  /* Abort process if linking failed. Issue error message */
  if(perSettings.masterSlaveLinked != PER_DEVICE_LINKED)
  {
    perSettings.masterSlaveLinked = PER_DEVICE_NOT_LINKED;
    /* The slave and the Master aren't linked properly */
    lcdBufferClear(0);
    lcdBufferPrintString(0,"    Device linking   ",0,eLcdPage2);
    lcdBufferPrintString(0,"        failed       ",0,eLcdPage3);
    lcdBufferPrintString(0,"  Please link again  ",0,eLcdPage4);
    lcdSendBuffer(0);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    /* Restoring the packetlength variable that is used when qualifying a 
    * valid packet upon reception 
    */
    perSettings.payloadLength = payloadlengthBak;
    /* Put radio in sleep until test is started */
    perRfApi.perEnterSleep();
    return PER_RETURN_FAILURE;
  }
  /* Put radio in sleep until test is started */
  perRfApi.perEnterSleep();
  
  /* Restoring the packetlength variable that is used when qualifying a 
  * valid packet upon reception 
  */
  perSettings.payloadLength = payloadlengthBak;
  return PER_RETURN_SUCCESS;
}
/******************************************************************************
* @fn          perMasterConfigureDeviceesLinkBypassApp
*
* @brief       None 
*              
*                
* input parameters
*             
* @param       pointer to pointer to void, value not used
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/
uint8 perMasterConfigureDevicesLinkBypassApp(void **pDummy)
{ 
  perSettings.deviceMode = MASTER_DEVICE;
  perSettings.masterSlaveLinked = PER_DEVICE_LINK_BYPASS;
  perRfApi.perEnterIdle();
  packetSemaphore = 0;
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  /* Configure radio according to perSettings */
  perSettings.payloadLength         = (CC1101_DEFAULT_PACKETLENGTH-1);;
  perSettings.linkTopology          = LINK_1_WAY;
  perSettings.packetRate            = perComputePacketRate();  
  perSettings.address               = 0x00;
  perRfApi.perTrxRegConfig();  
  
  
  
  return PER_RETURN_SUCCESS;
}
/******************************************************************************
* @fn          perMasterConfigureDeviceesValueLineApp
*
* @brief       None 
*              
*                
* input parameters
*             
* @param       pointer to pointer to void, value not used
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/
uint8 perMasterConfigureDevicesValueLineApp(void **pDummy)
{ 
  perSettings.deviceMode = MASTER_DEVICE;
  perSettings.masterSlaveLinked = PER_DEVICE_LINKED;
  perRfApi.perEnterIdle();
  packetSemaphore = 0;
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  perSettings.linkTopology          = LINK_1_WAY;
  perSettings.packetRate            = perComputePacketRate();  
  perSettings.address               = 0x00;
  perRfApi.perTrxRegConfig();  
  
  
  
  return PER_RETURN_SUCCESS;
}
/******************************************************************************   
* @fn          perSlaveStartApp()                                       
*                                                                                
* @brief       Function is run from the menu system. The function will manage   
*              it's own menus. It will configure the device as the Slave 
*              according to the configuration received from the Master. It 
*              will then execute a specific PER test given the received 
*              configuration.
*                 
* input parameters
*
* @param       pDummy - pointer to pointer to void. no value used
*          
* output parameters
*                                                                                
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/                    
uint8 perSlaveStartApp(void** pDummy)
{   
  /* Enable abort by only left and right button pushes */
  uint8 whichButtonPushed; 
  whichButtonPushed = 0x00;
  /*************************** Menu Initialization  ***************************
  * - Update status of slave configuration process 
  * - Write the "menu" to the screen and clearing reserved areas
  * - Set slave node and that linking process is incomplete
  */
  cpyStatusString(slaveStatus,slaveStatus0,PER_SLAVE_STATUS_LEN);
  menuClearReservedArea(&perSlaveMenu);
  menuDisplay(&perSlaveMenu);
  perSettings.deviceMode = SLAVE_DEVICE;
  perSettings.masterSlaveLinked = PER_DEVICE_NOT_LINKED;
  
  /************************* RF Initialization phase **************************
  * - Wake up radio.
  * - Configurate radio to operate with "base" setting.
  * - Reset variables neccessary for configuration of slave to work.
  *   This must be done in case the Slave received the configuration packet 
  *   from the Master, but the Master never got the ACK it expected. In this 
  *   this case the Slave mode must be restarted.
  * - Set minimum output power since devices will be close when configuration
  *   is initiated.
  * - Connect the receive/transmit ISR to interrupt vector
  * - Put radio in RX and put MCU in sleep
  */
  perRfApi.perEnterIdle();
  packetSemaphore            = 0;
  perSettings.payloadLength  = PER_CONFIGURATION_PAYLOAD;
  perSettings.address        = 0;
  perSettings.txPower        = MIN_OUTPUT_POWER_INDEX; 
  perRfApi.perTrxRegConfig();
  perRfApi.perSetTxPower(MIN_OUTPUT_POWER_INDEX);
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  perRfApi.perEnterRX();
  __low_power_mode_3();
  
  /************************* Receive Configuration  ***************************
  * - Wait for configuration packet or abort
  * - When packet is received, send N_CONFIG_ACKS acks to Master
  * - Change configuration according to the received perSettings
  * - Execute the selected test specified by the topology selected and the 
  *   provided parameters
  *
  */
  while(1)
  {
    whichButtonPushed = bspKeyPushed(BSP_KEY_ALL);
    if((whichButtonPushed == BSP_KEY_RIGHT) || (whichButtonPushed == BSP_KEY_LEFT))
    {
      perRfApi.perEnterSleep();
      return PER_RETURN_SUCCESS;
    }
    perRfApi.perEnterIdle(); /* Used as long PG2 silicon of CC112X is not ready */
    if((packetSemaphore & PACKET_RECEIVED) && (rxData->length == PER_CONFIGURATION_PAYLOAD))
    {
      /* Following line accounts for CC112X PG10 errata and is compatible with cc1101 */
      /* perRfApi.perEnterIdle();  Used when PG2 silicon of CC112X is ready */
      /* Configuration packet received */
      for(uint8 i=0;i<N_CONFIG_ACKS;i++)
      {
        /* delay to at least comply with settling time at Master node */
        halTimer32kMcuSleepTicks(4260); /* Wait 130 ms */
        perRfApi.perSendPacket((rxData->data));
      }
      perRfApi.perSetTxPower(MAX_OUTPUT_POWER_INDEX);
      for(uint8 i=0;i<N_CONFIG_ACKS;i++)
      {
        /* delay to at least comply with settling time at Master node */
        halTimer32kMcuSleepTicks(4260); /* Wait 130 ms */
        perRfApi.perSendPacket((rxData->data));
      }
      break;
    }
    else{                    
      perRfApi.perEnterRX(); 
    }                        
    __low_power_mode_3();
  }
  perSettings.masterSlaveLinked = PER_DEVICE_LINKED;
  cpyStatusString(slaveStatus,slaveStatus1,PER_SLAVE_STATUS_LEN);
  menuDisplay(&perSlaveMenu);
  
  /* Provide some time to the user to register the status of current operation */
  halTimer32kMcuSleepTicks(16384);
  
  /* Configure radio according to perSettings */
  perSettings.payloadLength         = rxData->data[1];
  perSettings.packetRate            = ((rxData->data[2]<<8)|rxData->data[3]);
  perSettings.txPower               = rxData->data[4];
  perSettings.smartRfConfiguration  = rxData->data[5];
  perSettings.linkTopology          = rxData->data[6];
  perSettings.address               = rxData->data[7];
  perRfApi.perTrxRegConfig();  
  
  /* Execute PER test according to linkTopology/link type */
  if(perSettings.linkTopology == LINK_1_WAY)
  {
    perSlaveOneWayTest();
  }
  else
  {
    perSlaveTwoWayTest();
  }
  return PER_RETURN_SUCCESS;
}
/******************************************************************************   
* @fn          perSlaveStartLinkBypassApp()                                       
*                                                                                
* @brief       Function is run from the menu system. The function will manage   
*              it's own menus. It will configure the device as the Slave 
*              according to the configuration received from the Master. It 
*              will then execute a specific PER test given the received 
*              configuration.
*                 
* input parameters
*
* @param       pDummy - pointer to pointer to void. no value used
*          
* output parameters
*                                                                                
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/                    
uint8 perSlaveStartLinkBypassApp(void** pDummy)
{   
  
  /*************************** Menu Initialization  ***************************
  * - Update status of slave configuration process 
  * - Write the "menu" to the screen and clearing reserved areas
  * - Set slave node and that linking process is incomplete
  */
  cpyStatusString(slaveStatus,slaveStatus0,PER_SLAVE_STATUS_LEN);
  menuClearReservedArea(&perSlaveMenu);
  menuDisplay(&perSlaveMenu);
  perSettings.deviceMode = SLAVE_DEVICE;
  
  perSettings.masterSlaveLinked = PER_DEVICE_LINK_BYPASS;
  cpyStatusString(slaveStatus,slaveStatus1,PER_SLAVE_STATUS_LEN);
  menuDisplay(&perSlaveMenu);
  
  /* Provide some time to the user to register the status of current operation */
  halTimer32kMcuSleepTicks(16384);
  
  /* Configure radio according to perSettings */
  perSettings.payloadLength         = (CC1101_DEFAULT_PACKETLENGTH-1);;
  perSettings.linkTopology          = LINK_1_WAY;
  perSettings.packetRate              = perComputePacketRate();   
  perSettings.address               = 0x00;
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  perRfApi.perTrxRegConfig();  
  
  /* Execute PER test according to linkTopology/link type */
  perSlaveOneWayTest();
  
  return PER_RETURN_SUCCESS;
}
/******************************************************************************   
* @fn          perSlaveStartValueLineApp()                                       
*                                                                                
* @brief       Function is run from the menu system. The function will manage   
*              it's own menus. It will configure the device as the Slave 
*              according to the configuration received from the Master. It 
*              will then execute a specific PER test given the received 
*              configuration.
*                 
* input parameters
*
* @param       pDummy - pointer to pointer to void. no value used
*          
* output parameters
*                                                                                
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/                    
uint8 perSlaveStartValueLineApp(void** pDummy)
{   
  
  /*************************** Menu Initialization  ***************************
  * - Update status of slave configuration process 
  * - Write the "menu" to the screen and clearing reserved areas
  * - Set slave node and that linking process is incomplete
  */
  cpyStatusString(slaveStatus,slaveStatus0,PER_SLAVE_STATUS_LEN);
  menuClearReservedArea(&perSlaveMenu);
  menuDisplay(&perSlaveMenu);
  perSettings.deviceMode = SLAVE_DEVICE;
  
  perSettings.masterSlaveLinked = PER_DEVICE_LINKED;
  cpyStatusString(slaveStatus,slaveStatus1,PER_SLAVE_STATUS_LEN);
  menuDisplay(&perSlaveMenu);
  
  /* Provide some time to the user to register the status of current operation */
  halTimer32kMcuSleepTicks(16384);
  
  /* Configure radio according to perSettings */
  perSettings.linkTopology          = LINK_1_WAY;
  perSettings.packetRate              = perComputePacketRate();   
  perSettings.address               = 0x00;
  perRfApi.perConnectTrxIsr(perRfApi.perTrxIsr);
  perRfApi.perTrxRegConfig();  
  
  /* Execute PER test according to linkTopology/link type */
  perSlaveOneWayTest();
  
  return PER_RETURN_SUCCESS;
}
/******************************************************************************
* @fn          perSlaveOneWayTest
*
* @brief       One-Way PER test executed on the Slave device. Packets are sent 
*              at programmed packet rate with selected length and data rate 
*              from the Slave to the Master. 
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
*/
static void perSlaveOneWayTest(void)
{
  uint8 whichButtonPushed; 
  whichButtonPushed = 0x00;
  /* Update screen with Slave status */
  cpyStatusString(slaveStatus,slaveStatus2,PER_SLAVE_STATUS_LEN);
  menuDisplay(&perSlaveMenu);
  /* Configure the TX output power */
  perRfApi.perSetTxPower(perSettings.txPower);
  /* Make a nearly DC balanced packet */
  txArray[0] = perSettings.payloadLength; /* Packet length equals number of payload bytes */
  txArray[1] = perSettings.address;
  /* Following packet is nice for RX performance but non-ideal for TX spectrum */
  for(uint8 i = 0; i<(perSettings.payloadLength-1);i++)
  {                          
    txArray[i+2] = 0xAA;
  }
  /* Setting up timer interrupt at packet rate and enter low power mode */
  perTimerState = NO_ACTION;
  halTimer32kIntConnect(&per32kTimerISR);
  halTimer32kSetIntFrequency(perSettings.packetRate);
  halTimer32kIntEnable();
  __low_power_mode_3();
  
  /* Test is run until aborted */  
  while(1)
  {
    if(perTimerState == ISR_ACTION_REQUIRED)
    {
      /* Time to send packet */
      perTimerState = NO_ACTION;
      perRfApi.perSendPacket(txArray);
    }
    whichButtonPushed = bspKeyPushed(BSP_KEY_ALL);
    if((whichButtonPushed == BSP_KEY_RIGHT) || (whichButtonPushed == BSP_KEY_LEFT))
    {
      /* Test is aborted by user */
      halTimer32kAbort();
      perTimerState = NO_ACTION;
      /* Put radio into sleep to save power */
      perRfApi.perEnterSleep();
      return;
    }
    __low_power_mode_3();
  } 
}

/****************************************************************************** 
* @fn          perSlaveTwoWayTest                                           
*                                                                              
* @brief       Function is run on the slave when the two-way PER test with
*              retransmit is chosen. The Slave will ack received packets and
*              then re-enter RX.
*
* input parameters
*                                                                              
* @param       none                                                            
*                 
* output parameters
*                                                             
* @return      void                                                            
*/
static void perSlaveTwoWayTest(void)
{
  uint8 whichButtonPushed;
  /* Create the standard ACK packet */
  uint8 ackArray[2];
  ackArray[0] = 1;
  ackArray[1] = perSettings.address;
  /* Update screen status */
  cpyStatusString(slaveStatus,slaveStatus2,PER_SLAVE_STATUS_LEN);
  menuDisplay(&perSlaveMenu);
  /* Set output power to wanted level */
  perRfApi.perSetTxPower(perSettings.txPower);
  /* Enter RX */
  packetSemaphore = 0;
  perRfApi.perEnterRX();
  __low_power_mode_3();
  while(1)
  {
    /* Wake up from powerdown only by abortion or a sync found/packet received */
    whichButtonPushed = bspKeyPushed(BSP_KEY_ALL);
    if((whichButtonPushed == BSP_KEY_RIGHT) || (whichButtonPushed == BSP_KEY_LEFT))
    {
      perRfApi.perEnterSleep();
      return;
    }  
    if(packetSemaphore & PACKET_RECEIVED)               
    {      
      /* ACK if Slave received a valid packet */ 
      perRfApi.perSendPacket(ackArray);
      /* radio is in idle-->calibrate and re-enter rx */
      perRfApi.perEnterRX();                         
    } 
    /* Reset packetSemaphore if slave is awaken*/      
    packetSemaphore = 0;
    __low_power_mode_3();
  }
}

/******************************************************************************
* @fn          perMasterStartTestApp
*
* @brief       Function is executed from menu system and starts the PER test 
*              that is selected.
*
* input parameters
*
* @param       pDummy  - pointer to pointer to void. Not used
*
* output parameters
*
* @return      PER_RETURN_SUCCESS
*/
uint8 perMasterStartTestApp(void **pDummy)
{
  perSettings.deviceMode = MASTER_DEVICE;
  if(perSettings.linkTopology == LINK_1_WAY)
  {
    perMasterOneWayTest();
  }
  else
  {
    perMasterTwoWayTest();
  }
  return PER_RETURN_SUCCESS;
}

/******************************************************************************
* @fn          perMasterOneWayTest
*
* @brief       One-Way PER test. Packets are sent at computed rate with 
*              selected length and data rate from the Slave to the
*              Master.
*            
*              The PER tester measures PER when the configured link is 
*              assumed. If packets are not received, test progress will still
*              increment at the programmed packet rate.
*
*              " : packet received
*              | : timer interrupt-> check if we've received a packet, 
*                  configure timer to packet rate interval.
*              # : lost packet
*
*              "|---------"|---------#|----
*              ----------------------------->
*                                        [t]
*              If the radio generates a synchronization interrupt, the radio will
*              be forced into IDLE and back into RX and the 32k packet rate timer
*              will be reset to computed packet rate. If the 32k packet rate
*              timer genrates an interrupt without any received packet the radio
*              remains in RX. If 32k timer interrupts and sync pin is high, the 
*              algorithm will wait for the sync interrupt and then the radio will
*              enter IDLE and then back into RX. Correctly received packets
*              doesn't contribute to PER, only those that are incorrectly received 
*              or missed.
*              
*              The application shows stats to screen depending on button input.
*
*              Updates to screen and general board activity is concentrated 
*              to a time interval right after packet reception to lower noise
*              when receiving a packet.
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
******************************************************************************/
static void perMasterOneWayTest(void)
{
  /* Signalize to sync ISR that test has started so that it adjusts the packet
  * 32k timeout to instants where a packet should have been received.
  */
  perSettings.testRunning = PER_TRUE;
  /* Initialize test:
  * - Wake up radio
  * - Configure Radio Registers
  * - Initialize test variabels
  */
  perRfApi.perEnterIdle();
  perRfApi.perTrxRegConfig();
  perMasterInitTestStatisticsAndVariables();
  
  /* Enter RX and wait until RX (~1000 us) : might also read marcstate to ensure RX */
  perRfApi.perEnterRX();
  halTimer32kMcuSleepTicks(33);
  
  /* Enable interrupts at computed packet rate and send MSP to sleep */
  /* Button push/radio sync interrupt / 32k timer interrupt will wake it up */
  halTimer32kIntConnect(&per32kTimerISR);
  halTimer32kSetIntFrequency(perSettings.packetRate);
  halTimer32kIntEnable();
  __low_power_mode_3();
  
  while(1)
  {
    /* Run PER algorithm only if test is not finished and a radio sync interrupt or a 32k timer 
    * interrupt occurred. The algorithm will synchronize to stream of packets from slave and 
    * compute the new PER if that's required.
    */
    if((perTestFinished != PER_TRUE) && ((packetSemaphore > 0) || (perTimerState == ISR_ACTION_REQUIRED)))
    {     
      /* Synchronize Master 32k Interupts to Packet Stream*/
      perMasterFindPacketSync();
      syncPinWasHigh = PER_FALSE; /* Not used in this test */
      
      /************************ PER COMPUTATION *************************/
      if(packetSemaphore & SYNC_FOUND)
      {
      	perStatistics.nSyncsFound++;
      }
      /* Register correctly received packets */
      if(packetSemaphore & PACKET_RECEIVED)
      {
        /* The radio received a good packet */
        perStatistics.numberOfGoodPackets++;
        perStatistics.nSyncsFound++;
      }
      else
      {
        /* Packet loss */
        /* 32k timer interrupt without sync pin high -> Update number of lost packets */
        perStatistics.numberOfLostPackets++;
      }
      perMasterUpdateRssiMembers(rxData->rssi);
      /* Variables that need to be updated either way a packet was received or not */
      perStatistics.testProgressNominator++;
      perStatistics.progress  = ((float)perStatistics.testProgressNominator/(float)perStatistics.testProgressDenominator)*100;
      perStatistics.currentPer= ((float)perStatistics.numberOfLostPackets/(float)perStatistics.testProgressNominator)*100;
      
      /* Maintaining the LCD update "frequency": This aligns all LCD updates to after packet reception.
      * This isn't fully utilized. Might be exploited at a later time if wanted LCD resoultion should be less.
      */
      if(--screenUpdateCounter == 0)
      {
        screenUpdateCounter = 1;
        updateScreen = PER_TRUE;
      }
      
      /* Maintaining the RSSI graph buffer with instant RSSI values */
      perMasterUpdateRssiGraph(perStatistics.graphBuffer,rxData->rssi,((packetSemaphore & PACKET_RECEIVED)>>7));
      
      /* The radio is in IDLE given that it found sync, put it back in RX and reset the timer interrupt */
      if(packetSemaphore > 0)
      {
        perRfApi.perEnterRX();
      }
      
      /* Abort if test now is finished - Put radio in sleep */
      if(perStatistics.testProgressNominator == perStatistics.testProgressDenominator)
      {
        perSettings.testRunning = PER_FALSE;
        perRfApi.perEnterSleep();
        perTestFinished = PER_TRUE;
      }
      /* reset the interrupt flags */
      packetSemaphore = 0;
      perTimerState   = NO_ACTION;
      
      /*************** END OF PER COMPUTATION ***********************/
    }
    
    /* Checking button input: 
    * - Checking if buttons have been pressed. 
    *   If this is the case the LCD update will be after an expected packet reception
    *   if the test isn't finished or right away if the test is finished. 
    */
    if(perMasterProcessButtonInput())
    {
      /* Abort if left button has been hit */
      if(perTestFinished == PER_FALSE)
      {
        /* In case test isn't finished: put radio in sleep */
        /* Must condition, else the radio will be awaken   */  
        perSettings.testRunning = PER_FALSE;
        perRfApi.perEnterSleep();
      }
      halTimer32kAbort();
      packetSemaphore = 0;   
      perTimerState   = NO_ACTION;
      return;
    }
    
    if(perTestFinished)
    {
      updateScreen = PER_TRUE;
    }
    /* Update the LCD */
    if(updateScreen == PER_TRUE) 
    {
      /* Update screen dependant on selected screen {1,2} */
      perMasterRefreshResults();
      updateScreen = PER_FALSE;
    }
    
    /* Only send MSP to sleep if no interrupts should be processed:
    * - If a packet is received when expected, go to sleep.
    * - If the MSP was waken and no packets turned out to be received when entering this loop
    *   but when exiting the loop a packet is indeed received: re-enter loop. 
    *   Example:  - a button push occurres right ahead of a sync or 32k timer interrupt.
    *             - reception of packet while updating LCD display
    * This is important to be able to synchronize to packet stream if synchronization has been 
    * lost for a while.
    */
    
    /******* Synchronize Master 32k Interupts to Packet Stream  *******/
    /* Do not enter sleep if the following evets are detected         */
    /*** End of Synchronize Master 32k Interupts to Packet Stream   ***/
    
    if(packetSemaphore == 0 && perTimerState == NO_ACTION)
    {
      __low_power_mode_3();
    }
  }
} 

/******************************************************************************
* @fn          perMasterTwoWayTest
*
* @brief       Two-Way PER test executed on the Master. Each time the Master 
*              transmits a packet it will check if the last transmitted packet
*              was ACKed by the Slave. If an ACK is not received within
*              perSettings.nAllowedRetransmits packet re-transmissions, the 
*              packet will be considered lost. 
*              The Slave node will ACK with a packet containing only the 
*              address. 
*
*              Two criteria is used to state that the test is finished:
*              - Aborted by user
*              - #ACKed packets + #lost packets = perSettings.totalNumPackets
*                The total number of transmitted packets will be 
*                perSettings.totalNumPackets + perSettings.nRetransmits
*                This way, the total number of packets sent will be dependant 
*                on radio environment, radio register setting and 
*                perSettings.nAllowedRetransmits.
*
* input parameters
*
* output parameters
*
* @return      void
*/
static void perMasterTwoWayTest(void)
{
  uint8 transmits, packetLenghtBak;
  /* Initialize: 
  * - Wake up radio
  * - Configure Radio Registers
  * - Initialize test variabels
  * - Set wanted TX power
  * - configure intial packet rate 
  */
  perRfApi.perEnterIdle();
  perRfApi.perTrxRegConfig();
  perMasterInitTestStatisticsAndVariables();
  perRfApi.perSetTxPower(perSettings.txPower);
  transmits = 1;
  perSettings.packetRate = perComputePacketRate();
  perSettings.testRunning = PER_TRUE;
  
  /* Make a DC balanced packet */
  txArray[0] = perSettings.payloadLength;
  txArray[1] = perSettings.address;
  for(uint8 i = 0; i<(perSettings.payloadLength-1);i++)
  {
    txArray[i+2] = 0xAA;
  }
  /* All acknowledged packets by the slave will have a length of 1 byte.
  * The perSettings.payloadLength member must equal this value from now on 
  * to be able to receive the packets correctly(filtering in the perTrxIsr
  * function uses this constraint).
  */
  packetLenghtBak = perSettings.payloadLength;
  /* The ACK packet length sent from slave is 1*/
  perSettings.payloadLength = 1;
  
  /* Set up initial timer interrupt: accounts for transmission + ACK + S\some slack */  
  halTimer32kIntConnect(&per32kTimerISR);
  halTimer32kSetIntFrequency(perSettings.packetRate);
  halTimer32kIntEnable();
  
  /* Send first packet */
  perRfApi.perSendPacket(txArray);
  /* Enter RX */
  perRfApi.perEnterRX();
  
  /* enter low power mode */
  __low_power_mode_3();
  
  /* Main loop for the two way PER test executed on the Master node: 
  * Refreshing LCD screen:
  *  - When the test is running the LCD screen is only updated
  *    after a packet has been/should have been received, i.e before
  *    a packet transmission.
  *  - When test is finished, the LCD screen is updated when the user 
  *    is pressing a button.
  *
  * 
  */
  while(1)
  {
    
    
    if((perTestFinished != PER_TRUE) && ((packetSemaphore > 0) || (perTimerState == ISR_ACTION_REQUIRED)))
    {
      /* Run PER algorithm only if test is not finished and a radio sync interrupt or a 32k timer 
      * interrupt occurred. The algorithm will synchronize ACK packets from slave and 
      * compute the new PER if that's required.
      */
      perMasterFindPacketSync();
      
      /************************ PER COMPUTATION *************************/
      
      /* Check if last packet was ACKed */
      if(packetSemaphore & PACKET_RECEIVED)
      {
        /* Update: 
        * - Test Progress
        * - Log good packets
        * - start new tracking of retransmits
        */
        perStatistics.testProgressNominator++;
        perStatistics.numberOfGoodPackets++;
        perStatistics.nSyncsFound++;
        perStatistics.progress  = ((float)perStatistics.testProgressNominator/(float)perStatistics.testProgressDenominator)*100;
        perStatistics.currentPer= ((float)perStatistics.numberOfLostPackets/(float)perStatistics.testProgressNominator)*100;
        transmits = 0;
      } 
      else
      { 
        /* Check number of retransmits */
        if(transmits == (perSettings.nAllowedRetransmits+1))
        {
          /* Update: 
          * - Failure statistics 
          * - Update test progress 
          * - start new tracking of retransmits 
          */
          perStatistics.testProgressNominator++;
          perStatistics.numberOfLostPackets++;
          perStatistics.progress  = ((float)perStatistics.testProgressNominator/(float)perStatistics.testProgressDenominator)*100;
          perStatistics.currentPer= ((float)perStatistics.numberOfLostPackets/(float)perStatistics.testProgressNominator)*100;
          transmits = 0; 
        }
        else
        {
          /* Update number of retransmits */
          perStatistics.nRetransmits++;
        }
      }
      perMasterUpdateRssiMembers(rxData->rssi);
      /* Maintaining the LCD update "frequency". currently feature isn't used */
      if(--screenUpdateCounter == 0)
      {
        screenUpdateCounter = 1;
        updateScreen = PER_TRUE;
      }
      /* Maintain RSSI Graph Buffer with the current RSSI values */
      perMasterUpdateRssiGraph(perStatistics.graphBuffer,rxData->rssi,((packetSemaphore & PACKET_RECEIVED)>>7));
      
      /******************* END OF PER COMPUTATION ***********************/
      
      /******* Synchronize Master 32k Interupts to Packet Stream  *******/
      
      if(packetSemaphore & PACKET_RECEIVED)
      {
      	/* Adjust the 32k timeout with latest value of 32k timer value when Sync interrupt was ran. 
        * Wakeup is 150-165 us, the counter can then wrap if sync and 32k interrupt are close in time
        */
        if(syncPinWasHigh)
        {
          /* The ACK packet was being received when 32k timer interrupted. When full packet is received, the 
          * wrapped timer value must be added to perSettings.packetRate to account for the delayed packet.
          */
          perSettings.packetRate += timer32kValue;  
          syncPinWasHigh = PER_FALSE;
        }
        else
        {
          if(timer32kValue <= 6)                                 
          { 
            /* 32k timer wrap case: wakeup on MSP is 150-165us */
            /* Extend timeout period if this happens by a tick or leave it. The former will decrease rssi 
            * fidelity while not extending might lead to slight increase in current consumption.
            * To have high RSSI fidelity, the wakeup response on MSP must be increased.
            */
            perSettings.packetRate = perSettings.packetRate;/* + 1;  */
          }                                                   
          else                                                
          {  
            /* Distance between nodes is highly decreased during packetlosses. When a packet finally is received 
            * the timing between 32k timer interrupt and sync interrupt is regained. 32k timer value is reduced. 
            */                                                 
            perSettings.packetRate = timer32kValue;              
          }
        }
      }
      /*** End of Synchronize Master 32k Interupts to Packet Stream   ***/
      
      packetSemaphore = 0;
      perTimerState   = NO_ACTION;
      
      /* Check for user input each time loop is iterated */
      if(perMasterProcessButtonInput())
      {
        /* Abort if left button has been pushed */
        if(perTestFinished == PER_FALSE)
        {
          /* In case test isn't finished: put radio in sleep */
          /* Must condition, else the radio will be awaken   */  
          perSettings.testRunning = PER_FALSE;
          perRfApi.perEnterSleep();
        }
        halTimer32kAbort();
        packetSemaphore = 0;   
        perTimerState   = NO_ACTION;
        
        /* Restoring packetLength field */
        perSettings.payloadLength = packetLenghtBak;
        return;
      }
      
      
      /* Update LCD before sending packet to avoid possible noise when receiving ACK */
      if(updateScreen == PER_TRUE) 
      {
        /* Dependant on selected screen{1,2} */
        perMasterRefreshResults();
        updateScreen = PER_FALSE;
      }
      
      /* Stop test if finished */
      if(perStatistics.testProgressNominator == perStatistics.testProgressDenominator)
      {
        perTestFinished = PER_TRUE;
        perRfApi.perEnterSleep();
      }
      else
      {
        /* Send next packet if test is not finished */
        perRfApi.perSendPacket(txArray);
        transmits++;
        
        /* 32k interrupt timer setup */
      	if(perStatistics.numberOfGoodPackets>0)
      	{
          /* 32k timer is synchronized to the response time of the Slave */
          halTimer32kInit(perSettings.packetRate);
      	}
      	else
      	{
          /* 32k timer is not yet synchronized to Slave response time. First valid
          * packet will synchronize it.
          */
          halTimer32kSetIntFrequency(perSettings.packetRate);
      	}
      	halTimer32kIntEnable();
        
        /*Enter RX */
        perRfApi.perEnterRX();
      } 
    }   
    if(perTestFinished == PER_TRUE)
    {   
      /* Check for user input each time loop is iterated */
      if(perMasterProcessButtonInput())
      { 
        /* Abort if left button has been pushed */
        if(perTestFinished == PER_FALSE)
        {
          /* In case test isn't finished: put radio in sleep */
          /* Must condition, else the radio will be awaken   */  
          perSettings.testRunning = PER_FALSE;
          perRfApi.perEnterSleep();
        }
        halTimer32kAbort();
        packetSemaphore = 0;   
        perTimerState   = NO_ACTION;
        
        /* Restoring packetLength field */
        perSettings.payloadLength = packetLenghtBak;
        return;
      }
      
      
      /* Test is finished, update screen whenever neccessary */
      if((updateScreen == PER_TRUE)) 
      {
        /* Dependant on selected screen {1,2} */
        perMasterRefreshResults();
        updateScreen = PER_FALSE;
      }
    }
    
    /* Re-enter low power mode */
    __low_power_mode_3();    
  }
  
}

/******************************************************************************
* @fn          perMasterRefreshResults
*
* @brief       This function updates the PER results on the screen according
*              to screenNumber and screenToggle. 
*
* input parameters
*
* output parameters
*
* @return      void
*/
static void perMasterRefreshResults(void)
{
  screenToggle = PER_TRUE;
  /* Processing Screen 1 */ 
  if(screenNumber == 1)
  {
    /* Screen is switched, update the whole screen */
    if(screenToggle == PER_TRUE)
    {
      lcdBufferClear(0);
      lcdBufferPrintString(0,"PER STATISTICS: 1/2"                                             , 12,    eLcdPage0);
      lcdBufferPrintString(0,"Progress[%]  :"                                                  ,  0,    eLcdPage1);
      lcdBufferPrintFloat(0,perStatistics.progress                                             ,  2, 88,eLcdPage1);
      lcdBufferPrintString(0,"Good Pkts    :"                                                  ,  0,    eLcdPage2);
      lcdBufferPrintInt(0,perStatistics.numberOfGoodPackets                                    , 88,    eLcdPage2);
      lcdBufferPrintString(0,"Lost Pkts    :"                                                  ,  0,    eLcdPage3);
      lcdBufferPrintInt(0,perStatistics.numberOfLostPackets                                    , 88,    eLcdPage3);
      lcdBufferPrintString(0,"PER[%]       :"                                                  ,  0,    eLcdPage4);
      lcdBufferPrintFloat(0,perStatistics.currentPer                                           ,  2, 88,eLcdPage4);
      lcdBufferPrintString(0,"AVG RSSI[dBm]:"                                                  ,  0,    eLcdPage5);
      lcdBufferPrintString(0,"Link M[dBm]  :"                                                  ,  0,    eLcdPage6);
      /* Accounting for RSSI ringbuffer startup transients */
      if(perStatistics.testProgressNominator>=perSettings.rssiWindowSize)
      {
        lcdBufferPrintInt(0,perStatistics.avgRSSI                                              , 88,    eLcdPage5);
        lcdBufferPrintInt(0,perStatistics.remainingLinkMargin                                  , 88,    eLcdPage6); 
      }
      if(perSettings.linkTopology == LINK_2_WAY)
      {
        lcdBufferPrintString(0,"#Re-Transmits:"                                                ,  0,    eLcdPage7);
        lcdBufferPrintInt(0,perStatistics.nRetransmits                                         ,  88,   eLcdPage7);
      }
      lcdSendBuffer(0);
    }
  }
  else if(screenNumber == 2)
  {
    /* Second results screen shows the RSSI graph */
    lcdBufferPrintString(perStatistics.graphBuffer,"PER STATISTICS: 2/2"                        , 12,    eLcdPage0);
    lcdSendBuffer(perStatistics.graphBuffer);
  }
  screenToggle = PER_FALSE;  
  return;
}

/******************************************************************************
* @fn          perMasterInitTestStatisticsAndVariables
*
* @brief       Initializes the members of the statistics struct and other 
*              PER test variables that need to be initiated on the Master side
*              at each test startup.
*
* input parameters
*
* output parameters
*
* @return      void
*/
static void perMasterInitTestStatisticsAndVariables(void)
{
  /* Reseting the RSSI ring buffer */
  for(uint8 i = 0; i<perSettings.rssiWindowSize;i++)
  {
    perStatistics.rssiBuffer[i] = 0; 
  }
  /* Reseting the perStatistics struct members */      
  perStatistics.rssiBufferCounter                   = 0;                       
  perStatistics.sumRSSI                             = 0;                      
  perStatistics.avgRSSI                             = RSSI_LOW;                    
  perStatistics.testProgressNominator               = 0;             
  perStatistics.testProgressDenominator             = perSettings.totalNumPackets; 
  perStatistics.numberOfGoodPackets                 = 0;                      
  perStatistics.numberOfLostPackets                 = 0;                      
  perStatistics.currentPer                          = 0.0;                                                
  perStatistics.sensitivity                         = perSettings.sensitivity;
  perStatistics.remainingLinkMargin                 = RSSI_LOW - perStatistics.sensitivity;                
  perStatistics.progress                            = 0.0;                           
  perStatistics.nSyncsFound                         = 0; 
  perStatistics.nRetransmits                        = 0;
  
  updateScreen        = PER_FALSE;
  screenUpdateCounter = 1;
  perTestFinished     = PER_FALSE;
  
  /* The PER test running on the Master node has its' own means to display results depending
  * on user input. 
  * screenToggle: Decides wether the screen screen is toggled during PER test->updates whole screen
  * screenNumber: Selects one of 2 screens to show{1,2(graph)}
  */
  screenToggle    = PER_TRUE;
  screenNumber    = 2;
  
  perTimerState   = NO_ACTION;
  packetSemaphore = 0;
  
  syncPinWasHigh = PER_FALSE;
  
  /* Clear RSSI graph buffer */
  lcdBufferClear(perStatistics.graphBuffer);
  /* Drawing graph borders and lines */
  perMasterInitRssiGraph(perStatistics.graphBuffer);    
  return;
}
/******************************************************************************
* @fn          perChipSelectApp
*
* @brief       Called from menu system to start PER tester application. It 
*              detects if chip is suported. If any other chip than cc1101 is
*              present, function will continue to perInitApp. If cc1101 or 
*              cc1120 is present, the application will continue to the 
*              combination selection menu
*
* input parameters
*              
* @param       pointer to void, not used
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/
uint8 perChipSelectApp(void** pDummy)
{  
  /* Detect if a supported radio is present */ 
  trxDetectChipType(&perRadioChipType);
  
  /*If detected radio is not cc1101 or cc1120 go directly initiation */
  if(perRadioChipType.deviceName == CHIP_TYPE_CC1120)
  {
    /*CC1120 detected, go to chip selection menu*/
    perAbstractHeadMenu = perCC1120ChipSelectMenu; 
    return PER_RETURN_SUCCESS;
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1101)
  {
    /*CC1101 detected, go to chip selection menu*/
    perAbstractHeadMenu = perCC1101ChipSelectMenu; 
    return PER_RETURN_SUCCESS;
  }
  else
  {
    perAbstractHeadMenu = perHeadMenu; 
    return perInitApp(0);  
  }
}

/******************************************************************************
* @fn          perInitApp
*
* @brief       Called from menu system to start PER tester application. It 
*              assigns functions to the perRfApi struct according to what 
*              supported CCxxxxEM is inserted and then order the menu system
*              to update itself accordingly.
*
* input parameters
*              
* @param        pChipSelect - manual chip selection for cc1190, casted to void**
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/

uint8 perInitApp(void** pChipSelect)
{
  
  uint16 param = (uint16) *pChipSelect;
  
  if(param == CC1101_CC1190_SELECTED)
  {
    /* Manualy set cc1101-cc1190 combo as device*/
    perRadioChipType.deviceName = CHIP_TYPE_CC1101_CC1190;      
  }
  else if(param == CC1120_CC1190_SELECTED)
  {
    /* Manualy set cc1120-cc1190 combo as device*/
    perRadioChipType.deviceName = CHIP_TYPE_CC1120_CC1190; 
  }
  else if(param == CC1101_SELECTED){
    /* Manualy set cc1101 stand alone as device*/
    perRadioChipType.deviceName = CHIP_TYPE_CC1101;
  }
  else if(param == CC1120_SELECTED){
    /* Manualy set cc1101 stand alone as device*/
    perRadioChipType.deviceName = CHIP_TYPE_CC1120;
  }
  
  /* Instantiate tranceiver RF spi interface */
  
  /* Assign functions to perAPI based on the chip detected */
  if(perRadioChipType.deviceName == CHIP_TYPE_CC1101)
  {
    perRfApi.perTrxRegConfig      = &perCC1101RegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;
    perRfApi.perTrxIsr            = &perCC1101RxTxISR;     
    perRfApi.perSetTxPower        = &perCC1101SetOutputPower;
    perRfApi.perEnterRX           = &perCC1101EnterRx;
    perRfApi.perSendPacket        = &perCC1101SendPacket;
    perRfApi.perEnterIdle         = &perCC1101EnterIdle;
    perRfApi.perEnterSleep        = &perCC1101EnterSleep;
    perRfApi.perWriteTxFifo       = &perCC1101WriteTxFifo;
    perRfApi.perGetDataRate       = &perCC1101GetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC1101GetGuiTxPower;
    perRfApi.perGetRssi           = &perCC1101Read8BitRssi;
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC110L)
  {
    perRfApi.perTrxRegConfig      = &perCC110LRegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;
    perRfApi.perTrxIsr            = &perCC110LRxTxISR;     
    perRfApi.perSetTxPower        = &perCC110LSetOutputPower;
    perRfApi.perEnterRX           = &perCC110LEnterRx;
    perRfApi.perSendPacket        = &perCC110LSendPacket;
    perRfApi.perEnterIdle         = &perCC110LEnterIdle;
    perRfApi.perEnterSleep        = &perCC110LEnterSleep;
    perRfApi.perWriteTxFifo       = &perCC110LWriteTxFifo;
    perRfApi.perGetDataRate       = &perCC110LGetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC110LGetGuiTxPower;
    perRfApi.perGetRssi           = &perCC110LRead8BitRssi;    
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC113L)
  {
    perRfApi.perTrxRegConfig      = &perCC113LRegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;
    perRfApi.perTrxIsr            = &perCC113LRxTxISR;     
    perRfApi.perEnterRX           = &perCC113LEnterRx;
    perRfApi.perEnterIdle         = &perCC113LEnterIdle;
    perRfApi.perEnterSleep        = &perCC113LEnterSleep;
    perRfApi.perGetDataRate       = &perCC113LGetDataRate;
    perRfApi.perGetRssi           = &perCC113LRead8BitRssi;    
  }  
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC115L)
  {
    perRfApi.perTrxRegConfig      = &perCC115LRegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect; 
    perRfApi.perTrxIsr            = &perCC115LRxTxISR;     
    perRfApi.perSetTxPower        = &perCC115LSetOutputPower;
    perRfApi.perSendPacket        = &perCC115LSendPacket;
    perRfApi.perEnterIdle         = &perCC115LEnterIdle;
    perRfApi.perEnterSleep        = &perCC115LEnterSleep;
    perRfApi.perWriteTxFifo       = &perCC115LWriteTxFifo;
    perRfApi.perGetDataRate       = &perCC115LGetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC115LGetGuiTxPower;    
  }  
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1101_CC1190) 
  {
    perRfApi.perTrxRegConfig      = &perCC1101CC1190RegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;
    perRfApi.perTrxIsr            = &perCC1101CC1190RxTxISR;     
    perRfApi.perSetTxPower        = &perCC1101CC1190SetOutputPower;
    perRfApi.perEnterRX           = &perCC1101CC1190EnterRx;
    perRfApi.perSendPacket        = &perCC1101CC1190SendPacket;
    perRfApi.perEnterIdle         = &perCC1101CC1190EnterIdle;
    perRfApi.perEnterSleep        = &perCC1101CC1190EnterSleep;
    perRfApi.perWriteTxFifo       = &perCC1101CC1190WriteTxFifo;
    
    perRfApi.perGetDataRate       = &perCC1101CC1190GetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC1101CC1190GetGuiTxPower;
    perRfApi.perGetRssi           = &perCC1101CC1190Read8BitRssi;
    
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1120_CC1190) 
  {
    perRfApi.perTrxRegConfig      = &perCC1120CC1190RegConfig;   
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;
    perRfApi.perTrxIsr            = &perCC1120CC1190RxTxISR;     
    perRfApi.perSetTxPower        = &perCC1120CC1190SetOutputPower;
    perRfApi.perEnterRX           = &perCC1120CC1190EnterRx;
    perRfApi.perSendPacket        = &perCC1120CC1190SendPacket;
    perRfApi.perEnterIdle         = &perCC1120CC1190EnterIdle;
    perRfApi.perEnterSleep        = &perCC1120CC1190EnterSleep;
    perRfApi.perWriteTxFifo       = &perCC1120CC1190WriteTxFifo;
    
    perRfApi.perGetDataRate       = &perCC1120CC1190GetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC1120CC1190GetGuiTxPower;
    perRfApi.perGetRssi           = &perCC1120CC1190Read8BitRssi;
    
  }  
  else if((perRadioChipType.deviceName == CHIP_TYPE_CC1125) 
          || (perRadioChipType.deviceName == CHIP_TYPE_CC1120)
            || (perRadioChipType.deviceName == CHIP_TYPE_CC1121))
  {
    perRfApi.perTrxRegConfig      = &perCC112xRegConfig;      
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;           
    perRfApi.perTrxIsr            = &perCC112xRxTxISR;        
    perRfApi.perSetTxPower        = &perCC112xSetOutputPower; 
    perRfApi.perEnterRX           = &perCC112xEnterRx;        
    perRfApi.perSendPacket        = &perCC112xSendPacket;     
    perRfApi.perEnterIdle         = &perCC112xEnterIdle;      
    perRfApi.perEnterSleep        = &perCC112xEnterSleep;   
    perRfApi.perWriteTxFifo       = &perCC112xWriteTxFifo;  
    perRfApi.perGetDataRate       = &perCC112xGetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC112xGetGuiTxPower;
    perRfApi.perGetRssi           = &perCC112xRead8BitRssi;
  }
  else if((perRadioChipType.deviceName == CHIP_TYPE_CC1200)
          || (perRadioChipType.deviceName == CHIP_TYPE_CC1201))  
  {
    perRfApi.perTrxRegConfig      = &perCC120xRegConfig;      
    perRfApi.perConnectTrxIsr     = &trxIsrConnect;           
    perRfApi.perTrxIsr            = &perCC120xRxTxISR;        
    perRfApi.perSetTxPower        = &perCC120xSetOutputPower; 
    perRfApi.perEnterRX           = &perCC120xEnterRx;        
    perRfApi.perSendPacket        = &perCC120xSendPacket;     
    perRfApi.perEnterIdle         = &perCC120xEnterIdle;      
    perRfApi.perEnterSleep        = &perCC120xEnterSleep;   
    perRfApi.perWriteTxFifo       = &perCC120xWriteTxFifo;  
    perRfApi.perGetDataRate       = &perCC120xGetDataRate;
    perRfApi.perGetGuiTxPower     = &perCC120xGetGuiTxPower;
    perRfApi.perGetRssi           = &perCC120xRead8BitRssi;
  }  
  else
  {
    /* Issue message to user to insert radio EM */
    lcdBufferClear(0);
    lcdBufferPrintString(0,"PER test could not",6,eLcdPage2);
    lcdBufferPrintString(0,"detect a supported",6,eLcdPage3);
    lcdBufferPrintString(0,"radio",36,eLcdPage4);
    lcdSendBuffer(0);
    /* Make the message visible for max 2 seconds */
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    halTimer32kMcuSleepTicks(TIMER_32K_CLK_FREQ);
    /* clear potential button pushes */
    bspKeyPushed(BSP_KEY_ALL);
    /* Menu system will keep its' present menu*/
    return PER_RETURN_FAILURE;
  }
  perRfApi.perSampleSyncPin  = &trxSampleSyncPin;
  /* Increasing speed on the tranceiver rf spi interface */
  trxRfSpiInterfaceInit(0x03);   
  /* Put radio in sleep */
  perRfApi.perEnterSleep();
  
  /* Reseting the variable GUI menu fields */
  perResetMenuVariables();
  return PER_RETURN_SUCCESS;
}

/***********************************************************************************
* @fn          per32kTimerISR
*
* @brief       32KHz timer interrupt service routine. A flag will be posted when the
*              interrupt is triggered.
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
*/
static void per32kTimerISR(void)
{
  perTimerState = ISR_ACTION_REQUIRED;
  return;
}

/***********************************************************************************
* @fn          perMasterUpdateRssiMembers
*
* @brief       Updates the perStatistics struct members that deal with RSSI
*              according to the provided RSSI value
*
* input parameters
*
* @param       rssi : provided RSSI value
*
* output parameters
*
* @return      void
*/
static void perMasterUpdateRssiMembers(int8 rssi)
{
  perStatistics.sumRSSI -= perStatistics.rssiBuffer[perStatistics.rssiBufferCounter];
  perStatistics.rssiBuffer[perStatistics.rssiBufferCounter] =  rssi;
  perStatistics.sumRSSI += perStatistics.rssiBuffer[perStatistics.rssiBufferCounter];
  if (++perStatistics.rssiBufferCounter == perSettings.rssiWindowSize) {
    perStatistics.rssiBufferCounter = 0;      /* Wrap ring buffer counter */
  }
  /* Ringbuffer startup errors not accounted for - handled when printing to screen */
  perStatistics.avgRSSI = (int8)(perStatistics.sumRSSI/perSettings.rssiWindowSize);
  perStatistics.remainingLinkMargin = perStatistics.avgRSSI - perStatistics.sensitivity;
  return;
}

/***********************************************************************************
* @fn          perMasterProcessButtonInput
*
* @brief       Checks if a button(except button S) has been pushed and acts 
*              accordingly. It manipulates the variables that the perMasterRefreshResults()
*              uses to display results. If left button is pushed it will return a 
*              positive number that will be used by the caller to abort the test.
*
* input parameters
*
* output parameters
*
* @return      PER_RETURN_SUCCESS/PER_RETURN_FAILURE
*/

static uint8 perMasterProcessButtonInput(void)
{
  uint8 buttonsPressed = bspKeyPushed(BSP_KEY_ALL);
  if(buttonsPressed) 
  {
    if((buttonsPressed == BSP_KEY_LEFT))
    {
      /* Report that an "escape" has been pushed */
      buttonsPressed = 0x00;
      return PER_RETURN_FAILURE;
    }
    /* Update the variables used by perMasterRefreshResults */
    else
    {
      /* Switch screen between two screens that are selected by using UP/DOWN buttons */
      if((buttonsPressed == BSP_KEY_DOWN) && (screenNumber == 1))
      {
        screenNumber = 2; 
        screenToggle = PER_TRUE;
      }
      else if((buttonsPressed == BSP_KEY_UP) && (screenNumber == 2))
      {
        screenNumber = 1; 
        screenToggle = PER_TRUE;
      }
    }
    /* Signal that the screen should be updated */
    updateScreen = PER_TRUE;
  }
  /* Default return */
  return PER_RETURN_SUCCESS;
}

/*****************************************************************************
* @fn         perMasterUpdateRssiGraph
*
* @brief     Updates destination buffer with argument values. If no packet
*            was received, the function draws a black line from the height
*            of the previous received value and down the graph area
*
* input parameters
*
* @param     pBuffer         - Destination buffer
* @param     rssi            - Received RSSI value
* @param     packetReceived  - Flag signaling received/lost packet
*                              1: Packet received
*                              0: Packet lost
*
* output parameters
*
* return     void
*/
static void perMasterUpdateRssiGraph(char *pBuffer,int8 rssiValue, char packetReceived)
{
  char *pBuf = lcdDefaultBuffer;  /* pBuf pointer defaults to the static halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer is set, pBuf points to it.            */
  uint8 rssiPixel;            /* Pixel row */
  
  /* Initial checking of column counter and packet lost */
  if(graphMarkerPos >= 127) graphMarkerPos = 29; /* First x-pos of graph area */
  
  /* Writing textual RSSI */
  lcdBufferPrintString(pBuf,"    ",60,eLcdPage7);  /* Clearing old text and adding unit */
  lcdBufferPrintInt(pBuf, rssiValue,60,eLcdPage7); /* Printing RSSI value below graph   */
  
  /* Clearing marker line */
  lcdBufferClearVLine(pBuf,graphMarkerPos,9,54);
  
  /* Drawing RSSI pixel */
  if(packetReceived)
  {
    rssiPixel = (((-1)*rssiValue+36)/3 ); /* Convert RSSI value to a pixel row */
    lcdBufferSetPx(pBuf,graphMarkerPos,rssiPixel);
  }
  else
  {
    /* Packet is lost. Vertical line is drawn downwards from current RSSI sample
    * if available or the previous RSSI value if not.
    */
    rssiPixel = (((-1)*rssiValue+36)/3 ); /* Convert RSSI value to a pixel row */
    lcdBufferSetVLine(pBuf,graphMarkerPos,rssiPixel,54);
  }
  /* Drawing marker line */
  lcdBufferSetVLine(pBuf,(++graphMarkerPos),9,54);  
  return;
}

/*****************************************************************************
* @fn         perMasterInitRssiGraph
*
* @brief     Writes out the axes etc. for the typical graph domain
*            First pixel (first col., first row) is pixel (0,0)
* +---------------------------------------------------------+
* |      ___ TITLE PAGE ____________________________________|
* |  +x +                ^Top border on pixel row 8        ||
* |     |< left border on pixel column 28                  ||
* |     |                                                  ||
* | -xx +                                                  ||
* |     |                right border on pixel column 127 >||
* |-xxx +__Bottom border on pixel row 55___________________||
* |      Textual RSSI val.: -xyz dBm                        |
* +---------------------------------------------------------+
*
* input parameter
*
* @param     pBuffer   - The target buffer
*
* output parameter
*
* return     void
*/
static void perMasterInitRssiGraph(char *pBuffer)
{
  char *pBuf = lcdDefaultBuffer;  /* Default buffer to point to    */
  if(pBuffer) pBuf = pBuffer; /* Point to the specified buffer */
  
  graphMarkerPos = 29;
  
  /* Top left of graph area: ( 28, 8) */
  /* Bottom right          : (127,55) */
  
  /* Write the surrounding axis box */
  lcdBufferSetVLine(pBuf,28,8,55);       /* Left y axis   */
  lcdBufferSetVLine(pBuf,127,8,55);      /* Right y axis  */
  lcdBufferSetHLine(pBuf,29,126,8);      /* Top x axis    */
  lcdBufferSetHLine(pBuf,29,126,55);     /* Bottom x axis */
  /* Write y axis labels and markers */
  lcdBufferPrintString(pBuf,"  +2",0,eLcdPage1); /* Page 1 label  */
  lcdBufferSetHLine(pBuf,26,27,11);      /* Page 1 marker */
  lcdBufferPrintString(pBuf," -22",0,eLcdPage2); /* Page 2 label  */
  lcdBufferSetHLine(pBuf,26,27,19);      /* Page 2 marker */
  lcdBufferPrintString(pBuf," -46",0,eLcdPage3); /* Page 3 label  */
  lcdBufferSetHLine(pBuf,26,27,27);      /* Page 3 marker */
  lcdBufferPrintString(pBuf," -70",0,eLcdPage4); /* Page 4 label  */
  lcdBufferSetHLine(pBuf,26,27,35);      /* Page 4 marker */
  lcdBufferPrintString(pBuf," -94",0,eLcdPage5); /* Page 5 label  */
  lcdBufferSetHLine(pBuf,26,27,43);      /* Page 5 marker */
  lcdBufferPrintString(pBuf,"-118",0,eLcdPage6); /* Page 6 label  */
  lcdBufferSetHLine(pBuf,26,27,51);      /* Page 6 marker */
  /* Write static "RSSI" text below graph area */
  lcdBufferPrintString(pBuf,"RSSI:    dBm",30,eLcdPage7);
  return;
}

/***********************************************************************************
* @fn          perComputePacketRate
*
* @brief       Computes the packet rate given the selected packet length, data
*              rate and link topology. The formula accounts for the time it takes to
*              send all the bits in the packet including preamble, sync word, length
*              byte and payload bytes. In additionm time used for MSP processing is 
*              added(LCD + DSP) with some slack.
*              Assumes 4 byte preamble, 4 byte sync word and 1 byte packet length 
*              field in all packets. 
*               
* input parameters
*
* @param       
*
* output parameters
*
* @return      Computed packet rate 
*              
*/
static uint16 perComputePacketRate(void)
{
  float packetRate, dataRate;
  dataRate = perRfApi.perGetDataRate(perSettings.smartRfConfiguration);
  if(perSettings.linkTopology == LINK_1_WAY)
  {
    if(dataRate < 1000)
    {
      /* Time it takes to send a packet + dsp + msp wakeup ++: */
      packetRate = (float)((perSettings.payloadLength+9)*8)/(dataRate*1000.0)+ 0.04; /* adding 40ms slack to MSP430 wakeup + DSP++ */
    }
    else
    {
      /* Time it takes to send a packet + dsp + msp wakeup ++: */
      packetRate = 0.2;//(float)((perSettings.payloadLength+9)*8)/(dataRate*1000.0)+ 0.04; /* adding 40ms slack to MSP430 wakeup + DSP++ */   
    }
  }
  else
  {
    
    /* The Master allows the slave to respond within 1s, i.e the test supports a distance between Slave and Master
    * of < 150 km when accounting for TX->RX, RX->TX times and the length of the ACK packet. 
    */
    packetRate = 1.0;
  }
  packetRate = 1.0/(float)packetRate;
  return (uint16)packetRate;
}



/***********************************************************************************
* @fn          perMasterFindPacketSync
*
* @brief       Routine that aligns the PER algorithm's 32k timer interrupts to the
*              expected packet sync(falling edge) interrupts. This makes it possible
*              to sample RSSI values close to when packets should have received in 
*              case they're lost. It makes, however, the flow a little bit more 
*              complex. If the mentioed RSSI feature is not wanted, it would be 
*              easier to use constant offsets instead.
*               
* input parameters
*
* @param       
*
* output parameters
*
* @return      void
*              
*/
static void perMasterFindPacketSync(void)
{
  /******* Synchronize Master 32k Interupts to Packet Stream  *******/
  if(packetSemaphore > 0)
  {
    /* Radio synchronization is found: Enter Idle and reset the 32k interrupt flag. 
    * The latter makes 32k interrupt/radio sync mutually exclusive in the algorithm.
    */
    perRfApi.perEnterIdle();
    perTimerState = NO_ACTION;  
    /* In case of 2-way link test: 32k timer is allready aborted */
  } 
  else
  {
    /* A 32k interrupt has occurred. Re-synchronize to packet stream 
    * if sync pin is high or register lost packet during PER computation.
    */
    if((perRfApi.perSampleSyncPin()>0) || (packetSemaphore>0))
    { 
      
      /* Sync pin was high or a transition just happened */
      syncPinWasHigh = PER_TRUE;
      while(perRfApi.perSampleSyncPin()>0); /* consumes more power on MSP */
      while(packetSemaphore == 0);          /* Ensure that sync interrupt has happened */
      perRfApi.perEnterIdle();
      perTimerState = NO_ACTION;
    }
    else
    {
      /* If no sync is found the radio will not be put in IDLE and 
      * and perTimerState == ISR_ACTION_REQUIRED.
      * Sample RSSI when a sync was not detected 
      */
      rxData->rssi = perRfApi.perGetRssi();
      
      /* The next radio state in case the current test topology is 2-way 
      * is IDLE.
      */
      if(perSettings.linkTopology == LINK_2_WAY)
      {
        perRfApi.perEnterIdle();  
      } 
    }
    /* Abort timing if this is a 2-way test where link is initiated by Master */
    if(perSettings.linkTopology == LINK_2_WAY)
    {
      halTimer32kAbort();
    }
  }
  /*** End of Synchronize Master 32k Interupts to Packet Stream   ***/
}

/*******************************************************************************
* @fn          trxIsrConnect
*
* @brief       Connects an ISR function to PORT1 interrupt vector and 
*              configures the interrupt to be a high-low transition. 
* 
* input parameters
*
* @param       pfnIntHandler  - function pointer to ISR
*
* output parameters
*
* @return      void
*/
void trxIsrConnect(void (*pfnIntHandler)(void))
{
  ioPinIntRegister(IO_PIN_PORT_1, 0x80, pfnIntHandler);
  // Interrupt on falling edge
  ioPinIntTypeSet(IO_PIN_PORT_1, 0x80, IO_PIN_FALLING_EDGE);
}
/*******************************************************************************
* @fn          trxEnableInt
*
* @brief       Enables sync interrupt 
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
*/ 
void trxEnableInt(void)  {
  ioPinIntEnable(IO_PIN_PORT_1, 0x080);
}
/*******************************************************************************
* @fn          trxDisableInt
*
* @brief       Disables sync interrupt
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
*/ 
void trxDisableInt(void)  {
  ioPinIntDisable(IO_PIN_PORT_1, 0x080);
}
/*******************************************************************************
* @fn          trxClearIntFlag
*
* @brief       Clears sync interrupt flag
*
* input parameters
*
* @param       none
*
* output parameters
*
* @return      void
*/
void trxClearIntFlag(void) {
  ioPinIntClear(IO_PIN_PORT_1, 0x080);
}
/******************************************************************************
* @fn          trxSampleSyncPin
*
* @brief       Reads the value of the sync pin. 
*                 
* input parameters
*   
* @param       none
*
* output parameters
*
* @return      uint8
*/
uint8 trxSampleSyncPin(void)
{
  return ((P1IN & (0x01<<7))>>7);
}