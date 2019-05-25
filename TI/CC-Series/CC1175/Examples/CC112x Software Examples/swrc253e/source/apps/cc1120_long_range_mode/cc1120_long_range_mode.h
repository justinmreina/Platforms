//*****************************************************************************
//! @file       cc1120_long_range_mode.h
//! @brief      This file contains structures and enums used by the 
//              menu system
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

#ifndef CC112X_LONG_RANGE_MODE_H
#define CC112X_LONG_RANGE_MODE_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * INCLUDES
 */
#include "hal_spi_rf_trxeb.h"
#include "cc112x_spi.h"


/*******************************************************************************
 * DEFINES
 */
#define MENU_NAME_SIZE  15  // Menu name length, without NULL terminator
#define VALUE_TEXT_SIZE 14  // Value text length, without NULL terminator

#define MENU_ITEM_COUNT ((uint8) (sizeof(pMenuTable) / sizeof(MENU_ITEM)))


/*******************************************************************************
 * ENUMS
 */
// ENUM used to index the pMenuTable of type MENU_ITEM
enum {
  FREQ_BAND = 0,
  SELECT_MODE,
  START_PER
};

// ENUM used to set the freqBand in MENU_DATA
enum {
  FREQ_470 = 0,
  FREQ_868
};

// ENUM used the set the mode in MENU_DATA
enum {
  RX_MODE = 0,
  TX_MODE
};

typedef enum {
  TEXT   = 0,
  ACTION,
} MENU_ITEM_TYPE;


/*******************************************************************************
 * STUCTS
 */
typedef struct {
  uint8 freqBand; //  FREQ_470 or FREQ_868
  uint8 mode;     // RX_MODE or TX_MODE
} MENU_DATA;


typedef struct {
  char pMenuName[MENU_NAME_SIZE + 1]; // The title of the menu item
  MENU_ITEM_TYPE type;                // Item type: TEXT or ACTION
  uint8 *pValue;                      // Current value
  uint8 minValue;                     // Minimum value
  uint8 maxValue;                     // Maximum value
  char *pText;                        // Pointer to value table in TEXT mode
} MENU_ITEM;


/*******************************************************************************
* GLOBAL VARIABLES
*/
static char ppFreqBand[][VALUE_TEXT_SIZE + 1] = {
  " 470 MHz      ", 
  " 868 MHz      "
};

static char ppMode[][VALUE_TEXT_SIZE + 1] = {
  " RX           ", 
  " TX           "
};

#ifdef  __cplusplus
}
#endif
#endif // CC112X_LONG_RANGE_MODE_H
