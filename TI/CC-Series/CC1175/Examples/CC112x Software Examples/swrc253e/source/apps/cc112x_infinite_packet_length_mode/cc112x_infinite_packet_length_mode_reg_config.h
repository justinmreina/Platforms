//*****************************************************************************
//! @file       cc112x_infinite_packet_length_mode_reg_config.h
//! @brief      Template for CC112x register export from SmartRF Studio
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
//*****************************************************************************/


/*******************************************************************************
 * INCLUDES
 */
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"


/*******************************************************************************
 * VARIABLES
 */
// Carrier frequency = 868.000000
// Symbol rate = 38.4
// Bit rate = 38.4
// Deviation = 20.019531
// Manchester enable = false
// Bit rate = 38.4
// Modulation format = 2-GFSK
// Bit rate = 38.4
// RX filter BW = 100.000000
// TX power = 15
// PA ramping = true
// Packet length mode = Variable
// Whitening = false
// Address config = No address check
// Packet length = 255
// Device address = 0
// Rf settings for CC1120
static const registerSetting_t preferredSettings[] = {
    {CC112X_IOCFG3,         0xB0},
    {CC112X_IOCFG2,         0x06},
    {CC112X_IOCFG1,         0xB0},
    {CC112X_IOCFG0,         0x40},
    {CC112X_SYNC_CFG1,      0x08},
    {CC112X_DEVIATION_M,    0x48},
    {CC112X_MODCFG_DEV_E,   0x0D},
    {CC112X_DCFILT_CFG,     0x1C},
    {CC112X_IQIC,           0x00},
    {CC112X_CHAN_BW,        0x02},
    {CC112X_MDMCFG0,        0x05},
    {CC112X_SYMBOL_RATE2,   0x93},
    {CC112X_AGC_CS_THR,     0x19},
    {CC112X_AGC_CFG1,       0xA9},
    {CC112X_AGC_CFG0,       0xCF},
    {CC112X_FIFO_CFG,       0x00},
    {CC112X_SETTLING_CFG,   0x03},
    {CC112X_FS_CFG,         0x12},
    {CC112X_PKT_CFG0,       0x20},
    {CC112X_PA_CFG0,        0x7B},
    {CC112X_PKT_LEN,        0xFF},
    {CC112X_IF_MIX_CFG,     0x00},
    {CC112X_FREQOFF_CFG,    0x22},
    {CC112X_FREQ2,          0x6C},
    {CC112X_FREQ1,          0x80},
    {CC112X_FS_DIG1,        0x00},
    {CC112X_FS_DIG0,        0x5F},
    {CC112X_FS_CAL1,        0x40},
    {CC112X_FS_CAL0,        0x0E},
    {CC112X_FS_DIVTWO,      0x03},
    {CC112X_FS_DSM0,        0x33},
    {CC112X_FS_DVC0,        0x17},
    {CC112X_FS_PFD,         0x50},
    {CC112X_FS_PRE,         0x6E},
    {CC112X_FS_REG_DIV_CML, 0x14},
    {CC112X_FS_SPARE,       0xAC},
    {CC112X_FS_VCO0,        0xB4},
    {CC112X_XOSC5,          0x0E},
    {CC112X_XOSC1,          0x03},
};