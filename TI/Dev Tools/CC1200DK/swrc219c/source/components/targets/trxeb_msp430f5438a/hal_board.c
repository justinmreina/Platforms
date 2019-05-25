/*****************************************************************************
  Filename:     hal_board.c

  Description:  Implementation of hal_board.h for MSP430F5438a on TrxEB

*****************************************************************************/

/*****************************************************************************
 * INCLUDES
 */

#include <msp430.h>
#include <hal_msp430.h>
#include <hal_board.h>
#include <hal_types.h>
#include <hal_mcu.h>
#include "hal_led_trxeb.h"
#include "hal_button_trxeb.h"

/*****************************************************************************
 * VARIABLES
 */
uint8  mclkFrequency;


/*****************************************************************************
 * @fn          halBoardInit
 *
 * @brief       Initializes all GPIO configurations.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halBoardInit(void)
{
    /* Settingcapacitor values for XT1, 32768 Hz */
    halMcuStartXT1();
    /* Clocks:
    * mclk  = mclkFrequency
    * smclk = mclkFrequency
    * aclk  = 32768 Hz
    */
    mclkFrequency = HAL_MCU_SYSCLK_8MHZ;
    halMcuSetSystemClock(mclkFrequency);

    /* Initialize LEDs as off */
    halLedInit();

    /* De-assert SPI CSn pins as default */
    MCU_IO_OUTPUT(HAL_BOARD_IO_LCD_CS_PORT,HAL_BOARD_IO_LCD_CS_PIN,1);      // LCD
    MCU_IO_OUTPUT(HAL_BOARD_IO_ACC_CS_PORT,HAL_BOARD_IO_ACC_CS_PIN,1);      // Accelerometer
    MCU_IO_OUTPUT(HAL_BOARD_IO_FLASH_CS_PORT,HAL_BOARD_IO_FLASH_CS_PIN,1);  // SPI Flash
    MCU_IO_OUTPUT(HAL_BOARD_RF_SPI0_CS_PORT,HAL_BOARD_RF_SPI0_CS_PIN,1);    // RF_SPI0
    MCU_IO_OUTPUT(HAL_BOARD_RF_SPI1_CS_PORT,HAL_BOARD_RF_SPI1_CS_PIN,1);    // RF_SPI1

    /* Power off MSP controlled peripheral units (IO default is GPIO input tristate) */
    MCU_IO_OUTPUT(HAL_BOARD_IO_FLASH_RST_PORT,HAL_BOARD_IO_FLASH_RST_PIN,0);            // Hold SPI flash in reset (to avoid inadvertent writes)
    MCU_IO_INPUT(HAL_BOARD_IO_ACC_PWR_PORT,HAL_BOARD_IO_ACC_PWR_PIN,MCU_IO_PULLDOWN);   // Accelerometer as pulldown
    MCU_IO_INPUT(HAL_BOARD_IO_ALS_PWR_PORT,HAL_BOARD_IO_ALS_PWR_PIN,MCU_IO_PULLDOWN);   // Lightsensor as pulldown

} // halBoardInit

/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
