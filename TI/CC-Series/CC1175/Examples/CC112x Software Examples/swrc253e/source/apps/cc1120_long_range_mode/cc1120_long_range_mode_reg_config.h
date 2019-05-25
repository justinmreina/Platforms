//*****************************************************************************
//! @file       cc112x_long_range_mode_reg_config.h
//! @brief      Template for CC112x register export from SmartRF Studio
//              The xml files for the register settings are found on the 
//              Sub-1GHz Wiki: 
//              http://processors.wiki.ti.com/index.php/Category:Sub-1GHz
//              (See Long Range Mode)
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
//****************************************************************************/

#ifndef CC112X_LONG_RANGE_MODE_REG_CONFIG_H
#define CC112X_LONG_RANGE_MODE_REG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
  
  
/******************************************************************************
 * INCLUDES
 */
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"


/******************************************************************************
 * VARIABLES
 */  
// Address config = No address check 
// Packet length = 255 
// Modulation format = 2-GFSK 
// PA ramping = true 
// Packet length mode = Variable 
// Bit rate = 0.6 
// Deviation = 1.499176 
// Packet bit length = 0 
// Performance mode = High Performance 
// Carrier frequency = 470.000000 
// RX filter BW = 7.812500 
// Manchester enable = false 
// Symbol rate = 0.6 
// TX power = 15 
// Device address = 0 
// Whitening = false 
static const registerSetting_t preferredSettings470[]= 
{
  {CC112X_IOCFG3,            0xB0},
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x40},
  {CC112X_SYNC_CFG1,         0x08},
  {CC112X_DEVIATION_M,       0x89},
  {CC112X_MODCFG_DEV_E,      0x09},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x50},
  {CC112X_SYMBOL_RATE2,      0x33},
  {CC112X_AGC_REF,           0x20},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x14},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PA_CFG0,           0x7E},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x30},
  {CC112X_FREQ2,             0x75},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_LNA,               0x03},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
};

// Address config = No address check 
// Packet bit length = 0 
// Whitening = false 
// Carrier frequency = 868.000000 
// Manchester enable = false 
// Bit rate = 0.6 
// Deviation = 1.499176 
// Symbol rate = 0.6 
// Performance mode = High Performance 
// RX filter BW = 12.500000 
// Device address = 0 
// Modulation format = 2-GFSK 
// PA ramping = true 
// Packet length = 255 
// TX power = 15 
// Packet length mode = Variable 
static const registerSetting_t preferredSettings868[]= 
{
  {CC112X_IOCFG3,            0xB0},
  {CC112X_IOCFG2,            0x06},
  {CC112X_IOCFG1,            0xB0},
  {CC112X_IOCFG0,            0x40},
  {CC112X_SYNC3,             0x26},
  {CC112X_SYNC2,             0x33},
  {CC112X_SYNC1,             0xD9},
  {CC112X_SYNC0,             0xCC},
  {CC112X_SYNC_CFG1,         0x08},
  {CC112X_DEVIATION_M,       0x89},
  {CC112X_MODCFG_DEV_E,      0x09},
  {CC112X_DCFILT_CFG,        0x1C},
  {CC112X_IQIC,              0xC6},
  {CC112X_CHAN_BW,           0x10},
  {CC112X_SYMBOL_RATE2,      0x33},
  {CC112X_AGC_REF,           0x20},
  {CC112X_AGC_CS_THR,        0x19},
  {CC112X_AGC_CFG1,          0xA9},
  {CC112X_AGC_CFG0,          0xCF},
  {CC112X_FIFO_CFG,          0x00},
  {CC112X_SETTLING_CFG,      0x03},
  {CC112X_FS_CFG,            0x12},
  {CC112X_PKT_CFG0,          0x20},
  {CC112X_PA_CFG0,           0x7E},
  {CC112X_PKT_LEN,           0xFF},
  {CC112X_IF_MIX_CFG,        0x00},
  {CC112X_FREQOFF_CFG,       0x30},
  {CC112X_FREQ2,             0x6C},
  {CC112X_FREQ1,             0x80},
  {CC112X_FS_DIG1,           0x00},
  {CC112X_FS_DIG0,           0x5F},
  {CC112X_FS_CAL1,           0x40},
  {CC112X_FS_CAL0,           0x0E},
  {CC112X_FS_DIVTWO,         0x03},
  {CC112X_FS_DSM0,           0x33},
  {CC112X_FS_DVC0,           0x17},
  {CC112X_FS_PFD,            0x50},
  {CC112X_FS_PRE,            0x6E},
  {CC112X_FS_REG_DIV_CML,    0x14},
  {CC112X_FS_SPARE,          0xAC},
  {CC112X_FS_VCO0,           0xB4},
  {CC112X_LNA,               0x03},
  {CC112X_XOSC5,             0x0E},
  {CC112X_XOSC1,             0x03},
};


#ifdef  __cplusplus
}
#endif
#endif // CC112X_LONG_RANGE_MODE_REG_CONFIG_H