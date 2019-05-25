
/***********************************************************************************
  Filename:     i2c_slave_send.c

  Description:  This example uses a slave to send data using I2C.

  Comments:     To execute this example, use the IAR project i2c_master_receive
                as the master. The slave's code must be executed before executing
                the master's code, since the slave is clocked by the Master.
                When a stop condition received, the LED1 will be set on the
                SmartRF05EB and the buffer index will be reset.

  Note:         On the SmartRF05EB, P0_6 and P0_7 is shared by the 
                EM_JOY_LEVEL and EM_LCD_CS signals. These have to be disconnected 
                by removing jumpers 7-8 and 3-4 on P10 I/O Connector. Thus the I2C 
                and SRF05EB LCD can't be used simultaneously.

                The I2C pins also have external pullups on the EM, at the as 
                required by the I2C standard.

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
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/***********************************************************************************
* CONSTANTS
*/

// Define size of buffer and number of bytes to send.
#define BUFFER_SIZE 0xFF
#define SLAVE_ADDRESS 0x53    // 7-bit addressing

#define READ_FROM_SLAVE 0x01
#define WRITE_TO_SLAVE 0x00


/***********************************************************************************
* LOCAL VARIABLES
*/

static uint8 buffer[BUFFER_SIZE];
static uint8 bufferIndex = 0;


/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn          I2C_ISR
*
* @brief       Function which sends the next I2C data byte.
*
* @param       none
*
* @return      0
*/
#pragma vector = I2C_VECTOR
__interrupt void I2C_ISR(void)
{ 
#if(chip==2541)
    // Clear I2C CPU interrupt flag
    P2IF = 0;
#elif(chip==2543 || chip==2545)
    // Clear I2C CPU interrupt flag
    I2CIF = 0;
#endif
 
    // If own SLA+R has been received or a Data byte has been transmitted,
    // and ACK has been returned ...  
    if (I2CSTAT == 0xA8 || I2CSTAT == 0xB8)
    { 
        // Load Data byte and increment index.
        I2CDATA = buffer[bufferIndex++];
    } 

    // If a Stop condition is detected ...
    if (I2CSTAT == 0xA0)
    {    
        // Reset buffer index.
        bufferIndex = 0;
    
        // Set SRF05EB LED1.
        P1_0 = 1;   
    }
  
    // I2CCFG.SI flag must be cleared by software at the end of the ISR.
    I2CCFG &= ~I2CCFG_SI;
}


/***********************************************************************************
* @fn          main
*
* @brief       Receive data from a Master using I2C in Slave mode
*
* @param       void
*
* @return      0
*/
int main(void)
{
    /****************************************************************************
     * Clock setup
     * See basic software example "clk_xosc_cc254x"
     */
  
    // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    while (CLKCONSTA & CLKCON_OSC);   // Wait until clock source has changed
  
    // Note the 32 kHz RCOSC starts calibrating, if not disabled.
    
#if (chip==2543)
    /***************************************************************************
     * Setup I/O ports
     *
     * Port and pins used by I2C, at the alternative 1 location:
     * I2C SCL:   P0_6    (Debug Connector P18_5)
     * I2C SDA:   P0_7    (Debug Connector P18_3)
     */

    // Configure I2C for location Alternative 1.
    PERCFG &= ~PERCFG_I2CCFG;
  
    // Give priority to I2C over Timer 1 for port 0 pins.
    PPRI &= ~PPRI_PRI1P0;

    // Set P0_6 and P0_7 as peripheral I/O.
    P0SEL |= BIT7 | BIT6;

#elif(chip==2545)
    /***************************************************************************
    * Setup I/O ports
    *
    * Port and pins used by I2C, at the alternative 2 location:
    * I2C SCL:   P2_5    (Debug Connector P18_5)
    * I2C SDA:   P2_6    (Debug Connector P18_3)
    */

    // Configure I2C for location Alternative 2.
    PERCFG |= PERCFG_I2CCFG;
   
    // Give priority to I2C over USART0, then Timer3.
    PPRI = (PPRI & ~PPRI_PRI1P1) | PPRI_PRI1P1_I2C;

    // Set P2_5 and P2_6 as peripheral I/O.
    P2SEL |= BIT6 | BIT5;
  
#elif(chip==2541)
    /***************************************************************************
    * Setup I/O ports
    *
    * CC2541 has dedicated I2C ports
    * I2C SCL:   Pin 2   (Debug Connector P18_5)
    * I2C SDA:   Pin 3   (Debug Connector P18_3)
    */
  
    // Enable I2C on CC2541.
    I2CWC &= ~0x80;
#endif

    // Configure P1_0 as GPIO output for LED1.
    P1SEL &= BIT0;      // GPIO.
    P1DIR |= BIT0;      // Output.
    P1_0 = 0;           // LED1 off.
  
  
    /***************************************************************************
    * Setup interrupt
    */

#if(chip==2541)
    // Clear I2C CPU interrupt flag.
    P2IF = 0;

    // Enable interrupt from I2C by setting [IEN2.I2CIE = 1].
    IEN2 |= IEN2_P2IE;
#else
    // Clear I2C CPU interrupt flag.
    I2CIF = 0;

    // Enable interrupt from I2C by setting [IEN2.I2CIE = 1].
    IEN2 |= IEN2_I2CIE;
#endif

    // Enable global interrupts.
    EA = 1;
  
  
    /***************************************************************************
    * Configure I2C
    */
  
    // Fill the buffer with values 0x00++.
    uint8 value = 0x00;
    uint8 i;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = value++;
    }
  
    // Set device address, with General-call address acknowledge.
    I2CADDR = (SLAVE_ADDRESS << 1) | I2CADDR_GC;
  
    // Enable the I2C module, the slave is clocked by the Master.
    // Enable Assert Acknowledge (AA bit).
    I2CCFG |= I2CCFG_ENS1 | I2CCFG_AA;
  
  
    /* Main Loop, the transfers are interrupt handled. */
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

