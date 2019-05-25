//******************************************************************************
//! @file       cc1120_lrm_config.c
//! @brief      This file contains register configuration and calibration 
//              routines necessary to operate the CC1120
//!
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
#include "msp430.h"
#include "lcd_dogm128_6.h"
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"
#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "io_pin_int.h"
#include "bsp_led.h"
#include "cc1120_long_range_mode_reg_config.h"
#include "cc1120_long_range_mode.h"


/*******************************************************************************
*   @fn	        registerConfig
*
*   @brief      Reset and configure the CC1120 based on selected freq. band
*
*   @param      freq 
*                 FREQ_470 or FREQ_868    
*
*   @return     none
*/
void registerConfig(uint8 freq) {
  uint8 writeByte;

  trxSpiCmdStrobe(CC112X_SRES);

  if (freq == FREQ_470) {
    for(uint16 i = 0; 
        i < (sizeof(preferredSettings470)/sizeof(registerSetting_t)); i++) {
      writeByte = preferredSettings470[i].data;
      cc112xSpiWriteReg(preferredSettings470[i].addr, &writeByte, 1);
    }
  } else {
    for(uint16 i = 0; 
        i < (sizeof(preferredSettings868)/sizeof(registerSetting_t)); i++) {
      writeByte = preferredSettings868[i].data;
      cc112xSpiWriteReg(preferredSettings868[i].addr, &writeByte, 1);
    } 
  }
}


/*******************************************************************************
*   @fn         manualCalibration
*
*   @brief      Calibrates radio according to CC112x errata
*
*   @param      none
*
*   @return     none
*/
#define VCDAC_START_OFFSET 2
#define FS_VCO2_INDEX 0
#define FS_VCO4_INDEX 1
#define FS_CHP_INDEX 2
void manualCalibration(void) {

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

  // 3) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  trxSpiCmdStrobe(CC112X_SCAL);

  do {
      cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
  } while (marcstate != 0x41);

  // 4) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained with 
  //    high VCDAC_START value
  cc112xSpiReadReg(CC112X_FS_VCO2, 
                   &calResults_for_vcdac_start_high[FS_VCO2_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_VCO4, 
                   &calResults_for_vcdac_start_high[FS_VCO4_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_CHP, 
                   &calResults_for_vcdac_start_high[FS_CHP_INDEX], 1);

  // 5) Set VCO cap-array to 0 (FS_VCO2 = 0x00)
  writeByte = 0x00;
  cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);

  // 6) Continue with mid VCDAC (original VCDAC_START):
  writeByte = original_fs_cal2; 
  cc112xSpiWriteReg(CC112X_FS_CAL2, &writeByte, 1);

  // 7) Calibrate and wait for calibration to be done
  //   (radio back in IDLE state)
  trxSpiCmdStrobe(CC112X_SCAL);

  do {
    cc112xSpiReadReg(CC112X_MARCSTATE, &marcstate, 1);
  } while (marcstate != 0x41);

  // 8) Read FS_VCO2, FS_VCO4 and FS_CHP register obtained 
  //    with mid VCDAC_START value
  cc112xSpiReadReg(CC112X_FS_VCO2, 
                   &calResults_for_vcdac_start_mid[FS_VCO2_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_VCO4, 
                   &calResults_for_vcdac_start_mid[FS_VCO4_INDEX], 1);
  cc112xSpiReadReg(CC112X_FS_CHP,
                   &calResults_for_vcdac_start_mid[FS_CHP_INDEX], 1);

  // 9) Write back highest FS_VCO2 and corresponding FS_VCO
  //    and FS_CHP result
  if (calResults_for_vcdac_start_high[FS_VCO2_INDEX] >
    calResults_for_vcdac_start_mid[FS_VCO2_INDEX]) {
    writeByte = calResults_for_vcdac_start_high[FS_VCO2_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_high[FS_VCO4_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_high[FS_CHP_INDEX];
    cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
  } else {
    writeByte = calResults_for_vcdac_start_mid[FS_VCO2_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO2, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_mid[FS_VCO4_INDEX];
    cc112xSpiWriteReg(CC112X_FS_VCO4, &writeByte, 1);
    writeByte = calResults_for_vcdac_start_mid[FS_CHP_INDEX];
    cc112xSpiWriteReg(CC112X_FS_CHP, &writeByte, 1);
  }
}
