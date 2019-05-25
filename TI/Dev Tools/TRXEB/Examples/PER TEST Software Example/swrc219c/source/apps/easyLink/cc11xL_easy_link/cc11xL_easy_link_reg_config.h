//*****************************************************************************
//! @file cc11xl_easy_link_reg_config.h  
//    
//! @brief  Template for CC11xL register export from SmartRF Studio 
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
#ifndef CC11XL_EASY_LINK_REG_CONFIG_H
#define CC11XL_EASY_LINK_REG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
 * INCLUDES
 */
#include "trx_rf_spi.h"
#include "cc11xL_spi.h"
  
/******************************************************************************
 * FUNCTIONS
 */  

// Sync word qualifier mode = 30/32 sync word bits detected 
// CRC autoflush = false 
// Channel spacing = 199.951172 
// Data format = Normal mode 
// Data rate = 1.19948 
// RX filter BW = 58.035714 
// PA ramping = false 
// Preamble count = 4 
// Address config = No address check 
// Carrier frequency = 867.999939 
// Device address = 0 
// TX power = 0 
// Manchester enable = false 
// CRC enable = true 
// Deviation = 5.157471 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Packet length = 255 
// Modulation format = GFSK 
// Base frequency = 867.999939 
// Modulated = true 
// PA table 
#define PA_TABLE {0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

static const registerSetting_t cc110LpreferredSettings[]= 
{
  {CC110L_IOCFG0,           0x06},
  {CC110L_FIFOTHR,          0x47},
  {CC110L_PKTCTRL0,         0x05},
  {CC110L_FSCTRL1,          0x06},
  {CC110L_FREQ2,            0x21},
  {CC110L_FREQ1,            0x62},
  {CC110L_FREQ0,            0x76},
  {CC110L_MDMCFG4,          0xF5},
  {CC110L_MDMCFG3,          0x83},
  {CC110L_MDMCFG2,          0x13},
  {CC110L_DEVIATN,          0x15},
  {CC110L_MCSM0,            0x18},
  {CC110L_FOCCFG,           0x16},
  {CC110L_RESERVED_0X20,    0xFB},
  {CC110L_FSCAL3,           0xE9},
  {CC110L_FSCAL2,           0x2A},
  {CC110L_FSCAL1,           0x00},
  {CC110L_FSCAL0,           0x1F},
  {CC110L_TEST2,            0x81},
  {CC110L_TEST1,            0x35},
  {CC110L_TEST0,            0x09},
};

// RX filter BW = 58.035714 
// CRC enable = true 
// Packet length = 255 
// Address config = No address check 
// Modulated = true 
// Data rate = 1.19948 
// Sync word qualifier mode = 30/32 sync word bits detected 
// Carrier frequency = 867.999939 
// Modulation format = GFSK 
// Manchester enable = false 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte received after sync word 
// Data format = Normal mode 
// Device address = 0 
// CRC autoflush = false 
// Base frequency = 867.999939 
// Deviation = 5.157471 

static const registerSetting_t cc113LpreferredSettings[]= 
{
  {CC113L_IOCFG0,           0x06},
  {CC113L_FIFOTHR,          0x47},
  {CC113L_PKTCTRL0,         0x05},
  {CC113L_FSCTRL1,          0x06},
  {CC113L_FREQ2,            0x21},
  {CC113L_FREQ1,            0x62},
  {CC113L_FREQ0,            0x76},
  {CC113L_MDMCFG4,          0xF5},
  {CC113L_MDMCFG3,          0x83},
  {CC113L_MDMCFG2,          0x13},
  {CC113L_DEVIATN,          0x15},
  {CC113L_MCSM0,            0x18},
  {CC113L_FOCCFG,           0x16},
  {CC113L_RESERVED_0X20,    0xFB},
  {CC113L_FSCAL3,           0xE9},
  {CC113L_FSCAL2,           0x2A},
  {CC113L_FSCAL1,           0x00},
  {CC113L_FSCAL0,           0x1F},
  {CC113L_TEST2,            0x81},
  {CC113L_TEST1,            0x35},
  {CC113L_TEST0,            0x09},
};
// Sync word qualifier mode = 30/32 sync word bits detected 
// CRC autoflush = false 
// Channel spacing = 199.951172 
// Data format = Normal mode 
// Data rate = 1.19948 
// RX filter BW = 58.035714 
// PA ramping = false 
// Preamble count = 4 
// Address config = No address check 
// Carrier frequency = 867.999939 
// Device address = 0 
// TX power = 0 
// Manchester enable = false 
// CRC enable = true 
// Deviation = 5.157471 
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Packet length = 255 
// Modulation format = GFSK 
// Base frequency = 867.999939 
// Modulated = true 
static const registerSetting_t cc115LpreferredSettings[]= 
{
  {CC115L_IOCFG2,           0x2E},
  {CC115L_IOCFG0,           0x06},
  {CC115L_FIFOTHR,          0x47},
  {CC115L_PKTCTRL0,         0x05},
  {CC115L_FREQ2,            0x21},
  {CC115L_FREQ1,            0x62},
  {CC115L_FREQ0,            0x76},
  {CC115L_MDMCFG4,          0xF5},
  {CC115L_MDMCFG3,          0x83},
  {CC115L_MDMCFG2,          0x13},
  {CC115L_DEVIATN,          0x15},
  {CC115L_MCSM0,            0x18},
  {CC115L_RESERVED_0X20,    0xFB},
  {CC115L_FSCAL3,           0xE9},
  {CC115L_FSCAL2,           0x2A},
  {CC115L_FSCAL1,           0x00},
  {CC115L_FSCAL0,           0x1F},
  {CC115L_TEST2,            0x81},
  {CC115L_TEST1,            0x35},
  {CC115L_TEST0,            0x09},
};

#ifdef  __cplusplus
}
#endif

#endif