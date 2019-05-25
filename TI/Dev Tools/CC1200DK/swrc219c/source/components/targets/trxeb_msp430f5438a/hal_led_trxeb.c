/*****************************************************************************
  Filename:     hal_led_trxeb.c

  Description:  Implementation of the led functionality in hal_led_trxeb.h

*****************************************************************************/

/*****************************************************************************
 * INCLUDES
 */

#include <msp430.h>
#include "hal_types.h"
#include "hal_led_trxeb.h"

/*****************************************************************************
 * CONSTANTS
 */

#define LED_PORT_SEL      P4SEL
#define LED_PORT_OUT      P4OUT
#define LED_PORT_DIR      P4DIR

/*****************************************************************************
 * @fn          halLedInit
 *
 * @brief       Initializes all leds as off
 *
 * input parameters
 *
 * output parameters
 *
 * @return			void
 *
 */
void halLedInit(void)
{
	
	/* IO function for pins */
	LED_PORT_SEL &= ~(LED_1|LED_2|LED_3|LED_4);

    /* Leds are off when initialized */
    LED_PORT_OUT |= (LED_1|LED_2|LED_3|LED_4);

    /* Changing pin direction */
    LED_PORT_DIR |= (LED_1|LED_2|LED_3|LED_4);
}

/******************************************************************************
 * @fn          halLedSet
 *
 * @brief		    Turns a specified led on
 *
 * input parameters
 *
 * @param		    led_id   - the led to turn on. led_id is defined as a
 *                         bitmask < 0x16
 *
 * output parameters
 *
 * @return		  void
 */
void halLedSet(uint8 led_id)
{
	/* All leds can be set in led_id if wanted */
	LED_PORT_OUT &= ~led_id;
}

/******************************************************************************
 * @fn          halLedClear
 *
 * @brief       Turns off a specified led
 *
 * input parameters
 *
 * @param		    led_id  - the led to turn off.led_id is defined as a
 *                        bitmask < 0x16
 *
 * output parameters
 *
 * @return	    void
 */
void halLedClear(uint8 led_id)
{
	/* All leds can be set in led_id if wanted */
  LED_PORT_OUT |= led_id;
}

/******************************************************************************
 * @fn          halLedToggle
 *
 * @brief       Toggles specified led
 *
 * input parameters
 *
 * @param       led_id  - the led to turn off
 *
 * output parameters
 *
 * @return	    void
 */
void halLedToggle(uint8 led_id)
{
	uint8 low, high;
	/* All leds can be set in led_id*/
	high  = LED_PORT_OUT & 0xF0;
	low   = LED_PORT_OUT & 0x0F;
	low  ^= led_id;
	LED_PORT_OUT = high | low;
}



/***********************************************************************************
  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

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