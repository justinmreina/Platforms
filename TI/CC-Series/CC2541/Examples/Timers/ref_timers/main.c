/************************************************************************************************************************************/
/** @file       main.c
 *  @brief      x
 *  @details	x
 *
 *  @target     Texas Instruments CC2541F128
 *  @board      all
 *
 *  @author     Justin Reina, Firmware Engineer
 *  @created	6/22/17
 *  @last rev	6/22/17
 *
 *  @section 	BLE Stack Vocabulary
 *      ARC - Advanced Remote Control
 *      ST - Sensor Tag
 *
 *  @section    Timers
 *      There are three timers on CC2541 
 *      - Timer 1 (sec. 9)
 *      - Timer 3 (sec. 10)
 *      - Timer 4 (sec. 10)
 *
 *  @section    Timer 1 (Sec. 9)
 *      Timer 1 is an independent 16-bit timer which supports typical timer and counter functions such as input capture, output 
 *      compare, and PWM functions. The timer has five independent capture-or-compare channels
 *
 *      The features of Timer 1 are as follows:
 *       • Five capture-or-compare channels
 *       • Rising-, falling-, or any-edge input capture
 *       • Set, clear, or toggle output compare
 *       • Free-running, modulo, or up-and down counter operation
 *       • Clock prescaler for divide by 1, 8, 32, or 128
 *       • Interrupt request generated on each capture or compare and terminal count
 *       • DMA trigger function
 *
 *  @section    Timer 3 and Timer 4 (Sec. 10)
 *      Timer 3 and Timer 4 are two 8-bit timers. Each timer has two independent capture-or-compare channels, each using 
 *      one I/O pin per channel.
 *
 *      Features of Timer 3 and Timer 4 are as follows:
 *       • Two capture-or-compare channels
 *       • Set, clear, or toggle output compare
 *       • Clock prescaler for divide by 1, 2, 4, 8, 16, 32, 64, 128
 *       • Interrupt request generated on each capture or compare and terminal-count event
 *       • DMA trigger function
 *
 *  @section 	Locations of HAL reference (in prog)
 *      BLE-Stack 1.4.2.2/Components/hal/target/CC2541ST/hal_board_cfg.h
 *      BLE-Stack 1.4.2.2/Components/hal/target/CC2541ST/hal_led.c
 *
 *  @section    Manual Reference
 *      [1] SWRU191F - CC2540/41 User's Guide
 *              - Timer 1       Sec 9  (p.103)
 *              - Timer 3       Sec 10 (p.120)
 *              - Timer 4       Sec 10 (p.120)
 *
 *  @section    Code Reference
 *      [2] SWRC257 - software examples - timer34
 *              - t3_down.c     Run Timer 3 in down mode                        <- used here
 *              - t3_free.c     Run Timer 3 in free running mode
 *              - t3_modulo.c   Runs Timer 3 in modulo mode
 *              - t4_pwm.c      Runs Timer 4 in Compare mode
 *
 *      [3] SWRC257 - software examples - timer1
 *
 *  @section	Opens
 *      x
 *
 *  @section	Legal Disclaimer
 *      All contents of this source file and/or any other related source files are the explicit property of Justin Reina. Do not 
 *      distribute. Do not copy.
 */
/************************************************************************************************************************************/
#include "globals.h"


/************************************************************************************************************************************/
/** @fcn        int main(void)
 *  @brief      x
 *  @details    x
 */
/************************************************************************************************************************************/
int main(void) {
  
  //Initialize
  sys_init();
  
  //Run Demo
  timer3_demo();
}


/************************************************************************************************************************************/
/** @fcn        void timer3_demo(void)
 *  @brief      just does the ISR
 *  @details    x
 */
/************************************************************************************************************************************/
void timer3_demo(void) {
  for(;;);  
}


/************************************************************************************************************************************/
/** @fcn        void t3_isr(void)
 *  @brief      Interrupt handler for Timer 3 overflow interrupts. Toggles P1.0
 *  @details    Interrupts from Timer 3 are level triggered, so the module  interrupt flag is cleared prior to the CPU interrupt flag
 */
/************************************************************************************************************************************/
#pragma vector = T3_VECTOR
__interrupt void t3_isr(void) {
    // Clears the module interrupt flag
    T3OVFIF = 0;

    //Toggle Pin
    P1_0 ^= 1;

    // Clears the CPU interrupt flag
    T3IF = 0;
    
    return;
}


/************************************************************************************************************************************/
/** @fcn        void sys_init(void)
 *  @brief      x
 *  @details    x
 */
/************************************************************************************************************************************/
void sys_init(void) {
    
    //******************************************************************************************************************************//
    //                                                      CLOCK                                                                   //
    //******************************************************************************************************************************//
    CLKCONCMD = (CLKCONCMD & ~CLKCON_TICKSPD) | CLKCON_TICKSPD_250K;        /* Setup Clk, set tick speed to 250 kHz                 */

    //******************************************************************************************************************************//
    //                                                      GPIO                                                                    //
    //******************************************************************************************************************************//
    P1SEL &= ~BIT0;                                                         /* Select P1_0 direction to GPIO output                 */
    P1DIR |= BIT0;


    //******************************************************************************************************************************//
    //                                                   INTERRUPTS                                                                 //
    //******************************************************************************************************************************//
    EA = 1;                                                                 /* Enables global interrupts (IEN0.EA = 1) and          */
    T3IE = 1;                                                               /* interrupts from Timer 3 (IEN1.T3IE = 1)              */

    
    //******************************************************************************************************************************//
    //                                                      TIMER 3                                                                 //
    // Timer 3 control (T3CTL) configuration:                                                                                       //
    // - Prescaler divider value: 128                                                                                               //
    // - Free running mode                                                                                                          //
    // - Overflow interrupt enabled                                                                                                 //
    // - The Timer is also cleared and started                                                                                      //
    //                                                                                                                              //
    //******************************************************************************************************************************//
    T3CTL = T3CTL_DIV_128 | T3CTL_MODE_FREERUN | T3CTL_OVFIM | T3CTL_CLR | T3CTL_START;
    
    
    return;
}

