/************************************************************************************************************************************/
/** @file       x
 *  @brief      x
 *  @details    x
 *
 *  @target     x
 *  @board      x
 *
 *  @author     Justin Reina, Firmware Engineer, Misc. Company
 *  @created    x
 *  @last rev   x
 *
 *
 *  @notes      x
 *
 *  @section    Opens
 *          none current
 *
 *  @section    Legal Disclaimer
 *          All contents of this source file and/or any other Misc. Product related source files are the explicit property of
 *          Misc. Company. Do not distribute. Do not copy.
 */
/************************************************************************************************************************************/
#include "spi.h"


/************************************************************************************************************************************/
/** @fcn        void spi_transmit(uint8_t value)
 *  @brief      transmit a byte over SPI0
 *  @details    x
 *
 *  @param      [in]    (uint8_t) value - 8-bit value to transmit, LSB
 */
/************************************************************************************************************************************/
void spi_transmit(uint8_t value) {

    // Write byte to USART0 buffer (transmit data).
    U0DBUF = value;

    // Check if byte is transmitted.
    while(!(U0CSR & U0CSR_TX_BYTE));

    // Clear transmit byte status.
    U0CSR &= ~U0CSR_TX_BYTE;

    return;
}


/************************************************************************************************************************************/
/** @fcn        void spi_init(void)
 *  @brief      x
 *  @details    x
 */
/************************************************************************************************************************************/
void spi_init(void) {

    /**
     *  @brief   Set USART to SPI mode and Master mode
     *      - b7: 0 - SPI Mode
     *      - b5: 0 - Master
     */
    U0CSR = 0x00;


    /**
     * @brief   New
     *      - Set M&E (SPI_BAUD_M / SPI_BAUD_E)
     *      - Set U0GCR
     *          - Clock Polarity (CPOL:0)
     *          ? Clock Phase (CPHA:1)
     *          MSB Order (ORDER:1)
     *          BAUD_E
     */
    U0BAUD = SPI_BAUD_M;                                                    /* apply Mantissa [7:0]                                 */
    U0GCR =  U0GCR_ORDER | SPI_BAUD_E;
    
    //Set SSN
    P0_4 = 1;

    return;
}


/************************************************************************************************************************************/
/** @fcn        void spi_gpio_init(void)
 *  @brief      x
 *  @details    x
 */
/************************************************************************************************************************************/
void spi_gpio_init(void) {
 
    // Configure USART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0).
    PERCFG = (PERCFG & ~PERCFG_U0CFG) | PERCFG_U0CFG_ALT1;

    // Give priority to USART 0 over Timer 1 for port 0 pins.
    P2DIR &= P2DIR_PRIP0_USART0;

    // Set pins 2, 3 and 5 as peripheral I/O and pin 4 as GPIO output.
    P0SEL = (P0SEL & ~BIT4) | BIT5 | BIT3 | BIT2;
    P0DIR |= BIT4;

    return;
}
