//*****************************************************************************
//! @file      cc1120_per_test_api.c
//  
//! @brief    Implementation file for api-like functions that the per test
//            will call if a CC1120_CC1190 combo was selected.             
//  
//            CC1120_CC1190 will support the PER test with the following data rates:
//            - 1.2 Kb/s  with 25 KHz bandwidth, almost tc_01, named 
//            cc112xLowDataRateRfSettings, hence more noise on RSSI samples and lower
//            sensitivity.
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
#include "per_test.h"
#include "trx_rf_spi.h"
#include "trx_rf_int.h"
#include "cc112x_spi.h"
#include "cc1120_cc1190_per_test_api.h"
#include "freq_xosc_detect.h"


/******************************************************************************
 * TYPEDEFS
 */
// defines used for the manual calibration
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2

/******************************************************************************
 * LOCAL FUNCTIONS
 */

static void cc1120cc1190RxIdle(void);
static void cc1120cc1190IdleRx(void);   
static void manualCalibration(void);


/* cc1190 pa and lna functions */
void perCC1120CC1190PaEnable(void);
void perCC1120CC1190PaDisable(void);
void perCC1120CC1190LnaEnable(void);
void perCC1120CC1190LnaDisable(void);
void perCC1120CC1190HgmEnable(void);
void perCC1120CC1190HgmDisable(void);
/******************************************************************************
 * CONSTANTS
 */
 
/********************************* TestCases *********************************/
   // Base settings: Variable packet length, CRC calculation, no Address check, 
   // Append Mode, Always give Clear channel indication 
   //

// Modem settings for test case 1 adjusted to 4 Byte preamble(")*/ 
static const registerSetting_t cc1120cc1190LowDataRateRfSettings[] =  
{
  {CC112X_IOCFG0            ,0x06}, // Route sync signal to GPIO0
  {CC112X_FS_DIG1           ,0x00},
  {CC112X_FS_DIG0           ,0x5F},
  {CC112X_FS_CAL0           ,0x0E},
  {CC112X_FS_DIVTWO         ,0x03},
  {CC112X_FS_DSM0           ,0x33},
  {CC112X_FS_DVC0           ,0x17},  
  {CC112X_FS_PFD            ,0x50},  
  {CC112X_FS_PRE            ,0x6E},
  {CC112X_FS_REG_DIV_CML    ,0x14},
  {CC112X_FS_SPARE          ,0xAC},
  {CC112X_XOSC5             ,0x0E},
  {CC112X_XOSC4             ,0xA0},
  {CC112X_XOSC3             ,0xC7},  
  {CC112X_XOSC1             ,0x03},
  {CC112X_ANALOG_SPARE      ,0x00},
  {CC112X_FIFO_CFG          ,0x00},
  {CC112X_DEV_ADDR          ,0x00},  
  {CC112X_SETTLING_CFG      ,0x03},
  {CC112X_FS_CFG            ,0x12}, //////////////////////////////////////////////////////////////////
  {CC112X_PKT_CFG2          ,0x00},
  {CC112X_PKT_CFG1          ,0x05}, // Address check off and CRC check on
  {CC112X_PKT_CFG0          ,0x20},  
  {CC112X_RFEND_CFG1        ,0x0F}, // Stay in RX after RX, No timeout for sync word search  
  {CC112X_RFEND_CFG0        ,0x00}, // IDle after TX, no interferring from MARC  
  {CC112X_SYNC3             ,0x93},/////////////////////////////////////////////////////////////////////
  {CC112X_SYNC2             ,0x0B}, 
  {CC112X_SYNC1             ,0x51}, 
  {CC112X_SYNC0             ,0xDE}, 
  {CC112X_SYNC_CFG1         ,0x0B}, 
  {CC112X_SYNC_CFG0         ,0x17}, 
  {CC112X_DEVIATION_M       ,0x06}, // (4000 Hz)       
  {CC112X_MODCFG_DEV_E      ,0x03}, // (4000 Hz)       
  {CC112X_DCFILT_CFG        ,0x1C}, //                 
  {CC112X_PREAMBLE_CFG1     ,0x18}, // 4" byte preamble 
  {CC112X_PREAMBLE_CFG0     ,0x2A}, //                 
  {CC112X_FREQ_IF_CFG       ,0x40}, // (62500 Hz)      
  {CC112X_IQIC              ,0xC6}, //                 
  {CC112X_CHAN_BW           ,0x08}, // (25000" Hz)      
  {CC112X_MDMCFG1           ,0x46}, //                 
  {CC112X_MDMCFG0           ,0x05}, //                 
  {CC112X_SYMBOL_RATE2            ,0x43}, // (1200 bps)      
  {CC112X_SYMBOL_RATE1            ,0xA9}, // (1200 bps)      
  {CC112X_SYMBOL_RATE0            ,0x2A}, // (1200 bps)      
  {CC112X_AGC_REF           ,0x20}, 
  {CC112X_AGC_CS_THR        ,0x19}, 
  {CC112X_AGC_GAIN_ADJUST   ,0x00}, 
  {CC112X_AGC_CFG3          ,0x91}, 
  {CC112X_AGC_CFG2          ,0x20}, 
  {CC112X_AGC_CFG1          ,0xA9}, 
  {CC112X_AGC_CFG0          ,0xCF}, 
  {CC112X_PA_CFG2           ,0x7F}, 
  {CC112X_PA_CFG1           ,0x56}, 
  {CC112X_PA_CFG0           ,0x7C}, 
  {CC112X_IF_MIX_CFG        ,0x00}, 
  {CC112X_FREQOFF_CFG       ,0x22}, 
  {CC112X_TOC_CFG           ,0x0B},
  {CC112X_CFM_DATA_CFG  ,0x00}
};
// RX filter BW = 100.000000 
// Address config = No address check 
// Packet length = 255 
// Symbol rate = 50 
// PA ramping = true 
// Carrier frequency = 915.000000 
// Bit rate = 50 
// Whitening = false 
// Manchester enable = false 
// Modulation format = 2-GFSK 
// Packet length mode = Variable 
// Device address = 0 
// TX power = 15 
// Deviation = 24.963379 

static const registerSetting_t cc1120cc1190MediumDataRateRfSettings[]= 
{
  {CC112X_IOCFG0            ,0x06}, // Route sync signal to GPIO0
  {CC112X_FS_DIG1           ,0x00},
  {CC112X_FS_DIG0           ,0x5F},
  {CC112X_FS_CAL0           ,0x0E},
  {CC112X_FS_DIVTWO         ,0x03},
  {CC112X_FS_DSM0           ,0x33},
  {CC112X_FS_DVC0           ,0x17},  
  {CC112X_FS_PFD            ,0x50},  
  {CC112X_FS_PRE            ,0x6E},
  {CC112X_FS_REG_DIV_CML    ,0x14},
  {CC112X_FS_SPARE          ,0xAC},
  {CC112X_XOSC5             ,0x0E},
  {CC112X_XOSC4             ,0xA0},
  {CC112X_XOSC3             ,0xC7},  
  {CC112X_XOSC1             ,0x03},
  {CC112X_ANALOG_SPARE      ,0x00},
  {CC112X_FIFO_CFG          ,0x00},
  {CC112X_DEV_ADDR          ,0x00},  
  {CC112X_SETTLING_CFG      ,0x03},
  {CC112X_FS_CFG            ,0x12}, //////////////////////////////////////////////////////////////////
  {CC112X_PKT_CFG2          ,0x00},
  {CC112X_PKT_CFG1          ,0x05},  // Address check off and CRC check on
  {CC112X_PKT_CFG0          ,0x20},  
  {CC112X_RFEND_CFG1        ,0x0F},  // Stay in RX after RX, No timeout for sync word search  
  {CC112X_RFEND_CFG0        ,0x00},  // IDle after TX, no interferring from MARC  
  {CC112X_SYNC3             ,0x93},////////////////////////////////////////////////////////////////////          
  {CC112X_SYNC2             ,0x0B},           
  {CC112X_SYNC1             ,0x51},          
  {CC112X_SYNC0             ,0xDE},          
  {CC112X_SYNC_CFG1         ,0x08},       
  {CC112X_SYNC_CFG0         ,0x17},      
  {CC112X_DEVIATION_M       ,0x99},    
  {CC112X_MODCFG_DEV_E      ,0x0D},   
  {CC112X_DCFILT_CFG        ,0x15},    
  {CC112X_PREAMBLE_CFG1     ,0x18},  
  {CC112X_PREAMBLE_CFG0     ,0x2A},  
  {CC112X_FREQ_IF_CFG       ,0x00},     
  {CC112X_IQIC              ,0x00},           
  {CC112X_CHAN_BW           ,0x01},         
  {CC112X_MDMCFG1           ,0x46},        
  {CC112X_MDMCFG0           ,0x05},         
  {CC112X_SYMBOL_RATE2            ,0x99},        
  {CC112X_SYMBOL_RATE1            ,0x99},        
  {CC112X_SYMBOL_RATE0            ,0x99},        
  {CC112X_AGC_REF           ,0x3C},        
  {CC112X_AGC_CS_THR        ,0xEF},    
  {CC112X_AGC_GAIN_ADJUST   ,0x00}, 
  {CC112X_AGC_CFG3          ,0x83},       
  {CC112X_AGC_CFG2          ,0x60},      
  {CC112X_AGC_CFG1          ,0xA9},      
  {CC112X_AGC_CFG0          ,0xC0},      
  {CC112X_PA_CFG2           ,0x7F},        
  {CC112X_PA_CFG1           ,0x56},        
  {CC112X_PA_CFG0           ,0x79},        
  {CC112X_IF_MIX_CFG        ,0x04},     
  {CC112X_FREQOFF_CFG       ,0x20},    
  {CC112X_TOC_CFG           ,0x0A},         
  {CC112X_CFM_DATA_CFG  ,0x00}
};

// Register settings for link bypass mode that differ from the
// other link cofigurations.
static const registerSetting_t linkBypassSettings[] = 
{
  {CC112X_SYNC3             ,0xD3},          
  {CC112X_SYNC2             ,0x91},           
  {CC112X_SYNC1             ,0xD3}, //sync word compatible with CC1101         
  {CC112X_SYNC0             ,0x91}          
};

//Band select settings for the LO divider.
//out of lock detector enabled
static uint8 cc112xFsCfgs[2] = 
{
  0x12, // 869 MHz
  0x12  // 915 MHz
};
//Frequency programming for CC1120_CC1190 Combo
// Currently only one frequency option due to
// output power restrictions in other bands.
// perSettings.frequencyBand = 0 => 869 MHz 

// For CC112x with 32 MHz XOSC
static uint8 freqConfiguration[2][3] =
{
  {0x6C,0xB0,0xCD}, // 869.525 MHz 
  {0x72,0x60,0x00} //  915 MHz  
};


/* Sensitivity table - Note: It's only valid for 3 bytes packets and for shipped register settings */
/* Used in Link Margin calculation */  
static const int8 sensitivity869Mhz[2] = 
{
  -126,
  -113
};
static const int8 sensitivity915Mhz[2] = 
{
  -125,
  -112
};

/* RSSI offset table */
static const int8 rssiOffset869Mhz[2] = 
{
  108,  //HGM 
  108
};
static const int8 rssiOffset915Mhz[2] = 
{
  108,  //HGM 
  108
};

/* Values in this table must be ored with the PA_CFG2 register to account for  
 * pa_shape_en
 * Formula: paPowerRamp[index] = dec2hex((wanted_dBm_level+18)*2-1)
 */
static const uint8 paPowerRamp[2] = 
{
  0x03, /* -00 dBm - lowest power       - index 0 */
  0x37  /*  26 dBm - highest power - index 1*/
};
/* Access by index gives GUI values for TX power corresponding to the table above */
static const int8 paPowerGuiValues869[2]=
{
    0,  /* - index 0 */
   27,  /* - index 1 */
};
static const int8 paPowerGuiValues915[2]=
{
    0,  /* - index 0 */
   26,  /* - index 1 */
};

static const float testCaseDataRate[2]=
{
    1.20,  /* testcase 1   <=> SMARTRF_CONFIGURATION_0 */
   50.00,  /* testcase 1   <=> SMARTRF_CONFIGURATION_1 */
};
 
/******************************************************************************
 * LOCAL VARIABLES
 */

/* Variable is CC112X_STATE_RX when in RX. If not in RX it is CC112X_STATE_TX or 
 * CC112X_STATE_IDLE. The use of this variable is only to avoid an RX interrupt
 * when sending a packet. This facilitates a reduction in power consumption.
 */
static rfStatus_t cc1120cc1190RadioTxRx; 
static int8 cc1120cc1190RssiOffset; 
 

/******************************************************************************
* @fn          perCC1120CC1190GetGuiTxPower
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
int8 perCC1120CC1190GetGuiTxPower(uint8 index)
{
  if(perSettings.frequencyBand == 0){
    return paPowerGuiValues869[index];
  }
  else{
    return paPowerGuiValues915[index];
  }
}
/******************************************************************************
* @fn          perCC1120CC1190GetDataRate
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
float perCC1120CC1190GetDataRate(uint8 index)
{
  return testCaseDataRate[index];
}


/******************************************************************************
* @fn          perCC1120CC1190SetOutputPower
*
* @brief       Configures the output power of CC112x according to the provided
*              index:
*              0 =  xx dBm
*              1 =  26 dBm

*              NOTE: for PG2.0 pa_shape_en and pa_power_ramp has swapped 
*                    position
*
*
*
* input parameters
*
* @param       index - index to table <=> wanted output level
*                  
* output parameters
*
* @return      void
*/
void perCC1120CC1190SetOutputPower(uint8 index)
{
  uint8 level; 
  
  /* Reading the PA_CFG2 value to account for pa_shape_en */
  cc112xSpiReadReg(CC112X_PA_CFG2,&level,1);
  /* Saving pa_shape_en */
  level &= 0x40;
  /* Oring in the PA power ramp value */
  level |= paPowerRamp[index];
  /* Updating PA_CFG2 register with its' new value */
  cc112xSpiWriteReg(CC112X_PA_CFG2,&level,1);
  return;
}

/******************************************************************************
 * @fn          perCC1120CC1190RegConfig
 *
 * @brief       Configures the CC1120 radio with the selected 
 *              paramteres and test properties or uses the base configuration 
 *              with no address check. Assumes that the radio is in IDLE.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      void
 */
void perCC1120CC1190RegConfig(void)
{
  uint8 data;
  /* Log that radio is in IDLE state */
  cc1120cc1190RxIdle();
  cc1120cc1190RadioTxRx = CC112X_STATE_IDLE;
    
  /* Extract what radio configuration to use */
  /* Only one configuration implemented so far, but keeping structure to add
     more configurations later */
  if((perSettings.masterSlaveLinked==PER_DEVICE_LINKED) || 
      (perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS))
  {
    switch(perSettings.smartRfConfiguration)
    {
     case 0:
        for(uint16 i = 0; i < (sizeof cc1120cc1190LowDataRateRfSettings/sizeof(registerSetting_t));i++)
        {
          data = cc1120cc1190LowDataRateRfSettings[i].data;
          cc112xSpiWriteReg(cc1120cc1190LowDataRateRfSettings[i].addr,&data,1);
        }
        if(perSettings.frequencyBand == 0){
          perSettings.sensitivity = sensitivity869Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset869Mhz[0];          
        }
        else{
          perSettings.sensitivity = sensitivity915Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset915Mhz[0];
        }
        break;
     case 1:
        for(uint16 i = 0; i < (sizeof cc1120cc1190MediumDataRateRfSettings/sizeof(registerSetting_t));i++)
        {
          data = cc1120cc1190MediumDataRateRfSettings[i].data;
          cc112xSpiWriteReg(cc1120cc1190MediumDataRateRfSettings[i].addr,&data,1);
        }
        if(perSettings.frequencyBand == 0){
          perSettings.sensitivity = sensitivity869Mhz[1];
          cc1120cc1190RssiOffset = rssiOffset869Mhz[1];          
        }
        else{
          perSettings.sensitivity = sensitivity915Mhz[1];
          cc1120cc1190RssiOffset = rssiOffset915Mhz[1];
        }
        break;      
      default:
        for(uint16 i = 0; i < (sizeof cc1120cc1190LowDataRateRfSettings/sizeof(registerSetting_t));i++)
        {
          data = cc1120cc1190LowDataRateRfSettings[i].data;
          cc112xSpiWriteReg(cc1120cc1190LowDataRateRfSettings[i].addr,&data,1);
        }
        if(perSettings.frequencyBand == 0){
          perSettings.sensitivity = sensitivity869Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset869Mhz[0];          
        }
        else{
          perSettings.sensitivity = sensitivity915Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset915Mhz[0];
        }
        break;
    }
  }
  else
  {
    /* Base settings for communication */
        for(uint16 i = 0; i < (sizeof cc1120cc1190LowDataRateRfSettings/sizeof(registerSetting_t));i++)
        {
          data = cc1120cc1190LowDataRateRfSettings[i].data;
          cc112xSpiWriteReg(cc1120cc1190LowDataRateRfSettings[i].addr,&data,1);
        }
        if(perSettings.frequencyBand == 0){
          perSettings.sensitivity = sensitivity869Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset869Mhz[0];          
        }
        else{
          perSettings.sensitivity = sensitivity915Mhz[0];
          cc1120cc1190RssiOffset = rssiOffset915Mhz[0];
        }
  }
  /* Correct for chosen frequency band */     
  switch(perSettings.frequencyBand)      
  {  
   case 0:
      cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[0],1);
      cc112xSpiWriteReg(CC112X_FREQ2,freqConfiguration[0],3);
      break;
   case 1:
      cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[1],1);
      cc112xSpiWriteReg(CC112X_FREQ2,freqConfiguration[1],3);
      break;
    default: // 869.525 MHz
      cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[0],1);
      cc112xSpiWriteReg(CC112X_FREQ2,freqConfiguration[0],3);
      break;
  }    


  if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
  {
    /* PKT_LEN set to user specified packet length: HW length filtering */
    cc112xSpiWriteReg(CC112X_PKT_LEN, &(perSettings.payloadLength),1);
    /* Turn on HW Address filtering */
    uint8 pkt_cfg1 = 0x10|0x05;
    cc112xSpiWriteReg(CC112X_PKT_CFG1,&pkt_cfg1,1);
    /* Set address */
    cc112xSpiWriteReg(CC112X_DEV_ADDR,&perSettings.address,1);
    /* Note: The Two-way link for the CC1120 PG1.0 use a different implementation
     *       of the two-way link on the master side than what CC1101 and CC1120 PG2.0 
     *       will. RXOFF and TXOFF mode is in this case the same as for the one-way link.
     *       
     */
    if((perSettings.linkTopology == LINK_2_WAY))
    {
      if(perSettings.deviceMode == MASTER_DEVICE)
      {
        /* IDLE after RX, RX after TX */
        data = 0x0F;
        cc112xSpiWriteReg(CC112X_RFEND_CFG1,&data,1);
        data = 0x30;
        cc112xSpiWriteReg(CC112X_RFEND_CFG0,&data,1);
      }
    } 
  }
   else if(perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS)
  {
    //Change sync word to apply to link bypass configuration
    for(uint16 i = 0; i < (sizeof linkBypassSettings/sizeof(registerSetting_t));i++)
    {
      data = linkBypassSettings[i].data;
      cc112xSpiWriteReg(linkBypassSettings[i].addr,&data,1);
    }  
    
    /* PKT_LEN set to user specified packet length: HW length filtering */
    cc112xSpiWriteReg(CC112X_PKT_LEN, &(perSettings.payloadLength),1);  
  }
  else
  {
    /* length of configuration packet + filter byte */
    data = PER_SETTINGS_PACKET_LEN; 
    cc112xSpiWriteReg(CC112X_PKT_LEN, &data,1);
  }  
  
  // do manual calibration according to errata
  manualCalibration();
  
  return;  
}

/******************************************************************************
 * @fn          perCC1120CC1190SendPacket
 *
 * @brief       Sends the contents that pData points to which has the 
 *              following structure:
 *
 *              txArray[0] = length byte
 *              txArray[n] = payload[n]
 *              | n<[sizeOf(RXFIFO)-2], variable packet length is assumed.
 * 
 *              The radio state after completing TX is dependant on the 
 *              CC112X_RFEND_CFG0 register setting. For PG10 this register 
 *              dictates IDLE after TX. For PG.0 this function must be 
 *              re-implemented since the 2way PER test relies on RX after TX.
 *              This function enables SYNC interrupt. This means that 
 *              an interrupt will fire when a packet has been sent, i.e sync 
 *              signal transitions from high to low. 
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
void perCC1120CC1190SendPacket(uint8 *pData)
{
  uint8 len = *pData;
  /* PG1.0 errate fix: Before entering TX, the frequency word must be altered from that of RX */
  /* This means in general that TX from Idle is the only option, not TX from RX */
  perCC1120CC1190EnterIdle();
  /* Will only try to transmit if the whole packet can fit i RXFIFO 
   * and we're not currently sending a packet.
   */
  if(!(len > (PER_MAX_DATA-2)) && (cc1120cc1190RadioTxRx != CC112X_STATE_TX) )
  {
    cc112xSpiWriteTxFifo(pData,(len+1));
    /* Indicate state to the ISR and issue the TX strobe */
    trxEnableInt();
    /* Enable PA on CC1190 and be sure LNA is off */
    //perCC1190HgmEnable();
    perCC1120CC1190LnaDisable();
    perCC1120CC1190PaEnable();
    cc1120cc1190RadioTxRx = CC112X_STATE_TX; 
    trxSpiCmdStrobe(CC112X_STX);
    /* Wait until packet is sent before doing anything else */
    __low_power_mode_3();

    /* This function will not return before the complete packet
     * is sent and the radio is back in IDLE. The MSP will be 
     * be sleeping while the packet is beeing sent unless waken
     * by button presses.
     */
    while(cc1120cc1190RadioTxRx == CC112X_STATE_TX);
    
    if(perSettings.linkTopology == LINK_1_WAY)
    {
      /* Back in Idle*/
      trxDisableInt();
      /* Turn off PA on CC1190 */
      perCC1120CC1190PaDisable();
    }
  }
  return;
}


/******************************************************************************
 * @fn          perCC1120CC1190EnterRx
 *
 * @brief       Enters RX from IDLE. Function is used to abstract the 
 *              cc1120cc1190RadioTxRx functionality away from the lower-level radio 
 *              interface.
 *              Note: assumes chip is ready and in IDLE
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void perCC1120CC1190EnterRx(void)
{
  perCC1120CC1190EnterIdle();
  cc1120cc1190IdleRx();
  cc1120cc1190RadioTxRx = CC112X_STATE_RX;
  return;
}


/******************************************************************************
 * @fn          perCC1120CC1190EnterSleep
 *
 * @brief       Enters Sleep. Function is used to abstract the cc1120cc1190RadioTxRx
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
void perCC1120CC1190EnterSleep(void)
{
  cc1120cc1190RxIdle();
  trxSpiCmdStrobe(CC112X_SPWD);
  /* Only important to differ between RX/TX and IDLE */
  cc1120cc1190RadioTxRx = CC112X_STATE_IDLE;
  return;
}


/******************************************************************************
 * @fn          perCC1120CC1190EnterIdle
 *
 * @brief       Enters IDLE from ANY state. Function is used to abstract the 
 *              cc1120cc1190RadioTxRx functionality away from the lower-level radio 
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
void perCC1120CC1190EnterIdle(void)
{
  /* wait until chip is ready */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  cc1120cc1190RxIdle();
  cc1120cc1190RadioTxRx = CC112X_STATE_IDLE;
  return;
}

/******************************************************************************
 * @fn          perCC1120CC1190RxTxISR
 *
 * @brief       ISR that's called when sync signal goes low. 
 *              In RX State: Filters incoming data. The global rxData pointer
 *              always points to this functions static rxData_tmp(struct of
 *              same kind). The validnes of rxData fields is indicated by the
 *              the global flag packetSemaphore.
 *              In TX State: Nothing is done except it facilitates power 
 *              consumption reduction when TX since the program doesn't need
 *              to wait until TX is done before re-enabling sync pin interrupt.
 *              cc1120cc1190RadioTxRx is also set to CC112X_STATE_IDLE to be consistent 
 *              with program.
 * 
 * input parameters
 *             
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void perCC1120CC1190RxTxISR(void)
{
  uint8 rxBytes,rxLength,rssiIndex,lqiIndex;
  /* This variable stores the data locally. Access is given to per_test by 
   * assigning this instance to the global rxData pointer
   */
  static rxData_t rxData_tmp;
            
  rxData = &rxData_tmp;
  
  /* Checking if the chip is in RX state:  */
  if(cc1120cc1190RadioTxRx != CC112X_STATE_RX)
  {
    /* Transmission finished */
    if((perSettings.deviceMode == MASTER_DEVICE) && (perSettings.linkTopology == LINK_2_WAY) && (perSettings.masterSlaveLinked ==PER_DEVICE_LINKED))
    {
      /* Only applicable when master in 2-way test */
      cc1120cc1190RadioTxRx=CC112X_STATE_RX;
    }
    else
    {
      cc1120cc1190RadioTxRx  = CC112X_STATE_IDLE;
    }
    return;
  }
  
  packetSemaphore |= SYNC_FOUND;
  
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
    
  cc112xSpiReadReg(CC112X_NUM_RXBYTES,&rxBytes,1);
  /* Checking if the FIFO is empty */
  if(rxBytes == PER_FALSE)
  {
    /* The packet was removed by HW due to addr or length filtering -> Do nothing */
    /* Report that a sync was detected */ 
    rxData_tmp.rssi = perCC1120CC1190Read8BitRssi();
    return;
  }
  else
  {
    /* The RX FIFO is not empty, process contents */    
    cc112xSpiReadRxFifo(&rxLength, 1);  
    /* Check that the packet length just read + FCS(2B) + length byte match the RXBYTES */
    /* If these are not equal:
     * - RXFIFO overflow: Received packets not processed while beeing in RX. 
     */
    if(rxBytes != (rxLength+3))
    {
      /* This is a fault FIFO condition -> clean FIFO and register a sync detection */
      /* IDLE -> FLUSH RX FIFO -> RX */
      cc1120cc1190RxIdle();     
      perCC1120CC1190EnterRx(); 
      /* Report that a sync was detected */
      rxData_tmp.rssi = perCC1120CC1190Read8BitRssi();
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
      cc112xSpiReadRxFifo(&rxData_tmp.data[1], lqiIndex);
      
      /* The whole packet has been read from the FIFO.
       * Check if the CRC is correct and that the packet length is as expected.
       * If not correct: report sync found and do not update RSSI or LQI.
       */
      if((!(rxData_tmp.data[lqiIndex] & CC112X_LQI_CRC_OK_BM)) || (perSettings.payloadLength != rxLength ))
      {
        rxData_tmp.rssi = perCC1120CC1190Read8BitRssi();
        return;
      }
      /* A complete error-free packet has arrived  */
      
      /* Measured data */
      rxData_tmp.length  = rxLength;
      rxData_tmp.lqi     = rxData_tmp.data[lqiIndex] & CC112X_LQI_EST_BM;
      rxData_tmp.addr    = rxData_tmp.data[1]; 
      
      /* Convert RSSI value from 2's complement to decimal value accounting for offset value */
      rxBytes = rxData_tmp.data[rssiIndex];        
      rxData_tmp.rssi = (int8)((int8)rxBytes) - cc1120cc1190RssiOffset;
      /* Signal a good packet is received */
      packetSemaphore |= PACKET_RECEIVED;
      return;
    } 
  }   
}     
      
/*******************************************************************************
 * @fn          perCC1120CC1190Read8BitRssi
 *    
 * @brief       Reads MSB RSSI value from register, converts the dBm value to
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
int8 perCC1120CC1190Read8BitRssi(void)
{
  uint8 rssi2compl,rssiValid;
  int16 rssiConverted;
  
  cc112xSpiReadReg(CC112X_RSSI0, &rssiValid,1);
  if(rssiValid & 0x01)
  {
    /* Read RSSI from MSB register */
    cc112xSpiReadReg(CC112X_RSSI1, &rssi2compl,1);
    rssiConverted = (int8)((int8)rssi2compl) - cc1120cc1190RssiOffset;
    return rssiConverted;
  }
  /* keep last value since new value is not valid */
  return rxData->rssi;
}

/*******************************************************************************
 * @fn          cc1120cc1190RxIdle
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
static void cc1120cc1190RxIdle(void)
{
  /* Disable pin interrupt */
  trxDisableInt();
  /* Strobe IDLE */
  trxSpiCmdStrobe(CC112X_SIDLE); 
  /* Wait until chip is in IDLE */
  while(trxSpiCmdStrobe(CC112X_SNOP) & 0xF0);
  //Disable LNA
  perCC1120CC1190LnaDisable();
  /* Flush the Receive FIFO */
  trxSpiCmdStrobe(CC112X_SFRX);
  /* Clear pin interrupt flag */
  trxClearIntFlag();
  return;
}

/*******************************************************************************
 * @fn          cc1120cc1190IdleRx
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
static void cc1120cc1190IdleRx(void)
{
  trxClearIntFlag();
  //perCC1190HgmEnable();
  perCC1120CC1190PaDisable();
  perCC1120CC1190LnaEnable();
  trxSpiCmdStrobe(CC112X_SRX);
  trxEnableInt();
  return;
}
/*******************************************************************************
 * @fn          perCC1120cc1190WriteTxFifo
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
void perCC1120CC1190WriteTxFifo(uint8 *pData, uint8 len)
{
  cc112xSpiWriteTxFifo(pData,len);
  return;
}

/*******************************************************************************
 * @fn                  perCC1120CC1190PaEnable
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
void perCC1120CC1190PaEnable(void)
{
    TRXEM_CC1190_PORT_SEL &= ~(TRXEM_CC1190_PA);  // Set pin to zero for I/O function
    TRXEM_CC1190_PORT_DIR |=  (TRXEM_CC1190_PA);  // Set pin direction to output
    TRXEM_CC1190_PORT_OUT |=  (TRXEM_CC1190_PA);  // Set output pin high
}
/*******************************************************************************
 * @fn                  perCC1190LnaEnable
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
void perCC1120CC1190LnaEnable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_LNA);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_LNA);
    TRXEM_PORT_OUT |=  (TRXEM_CC1190_LNA);
}
/*******************************************************************************
 * @fn                  perCC1190PaDisable
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
void perCC1120CC1190PaDisable(void)
{
    TRXEM_CC1190_PORT_SEL &= ~(TRXEM_CC1190_PA);  // Set pin to zero for I/O function
    TRXEM_CC1190_PORT_DIR |=  (TRXEM_CC1190_PA);  // Set pin direction to output
    TRXEM_CC1190_PORT_OUT &= ~(TRXEM_CC1190_PA);  // Set output pin low
}
/*******************************************************************************
 * @fn                  perCC1190LnaDisable
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
void perCC1120CC1190LnaDisable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_LNA);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_LNA);
    TRXEM_PORT_OUT &= ~(TRXEM_CC1190_LNA);    
}
/*******************************************************************************
 * @fn                  perCC1190HgmEnable
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
void perCC1120CC1190HgmEnable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_HGM);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_HGM);
    TRXEM_PORT_OUT |=  (TRXEM_CC1190_HGM);
}
/*******************************************************************************
 * @fn                  perCC1190HgmDisable
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
void perCC1120CC1190HgmDisable(void)
{
    TRXEM_PORT_SEL &= ~(TRXEM_CC1190_HGM);
    TRXEM_PORT_DIR |=  (TRXEM_CC1190_HGM);
    TRXEM_PORT_OUT &= ~(TRXEM_CC1190_HGM);
}
/*******************************************************************************
* @fn          manualCalibration
*
* @brief       Perform manual calibration according to the errata note
* @param       none
*
* @return      none
*/
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

  // 3) Calibrate and wait for calibration to be done (radio back in IDLE state)
  trxSpiCmdStrobe(CC112X_SCAL);
  do {
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
  do {
    cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
  } while (marcstate != 0x41);

  // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with mid VCDAC_START value
  cc112xSpiReadReg(CC112X_FS_VCO2, &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_VCO4, &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_CHP, &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

  // 9) Write back highest FS_VCO2 and corresponding FS_VCO and FS_CHP result
  if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] > calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
    writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
    cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
  }
  else {
    writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
    cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
  }
}
