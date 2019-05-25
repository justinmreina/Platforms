/*******************************************************************************
  Filename:    wdt_wd.c

  Description: Uses the WDT in Watchdog mode. Since the Watchdog Timer is never
               cleared, it will cause a reset. The reset source is indicated
               with the LEDs (reset source is obtained from [SLEEPSTA.RST]). The
               LEDs are in no way directly related to the Watchdog Timer, they
               are just used to indicate what is happening.

*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2544)
#include "ioCC2544.h"
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif

/*******************************************************************************
* CONSTANTS
*/
// The LED ON duration
#define LED_ON_TIME  5000


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint32 __xdata ledOnCnt = 0;


/*******************************************************************************
* LOCAL FUNCTIONS
*/
void resetCauseLedIndication(void);


/*******************************************************************************
* @fn          resetCauseLedIndication
*
* @brief       Not directly related to the Watchdog Timer. Blinks LED1, LED3
*              or both LEDs depending on the cause of the last reset.
*              A power-on or a brown-out reset blinks both LEDs,
*              an external reset blinks LED1, a Watchdog reset blinks
*              the LED3. See the datasheet for details on reset sources.
*
* @param       void
*
* @return      void
*******************************************************************************/
void resetCauseLedIndication()
{
    /* The cause of the last reset can be read from the [SLEEPSTA.RST] bits
     * register. The switch blinks one or both LEDs accordingly.
     */
    switch (SLEEPSTA & SLEEPSTA_RST)
    {
    case SLEEPSTA_RST_POR_BOD:
#if (chip==2541)
        P1_0 = 1;   // Set SRF05EB LED1
        P1_4 = 1;   // Set SRF05EB LED3
#elif (chip==2543)
        P1_0 = 1;   // Set SRF05EB LED1
        P1_1 = 1;   // Set SRF05EB LED3
#elif (chip==2545)
        P1_0 = 1;   // Set SRF05EB LED1
        P2_1 = 1;   // Set SRF05EB LED3
#elif (chip==2544)
        P0_2 = 1;   // Set GREEN LED1 on CC2544Dongle
        P0_1 = 1;   // Set RED LED2 on CC2544Dongle
#endif  
        for(ledOnCnt = 0; ledOnCnt < LED_ON_TIME; ledOnCnt++);
#if (chip==2541)
        P1_0 = 0;   // Clear SRF05EB LED1
        P1_4 = 0;   // Clear SRF05EB LED3
#elif (chip==2543)
        P1_0 = 0;   // Clear SRF05EB LED1
        P1_1 = 0;   // Clear SRF05EB LED3
#elif (chip==2545)
        P1_0 = 0;   // Clear SRF05EB LED1
        P2_1 = 0;   // Clear SRF05EB LED3
#elif (chip==2544)
        P0_2 = 0;   // Clear GREEN LED1 on CC2544Dongle
        P0_1 = 0;   // Clear RED LED2 on CC2544Dongle
#endif
        break;
    case SLEEPSTA_RST_EXT:
#if (chip==2541 || chip==2543 || chip==2545)
        P1_0 = 1;   // Set SRF05EB LED1
#elif (chip==2544)
        P0_2 = 1;   // Set GREEN LED1 on CC2544Dongle
#endif
        for(ledOnCnt = 0; ledOnCnt < LED_ON_TIME; ledOnCnt++);  
#if (chip==2541 || chip==2543 || chip==2545)
        P1_0 = 0;   // Clear SRF05EB LED1
#elif (chip==2544)
        P0_2 = 0;   // Clear GREEN LED1 on CC2544Dongle
#endif
        break;
    case SLEEPSTA_RST_WDT:
#if (chip==2541)
        P1_4 = 1;   // Set SRF05EB LED3
#elif (chip==2543)
        P1_1 = 1;   // Set SRF05EB LED3
#elif (chip==2545)
        P2_1 = 1;   // Set SRF05EB LED3
#elif (chip==2544)
        P0_1 = 1;   // Set RED LED2 on CC2544Dongle
#endif  
        for(ledOnCnt = 0; ledOnCnt < LED_ON_TIME; ledOnCnt++);
#if (chip==2541)
        P1_4 = 0;   // Clear SRF05EB LED3
#elif (chip==2543)
        P1_1 = 0;   // Clear SRF05EB LED3
#elif (chip==2545)
        P2_1 = 0;   // Clear SRF05EB LED3
#elif (chip==2544)
        P0_1 = 0;   // Clear RED LED2 on CC2544Dongle
#endif
        break;
    default:
      break;
    }
}

/*******************************************************************************
* @fn          main
*
* @brief       Runs the WDT in Watchdog Mode. The Watchdog causes a reset
*              approx. every second. The LEDs indicate what caused the reset
*              (read from [SLEEPSTA.RST]). The LEDs are in no way directly related
*              to the Watchdog Timer, they are just used to indicate what is
*              happening.
*
* @param       void
*
* @return      0
*******************************************************************************/
int main(void)
{
    /* Setup I/O */
#if (chip==2541)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~(BIT4 | BIT0);   
    // Set LED1 and LED3 as output.
    P1DIR |= (BIT0 | BIT4);
    // LEDs off.
    P1_0 = 0; P1_4 = 0;       
#elif (chip==2543)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~(BIT1 | BIT0);   
    // Set LED1 and LED3 as output.
    P1DIR |= (BIT0 | BIT1);
    // LEDs off.
    P1_0 = 0; P1_1 = 0;       
#elif (chip==2545)
    // LED1 and LED3 as GPIO.
    P1SEL &= ~BIT0; 
    P2SEL &= ~BIT1; 
    // Set LED1 and LED3 as output.
    P1DIR |= BIT0;
    P2DIR |= BIT1;
    // LEDs off.
    P1_0 = 0; P2_1 = 0;       
#elif (chip==2544)
    // LED1 (GREEN) and LED2 (RED) as GPIO.
    P0SEL0 &= ~P0SEL0_SELP0_1; 
    P0SEL1 &= ~P0SEL1_SELP0_2;
    // Set LED1 (GREEN) and LED2 (RED) as output.
    PDIR |= (PDIR_DIRP0_1 | PDIR_DIRP0_2);
    // LEDs off.
    P0_2 = 0; P0_1 = 0;       
#endif

    /* Sets the Watchdog Timer timeout interval to approx. 1 s. The low power
     * LS RCOSC is used (default); the accuracy of timout interval will vary
     * with the choise of low speed OSC, see the datasheet for details.
     */
    WDCTL = (WDCTL & ~WDCTL_INT) | WDCTL_INT_1_SEC;

    /* Sets the Watchdog Timer in Watchdog mode and starts it. Note that when
     * the WDT is in Watchdog mode, setting IDLE mode has no effect.
     * Furthermore, if the timeout interval is to be changed, the change should
     * be followed by a clearing of the Watchdog Timer to avoid an unwanted
     * reset. See the datasheet for details. The Watchdog Timer is disabled
     * after a reset.
     */
    WDCTL = (WDCTL & ~WDCTL_MODE) | WDCTL_MODE_WD;

    /* Blinks LEDs given the cause of the last reset. */
    resetCauseLedIndication();

    /* Infinite loop. */
    while (1);
}

/*******************************************************************************
  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

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
*******************************************************************************/