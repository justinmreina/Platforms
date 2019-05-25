//*****************************************************************************
//! @file      cc1120_per_test_api.c
//  
//! @brief    Implementation file for api-like functions that the per test
//            will call if a CC1120 was detected.             
//  
//            CC1120 will support the PER test with the following data rates:
//            - 50.00 Kb/s
//            - 1.2 Kb/s  with 25 KHz bandwidth, almost tc_01, named 
//            cc112xLowDataRateRfSettings, hence more noise on RSSI samples and lower
//            sensitivity.
//            - 150 kbps 4-FSK
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
#include "cc112x_per_test_api.h"
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

static void perCC112xRxIdle(void);
static void perCC112xIdleRx(void);
static void cc1125Category1WorkAround(void);
static void manualCalibration(void);

/******************************************************************************
 * CONSTANTS
 */
 
/********************************* TestCases *********************************/
   // Base settings: Variable packet length, CRC calculation, no Address check, 
   // Append Mode, Always give Clear channel indication 
   //

// Modem settings for test case 1 adjusted to 25kHz RXBW and 4 Byte preamble(")*/ 
static const registerSetting_t cc112xLowDataRateRfSettings[] =  
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
  {CC112X_SYMBOL_RATE2      ,0x43}, // (1200 bps)      
  {CC112X_SYMBOL_RATE1      ,0xA9}, // (1200 bps)      
  {CC112X_SYMBOL_RATE0      ,0x2A}, // (1200 bps)      
  {CC112X_AGC_REF           ,0x20}, 
  {CC112X_AGC_CS_THR        ,0x02}, 
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

// Modem settings for medium: 50 kb/s, 25kHz deviation, 200 kHz RXBW, IF = 68359 kHz
static const registerSetting_t cc112xMediumDataRateRfSettings[] = 
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

// Settings for test case 9: 150 kb/s, 200 kHz RXBW, 4-GFSK, no IF  
static const registerSetting_t cc112xHighDataRateRfSettings[] =  
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
  {CC112X_SYNC3             ,0x93},///////////////////////////////////////////////////////////////////           
  {CC112X_SYNC2             ,0x0B},            // TEST CASE CONFIGURATION
  {CC112X_SYNC1             ,0x51},           
  {CC112X_SYNC0             ,0xDE},           
  {CC112X_SYNC_CFG1         ,0x08},        
  {CC112X_SYNC_CFG0         ,0x17},       
  {CC112X_DEVIATION_M       ,0x53},  // (83000 Hz)        
  {CC112X_MODCFG_DEV_E      ,0x2F},  // (83000 Hz)       
  {CC112X_DCFILT_CFG        ,0x04},  //                    
  {CC112X_PREAMBLE_CFG1     ,0x18},  // 4 byte preamble  
  {CC112X_PREAMBLE_CFG0     ,0x2A},  //                  
  {CC112X_FREQ_IF_CFG       ,0x00},  // (0 Hz)             
  {CC112X_IQIC              ,0x00},  //                         
  {CC112X_CHAN_BW           ,0x01},  // (200000 Hz)            
  {CC112X_MDMCFG1           ,0x46},  //                       
  {CC112X_MDMCFG0           ,0x05},  //                        
  {CC112X_SYMBOL_RATE2            ,0xA3},  // (75000 bps)           
  {CC112X_SYMBOL_RATE1            ,0x33},  // (75000 bps)           
  {CC112X_SYMBOL_RATE0            ,0x33},  // (75000 bps)           
  {CC112X_AGC_REF           ,0x3C},         
  {CC112X_AGC_CS_THR        ,0xEC},     
  {CC112X_AGC_GAIN_ADJUST   ,0x00},  
  {CC112X_AGC_CFG3          ,0x83},        
  {CC112X_AGC_CFG2          ,0x60},       
  {CC112X_AGC_CFG1          ,0xA9},       
  {CC112X_AGC_CFG0          ,0xC0},       
  {CC112X_PA_CFG2           ,0x7F},         
  {CC112X_PA_CFG1           ,0x56},         
  {CC112X_PA_CFG0           ,0x01},          
  {CC112X_IF_MIX_CFG        ,0x00},      
  {CC112X_FREQOFF_CFG       ,0x20},     
  {CC112X_TOC_CFG           ,0x0A},          
  {CC112X_CFM_DATA_CFG  ,0x00}
};       
/**************************************************************************/
// SMARTRF REGISTER SETTINGS FOR CC1125
// 2 FSK, 300 bps 1 kHz deviation 3.8 kHz RXBW (TC110)
static const registerSetting_t cc1125LowDataRateRfSettings[]= 
{
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x0B},
  {CC112X_DEVIATION_M,       0xD2},
  {CC112X_MODCFG_DEV_E,      0x00},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_FREQ_IF_CFG,       0x33},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x69},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,            0x1F},
  {CC112X_SYMBOL_RATE1,            0x75},
  {CC112X_SYMBOL_RATE0,            0x10},
  {CC112X_AGC_REF,           0x15},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_PKT_CFG2          ,0x00},
  {CC112X_PKT_CFG1          ,0x05},  // Address check off and CRC check on
  {CC112X_PKT_CFG0          ,0x20},  
  {CC112X_RFEND_CFG1        ,0x0F},  // Stay in RX after RX, No timeout for sync word search  
  {CC112X_RFEND_CFG0        ,0x00},  // IDle after TX, no interferring from MARC 
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_PA_CFG0,           0x7C},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x22},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
};

// 2-GFSK 1.2 kbps data rate 4 kHz deviation, 16 kHz RXBW, Cat1
static const registerSetting_t cc1125MediumDataRateRfSettings[] = 
{
    {CC112X_IOCFG0,         0x06},
    {CC112X_SYNC_CFG1,      0x0B},
    {CC112X_DEVIATION_M,    0xA3},
    {CC112X_MODCFG_DEV_E,   0x0A},
    {CC112X_DCFILT_CFG,     0x1C},
    {CC112X_FREQ_IF_CFG,    0x33},
    {CC112X_IQIC,           0xC6},
    {CC112X_CHAN_BW,        0x10},
    {CC112X_MDMCFG0,        0x05},
    {CC112X_SYMBOL_RATE2,         0x3F},
    {CC112X_SYMBOL_RATE1,         0x75},
    {CC112X_SYMBOL_RATE0,         0x10},
    {CC112X_AGC_REF,        0x20},
    {CC112X_AGC_CS_THR,     0x19},
    {CC112X_AGC_CFG1,       0xA9},
    {CC112X_AGC_CFG0,       0xCF},
    {CC112X_FIFO_CFG,       0x00},
    {CC112X_SETTLING_CFG,   0x03},
    {CC112X_FS_CFG,         0x12},
    {CC112X_PA_CFG2,        0x66},
    {CC112X_PA_CFG0,        0x7C},
    {CC112X_PKT_CFG2       ,0x00},
    {CC112X_PKT_CFG1       ,0x05},  // Address check off and CRC check on
    {CC112X_PKT_CFG0       ,0x20},  
    {CC112X_RFEND_CFG1     ,0x0F},  // Stay in RX after RX, No timeout for sync word search  
    {CC112X_RFEND_CFG0     ,0x00},  // IDle after TX, no interferring from MARC     
    {CC112X_PKT_LEN,        0xFF},
    {CC112X_IF_MIX_CFG,     0x00},
    {CC112X_FREQOFF_CFG,    0x22},
    {CC112X_FREQ2,          0x56},
    {CC112X_FREQ1,          0xEC},
    {CC112X_FREQ0,          0x28},
    {CC112X_IF_ADC0,        0x05},
    {CC112X_FS_DIG1,        0x00},
    {CC112X_FS_DIG0,        0x5F},
    {CC112X_FS_CAL0,        0x0E},
    {CC112X_FS_DIVTWO,      0x03},
    {CC112X_FS_DSM0,        0x33},
    {CC112X_FS_DVC0,        0x17},
    {CC112X_FS_PFD,         0x50},
    {CC112X_FS_PRE,         0x6E},
    {CC112X_FS_REG_DIV_CML, 0x14},
    {CC112X_FS_SPARE,       0xAC},
    {CC112X_XOSC5,          0x0E},
    {CC112X_XOSC3,          0xC7},
    {CC112X_XOSC1,          0x07},
};
// 2-GFSK 50 kbps data rate, 25 kHz deviation, 125 kHz RxBW, IEEE 802.15.4g
static const registerSetting_t cc1125HighDataRateRfSettings[]= 
{
  {CC112X_IOCFG0,            0x06},
  {CC112X_SYNC_CFG1,         0x08},
  {CC112X_DEVIATION_M,       0x47},
  {CC112X_MODCFG_DEV_E,      0x0D},
  {CC112X_DCFILT_CFG,        0x15},
  {CC112X_PREAMBLE_CFG1,     0x18},
  {CC112X_FREQ_IF_CFG,       0x2E},
  {CC112X_IQIC,              0x00},
  {CC112X_CHAN_BW,           0x02},
  {CC112X_MDMCFG0,           0x05},
  {CC112X_SYMBOL_RATE2,            0x94},
  {CC112X_SYMBOL_RATE1,            0x7A},
  {CC112X_SYMBOL_RATE0,            0xE1},
  {CC112X_AGC_REF,           0x3C},
  {CC112X_AGC_CS_THR,        0xEF},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xC0},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x14},
  {CC112X_PKT_CFG2          ,0x00},
  {CC112X_PKT_CFG1          ,0x05},  // Address check off and CRC check on
  {CC112X_PKT_CFG0          ,0x20},  
  {CC112X_RFEND_CFG1        ,0x0F},  // Stay in RX after RX, No timeout for sync word search  
  {CC112X_RFEND_CFG0        ,0x00},  // IDle after TX, no interferring from MARC 
  {CC112X_PA_CFG0,           0x79},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_TOC_CFG,           0x0A},
  {CC112X_FREQ2,             0x56},
  {CC112X_FREQ1,             0xCC},
  {CC112X_FREQ0,             0xCC},
  {CC112X_IF_ADC0,           0x05},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC3,             0xC7},
  {CC112X_XOSC1,             0x07},
};
/**************************************************************************/

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
static uint8 cc112xFsCfgs[5] = 
{
  0x1A, // 169 MHz
  0x14, // 434 MHz 
  0x12, // 868 MHz
  0x12, // 915 MHz
  0x12, // 955 MHz  
};
//Frequency programming for CC112X PG2.0 and above
// perSettings.frequencyBand = 0 => 169 MHz 
// perSettings.frequencyBand = 1 => 434 MHz 
// perSettings.frequencyBand = 2 => 868 MHz 
// perSettings.frequencyBand = 3 => 915 Mhz 
// perSettings.frequencyBand = 4 => 955 Mhz 

// For CC112x with 32 MHz XOSC
static uint8 freqConfiguration[5][3] =
{
  {0x69,0xF1,0xFF}, // 169.5125 MHz
  {0x6C,0x80,0x00}, // 434 MHz
  {0x6C,0x80,0x00}, // 868 MHz   
  {0x72,0x60,0x00}, // 915 MHz
  {0x77,0x60,0x00}  // 955 MHz
};
// For CC1125 with 40 MHz TSXO
static uint8 freqConfiguration40Mhz[5][3] = 
{ 
  {0x54, 0xC1, 0x89},  // 169.5125 MHz 
  {0x56, 0xCC, 0xCC},  // 433 MHz  
  {0x56, 0xEC, 0x28},  // 869.225 MHz 
  {0x5B, 0x80, 0x00},  // 915 MHz
  {0x5F, 0x80, 0x00}   // 955 MHz
};
/* These structs shall be 2-dimensional when PG2 is present */
/* Sensitivity table - Note: It's only valid for 3 bytes packets and for shipped register settings */
/* Used in Link Margin calculation */  
static const int16 sensitivity868Mhz[3] = 
{
  -118, 
  -98,
  -100
};
static const int16 cc1125sensitivity868Mhz[3] = 
{
  -129, 
  -123,
  -110
};

/* RSSI offset table */
static const int8 rssiOffset868Mhz[3] = 
{
  102, 
  102, 
  102 
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
   50.00,  /* testcase 38  <=> SMARTRF_CONFIGURATION_1 */
  150.00   /* testcase 9   <=> SMARTRF_CONFIGURATION_2 */
};
static const float cc1125testCaseDataRate[3] =
{
    0.30,  /*    <=> SMARTRF_CONFIGURATION_0 */
    1.20,  /*    <=> SMARTRF_CONFIGURATION_1 */
   50.00   /*    <=> SMARTRF_CONFIGURATION_2 */
};

/******************************************************************************
 * LOCAL VARIABLES
 */

/* Variable is CC112X_STATE_RX when in RX. If not in RX it is CC112X_STATE_TX or 
 * CC112X_STATE_IDLE. The use of this variable is only to avoid an RX interrupt
 * when sending a packet. This facilitates a reduction in power consumption.
 */
static rfStatus_t cc112xRadioTxRx; 
static int8 cc112xRssiOffset; 
 
/******************************************************************************
* @fn          perCC112xGetGuiTxPower
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
int8 perCC112xGetGuiTxPower(uint8 index)
{
  return paPowerGuiValues[index];
}

/******************************************************************************
* @fn          perCC112xGetDataRate
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
float perCC112xGetDataRate(uint8 index)
{
  float guiDataRate;
   if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
   {
      guiDataRate= cc1125testCaseDataRate[index];
   }
   else
   {
     guiDataRate= testCaseDataRate[index];
   }
   return guiDataRate;
}


/******************************************************************************
* @fn          perCC112xSetOutputPower
*
* @brief       Configures the output power of CC112x according to the provided
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
void perCC112xSetOutputPower(uint8 index)
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
 * @fn          perCC112xRegConfig
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
void perCC112xRegConfig(void)
{
  uint8 data;
  uint16 arraySize;
  const registerSetting_t *pRegisterSettings;
  uint8 *pFreqConfig[5];
  
  
  /* Log that radio is in IDLE state */
  perCC112xRxIdle();
  cc112xRadioTxRx = CC112X_STATE_IDLE;
    
  /* Extract what radio configuration to use */
  if((perSettings.masterSlaveLinked==PER_DEVICE_LINKED) || 
      (perSettings.masterSlaveLinked == PER_DEVICE_LINK_BYPASS))
  {
    switch(perSettings.smartRfConfiguration)
    {
      case SMARTRF_CONFIGURATION_0:
        if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
        {
          pRegisterSettings      = cc1125LowDataRateRfSettings;
          arraySize      = (sizeof cc112xLowDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = cc1125sensitivity868Mhz[0];
          cc112xRssiOffset = rssiOffset868Mhz[0];
        }
        else
        {
          pRegisterSettings      = cc112xLowDataRateRfSettings;
          arraySize      = (sizeof cc112xLowDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = sensitivity868Mhz[0];
          cc112xRssiOffset = rssiOffset868Mhz[0];
        }
        break;
      case SMARTRF_CONFIGURATION_2:
        if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
        {
          pRegisterSettings      = cc1125HighDataRateRfSettings;
          arraySize      = (sizeof cc1125HighDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = cc1125sensitivity868Mhz[2];
          cc112xRssiOffset = rssiOffset868Mhz[2];
        }
        else
        {
          pRegisterSettings      = cc112xHighDataRateRfSettings;
          arraySize      = (sizeof cc112xHighDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = sensitivity868Mhz[2];
          cc112xRssiOffset = rssiOffset868Mhz[2];
        }
        break;
      default:
        if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
        {
          pRegisterSettings      = cc1125MediumDataRateRfSettings;
          arraySize      = (sizeof cc1125MediumDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = cc1125sensitivity868Mhz[2];
          cc112xRssiOffset = rssiOffset868Mhz[2];
        }
        else
        {
          pRegisterSettings      = cc112xMediumDataRateRfSettings;
          arraySize      = (sizeof cc112xMediumDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = sensitivity868Mhz[1];
          cc112xRssiOffset = rssiOffset868Mhz[1];
        }
        break;
    }
  }
  else
  {
        if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
        {
          pRegisterSettings      = cc1125MediumDataRateRfSettings;
          arraySize      = (sizeof cc1125MediumDataRateRfSettings/sizeof(registerSetting_t));
          perSettings.sensitivity = cc1125sensitivity868Mhz[2];
          cc112xRssiOffset = rssiOffset868Mhz[2];
        }
        else
        {
          /* Base settings for communication */
          pRegisterSettings      = cc112xMediumDataRateRfSettings;
          arraySize      = (sizeof cc112xMediumDataRateRfSettings/sizeof(registerSetting_t));
          /* In lack of numbers for other freq's than 868 MHz */
          perSettings.sensitivity = sensitivity868Mhz[2];
          cc112xRssiOffset = rssiOffset868Mhz[2];
        }
  }
  
  /* Write register settings to radio */
  for(uint16 i = 0; i < arraySize;i++)
  {
    data = pRegisterSettings[i].data;
    cc112xSpiWriteReg(pRegisterSettings[i].addr,&data,1);
  }
 
  /* Load freq word for CC1125 with 40 MHz TSXO */
  if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
  {
    pFreqConfig[0] = freqConfiguration40Mhz[0];
    pFreqConfig[1] = freqConfiguration40Mhz[1];
    pFreqConfig[2] = freqConfiguration40Mhz[2];
    pFreqConfig[3] = freqConfiguration40Mhz[3];
    pFreqConfig[4] = freqConfiguration40Mhz[4];
  }
  else
  {
    pFreqConfig[0] = freqConfiguration[0];
    pFreqConfig[1] = freqConfiguration[1];
    pFreqConfig[2] = freqConfiguration[2];
    pFreqConfig[3] = freqConfiguration[3];
    pFreqConfig[4] = freqConfiguration[4];
  }
    
    switch(perSettings.frequencyBand)      
    {
      case 0: //169 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[0],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[0],3);
        break;
      case 1: // 434 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[1],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[1],3);
        break;
      case 2: // 868 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[2],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[2],3);
        break;      
      case 3: // 915 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[3],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[3],3);
        break;
      case 4: // 955 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[4],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[4],3);
        break;      
      default: // 868 MHz
        cc112xSpiWriteReg(CC112X_FS_CFG,&cc112xFsCfgs[2],1);
        cc112xSpiWriteReg(CC112X_FREQ2,pFreqConfig[2],3);
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
  // Do CC1125 Cat1 work around if 1.2 kbps test case is chosen for CC1125
  if((perSettings.masterSlaveLinked==PER_DEVICE_LINKED)
     &&(perSettings.smartRfConfiguration == SMARTRF_CONFIGURATION_1) 
        && (perRadioChipType.deviceName == CHIP_TYPE_CC1125))
  {
    cc1125Category1WorkAround();
    
  }
  
  return;  
}

/******************************************************************************
 * @fn          perCC112xSendPacket
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
void perCC112xSendPacket(uint8 *pData)
{
  uint8 len = *pData;
  /* PG1.0 errate fix: Before entering TX, the frequency word must be altered from that of RX */
  /* This means in general that TX from Idle is the only option, not TX from RX */
  perCC112xEnterIdle();
  /* Will only try to transmit if the whole packet can fit i RXFIFO 
   * and we're not currently sending a packet.
   */
  if(!(len > (PER_MAX_DATA-2)) && (cc112xRadioTxRx != CC112X_STATE_TX) )
  {
    cc112xSpiWriteTxFifo(pData,(len+1));
    /* Indicate state to the ISR and issue the TX strobe */
    trxEnableInt();
    cc112xRadioTxRx = CC112X_STATE_TX;
    trxSpiCmdStrobe(CC112X_STX);
    /* Wait until packet is sent before doing anything else */
    __low_power_mode_3();

    /* This function will not return before the complete packet
     * is sent and the radio is back in IDLE. The MSP will be 
     * be sleeping while the packet is beeing sent unless waken
     * by button presses.
     */
    while(cc112xRadioTxRx == CC112X_STATE_TX);
    if(perSettings.linkTopology == LINK_1_WAY)
    {
      /* Back in Idle*/
      trxDisableInt();
    }
  }
  return;
}


/******************************************************************************
 * @fn          perCC112xEnterRx
 *
 * @brief       Enters RX from IDLE. Function is used to abstract the 
 *              cc112xRadioTxRx functionality away from the lower-level radio 
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
void perCC112xEnterRx(void)
{
  perCC112xEnterIdle();
  perCC112xIdleRx();
  cc112xRadioTxRx = CC112X_STATE_RX;
  return;
}


/******************************************************************************
 * @fn          perCC112xEnterSleep
 *
 * @brief       Enters Sleep. Function is used to abstract the cc112xRadioTxRx
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
void perCC112xEnterSleep(void)
{
  perCC112xRxIdle();
  trxSpiCmdStrobe(CC112X_SPWD);
  /* Only important to differ between RX/TX and IDLE */
  cc112xRadioTxRx = CC112X_STATE_IDLE;
  return;
}


/******************************************************************************
 * @fn          perCC112xEnterIdle
 *
 * @brief       Enters IDLE from ANY state. Function is used to abstract the 
 *              cc112xRadioTxRx functionality away from the lower-level radio 
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
void perCC112xEnterIdle(void)
{
  /* wait until chip is ready */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  perCC112xRxIdle();
  cc112xRadioTxRx = CC112X_STATE_IDLE;
  return;
}

/******************************************************************************
 * @fn          perCC112xRxTxISR
 *
 * @brief       ISR that's called when sync signal goes low. 
 *              In RX State: Filters incoming data. The global rxData pointer
 *              always points to this functions static rxData_tmp(struct of
 *              same kind). The validnes of rxData fields is indicated by the
 *              the global flag packetSemaphore.
 *              In TX State: Nothing is done except it facilitates power 
 *              consumption reduction when TX since the program doesn't need
 *              to wait until TX is done before re-enabling sync pin interrupt.
 *              cc112xRadioTxRx is also set to CC112X_STATE_IDLE to be consistent 
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
void perCC112xRxTxISR(void)
{
  uint8 rxBytes,rxLength,rssiIndex,lqiIndex;
  /* This variable stores the data locally. Access is given to per_test by 
   * assigning this instance to the global rxData pointer
   */
  static rxData_t rxData_tmp;
            
  rxData = &rxData_tmp;
  
  /* Checking if the chip is in RX state:  */
  if(cc112xRadioTxRx != CC112X_STATE_RX)
  {
    /* Transmission finished */
    if((perSettings.deviceMode == MASTER_DEVICE) && (perSettings.linkTopology == LINK_2_WAY) && (perSettings.masterSlaveLinked ==PER_DEVICE_LINKED))
    {
      /* Only applicable when master in 2-way test */
      cc112xRadioTxRx=CC112X_STATE_RX;
    }
    else
    {
      cc112xRadioTxRx  = CC112X_STATE_IDLE;
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
  if(rxBytes == 0)
  {
    /* The packet was removed by HW due to addr or length filtering -> Do nothing */
    /* Report that a sync was detected */ 
    rxData_tmp.rssi = perCC112xRead8BitRssi();
    return;
  }
  else
  {
    /* The RX FIFO is not empty, process contents */    
    cc112xSpiReadRxFifo(&rxLength, 1);  
    /* Check that the packet length just read + status bytes(2B) + length byte match the RXBYTES */
    /* If these are not equal:
     * - Packet is not like expected
     */
    if(rxBytes != (rxLength+3))
    {
      /* This is a fault FIFO condition -> clean FIFO and register a sync detection */
      /* IDLE -> FLUSH RX FIFO -> RX */
      perCC112xRxIdle();     
      perCC112xEnterRx(); 
      /* Report that a sync was detected */
      rxData_tmp.rssi = perCC112xRead8BitRssi();
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
      cc112xSpiReadRxFifo(&rxData_tmp.data[1], lqiIndex);
      
      /* The whole packet has been read from the FIFO.
       * Check if the CRC is correct and that the packet length is as expected.
       * If not correct: report sync found and do not update RSSI or LQI.
       */
      if((!(rxData_tmp.data[lqiIndex] & CC112X_LQI_CRC_OK_BM)) || (perSettings.payloadLength != rxLength ))
      {
        rxData_tmp.rssi = perCC112xRead8BitRssi();
        return;
      }
      /* A complete error-free packet has arrived  */
      
      /* Measured data */
      rxData_tmp.length  = rxLength;
      rxData_tmp.lqi     = rxData_tmp.data[lqiIndex] & CC112X_LQI_EST_BM;
      rxData_tmp.addr    = rxData_tmp.data[1]; 
      
      /* Convert RSSI value from 2's complement to decimal value accounting for offset value */
      rxBytes = rxData_tmp.data[rssiIndex];        
      rxData_tmp.rssi = (int16)((int8)rxBytes) - cc112xRssiOffset;
      /* Signal a good packet is received */
      packetSemaphore |= PACKET_RECEIVED;
      return;
    } 
  }   
}     
      
/*******************************************************************************
 * @fn          perCC112xRead8BitRssi
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
int8 perCC112xRead8BitRssi(void)
{
  uint8 rssi2compl,rssiValid;
  int16 rssiConverted;
  
  cc112xSpiReadReg(CC112X_RSSI0, &rssiValid,1);
  if(rssiValid & 0x01)
  {
    /* Read RSSI from MSB register */
    cc112xSpiReadReg(CC112X_RSSI1, &rssi2compl,1);
    rssiConverted = (int16)((int8)rssi2compl) - cc112xRssiOffset;
    return rssiConverted;
  }
  /* keep last value since new value is not valid */
  return rxData->rssi;
}

/*******************************************************************************
 * @fn          perCC112xRxIdle
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
static void perCC112xRxIdle(void)
{
  /* Disable pin interrupt */
  trxDisableInt();
  /* Strobe IDLE */
  trxSpiCmdStrobe(CC112X_SIDLE); 
  /* Wait until chip is in IDLE */
  while(trxSpiCmdStrobe(CC112X_SNOP) & 0xF0);
  /* Clear pin interrupt flag */
  trxClearIntFlag();
  return;
}

/*******************************************************************************
 * @fn          perCC112xIdleRx
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
static void perCC112xIdleRx(void)
{
  trxClearIntFlag();
  trxSpiCmdStrobe(CC112X_SRX);
  trxEnableInt();
  return;
}
/*******************************************************************************
 * @fn          perCC112xWriteTxFifo
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
void perCC112xWriteTxFifo(uint8 *pData, uint8 len)
{
  cc112xSpiWriteTxFifo(pData,len);
  return;
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
/*******************************************************************************
* @fn          cc1125Category1WorkAround
*
* @brief       Expand loop filter bandwith to comply with ETSI Cat 1.
*
* @param       none
*
* @return      none
*/
static void cc1125Category1WorkAround(void)
{
  uint8 writeByte;
  // Cat. 1 work-around
  cc112xSpiReadReg(CC112X_FS_CHP, &writeByte, 1);
  writeByte += 12;
  cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);  
}