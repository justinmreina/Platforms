/*****************************************************************************
  Filename:     hal_button_trxeb.c

  Description:  HAL button implementation file. Uses WDT for button debouncer.

******************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
#include "hal_digio2.h"
#include "hal_button_trxeb.h"

/*****************************************************************************
 * LOCAL VARIABLES
 */
static volatile uint8 buttonsPressed;
static uint8 intDisabledMask;

/*****************************************************************************
 * CONSTANTS
 */

#define BUTTON_PORT_DIR   P2DIR
#define BUTTON_PORT_SEL   P2SEL
#define BUTTON_PORT_OUT   P2OUT
#define BUTTON_PORT_REN   P2REN
#define BUTTON_PORT_IE    P2IE
#define BUTTON_PORT_IES   P2IES
#define BUTTON_PORT_IFG   P2IFG
#define BUTTON_PORT_IN    P2IN


/*****************************************************************************
 * LOCAL FUNCTIONS
 */


/*****************************************************************************
 * @fn          buttonsPressedISR
 *
 * @brief       Interrupt Service Routine for a pressed button.
 *              Stores the pin where the interrupt occured, disables the
 *              interrupt on that pin and starts the debouncing by use of WDT.
 *
 * input parameters
 *
 * output parameters
 *
 * @return    none
 */
static void buttonsPressedISR(void)
{
  SFRIE1 &= ~WDTIE;         /* Disable interrupt */
  WDTCTL = WDTPW + WDTHOLD; /* Stop WDT */

  uint8 value    = BUTTON_PORT_IFG;

  value &= HAL_BUTTON_ALL;   /* Masking out the buttons */
  buttonsPressed  |= value;  /* All pushes are stored until read */
  intDisabledMask |= value;  /* Marks which pin should be interrupt disabled */

  /* Pin interrupts are not enabled before the WDT expires */
  /* The WDT is reset on each button iterrupt */
  BUTTON_PORT_IE  &= ~intDisabledMask;

  /* Starting the WDT  */
  SFRIFG1 &= ~WDTIFG; /* Clears pending interrupt flag */
  /* WDT as 250 ms interval counter when ACLK is 32768 Hz */
  WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL +WDTIS_5;
  SFRIE1 |= WDTIE; /* Enable WDT interrupt */

  /* Power modes are handled by the common PORT2_VECTOR ISR */
}


/*****************************************************************************
 * GLOBAL FUNCTIONS
 */

/*****************************************************************************
 * @fn          halButtonsInit
 *
 * @brief       Initializes the GPIO ports to act as buttons. Pins configured as
 *              input with pull-ups.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halButtonsInit(void)
{
  buttonsPressed    = 0x00;
  uint8 buttonsMask = HAL_BUTTON_ALL;
  intDisabledMask   = HAL_BUTTON_ALL; /* All interrupts are disabled */

  BUTTON_PORT_OUT |= buttonsMask;
  BUTTON_PORT_DIR &= ~buttonsMask;
  BUTTON_PORT_REN |= buttonsMask;
  BUTTON_PORT_SEL &= ~buttonsMask;
}

/*****************************************************************************
 * @fn          halButtonsInterruptEnable
 *
 * @brief       Enables interrupt on all buttons, high->low transition
 *
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halButtonsInterruptEnable(void)
{
  uint8 buttonIntEnableMask = HAL_BUTTON_ALL;
  digio io;
  io.port = 2;
  io.pin  = 1;
  for (io.pin = 1; io.pin < 6; io.pin++)
  {
    halDigio2IntConnect(io, &buttonsPressedISR);
  }
  intDisabledMask &= ~buttonIntEnableMask; /* all buttons have interrupt enabled */
  BUTTON_PORT_IES &= ~buttonIntEnableMask;
  BUTTON_PORT_IE  |=  buttonIntEnableMask;
}

/****************************************************************************
 * @fn          halButtonsInterruptDisable
 *
 * @brief       Disables button interrupts
 *
 * input parameters
 *
 * @param       buttonIntEnableMask - The button pin(s) for which the
 *                                    interrupt(s) should be disabled.
 * output parameters
 *
 * @return none
 */
void halButtonsInterruptDisable(uint8 buttonIntEnableMask)
{
  BUTTON_PORT_IE &= ~buttonIntEnableMask;
}



/*****************************************************************************
 * @fn          halButtonsPushed
 *
 * @brief       Reads the value of buttonsPressed and then RESETS buttonsPressed
 *
 * input parameters
 *
 * output parameters
 *
 * @return      value - Contains the bitmask code for the buttons pushed
 *                      since last call
 */
uint8 halButtonsPushed(void)
{
  uint8 value = buttonsPressed;
  buttonsPressed = 0x00; // Clear button bitmask variable
  return value;
}


/*****************************************************************************
 * @fn        WDT_ISR
 *
 * @brief     WDT ISR used to debounce button pushes. Timer period = 15.6 ms
 *            When executed, disabled interrupts are re-enabled.
 *
 * input parameters
 *
 * output parameters
 *
 * @return   none
 */

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{

  SFRIFG1 &= ~WDTIFG;                  /* Clearing interrupt flag*/
  SFRIE1 &= ~WDTIE;                    /* Disable interrupt */
  WDTCTL = WDTPW + WDTHOLD;            /* Conserves power when not used */
  BUTTON_PORT_IFG &= ~intDisabledMask; /* Clearing pending interrupts*/
  BUTTON_PORT_IE  |= intDisabledMask;  /* Re-enables the the pin interrupt(s)*/
  intDisabledMask  = 0x00;
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