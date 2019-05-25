/*******************************************************************************
  Filename:     t4_pwm.c

  Description:  Generates a sine wave signal, by using Timer 4 to generate
                a PWM signal with varying duty cycle. The frequency f of the
                sine wave is
                                f = f_tts / (t4_div * 2 * t4_tcv * N)
  
                where f_tts is the Timer tick speed (can be adjusted in the
                CLKCON register), t4_div is the Timer 4 prescaler divider value
                (can be adjusted in the T4CTL register), t4_tcv is the Timer 4
                terminal count value (can be adjusted in the T4CC0 register) and
                N is the number of samples per sine wave.
    
                The values for adjusting the PWM duty cycle are stored in a
                lookup table, and this value sets the compare value for
                Timer 4, which is running in up/down mode, setting output on
                compare-up and clearing output on compare-down. The compare
                value is changed when Timer 4 reaches 0x00 (and generates an
                interrupt).
  

                On the SmartRF05EB, Timer 4 channel 1 is connected to P1_0 
                (GREEN LED1) for the CC2543EM and CC2545EM and to P1_1 
                (RED LED2) for the CC2541EM. On the CC2544Dongle Timer 4 
                channel 1 is connected to P0_2 (g0reen LED). I/O location 
                alternative 1 is chosen. 


                This example uses f_tts = 4 Mhz and t4_div = 128, to keep the
                pwm period above 120 Hz, to appear smooth to the eye with low 
                duty cycles. We use N = 16, and t4_tcv = 244 is used to fine
                tune a 4 Hz sine wave.

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

// Number of sine samples.
#define NO_SAMPLES 16

/*******************************************************************************
* LOCAL VARIABLES
*/

/*******************************************************************************
* LOCAL FUNCTIONS
*/

/*******************************************************************************
* @fn      t4_isr
*
* @brief   Interrupt handler for Timer 4 overflow interrupts. Interrupts from
*          Timer 4 are level triggered, so the module interrupt flag is cleared
*          prior to the CPU interrupt flag. Timer 4 interrupt source is not
*          checked, since only overflow interrupts are enabled. On Timer 4
*          overflow interrupts, the PWM duty cycle is changed by altering the
*          compare value.
*
* @param   void
*
* @return  void
*
*******************************************************************************/
#pragma vector = T4_VECTOR
__interrupt void t4_isr(void)
{
    /* Local variables. The sine wave consists of 16 samples, so the PWM
     * lookup table pwmLut has 16 values. These are calculated from
     *
     *      121 * cos(k * 2pi/N) + T4CC0/2, k = 0, 1, ..., 15
     *
     * Since a too large amplitude may saturate the LED or the 8 bit register,
     * we attenuate the amplitude by atleast T4CC0/2 - 1.
     */
    static uint8 i = 0;
    static const uint8 pwmLut[NO_SAMPLES] = {243, 234, 208, 168, 122, 76, 36, 10,
                                             1, 10, 36, 76, 122, 168, 208, 234};
    
    // Clears the module interrupt flag.
    T4OVFIF = 0;
  
    /* Writes the PWM value to the compare register and increments the
     * index. Writing to the compare register TxCC1 takes effect immediately.
     */
    T4CC1 = pwmLut[i++];

    if (i == NO_SAMPLES)
    {
        i = 0;
    }
  
    // Clears the CPU interrupt flag.
    T4IF = 0;
}

/*******************************************************************************
* @fn          main
*
* @brief       Configures P1_0 as Timer 4 output, sets up Timer 4 for centre-
*              aligned PWM operation and enables overflow interrupts. The rest
*              of the program is interrupt driven, so an infinite loop is needed
*              after the initialization.
*
* @param       void
*
* @return      0
*******************************************************************************/
int main(void)
{
    /***************************************************************************
     * Setup clock & frequency
     */
    // Set global timer tick frequency to 4 MHz
    CLKCONCMD = (CLKCONCMD & ~CLKCON_TICKSPD) | CLKCON_TICKSPD_4M;
  
  
    /***************************************************************************
     * Setup peripheral I/O for Timer 4
     */
#if (chip==2541)
    // Selects P1_1 as peripheral I/O (RED LED2).
    P1SEL |= P1SEL_SELP1_1;   
#elif (chip==2543 || chip==2545)
    // Selects P1_1 as peripheral I/O (GREEN LED1).
    P1SEL |= P1SEL_SELP1_0;   
#elif (chip==2544)
    // Select P0_2 for peripheral function Timer4 on channel 1.
    P0SEL1 |= 0x0B;
#endif

#if (chip==2541)
    //  Timer 4 has priotity over Timer 1. 
    P2SEL |= P2SEL_PRI1P1;
#elif (chip==2543)
    // Set port 1 peripheral priority to Timer 1 over USART0. 
    PPRI &= ~PPRI_PRI0P1;
#elif (chip==2545)
    // Set P1_0 Timer 4 to have priority over Timer 1.
    PPRI |= PPRI_PRI0P1; 
#endif

#if (chip==2541 || chip==2543 || chip==2545)
    // Select Timer 4 pin location as alternative 1
    PERCFG &= ~PERCFG_T4CFG;
#endif
  
    /***************************************************************************
     * Setup interrupt
     *
     * Enables global interrupts (IEN0.EA = 1) and interrupts from Timer 4
     * (IEN1.T3IE = 1).
     */
    EA = 1;
    T4IE = 1;
  
  
    /***************************************************************************
     * Timer 4 Setup 
     *
     * Timer 4 channel 1 compare control configuration. Selects the output mode
     * so that the output is set on compare-up and cleared on compare-down and
     * enables compare mode. This also disables compare interrupts.
     */
    T4CCTL1 = T4CCTLn_CMP_SET_CMP_UP_CLR_0 | T4CCTLn_MODE;
  
    /* Timer 4 channel 0 compare value. Sets the Timer 4 terminal count value
    * to 244, which makes the Timer count up to 244 before counting down to 0.
    * Hence it controls the PWM period and dynamic range.
    */
    T4CC0 = 244;
  
    /* Timer 4 channel 1 compare value. Sets the initial compare value to 156
    * This value controls the pulse width (duty cycle) and is changed at each
    * Timer overflow interrupt.
    */
    T4CC1 = 156;
  
    /* Timer 4 control. Sets the prescaler divider value to 128, starts the Timer,
    * enables overflow interrupts, clears the Timer and sets the mode to
    * up-down.
    */
    T4CTL = T4CTL_DIV_128 | T4CTL_START | T4CTL_OVFIM |
    T4CTL_CLR | T4CTL_MODE_UPDOWN;
  
    // Infinite loop.
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