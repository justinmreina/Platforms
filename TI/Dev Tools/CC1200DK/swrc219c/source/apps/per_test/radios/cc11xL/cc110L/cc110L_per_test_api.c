//*****************************************************************************
//! @file      cc110L_per_test_api.c
//  
//! @brief    Implemenation file for api-like functions that the per test
//              will call if a CC110L was detected.
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
#include "cc110L_per_test_api.h"
#include "cc11xL_spi.h"
#include "per_test.h"


/******************************************************************************
 * LOCAL FUNCTIONS
 */

static void cc110LRxIdle(void);
static void cc110LIdleRx(void);
static int8 cc110LConvert8BitRssi(uint8 rawRssi);

/******************************************************************************
 * LOCAL VARIABLES
 */

/* Variable is STATE_RX when in RX. If not in RX it is STATE_TX or STATE_IDLE. 
 * The use of this variable is only to avoid an RX interrupt when sending a 
 * packet. This facilitates a reduction in power consumption.
 */
static rfStatus_t cc110LRadioTxRx;

/******************************************************************************
 * CONSTANTS
 */ 


/* The following must be added to base and selected parameters */
#define PER_CC110L_MCSM1 0x0C /* CCA disabled, RX after packet, Idle after TX */
#define PER_CC110L_MCSM2 0x07 /* RX Time disabled - no termination */
#define PER_CC110L_MCSM0 0x18 /* Calibrate from IDLE to RX/TX, PO_TIMEOUT set to rec. settings */

/* No CCA, No RX timeout, Calibration from IDLE->RX/TX, PO_TIMEOUT, RX after pakt, 
 * IDLE after TX
 */
static uint8 CC110L_MCSMs[3]=
{
  PER_CC110L_MCSM2,
  PER_CC110L_MCSM1,
  PER_CC110L_MCSM0
};

static const int8 cc110LRssiOffset = 74;


/* Radio configurations exported from SmartRF Studio*/
 
static const registerSetting_t cc110LlowDataRateRfSettings[] = {
    {CC110L_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                      */  
    {CC110L_ADDR      ,0x00},  /* ADDR          Device Address                                    */     
    {CC110L_FSCTRL1   ,0x06},  /* FSCTRL1       Frequency Synthesizer Control                  */
    {CC110L_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                  */
    {CC110L_MDMCFG4   ,0xF5},  /* MDMCFG4       Modem Configuration                            */
    {CC110L_MDMCFG3   ,0x83},  /* MDMCFG3       Modem Configuration                            */
    {CC110L_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                            */
    {CC110L_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                            */
    {CC110L_DEVIATN   ,0x15},  /* DEVIATN       Modem Deviation Setting                        */
    {CC110L_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration    */    
    {CC110L_FOCCFG    ,0x16},  /* FOCCFG        Frequency Offset Compensation Configuration    */
    {CC110L_BSCFG     ,0x6C},  /* BSCFG         Bit Synchronization Configuration              */
    {CC110L_AGCCTRL2  ,0x03},  /* AGCCTRL2      AGC Control                                    */
    {CC110L_AGCCTRL1  ,0x40},  /* AGCCTRL1      AGC Control                                    */
    {CC110L_AGCCTRL0  ,0x91},  /* AGCCTRL0      AGC Control                                    */
    {CC110L_RESERVED_0X20   ,0xFB},  /*RESERVED_0X20    Reserved register                             */  
    {CC110L_FREND1    ,0xB6},  /* FREND1        Front End RX Configuration                        */
    {CC110L_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                        */    
    {CC110L_TEST2     ,0x81},  /* TEST2         Various Test Settings                          */
    {CC110L_TEST1     ,0x35},  /* TEST1         Various Test Settings                          */
    {CC110L_TEST0     ,0x09}  /* TEST0         Various Test Settings                           */
}; 

static const registerSetting_t cc110LmediumDataRateRfSettings[] = {
    {CC110L_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                         */ 
    {CC110L_ADDR      ,0x00},  /* ADDR          Device Address                                    */     
    {CC110L_FSCTRL1   ,0x06},  /* FSCTRL1       Frequency Synthesizer Control                  */
    {CC110L_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                  */
    {CC110L_MDMCFG4   ,0xCA},  /* MDMCFG4       Modem Configuration                            */
    {CC110L_MDMCFG3   ,0x83},  /* MDMCFG3       Modem Configuration                            */
    {CC110L_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                            */
    {CC110L_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                            */
    {CC110L_DEVIATN   ,0x35},  /* DEVIATN       Modem Deviation Setting                        */
    {CC110L_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration    */    
    {CC110L_FOCCFG    ,0x16},  /* FOCCFG        Frequency Offset Compensation Configuration    */
    {CC110L_BSCFG     ,0x6C},  /* BSCFG         Bit Synchronization Configuration              */
    {CC110L_AGCCTRL2  ,0x43},  /* AGCCTRL2      AGC Control                                    */
    {CC110L_AGCCTRL1  ,0x40},  /* AGCCTRL1      AGC Control                                    */
    {CC110L_AGCCTRL0  ,0x91},  /* AGCCTRL0      AGC Control                                    */
    {CC110L_RESERVED_0X20   ,0xFB},  /*RESERVED_0X20    Reserved register                             */  
    {CC110L_FREND1    ,0xB6},  /* FREND1        Front End RX Configuration                        */
    {CC110L_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                        */    
    {CC110L_TEST2     ,0x81},  /* TEST2         Various Test Settings                          */
    {CC110L_TEST1     ,0x35},  /* TEST1         Various Test Settings                          */
    {CC110L_TEST0     ,0x09}  /* TEST0         Various Test Settings                           */    
}; 


static const registerSetting_t cc110LhighDataRateRfSettings[] = {
    {CC110L_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                         */ 
    {CC110L_ADDR      ,0x00},  /* ADDR          Device Address                                    */     
    {CC110L_FSCTRL1   ,0x0C},  /* FSCTRL1       Frequency Synthesizer Control                     */
    {CC110L_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                     */
    {CC110L_MDMCFG4   ,0x2D},  /* MDMCFG4       Modem Configuration                               */
    {CC110L_MDMCFG3   ,0x3B},  /* MDMCFG3       Modem Configuration                               */
    {CC110L_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                               */
    {CC110L_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                               */
    {CC110L_DEVIATN   ,0x62},  /* DEVIATN       Modem Deviation Setting                           */
    {CC110L_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration    */   
    {CC110L_FOCCFG    ,0x1D},  /* FOCCFG        Frequency Offset Compensation Configuration       */
    {CC110L_BSCFG     ,0x1C},  /* BSCFG         Bit Synchronization Configuration                 */
    {CC110L_AGCCTRL2  ,0xC7},  /* AGCCTRL2      AGC Control                                       */
    {CC110L_AGCCTRL1  ,0x00},  /* AGCCTRL1      AGC Control                                       */
    {CC110L_AGCCTRL0  ,0xB0},  /* AGCCTRL0      AGC Control                                       */
    {CC110L_RESERVED_0X20   ,0xFB},  /*RESERVED_0X20    Reserved register                             */  
    {CC110L_FREND1    ,0xB6},  /* FREND1        Front End RX Configuration                        */
    {CC110L_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                        */    
    {CC110L_TEST2     ,0x88},  /* TEST2         Various Test Settings                             */
    {CC110L_TEST1     ,0x31},  /* TEST1         Various Test Settings                             */
    {CC110L_TEST0     ,0x09}   /* TEST0         Various Test Settings                             */
};

/* Common register settings for CC11xL radios and test case*/
static const registerSetting_t commonRfSettings[] = {
    {CC110L_IOCFG2        ,0x29},  /* IOCFG2          GDO2 Output Pin Configuration                     */
    {CC110L_IOCFG1        ,0x2E},  /* IOCFG1          GDO1 Output Pin Configuration                     */
    {CC110L_IOCFG0        ,0x06},  /* IOCFG0          GDO0 Output Pin Configuration                     */
    {CC110L_FIFOTHR       ,0x07},  /* FIFOTHR         RX FIFO and TX FIFO Thresholds                    */
    {CC110L_SYNC1         ,0xD3},  /* SYNC1           Sync Word, High Byte                              */
    {CC110L_SYNC0         ,0x91},  /* SYNC0           Sync Word, Low Byte                               */
    {CC110L_PKTLEN        ,0xFF},  /* PKTLEN          Packet Length                                     */
    {CC110L_PKTCTRL0      ,0x05},  /* PKTCTRL0        Packet Automation Control                         */ 
    {CC110L_FREQ2         ,0x10},  /* FREQ2           Frequency Control Word, High Byte                 */
    {CC110L_FREQ1         ,0xB1},  /* FREQ1           Frequency Control Word, Middle Byte               */
    {CC110L_FREQ0         ,0x3B},  /* FREQ0           Frequency Control Word, Low Byte                  */  
    {CC110L_MCSM1         ,0x30},  /* MCSM1           Main Radio Control State Machine Configuration    */
    {CC110L_MCSM0         ,0x18},  /* MCSM0           Main Radio Control State Machine Configuration    */
    {CC110L_FSCAL3        ,0xEA},  /* FSCAL3          Frequency Synthesizer Calibration                 */
    {CC110L_FSCAL2        ,0x2A},  /* FSCAL2          Frequency Synthesizer Calibration                 */
    {CC110L_FSCAL1        ,0x00},  /* FSCAL1          Frequency Synthesizer Calibration                 */
    {CC110L_FSCAL0        ,0x1F},  /* FSCAL0          Frequency Synthesizer Calibration                 */ 
    {CC110L_RESERVED_0X29 ,0x59},  /* RESERVED_0X29   Reserved register                                 */
    {CC110L_RESERVED_0X2A ,0x7F},  /* RESERVED_0X2A   Reserved register                                 */
    {CC110L_RESERVED_0X2B ,0x3F},  /* RESERVED_0X2B   Reserved register                                 */    
};

static const registerSetting_t linkBypassSettings[] = 
{
  {CC110L_MDMCFG2   ,0x03}, // 2-FSK
  {CC110L_DEVIATN   ,0x12},  // DEVIATN       Modem Deviation Setting
};

/* PA tables for different frequency bands - Use of which is decided by PER test GUI */
/* NB: If any of the output power tables are changed it must be reflected in per_test_gui.c */
static const uint8 per315MHzPowerTable[]=
{
  0x12, /* -30 dBm  - min */
  0xC2, /*  10 dBm  - max */
  0x0D, /* -20 dBm  - next lowest power */
  0x1C, /* -15 dBm  */
  0x34, /* -10 dBm  */
  0x51, /*   0 dBm  */
  0x85, /*   5 dBm  */
  0xCB  /*   7 dBm  - next highest power */ 
};

static const uint8 per434MHzPowerTable[]=
{
  0x12, /* -30 dBm - min */
  0xC0, /*  10 dBm - max */
  0x0E, /* -20 dBm - next lowest power */
  0x1D, /* -15 dBm */
  0x34, /* -10 dBm */
  0x60, /*   0 dBm */
  0x84, /*   5 dBm */
  0xC8  /*   7 dBm - next highest power */
};


static const uint8 per915MHzPowerTable[]=
{
  0x03, /* -30 dBm - min */
  0xC0, /*  10 dBm - max */
  0x0E, /* -20 dBm - next lowest power */
  0x1E, /* -15 dBm */
  0x27, /* -10 dBm */
  0x8E, /*   0 dBm */
  0xCD, /*   5 dBm */
  0xC7  /*   7 dBm - next highest power */ 
};


static const uint8 per868MHzPowerTable[]=
{
  0x03, /* -30 dBm  - min */
  0xC2, /*  10 dBm  - max */
  0x0F, /* -20 dBm  - next lowest power */
  0x1E, /* -15 dBm  */
  0x27, /* -10 dBm  */
  0x50, /*   0 dBm  */
  0x81, /*   5 dBm  */
  0xCB  /*   7 dBm  - next highest power */
};

static uint8 CC110L_FREQs[4][3] =
{
  {0x0C,0x1D,0x89}, /* 315 MHz */
  {0x10,0xB1,0x3B}, /* 434 MHz */
  {0x21,0x62,0x76}, /* 868 MHz */
  {0x23,0x31,0x3B}  /* 915 MHz */
};

static const int8 paPowerGuiValues[8] = 
{
  -30, /* - index 0 */
   10, /* - index 1 */
  -20, /* - index 2 */
  -15, /* - index 3 */
  -10, /* - index 4 */
    0, /* - index 5 */
    5, /* - index 6 */
    7  /* - index 7 */
};


/* Sensitivity table according to CC11xL datasheet, p. 11-12 and data rate */
static const int16 perSensitivityTable[]=
{
  -112,
  -104,
  -95
};

static const float testCaseDataRate[3] =
{
    1.20,  /* <=> SMARTRF_CONFIGURATION_0 */
   38.38,  /* <=> SMARTRF_CONFIGURATION_1 */
  249.94   /* <=> SMARTRF_CONFIGURATION_2 */
};

/******************************************************************************
 * FUNCTIONS
 */



/******************************************************************************
* @fn          perCC110LGetGuiTxPower
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
int8 perCC110LGetGuiTxPower(uint8 index)
{
  return paPowerGuiValues[index];
}

/******************************************************************************
* @fn          perCC110LGetDataRate
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
float perCC110LGetDataRate(uint8 index)
{
  return testCaseDataRate[index];
} 
/******************************************************************************
* @fn          perCC110LSetOutputPower
*
* @brief       Configures the output power of CC110L according to the provided
*              index:
*              0 = -30 dBm
*              1 =  10 dBm
*              2 = -20 dBm
*              3 = -15 dBm
*              4 = -10 dBm
*              5 =   0 dBm
*              6 =   5 dBm
*              7 =   7 dBm
*               
*
* input parameters
*
* @param       index - index to power table <=> wanted output level
*                  
* output parameters
*
* @return      void
*/
void perCC110LSetOutputPower(uint8 index)
{
  uint8 level;
  switch(perSettings.frequencyBand)
  {
    case 0:
      level =  per315MHzPowerTable[index];
      break;
    case 1: 
      level =  per434MHzPowerTable[index];
      break;
    case 2:
      level =  per868MHzPowerTable[index];
      break;
    case 3:
      level =  per915MHzPowerTable[index];
      break;
    default:
      level =  per868MHzPowerTable[index];
      break;
  }
  cc11xLSpiWriteReg(CC110L_PA_TABLE0,&level,1);
  return;
}


/******************************************************************************
 * @fn          perCC110LRegConfig
 *
 * @brief       Configures the CC110L radio with the selected SmartRF Studio
 *              paramters and test properties or the base configuration 
 *              with no address check. Assumes that the radio is in IDLE.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      void
 */
void perCC110LRegConfig(void)
{
  /* initialize radio registers given the selected perSettings */
  uint8 data;
  
  /* Log that radio is in IDLE state */
  cc110LRxIdle();
  cc110LRadioTxRx = CC110L_STATE_IDLE;
  
  /* Extract what radio configuration to use */
  if((perSettings.masterSlaveLinked==PER_DEVICE_LINKED)|| 
      (perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS))
  {
    switch(perSettings.smartRfConfiguration)
    {
    case 0:
      for(uint16 i = 0; i < (sizeof cc110LlowDataRateRfSettings/sizeof(registerSetting_t));i++)
      {
        data = cc110LlowDataRateRfSettings[i].data;
        cc11xLSpiWriteReg(cc110LlowDataRateRfSettings[i].addr,&data,1);
      }
      perSettings.sensitivity = perSensitivityTable[0];
      break;
    case 1:
      for(uint16 i = 0; i < (sizeof cc110LmediumDataRateRfSettings/sizeof(registerSetting_t));i++)
      {
        data = cc110LmediumDataRateRfSettings[i].data;
        cc11xLSpiWriteReg(cc110LmediumDataRateRfSettings[i].addr,&data,1);
      }
      perSettings.sensitivity = perSensitivityTable[1];
      break;
    case 2:
      for(uint16 i = 0; i < (sizeof cc110LhighDataRateRfSettings/sizeof(registerSetting_t));i++)
      {
        data = cc110LhighDataRateRfSettings[i].data;
        cc11xLSpiWriteReg(cc110LhighDataRateRfSettings[i].addr,&data,1);
      }
      perSettings.sensitivity = perSensitivityTable[2];
      break;
    default:
      for(uint16 i = 0; i < (sizeof cc110LhighDataRateRfSettings/sizeof(registerSetting_t));i++)
      {
        data = cc110LhighDataRateRfSettings[i].data;
        cc11xLSpiWriteReg(cc110LhighDataRateRfSettings[i].addr,&data,1);
      }
      perSettings.sensitivity = perSensitivityTable[2];
      break;
    }
    /* add support for configuratos mode */
  }
  else
  {
    for(uint16 i = 0; i < (sizeof cc110LmediumDataRateRfSettings/sizeof(registerSetting_t));i++)
    {
      data = cc110LmediumDataRateRfSettings[i].data;
      cc11xLSpiWriteReg(cc110LmediumDataRateRfSettings[i].addr,&data,1);
    }
    perSettings.sensitivity = perSensitivityTable[1];
  }
  
  /* Common settings for PER test regardless of radio configuration */
  for(uint16 i = 0; i < (sizeof commonRfSettings/sizeof(registerSetting_t));i++)
  {
    data = commonRfSettings[i].data;
    cc11xLSpiWriteReg(commonRfSettings[i].addr,&data,1);
  }
 
  /* Differences from recommended studio values and values needed */
  cc11xLSpiWriteReg(CC110L_MCSM2, CC110L_MCSMs,3);
  
  /* Correct for chosen frequency band */
  cc11xLSpiWriteReg(CC110L_FREQ2, CC110L_FREQs[perSettings.frequencyBand],3);

  
  if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
  {
    /* PKTLEN set to user specified packet length: HW length filtering */
    cc11xLSpiWriteReg(CC110L_PKTLEN, &(perSettings.payloadLength),1);
    /* Turn on HW Address filtering */
    data = 0x05;
    cc11xLSpiWriteReg(CC110L_PKTCTRL1,&data,1);
    /* Set address */
    cc11xLSpiWriteReg(CC110L_ADDR,&perSettings.address,1);
    /* Two way uses different MCSM1 settings */
    if((perSettings.linkTopology == LINK_2_WAY))
    {
      if(perSettings.deviceMode == SLAVE_DEVICE)
      {
        /* RX after RX, IDLE after TX --> Calibration prioritized */ 
        /* RX->TX forced by command strobe */
        data = 0x0C;
        cc11xLSpiWriteReg(CC110L_MCSM1,&data,1);
      }
      else
      {
        /* RX after TX, IDLE after RX */
        data = 0x03;
        cc11xLSpiWriteReg(CC110L_MCSM1,&data,1);
      }
    }
  }
  else if(perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS)
  {
    //Change modulation and deviation to apply to link bypass configuration
    for(uint16 i = 0; i < (sizeof linkBypassSettings/sizeof(registerSetting_t));i++)
    {
      data = linkBypassSettings[i].data;
      cc11xLSpiWriteReg(linkBypassSettings[i].addr,&data,1);
    }  
    
    /* PKTLEN set to user specified packet length: HW length filtering */
    cc11xLSpiWriteReg(CC110L_PKTLEN, &(perSettings.payloadLength),1);
  }
  else
  {
    /* length of configuration packet + filter byte */
    data = PER_SETTINGS_PACKET_LEN; 
    cc11xLSpiWriteReg(CC110L_PKTLEN, &data,1);
  }
  return;
}

/******************************************************************************
 * @fn          perCC110LSendPacket
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
 * @return      void
 */
void perCC110LSendPacket(uint8 *pData)
{
  uint8 len = *pData;
  /* Will only try to transmit if the whole packet can fit i RXFIFO 
   * and we're not currently sending a packet.
   */
  if(!(len > (PER_MAX_DATA-2)) && (cc110LRadioTxRx != CC110L_STATE_TX) )
  {
    cc11xLSpiWriteTxFifo(pData,(len+1));
    /* Indicate state to the ISR and issue the TX strobe */
    trxEnableInt();
    cc110LRadioTxRx = CC110L_STATE_TX; 
    trxSpiCmdStrobe(CC110L_STX);
    /* Wait until packet is sent before doing anything else */
    __low_power_mode_3();
    while(cc110LRadioTxRx == CC110L_STATE_TX);
    if(perSettings.linkTopology == LINK_1_WAY)
    {
      /* Back in Idle*/
      trxDisableInt();
    }
  }
  return;
}

/******************************************************************************
 * @fn          perCC110LEnterRx
 *
 * @brief       Enters RX. Function is used to abstract the cc110LRadioTxRx
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
void perCC110LEnterRx(void)
{
  cc110LIdleRx();
  cc110LRadioTxRx = CC110L_STATE_RX;
  return;
}

/******************************************************************************
 * @fn          perCC110LEnterSleep
 *
 * @brief       Enters Sleep. Function is used to abstract the cc110LRadioTxRx
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
void perCC110LEnterSleep(void)
{
  cc110LRxIdle();
  trxSpiCmdStrobe(CC110L_SPWD);
  /* Only important to differ between RX/TX and IDLE */
  cc110LRadioTxRx = CC110L_STATE_IDLE;
  return;
}

/******************************************************************************
 * @fn          perCC110LEnterIdle
 *
 * @brief       Enters IDLE from ANY state. Function is used to abstract the 
 *              cc110LRadioTxRx functionality away from the lower-level radio 
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
void perCC110LEnterIdle(void)
{
  /* wait until chip is ready */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  cc110LRxIdle();
  cc110LRadioTxRx = CC110L_STATE_IDLE;
  return;
}

/******************************************************************************
 * @fn          cc110LRxISR
 *
 * @brief       ISR that's called when sync signal goes low. 
 *              In RX State: Filters incoming data. The global rxData pointer
 *              always points to this functions static rxData_tmp(struct of
 *              same kind). The validnes of rxData fields is indicated by the
 *              the global flag packetSemaphore.
 *              In TX State: Nothing is done except it facilitates power 
 *              consumption reduction when TX since the program doesn't need
 *              to wait until TX is done before re-enabling sync pin interrupt.
 *              cc11xLRadioTxRx is also set to CC110L_STATE_IDLE to be consistent with
 *              program.
 * 
 * input parameters
 *             
 * @param       none
 *
 * output parameters 
 *
 * @return      void
 */
void perCC110LRxTxISR(void)
{
  uint8 rxBytes,rxBytesVerify,rxLength,rssiIndex,lqiIndex;
  /* This variable stores the data locally. Access is given to per_test by 
   * assigning this instance to the global rxData pointer
   */
  static rxData_t rxData_tmp;
  
  rxData = &rxData_tmp;
  /* Checking if the chip is in RX state: */
  if(cc110LRadioTxRx != CC110L_STATE_RX)
  {
    /* Transmission finished */
    if((perSettings.deviceMode == MASTER_DEVICE) && (perSettings.linkTopology == LINK_2_WAY) && (perSettings.masterSlaveLinked == PER_DEVICE_LINKED))
    {
      /* RX after TX only applicable when master in 2-way test */
      cc110LRadioTxRx=CC110L_STATE_RX;
    }
    else
    {
      /* IDLE after TX in all other cases */
      cc110LRadioTxRx  = CC110L_STATE_IDLE;
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
  cc11xLSpiReadReg(CC110L_RXBYTES,&rxBytesVerify,1);
  do
  {
    rxBytes = rxBytesVerify;
    cc11xLSpiReadReg(CC110L_RXBYTES,&rxBytesVerify,1);
  }
  while(rxBytes != rxBytesVerify);
  
  /* Checking if the FIFO is empty */
  if(rxBytes == PER_FALSE)
  {
    /* The packet was removed by HW due to addr or length filtering -> Do nothing */
    /* Report that a sync was detected */ 
    rxData_tmp.rssi = perCC110LRead8BitRssi();
    return;
  }
  else
  {
    /* The RX FIFO is not empty, process contents */    
    cc11xLSpiReadRxFifo(&rxLength, 1);  
    /* Check that the packet length just read + FCS(2B) + length byte match the RXBYTES */
    /* If these are not equal:
     * - RXFIFO overflow: Received packets not processed while beeing in RX. 
     */
    if(rxBytes != (rxLength+3))
    {
      /* This is a fault FIFO condition -> clean FIFO and register a sync detection */
      /* IDLE -> FLUSH RX FIFO -> RX */
      cc110LRxIdle();
      cc110LIdleRx();
      /* Report that a sync was detected */
      rxData_tmp.rssi = perCC110LRead8BitRssi();
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
      cc11xLSpiReadRxFifo(&rxData_tmp.data[1], lqiIndex);
      
      /* The whole packet has been read from the FIFO.
       * Check if the CRC is correct and that the packet length is as expected.
       * If not correct: report sync found and do not update RSSI or LQI.
       */
      if( (!(rxData_tmp.data[lqiIndex] & CC110L_CRC_OK_BM)) || (perSettings.payloadLength != rxLength ))
      {
        rxData_tmp.rssi = perCC110LRead8BitRssi();
        return;
      }
      /* A complete error-free packet has arrived  */
      rxData_tmp.length  = rxLength;
      rxData_tmp.lqi     = rxData_tmp.data[lqiIndex] & CC110L_LQI_EST_BM;
      rxData_tmp.addr    = rxData_tmp.data[1]; /* May not be the address, dependant on if this is used or not */
     
      /* Convert RSSI value from 2's complement to decimal value accounting for offset value */
      rxBytes = rxData_tmp.data[rssiIndex];
      rxData_tmp.rssi = cc110LConvert8BitRssi(rxBytes);
      /* Signal a good packet is received */
      packetSemaphore |= PACKET_RECEIVED;
      return;
    }
  }
}

/*******************************************************************************
 * @fn          perCC110LRead8BitRssi
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
int8 perCC110LRead8BitRssi(void)
{
  uint8 rssi2compl,rssi2compl_1;
  int16 rssiConverted;
  
  /* Read RSSI from MSB register */
  cc11xLSpiWriteReg(CC110L_RSSI, &rssi2compl,1);
  do
  {
    rssi2compl_1 = rssi2compl;
    cc11xLSpiReadReg(CC110L_RSSI,&rssi2compl,1);
  }
  while(rssi2compl_1 != rssi2compl);
  
  rssiConverted = cc110LConvert8BitRssi(rssi2compl);
  return rssiConverted;
}

static int8 cc110LConvert8BitRssi(uint8 rawRssi)
{
  int16 rssiConverted;
  
  if(rawRssi >= 128)
  {
    rssiConverted = (int16)(((int16)(rawRssi-256)/2) - cc110LRssiOffset);
  }
  else
  {
    rssiConverted = (int16)((rawRssi/2) - cc110LRssiOffset);
  }
  /* Restricting to 8 bit signed number range */
  if(rssiConverted < -128)
  {
    rssiConverted = -128;
  }
  return (int8)rssiConverted;
} 



/*******************************************************************************
 * @fn          cc110LRxIdle
 *
 * @brief       Radio state is switched from RX to IDLE
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */ 
static void cc110LRxIdle(void)
{
  /* Disable pin interrupt */
  trxDisableInt();
  /* Strobe IDLE */
  trxSpiCmdStrobe(CC110L_SIDLE); 
  /* Wait until chip is in IDLE */
  while(trxSpiCmdStrobe(CC110L_SNOP) & 0xF0);
  /* Flush the Receive FIFO */
  trxSpiCmdStrobe(CC110L_SFRX);
  /* Clear pin interrupt flag */
  trxClearIntFlag();
  return;
}

/*******************************************************************************
 * @fn          cc110LIdleRx
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
 * @return      void
 */ 
static void cc110LIdleRx(void)
{
  trxClearIntFlag();
  trxSpiCmdStrobe(CC110L_SRX);
  trxEnableInt();
  return;
}

/*******************************************************************************
 * @fn          perCC110LWriteTxFifo
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
 * @return      void
 */ 
void perCC110LWriteTxFifo(uint8 *pData, uint8 len)
{
  cc11xLSpiWriteTxFifo(pData,len);
  return;
}