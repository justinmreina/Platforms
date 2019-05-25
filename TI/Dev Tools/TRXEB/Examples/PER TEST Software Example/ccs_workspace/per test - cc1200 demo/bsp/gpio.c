#include "gpio.h"


void gpio_init(void) {

    // Initialize LEDs as off (pins as GPIO output high)
    P4SEL &= ~BSP_LED_ALL;
    P4OUT |=  BSP_LED_ALL;
    P4DIR |=  BSP_LED_ALL;

    // RF SPI0 CS as GPIO output high
    P3SEL &= ~BIT0;
    P3OUT |=  BIT0;
    P3DIR |=  BIT0;

    //******************************************************************************************************************************//
    //                                                  INIT DBG GPIO                                                               //
    //******************************************************************************************************************************//
    P4OUT &= ~BIT0;                                                         /* LED0 On                                              */
    P4DIR |= BIT0;
    P4SEL &= ~BIT0;

    P4OUT &= ~(BIT5|BIT6|BIT7);                                             /* Debug Pins                                           */
    P4DIR |= (BIT5|BIT6|BIT7);
    P4SEL &= ~(BIT5|BIT6|BIT7);

    P4OUT ^= (BIT5 | BIT0 | BIT6 | BIT7);                                   /* test                                                 */
    P4OUT ^= (BIT5 | BIT0 | BIT6 | BIT7);
    P4OUT ^= (BIT5 | BIT0 | BIT6 | BIT7);
    P4OUT ^= (BIT5 | BIT0 | BIT6 | BIT7);

    //exit
    P4OUT |= BIT0;                                                          /* leave as off                                         */
    P4OUT &= ~(BIT5|BIT6|BIT7);

    return;
}

