
/***********************************************************************************
  Filename:     spi_master_receive.c

  Description:  This example uses a master to receive data from a slave using SPI.

  Comments:     To execute this example, use the IAR project spi_slave_send as
                the slave. The slave's code must be executed before executing
                the master's code, since the slave is clocked by the master.
                When bytes up to BUFFER_SIZE are received, the example will end
                and LED1 will be set on SmartRF05EB. You can debug the
                rxBufferMaster array to verify that the bytes received are the
                correct ones and are in correct order.

Configuration for each module:

CC2541EM, CC2543EM, CC2545EM: 
                Connect from each board:
                 - MISO:  P0_2  (PIN9 on Debug Connector P18)
                 - MOSI:  P0_3  (PIN11 on Debug Connector P18)
                 - SSN:   P0_4  (PIN13 on Debug Connector P18)
                 - SCK:   P0_5  (PIN15 on Debug Connector P18)
                 - GND:         (PIN20 on Debug Connector P18)

CC2544Dongle:
                No configuration for Dongle in this example. 
                The code illsutrate how to set up the interface for CC2544.
                There is not a CC2544 HW plattform available for running 
                this excact code.

***********************************************************************************/

/***********************************************************************************
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
#warning "The CC2544Dongle is not supported for this software example."
#warning "This code illustrate how to set up the SPI interface on the CC2544 device."
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/***********************************************************************************
* CONSTANTS
*/

// These values will give a baud rate of approx. 2.00 Mbps at 32 MHz system clock.
#define SPI_BAUD_M  0
#define SPI_BAUD_E  16

// Dummy byte used to receive data from slave.
static const uint8 dummyByte = 0x00;

// Define size of buffer.
#define BUFFER_SIZE 252

/***********************************************************************************
* LOCAL VARIABLES
*/
// Master's transmit buffer.
static uint8 rxBufferMaster[BUFFER_SIZE];

/***********************************************************************************
* LOCAL FUNCTIONS
*/


/***********************************************************************************
* @fn          main
*
* @brief       Receive data using SPI in master mode
*
* @param       void
*
* @return      void
*/

void main(void)
{
    /****************************************************************************
     * Clock setup
     * See basic software example "clk_xosc_cc254x"
     */
    
    // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    // Wait until clock source has changed.
    while (CLKCONSTA & CLKCON_OSC);
    
    // Note the 32 kHz RCOSC starts calibrating, if not disabled.
    
    
    /***************************************************************************
     * Setup I/O ports
     *
     * Port and pins used USART0 operating in SPI-mode are
     * MISO (MI): P0_2
     * MOSI (MO): P0_3
     * SSN (SS) : P0_4
     * SCK (C)  : P0_5
     *
     * These pins can be set to function as peripheral I/O so that they
     * can be used by USART0 SPI. Note however, when SPI is in master mode,
     * only MOSI, MISO, and SCK should be configured as peripheral I/O's.
     * If the external slave device requires a slave select signal (SSN),
     * this can be implemented by using general-purpose I/O pin configured as
     * output.
     */


#if (chip==2541 || chip==2543 || chip==2545)
    // Configure USART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0).
    PERCFG = (PERCFG & ~PERCFG_U0CFG) | PERCFG_U0CFG_ALT1;    
#endif
        
#if (chip==2541)
    // Give priority to USART 0 over Timer 1 for port 0 pins.
    P2DIR &= P2DIR_PRIP0_USART0;
#elif (chip==2543 || chip==2545)
    // Give priority to USART 0 over Timer 1 for port 0 pins.
    PPRI &= ~PPRI_PRI0P0;
#endif


#if (chip==2541 || chip==2543 || chip==2545)
    // Set pins 2, 3 and 5 as peripheral I/O and pin 4 as GPIO output.
    P0SEL = (P0SEL & ~BIT4) | BIT5 | BIT3 | BIT2;
    P0DIR |= BIT4;
#elif (chip==2544)
    // Set pins 1, 2 and 3 as peripheral I/O and pin 0 as GPIO output.
    P0SEL0 = 0x10;        // Map P0_1 as UASRT0 SPI and P0.0 as GPIO. 
    P0SEL1 = 0x11;        // Map P0_3 and P0_2 as UASRT0 SPI. 
    PDIR |= PDIR_DIRP1_0; // Set P1.0 as output. 
#endif
    
    
#if (chip==2541 || chip==2543 || chip==2545)
    // Configure P1_0 as GPIO output for LED1.
    P1SEL &= BIT0;      // GPIO.
    P1DIR |= BIT0;      // Output.
    P1_0 = 0;           // LED1 off.
#endif


    /***************************************************************************
     * Configure SPI
     */

    // Set USART to SPI mode and Master mode.
    U0CSR &= ~(U0CSR_MODE | U0CSR_SLAVE);

    // Set:
    // - mantissa value
    // - exponent value
    // - clock phase to be centered on first edge of SCK period
    // - negative clock polarity (SCK low when idle)
    // - bit order for transfers to LSB first
    U0BAUD =  SPI_BAUD_M;
    U0GCR = (U0GCR & ~(U0GCR_BAUD_E | U0GCR_CPOL | U0GCR_CPHA | U0GCR_ORDER))
        | SPI_BAUD_E;

    
    /***************************************************************************
     * Transfer data
     */

#if (chip==2541 || chip==2543 || chip==2545)
    // Clear SSN, the SPI slave is active when SSN is low.
    P0_4 = 0;
#elif (chip==2544)
    // Clear SSN, the SPI slave is active when SSN is low.
    P1_0 = 0;
#endif
    
    int i;
    for (i = 0; i< BUFFER_SIZE; i++)
    {
        // Write dummy byte to USART0 buffer (transmit data).
        U0DBUF = dummyByte;

        // Check if byte is transmitted (and a byte is recieved).
        while(!(U0CSR & U0CSR_TX_BYTE));

         // Clear transmit byte status.
        U0CSR &= ~U0CSR_TX_BYTE;

        // Write received byte to buffer.
        rxBufferMaster[i] = U0DBUF;
    }

#if (chip==2541 || chip==2543 || chip==2545)
    // Set SSN, the SPI slave is inactive when SSN is high.
    P0_4 = 1;
#elif (chip==2544)
    // Set SSN, the SPI slave is inactive when SSN is high.
    P1_0 = 1;
#endif

#if (chip==2541 || chip==2543 || chip==2545)
    // When finished receiving, set LED1.
    P1_0 = 1;   // SRF05EB LED1 on.
#endif

    // end function with infinite loop (for debugging purposes). 
    while(1);
}


/***********************************************************************************
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
***********************************************************************************/