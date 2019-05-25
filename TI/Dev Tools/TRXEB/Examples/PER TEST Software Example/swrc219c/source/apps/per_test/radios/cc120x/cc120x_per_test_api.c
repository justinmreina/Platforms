//*****************************************************************************
//! @file      cc120x_per_test_api.c
//  
//! @brief    Implementation file for api-like functions that the per test
//            will call if a CC120x was detected.             
//  
//               CC120x will support the PER test with the following data rates:
//               -TBD
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
#include "cc120x_spi.h"
#include "cc120x_per_test_api.h"
#include "freq_xosc_detect.h"


/******************************************************************************
 * TYPEDEFS
 */


/******************************************************************************
 * LOCAL FUNCTIONS
 */

static void perCC120xRxIdle(void);
static void perCC120xIdleRx(void);   

/******************************************************************************
 * CONSTANTS
 */
 
/********************************* TestCases *********************************/
   // Base settings: Variable packet length, CRC calculation, no Address check, 
   // Append Mode, Always give Clear channel indication 
   //


// RX filter BW = 25.252525 
// Address config = No address check. 
// Packet length = 255 
// Symbol rate = 1.2 
// Carrier frequency = 867.999878 
// Bit rate = 1.2 
// Packet bit length = 0 
// Whitening = false 
// Manchester enable = false 
// Modulation format = 2-FSK 
// Packet length mode = Variable 
// Device address = 0 
// Deviation = 3.986359 

// Modem settings for TC01 adjusted to 4 Byte preamble and increased to 
// 25 kHz RX filter BW 
static const registerSetting_t cc120xLowDataRateRfSettings[] =  
{
  {CC120X_IOCFG0,              0x06},
  {CC120X_DEVIATION_M,         0xD1},
  {CC120X_MODCFG_DEV_E,        0x00},
  {CC120X_DCFILT_CFG,          0x5D},
  {CC120X_PREAMBLE_CFG0,       0x8A},
  {CC120X_IQIC,                0xCB},
  {CC120X_CHAN_BW,             0x61},
  {CC120X_MDMCFG1,             0x40},
  {CC120X_MDMCFG0,             0x05},
  {CC120X_SYMBOL_RATE2,        0x3F},
  {CC120X_SYMBOL_RATE1,        0x75},
  {CC120X_SYMBOL_RATE0,        0x10},
  {CC120X_AGC_REF,             0x20},
  {CC120X_AGC_CS_THR,          0xEC},
  {CC120X_AGC_CFG1,            0x51},
  {CC120X_AGC_CFG0,            0xC7},
  {CC120X_FIFO_CFG,            0x00},
  {CC120X_FS_CFG,              0x12},
  {CC120X_WOR_CFG1,            0x12},
  {CC120X_PKT_CFG0,            0x20},
  {CC120X_PA_CFG1,             0x3F},
  {CC120X_PKT_LEN,             0xFF},
  {CC120X_IF_MIX_CFG,          0x1C},
  {CC120X_FREQOFF_CFG,         0x22},
  {CC120X_MDMCFG2,             0x0C},
  {CC120X_FREQ2,               0x56},
  {CC120X_FREQ1,               0xCC},
  {CC120X_FREQ0,               0xCC},
  {CC120X_FS_DIG1,             0x07},
  {CC120X_FS_DIG0,             0xAF},
  {CC120X_FS_CAL1,             0x40},
  {CC120X_FS_CAL0,             0x0E},
  {CC120X_FS_DIVTWO,           0x03},
  {CC120X_FS_DSM0,             0x33},
  {CC120X_FS_DVC0,             0x17},
  {CC120X_FS_PFD,              0x50},
  {CC120X_FS_PRE,              0x6E},
  {CC120X_FS_REG_DIV_CML,      0x14},
  {CC120X_FS_SPARE,            0xAC},
  {CC120X_FS_VCO0,             0xB5},
  {CC120X_XOSC5,               0x0E},
  {CC120X_XOSC1,               0x03},
};

// RX filter BW = 104.166667 
// Address config = No address check. 
// Packet length = 255 
// Symbol rate = 50 
// Carrier frequency = 867.999878 
// Bit rate = 50 
// Packet bit length = 0 
// Whitening = false 
// Manchester enable = false 
// Modulation format = 2-GFSK 
// Packet length mode = Variable 
// Device address = 0 
// Deviation = 24.948120 

// Modem settings for TC54, 50 kbps 2-GFSK (IEEE 802.15.4g compliant)
static const registerSetting_t cc120xMediumDataRateRfSettings[] = 
{
  {CC120X_IOCFG0,            0x06},
  {CC120X_SYNC3,             0x6F},
  {CC120X_SYNC2,             0x4E},
  {CC120X_SYNC1,             0x90},
  {CC120X_SYNC0,             0x4E},
  {CC120X_SYNC_CFG1,         0xE5},
  {CC120X_SYNC_CFG0,         0x23},
  {CC120X_DEVIATION_M,       0x47},
  {CC120X_MODCFG_DEV_E,      0x0B},
  {CC120X_DCFILT_CFG,        0x56},
  {CC120X_PREAMBLE_CFG1,     0x18},
  {CC120X_PREAMBLE_CFG0,     0xBA},
  {CC120X_IQIC,              0xCA},
  {CC120X_CHAN_BW,           0x84},
  {CC120X_MDMCFG1,           0x42},
  {CC120X_MDMCFG0,           0x05},
  {CC120X_SYMBOL_RATE2,      0x94},
  {CC120X_SYMBOL_RATE1,      0x7A},
  {CC120X_SYMBOL_RATE0,      0xE1},
  {CC120X_AGC_REF,           0x31},
  {CC120X_AGC_CS_THR,        0xF1},
  {CC120X_AGC_CFG1,          0x11},
  {CC120X_AGC_CFG0,          0xD0},
  {CC120X_FIFO_CFG,          0x00},
  {CC120X_FS_CFG,            0x12},
  {CC120X_PKT_CFG0,          0x20},
  {CC120X_PKT_LEN,           0xFF},
  {CC120X_IF_MIX_CFG,        0x18},
  {CC120X_TOC_CFG,           0x03},
  {CC120X_MDMCFG2,           0x02},
  {CC120X_FREQ2,             0x56},
  {CC120X_FREQ1,             0xCC},
  {CC120X_FREQ0,             0xCC},
  {CC120X_FS_DIG1,           0x07},
  {CC120X_FS_DIG0,           0xAA},
  {CC120X_FS_CAL1,           0x40},
  {CC120X_FS_CAL0,           0x0E},
  {CC120X_FS_DIVTWO,         0x03},
  {CC120X_FS_DSM0,           0x33},
  {CC120X_FS_DVC0,           0x17},
  {CC120X_FS_PFD,            0x00},
  {CC120X_FS_PRE,            0x6E},
  {CC120X_FS_REG_DIV_CML,    0x14},
  {CC120X_FS_SPARE,          0xAC},
  {CC120X_FS_VCO0,           0xB5},
  {CC120X_IFAMP,             0x05},
  {CC120X_XOSC5,             0x0E},
  {CC120X_XOSC1,             0x03},
};

// RX filter BW = 416.666667 
// Address config = No address check. 
// Packet length = 255 
// Symbol rate = 200 
// Carrier frequency = 867.999878 
// Bit rate = 200 
// Packet bit length = 0 
// Whitening = false 
// Manchester enable = false 
// Modulation format = 2-GFSK 
// Packet length mode = Variable 
// Device address = 0 
// Deviation = 49.896240 

// Settings for test case 55: 200 kbit, 2-GFSK  
static const registerSetting_t cc120xHighDataRateRfSettings[] =  
{ 
  {CC120X_IOCFG0,            0x06},
  {CC120X_SYNC_CFG1,         0xA9},
  {CC120X_SYNC_CFG0,         0x23},
  {CC120X_DEVIATION_M,       0x47},
  {CC120X_MODCFG_DEV_E,      0x0C},
  {CC120X_DCFILT_CFG,        0x66},
  {CC120X_PREAMBLE_CFG1,     0x18},
  {CC120X_PREAMBLE_CFG0,     0x88},
  {CC120X_IQIC,              0xC9},
  {CC120X_CHAN_BW,           0x04},
  {CC120X_MDMCFG1,           0x42},
  {CC120X_MDMCFG0,           0x05},
  {CC120X_SYMBOL_RATE2,      0xB4},
  {CC120X_SYMBOL_RATE1,      0x7A},
  {CC120X_SYMBOL_RATE0,      0xE1},
  {CC120X_AGC_REF,           0x39},
  {CC120X_AGC_CS_THR,        0xE7},
  {CC120X_AGC_CFG1,          0x12},
  {CC120X_AGC_CFG0,          0xD0},
  {CC120X_FIFO_CFG,          0x00},
  {CC120X_FS_CFG,            0x12},
  {CC120X_PKT_CFG0,          0x20},
  {CC120X_PKT_LEN,           0xFF},
  {CC120X_IF_MIX_CFG,        0x1C},
  {CC120X_TOC_CFG,           0x03},
  {CC120X_MDMCFG2,           0x00},
  {CC120X_FREQ2,             0x56},
  {CC120X_FREQ1,             0xCC},
  {CC120X_FREQ0,             0xCC},
  {CC120X_FS_DIG1,           0x07},
  {CC120X_FS_DIG0,           0xAA},
  {CC120X_FS_CAL1,           0x40},
  {CC120X_FS_CAL0,           0x0E},
  {CC120X_FS_DIVTWO,         0x03},
  {CC120X_FS_DSM0,           0x33},
  {CC120X_FS_DVC0,           0x17},
  {CC120X_FS_PFD,            0x00},
  {CC120X_FS_PRE,            0x6E},
  {CC120X_FS_REG_DIV_CML,    0x14},
  {CC120X_FS_SPARE,          0xAC},
  {CC120X_FS_VCO0,           0xB5},
  {CC120X_IFAMP,             0x09},
  {CC120X_XOSC5,             0x0E},
  {CC120X_XOSC1,             0x03},
};  

// Register settings for link bypass mode that differ from the
// other link cofigurations.
static const registerSetting_t linkBypassSettings[] = 
{
  {CC120X_SYNC3             ,0xD3},          
  {CC120X_SYNC2             ,0x91},           
  {CC120X_SYNC1             ,0xD3}, //sync word compatible with CC1101         
  {CC120X_SYNC0             ,0x91}          
};

//Band select settings for the LO divider.
//out of lock detector enabled
static uint8 cc120xFsCfgs[5] = 
{
  0x1A, // 169 MHz
  0x14, // 434 MHz 
  0x12, // 868 MHz
  0x12, // 915 MHz
  0x12, // 955 MHz  
};
//Frequency programming for CC120X and above
// perSettings.frequencyBand = 0 => 169 MHz 
// perSettings.frequencyBand = 1 => 434 MHz 
// perSettings.frequencyBand = 2 => 868 MHz 
// perSettings.frequencyBand = 3 => 915 Mhz 
// perSettings.frequencyBand = 4 => 955 Mhz 

static uint8 freqConfiguration[5][3] = 
{ 
  {0x54, 0xC1, 0x89},  // 169.5125 MHz 
  {0x56, 0xCC, 0xCC},  // 433 MHz  
  {0x56, 0xCC, 0xCC},  // 868 MHz 
  {0x5B, 0x80, 0x00},  // 915 MHz
  {0x5F, 0x80, 0x00}   // 955 MHz
};
/* These structs shall be 2-dimensional when PG2 is present */
/* Sensitivity table - Note: It's only valid for 3 bytes packets and for shipped register settings */
/* Used in Link Margin calculation */  
static const int16 sensitivity868Mhz[3] = 
{
  -120, 
  -109,
  -103
};

/* RSSI offset table */
static const int8 rssiOffset868Mhz[3] = 
{
  102, 
  84, 
  84 
};

/* Values in this table must be ored with the PA_CFG2 register to account for  
 * pa_shape_en
 * Formula: paPowerRamp[index] = dec2hex((wanted_dBm_level+18)*2-1)
 */
static const uint8 paPowerRamp[7] = 
{
  0x03, /* -16 dBm - lowest power       - index 0 */
  0x3F, /*  14 dBm - highest power      - index 1 */
  0x0F, /* -10 dBm - next lowest power  - index 2 */
  0x19, /*  -5 dBm -                    - index 3 */
  0x23, /*   0 dBm -                    - index 4 */
  0x2D, /*   5 dBm -                    - index 5 */
  0x37  /*  10 dBm - next highest power - index 6 */
};
/* Access by index gives GUI values for TX power corresponding to the table above */
static const int8 paPowerGuiValues[7] =
{
  -16,  /* - index 0 */
   14,  /* - index 1 */
  -10,  /* - index 2 */
   -5,  /* - index 3 */
    0,  /* - index 4 */
    5,  /* - index 5 */
   10   /* - index 6 */ 
};

static const float testCaseDataRate[3] =
{
    1.20,  /* testcase 1   <=> SMARTRF_CONFIGURATION_0 */
   50.00,  /* testcase 55  <=> SMARTRF_CONFIGURATION_1 */
  200.00   /* testcase 55   <=> SMARTRF_CONFIGURATION_2 */
};

/******************************************************************************
 * LOCAL VARIABLES
 */

/* Variable is CC120X_STATE_RX when in RX. If not in RX it is CC120X_STATE_TX or 
 * CC120X_STATE_IDLE. The use of this variable is only to avoid an RX interrupt
 * when sending a packet. This facilitates a reduction in power consumption.
 */
static rfStatus_t cc120xRadioTxRx; 
static int8 cc120xRssiOffset; 
 
/******************************************************************************
* @fn          perCC120xGetGuiTxPower
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
int8 perCC120xGetGuiTxPower(uint8 index)
{
  return paPowerGuiValues[index];
}

/******************************************************************************
* @fn          perCC120xGetDataRate
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
float perCC120xGetDataRate(uint8 index)
{
  return testCaseDataRate[index];
}


/******************************************************************************
* @fn          perCC120xSetOutputPower
*
* @brief       Configures the output power of CC120x according to the provided
*              index:
*              0 = -16 dBm
*              1 =  14 dBm
*              2 = -10 dBm
*              3 = -5 dBm 
*              4 =  0 dBm 
*              5 =  5 dBm
*              6 = 10 dBm
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
void perCC120xSetOutputPower(uint8 index)
{
  uint8 level; 
  
  /* Reading the PA_CFG2 value to account for pa_shape_en */
  cc120xSpiReadReg(CC120X_PA_CFG1,&level,1);
  /* Saving pa_shape_en */
  level &= 0x40;
  /* Oring in the PA power ramp value */
  level |= paPowerRamp[index];
  /* Updating PA_CFG2 register with its' new value */
  cc120xSpiWriteReg(CC120X_PA_CFG1,&level,1);
  return;
}

/******************************************************************************
 * @fn          perCC120xRegConfig
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
void perCC120xRegConfig(void)
{
  uint8 data;
  uint16 arraySize;
  const registerSetting_t *pRegisterSettings;
  uint8 *pFreqConfig[5];
  
  
  /* Log that radio is in IDLE state */
  perCC120xRxIdle();
  cc120xRadioTxRx = CC120X_STATE_IDLE;
    
  /* Extract what radio configuration to use */
  if((perSettings.masterSlaveLinked==PER_DEVICE_LINKED) || 
      (perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS))
  {
    switch(perSettings.smartRfConfiguration)
    {
      case SMARTRF_CONFIGURATION_0:
        pRegisterSettings      = cc120xLowDataRateRfSettings;
        arraySize      = (sizeof cc120xLowDataRateRfSettings/sizeof(registerSetting_t));
        perSettings.sensitivity = sensitivity868Mhz[0];
        cc120xRssiOffset = rssiOffset868Mhz[0];
        break;
      case SMARTRF_CONFIGURATION_2:
        pRegisterSettings      = cc120xHighDataRateRfSettings;
        arraySize      = (sizeof cc120xHighDataRateRfSettings/sizeof(registerSetting_t));
        perSettings.sensitivity = sensitivity868Mhz[2];
        cc120xRssiOffset = rssiOffset868Mhz[2];
        break;
      default:
        pRegisterSettings      = cc120xMediumDataRateRfSettings;
        arraySize      = (sizeof cc120xMediumDataRateRfSettings/sizeof(registerSetting_t));
        perSettings.sensitivity = sensitivity868Mhz[1];
        cc120xRssiOffset = rssiOffset868Mhz[1];
        break;
    }
  }
  else
  {
    /* Base settings for communication */
    pRegisterSettings      = cc120xHighDataRateRfSettings;
    arraySize      = (sizeof cc120xHighDataRateRfSettings/sizeof(registerSetting_t));
    /* In lack of numbers for other freq's than 868 MHz */
    perSettings.sensitivity = sensitivity868Mhz[2];
    cc120xRssiOffset = rssiOffset868Mhz[2];
  }
  
  // Reset radio registers before write
  trxSpiCmdStrobe(CC120X_SRES);
  
  /* Write register settings to radio */
  for(uint16 i = 0; i < arraySize;i++)
  {
    data = pRegisterSettings[i].data;
    cc120xSpiWriteReg(pRegisterSettings[i].addr,&data,1);
  }

 
  /* Load freq word depending on crystal detected */
    pFreqConfig[0] = freqConfiguration[0];
    pFreqConfig[1] = freqConfiguration[1];
    pFreqConfig[2] = freqConfiguration[2];
    pFreqConfig[3] = freqConfiguration[3];
    pFreqConfig[4] = freqConfiguration[4];
  
    
    switch(perSettings.frequencyBand)      
    {
      case 0: //169 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[0],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[0],3);
        break;
      case 1: // 434 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[1],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[1],3);
        break;
      case 2: // 868 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[2],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[2],3);
        break;      
      case 3: // 915 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[3],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[3],3);
        break;
      case 4: // 955 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[4],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[4],3);
        break;      
      default: // 868 MHz
        cc120xSpiWriteReg(CC120X_FS_CFG,&cc120xFsCfgs[2],1);
        cc120xSpiWriteReg(CC120X_FREQ2,pFreqConfig[2],3);
        break;
    }    

  if(perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
  {
    /* PKT_LEN set to user specified packet length: HW length filtering */
    cc120xSpiWriteReg(CC120X_PKT_LEN, &(perSettings.payloadLength),1);
    /* Turn on HW Address filtering */
    uint8 pkt_cfg1 = 0x00|0x0B;
    cc120xSpiWriteReg(CC120X_PKT_CFG1,&pkt_cfg1,1);
    /* Set address */
    cc120xSpiWriteReg(CC120X_DEV_ADDR,&perSettings.address,1);

    if((perSettings.linkTopology == LINK_2_WAY))
    {
      if(perSettings.deviceMode == MASTER_DEVICE)
      {
        /* IDLE after RX, RX after TX */
        data = 0x0F;
        cc120xSpiWriteReg(CC120X_RFEND_CFG1,&data,1);
        data = 0x30;
        cc120xSpiWriteReg(CC120X_RFEND_CFG0,&data,1);
      }
    } 
  }
  else if(perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS)
  {
    //Change sync word to apply to link bypass configuration
    for(uint16 i = 0; i < (sizeof linkBypassSettings/sizeof(registerSetting_t));i++)
    {
      data = linkBypassSettings[i].data;
      cc120xSpiWriteReg(linkBypassSettings[i].addr,&data,1);
    }  
    
    /* PKT_LEN set to user specified packet length: HW length filtering */
    cc120xSpiWriteReg(CC120X_PKT_LEN, &(perSettings.payloadLength),1);  
  }
  else
  {
    /* length of configuration packet + filter byte */
    data = PER_SETTINGS_PACKET_LEN; 
    cc120xSpiWriteReg(CC120X_PKT_LEN, &data,1);
  }  
  return;  
}

/******************************************************************************
 * @fn          perCC120xSendPacket
 *
 * @brief       Sends the contents that pData points to which has the 
 *              following structure:
 *
 *              txArray[0] = length byte
 *              txArray[n] = payload[n]
 *              | n<[sizeOf(RXFIFO)-2], variable packet length is assumed.
 * 
 *              The radio state after completing TX is dependant on the 
 *              CC120X_RFEND_CFG0 register setting. For PG10 this register 
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
void perCC120xSendPacket(uint8 *pData)
{
  uint8 len = *pData;
  /* PG1.0 errate fix: Before entering TX, the frequency word must be altered from that of RX */
  /* This means in general that TX from Idle is the only option, not TX from RX */
  perCC120xEnterIdle();
  /* Will only try to transmit if the whole packet can fit i RXFIFO 
   * and we're not currently sending a packet.
   */
  if(!(len > (PER_MAX_DATA-2)) && (cc120xRadioTxRx != CC120X_STATE_TX) )
  {
    cc120xSpiWriteTxFifo(pData,(len+1));
    /* Indicate state to the ISR and issue the TX strobe */
    trxEnableInt();
    cc120xRadioTxRx = CC120X_STATE_TX; 
    trxSpiCmdStrobe(CC120X_STX);
    /* Wait until packet is sent before doing anything else */
    __low_power_mode_3();

    /* This function will not return before the complete packet
     * is sent and the radio is back in IDLE. The MSP will be 
     * be sleeping while the packet is beeing sent unless waken
     * by button presses.
     */
    while(cc120xRadioTxRx == CC120X_STATE_TX);
    if(perSettings.linkTopology == LINK_1_WAY)
    {
      /* Back in Idle*/
      trxDisableInt();
    }
  }
  return;
}


/******************************************************************************
 * @fn          perCC120xEnterRx
 *
 * @brief       Enters RX from IDLE. Function is used to abstract the 
 *              cc120xRadioTxRx functionality away from the lower-level radio 
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
void perCC120xEnterRx(void)
{
  perCC120xEnterIdle();
  perCC120xIdleRx();
  cc120xRadioTxRx = CC120X_STATE_RX;
  return;
}


/******************************************************************************
 * @fn          perCC120xEnterSleep
 *
 * @brief       Enters Sleep. Function is used to abstract the cc120xRadioTxRx
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
void perCC120xEnterSleep(void)
{
  perCC120xRxIdle();
  trxSpiCmdStrobe(CC120X_SPWD);
  /* Only important to differ between RX/TX and IDLE */
  cc120xRadioTxRx = CC120X_STATE_IDLE;
  return;
}


/******************************************************************************
 * @fn          perCC120xEnterIdle
 *
 * @brief       Enters IDLE from ANY state. Function is used to abstract the 
 *              cc120xRadioTxRx functionality away from the lower-level radio 
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
void perCC120xEnterIdle(void)
{
  /* wait until chip is ready */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  perCC120xRxIdle();
  cc120xRadioTxRx = CC120X_STATE_IDLE;
  return;
}

/******************************************************************************
 * @fn          perCC120xRxTxISR
 *
 * @brief       ISR that's called when sync signal goes low. 
 *              In RX State: Filters incoming data. The global rxData pointer
 *              always points to this functions static rxData_tmp(struct of
 *              same kind). The validnes of rxData fields is indicated by the
 *              the global flag packetSemaphore.
 *              In TX State: Nothing is done except it facilitates power 
 *              consumption reduction when TX since the program doesn't need
 *              to wait until TX is done before re-enabling sync pin interrupt.
 *              cc120xRadioTxRx is also set to CC120X_STATE_IDLE to be consistent 
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
void perCC120xRxTxISR(void)
{
  uint8 rxBytes,rxLength,rssiIndex,lqiIndex;
  /* This variable stores the data locally. Access is given to per_test by 
   * assigning this instance to the global rxData pointer
   */
  static rxData_t rxData_tmp;
            
  rxData = &rxData_tmp;
  
  /* Checking if the chip is in RX state:  */
  if(cc120xRadioTxRx != CC120X_STATE_RX)
  {
    /* Transmission finished */
    if((perSettings.deviceMode == MASTER_DEVICE) && (perSettings.linkTopology == LINK_2_WAY) && (perSettings.masterSlaveLinked ==PER_DEVICE_LINKED))
    {
      /* Only applicable when master in 2-way test */
      cc120xRadioTxRx=CC120X_STATE_RX;
    }
    else
    {
      cc120xRadioTxRx  = CC120X_STATE_IDLE;
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
    
  cc120xSpiReadReg(CC120X_NUM_RXBYTES,&rxBytes,1);
  /* Checking if the FIFO is empty */
  if(rxBytes == 0)
  {
    /* The packet was removed by HW due to addr or length filtering -> Do nothing */
    /* Report that a sync was detected */ 
    rxData_tmp.rssi = perCC120xRead8BitRssi();
    return;
  }
  else
  {
    /* The RX FIFO is not empty, process contents */    
    cc120xSpiReadRxFifo(&rxLength, 1);  
    /* Check that the packet length just read + status bytes(2B) + length byte match the RXBYTES */
    /* If these are not equal:
     * - Packet is not like expected
     */
    if(rxBytes != (rxLength+3))
    {
      /* This is a fault FIFO condition -> clean FIFO and register a sync detection */
      /* IDLE -> FLUSH RX FIFO -> RX */
      perCC120xRxIdle();     
      perCC120xEnterRx(); 
      /* Report that a sync was detected */
      rxData_tmp.rssi = perCC120xRead8BitRssi();
      return;
    }
    else
    {
      /* We don't have a FIFO error condition -> get packet */
      
      /* Length Field */
      rxData_tmp.data[0] = rxLength;
      rssiIndex = rxLength+1;
      lqiIndex  = rssiIndex +1;
      
      /* Payload(ADDR + DATA + STATUS BYTES) */
      cc120xSpiReadRxFifo(&rxData_tmp.data[1], lqiIndex);
      
      /* The whole packet has been read from the FIFO.
       * Check if the CRC is correct and that the packet length is as expected.
       * If not correct: report sync found and do not update RSSI or LQI.
       */
      if((!(rxData_tmp.data[lqiIndex] & CC120X_LQI_CRC_OK_BM)) || (perSettings.payloadLength != rxLength ))
      {
        rxData_tmp.rssi = perCC120xRead8BitRssi();
        return;
      }
      /* A complete error-free packet has arrived  */
      
      /* Measured data */
      rxData_tmp.length  = rxLength;
      rxData_tmp.lqi     = rxData_tmp.data[lqiIndex] & CC120X_LQI_EST_BM;
      rxData_tmp.addr    = rxData_tmp.data[1]; 
      
      /* Convert RSSI value from 2's complement to decimal value accounting for offset value */
      rxBytes = rxData_tmp.data[rssiIndex];        
      rxData_tmp.rssi = (int16)((int8)rxBytes) - cc120xRssiOffset;
      /* Signal a good packet is received */
      packetSemaphore |= PACKET_RECEIVED;
      return;
    } 
  }   
}     
      
/*******************************************************************************
 * @fn          perCC120xRead8BitRssi
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
int8 perCC120xRead8BitRssi(void)
{
  uint8 rssi2compl,rssiValid;
  int16 rssiConverted;
  
  cc120xSpiReadReg(CC120X_RSSI0, &rssiValid,1);
  if(rssiValid & 0x01)
  {
    /* Read RSSI from MSB register */
    cc120xSpiReadReg(CC120X_RSSI1, &rssi2compl,1);
    rssiConverted = (int16)((int8)rssi2compl) - cc120xRssiOffset;
    return rssiConverted;
  }
  /* keep last value since new value is not valid */
  return rxData->rssi;
}

/*******************************************************************************
 * @fn          perCC120xRxIdle
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
static void perCC120xRxIdle(void)
{
  /* Disable pin interrupt */
  trxDisableInt();
  /* Strobe IDLE */
  trxSpiCmdStrobe(CC120X_SIDLE); 
  /* Wait until chip is in IDLE */
  while(trxSpiCmdStrobe(CC120X_SNOP) & 0xF0);
  /* Clear pin interrupt flag */
  trxClearIntFlag();
  return;
}

/*******************************************************************************
 * @fn          perCC120xIdleRx
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
static void perCC120xIdleRx(void)
{
  trxClearIntFlag();
  trxSpiCmdStrobe(CC120X_SRX);
  trxEnableInt();
  return;
}
/*******************************************************************************
 * @fn          perCC120xWriteTxFifo
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
void perCC120xWriteTxFifo(uint8 *pData, uint8 len)
{
  cc120xSpiWriteTxFifo(pData,len);
  return;
}