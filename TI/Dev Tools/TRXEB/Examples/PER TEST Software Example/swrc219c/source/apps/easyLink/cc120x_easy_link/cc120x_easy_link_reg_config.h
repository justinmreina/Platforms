//*****************************************************************************
//! @file cc120x_easy_link_reg_config.h  
//   
//! @brief  Template for CC112x register export from SmartRF Studio 
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
#ifndef CC120X_EASY_LINK_REG_CONFIG_H
#define CC120X_EASY_LINK_REG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * INCLUDES
 */
#include "trx_rf_spi.h"
#include "cc120x_spi.h"
  
/******************************************************************************
 * FUNCTIONS
 */  
  
// RX filter BW = 25.252525 
// Address config = No address check. 
// Packet length = 255 
// Symbol rate = 1.2 
// Carrier frequency = 955.000000 
// Bit rate = 1.2 
// Packet bit length = 0 
// Whitening = false 
// Manchester enable = false 
// Modulation format = 2-FSK 
// Packet length mode = Variable 
// Device address = 0 
// Deviation = 3.986359 

static const registerSetting_t preferredSettings[]= 
{
  {CC120X_IOCFG0,            0x06},
  {CC120X_DEVIATION_M,       0xD1},
  {CC120X_MODCFG_DEV_E,      0x00},
  {CC120X_DCFILT_CFG,        0x5D},
  {CC120X_PREAMBLE_CFG0,     0x8A},
  {CC120X_IQIC,              0xCB},
  {CC120X_CHAN_BW,           0x61},
  {CC120X_MDMCFG1,           0x40},
  {CC120X_MDMCFG0,           0x05},
  {CC120X_SYMBOL_RATE2,            0x3F},
  {CC120X_SYMBOL_RATE1,            0x75},
  {CC120X_SYMBOL_RATE0,            0x10},
  {CC120X_AGC_REF,           0x20},
  {CC120X_AGC_CS_THR,        0xEC},
  {CC120X_AGC_CFG1,          0x51},
  {CC120X_AGC_CFG0,          0xC7},
  {CC120X_FIFO_CFG,          0x00},
  {CC120X_FS_CFG,            0x12},
  {CC120X_WOR_CFG1,          0x12},
  {CC120X_PKT_CFG0,          0x20},
  {CC120X_PA_CFG1,           0x3F},
  {CC120X_PKT_LEN,           0xFF},
  {CC120X_IF_MIX_CFG,        0x1C},
  {CC120X_FREQOFF_CFG,       0x22},
  {CC120X_MDMCFG2,           0x0C},
  {CC120X_FREQ2,             0x5F},
  {CC120X_FREQ1,             0x80},
  {CC120X_FS_DIG1,           0x07},
  {CC120X_FS_DIG0,           0xAF},
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
  {CC120X_XOSC5,             0x0E},
  {CC120X_XOSC1,             0x03},
};
#ifdef  __cplusplus
}
#endif

#endif