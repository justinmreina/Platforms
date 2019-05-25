/******************************************************************************
    Filename: hal_spi_rf_trxeb.c

    Description: Implementation file for common spi access with the CCxxxx
                 tranceiver radios using trxeb. Supports CC1101/CC112X radios

*******************************************************************************/


/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
#include "hal_defs.h"
#include "hal_spi_rf_trxeb.h"
#include "hal_digio2.h"



/******************************************************************************
 * LOCAL FUNCTIONS
 */
static void trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len) ;


/******************************************************************************
 * FUNCTIONS
 */

/******************************************************************************
 * @fn          trxRfSpiInterfaceInit
 *
 * @brief       Function to initialize TRX SPI. CC1101/CC112x is currently
 *              supported. The supported prescalerValue must be set so that
 *              SMCLK/prescalerValue does not violate radio SPI constraints.
 *
 * input parameters
 *
 * @param       prescalerValue - SMCLK/prescalerValue gives SCLK frequency
 *
 * output parameters
 *
 * @return      void
 */
void trxRfSpiInterfaceInit(uint8 prescalerValue)
{

  /* Keep peripheral in reset state*/
  UCB0CTL1 |= UCSWRST;

  /* Configuration
   * -  8-bit
   * -  Master Mode
   * -  3-pin
   * -  synchronous mode
   * -  MSB first
   * -  Clock phase select = captured on first edge
   * -  Inactive state is low
   * -  SMCLK as clock source
   * -  Spi clk is adjusted corresponding to systemClock as the highest rate
   *    supported by the supported radios: this could be optimized and done
   *    after chip detect.
   */
  UCB0CTL0  =  0x00+UCMST + UCSYNC + UCMODE_0 + UCMSB + UCCKPH;
  UCB0CTL1 |=  UCSSEL_2;
  UCB0BR1   =  0x00;

  UCB0BR0 = prescalerValue;

  ///* Configuring UCB0BR0
  // * Set up spi clk to comply with the maximum spi clk speed for the radios
  // * according to userguides. Takes the slowest spi clk into account.
  // */
  //switch(systemClock)
  //{
  //  case 0:
  //    /* Do not divide SMCLK */
  //    UCB0BR0 = 0x01;
  //    break;
  //  case 1:
  //    /* Do not divide SMCLK */
  //    UCB0BR0 = 0x01;
  //    break;
  //  case 2:
  //    /* Divide SMCLK by 2*/
  //    UCB0BR0 = 0x02;
  //    break;
  //  case 3:
  //    /* Divide SMCLK by 2*/
  //    UCB0BR0 = 0x02;
  //    break;
  //  case 4:
  //    /* Divide SMCLK by 3*/
  //    UCB0BR0 = 0x03;
  //    break;
  //  case 5:
  //    /* Divide SMCLK by 4*/
  //    UCB0BR0 = 0x04;
  //    break;
  //  case 6:
  //    /* Divide SMCLK by 4*/
  //    UCB0BR0 = 0x04;
  //    break;
  //  default:
  //    /* Divide SMCLK by 4*/
  //    UCB0BR0 = 0x04;
  //    break;
  //}

  /* Configure port and pins
   * - MISO/MOSI/SCLK GPIO controlled by peripheral
   * - CS_n GPIO controlled manually, set to 1
   */
  TRXEM_PORT_SEL |= TRXEM_SPI_MOSI_PIN + TRXEM_SPI_MISO_PIN + TRXEM_SPI_SCLK_PIN;
  TRXEM_PORT_SEL &= ~TRXEM_SPI_SC_N_PIN;
  TRXEM_PORT_OUT |= TRXEM_SPI_SC_N_PIN + TRXEM_SPI_MISO_PIN;/* Pullup on MISO */


  TRXEM_PORT_DIR |= TRXEM_SPI_SC_N_PIN;
  /* In case not automatically set */
  TRXEM_PORT_DIR |= TRXEM_SPI_MOSI_PIN + TRXEM_SPI_SCLK_PIN;
  TRXEM_PORT_DIR &= ~TRXEM_SPI_MISO_PIN;

  /* Release for operation */
  UCB0CTL1 &= ~UCSWRST;
  return;
}


/*******************************************************************************
 * @fn          trx8BitRegAccess
 *
 * @brief       This function performs a read or write from/to a 8bit register
 *              address space. The function handles burst and single read/write
 *              as specfied in addrByte. Function assumes that chip is ready.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       addrByte - address byte of register.
 * @param       pData    - data array
 * @param       len      - Length of array to be read(TX)/written(RX)
 *
 * output parameters
 *
 * @return      chip status
 */
rfStatus_t trx8BitRegAccess(uint8 accessType, uint8 addrByte, uint8 *pData, uint16 len)
{
  uint8 readValue;

  /* Pull CS_N low and wait for SO to go low before communication starts */
  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  /* send register address byte */
  TRXEM_SPI_TX(accessType|addrByte);
  TRXEM_SPI_WAIT_DONE();
  /* Storing chip status */
  readValue = TRXEM_SPI_RX();
  trxReadWriteBurstSingle(accessType|addrByte,pData,len);
  TRXEM_SPI_END();
  /* return the status byte value */
  return(readValue);
}

/******************************************************************************
 * @fn          trx16BitRegAccess
 *
 * @brief       This function performs a read or write in the extended adress
 *              space of CC112X.
 *
 * input parameters
 *
 * @param       accessType - Specifies if this is a read or write and if it's
 *                           a single or burst access. Bitmask made up of
 *                           RADIO_BURST_ACCESS/RADIO_SINGLE_ACCESS/
 *                           RADIO_WRITE_ACCESS/RADIO_READ_ACCESS.
 * @param       extAddr - Extended register space address = 0x2F.
 * @param       regAddr - Register address in the extended address space.
 * @param       *pData  - Pointer to data array for communication
 * @param       len     - Length of bytes to be read/written from/to radio
 *
 * output parameters
 *
 * @return      rfStatus_t
 */
rfStatus_t trx16BitRegAccess(uint8 accessType, uint8 extAddr, uint8 regAddr, uint8 *pData, uint8 len)
{
  uint8 readValue;

  TRXEM_SPI_BEGIN();
  while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
  /* send extended address byte with access type bits set */
  TRXEM_SPI_TX(accessType|extAddr);
  TRXEM_SPI_WAIT_DONE();
  /* Storing chip status */
  readValue = TRXEM_SPI_RX();
  TRXEM_SPI_TX(regAddr);
  TRXEM_SPI_WAIT_DONE();
  /* Communicate len number of bytes */
  trxReadWriteBurstSingle(accessType|extAddr,pData,len);
  TRXEM_SPI_END();
  /* return the status byte value */
  return(readValue);
}

/*******************************************************************************
 * @fn          trxSpiCmdStrobe
 *
 * @brief       Send command strobe to the radio. Returns status byte read
 *              during transfer of command strobe. Validation of provided
 *              is not done. Function assumes chip is ready.
 *
 * input parameters
 *
 * @param       cmd - command strobe
 *
 * output parameters
 *
 * @return      status byte
 */
rfStatus_t trxSpiCmdStrobe(uint8 cmd)
{
    uint8 rc;
    TRXEM_SPI_BEGIN();
    while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
    TRXEM_SPI_TX(cmd);
    TRXEM_SPI_WAIT_DONE();
    rc = TRXEM_SPI_RX();
    TRXEM_SPI_END();
    return(rc);
}

/*******************************************************************************
 * @fn          trxReadWriteBurstSingle
 *
 * @brief       When the address byte is sent to the SPI slave, the next byte
 *              communicated is the data to be written or read. The address
 *              byte that holds information about read/write -and single/
 *              burst-access is provided to this function.
 *
 *              Depending on these two bits this function will write len bytes to
 *              the radio in burst mode or read len bytes from the radio in burst
 *              mode if the burst bit is set. If the burst bit is not set, only
 *              one data byte is communicated.
 *
 *              NOTE: This function is used in the following way:
 *
 *              TRXEM_SPI_BEGIN();
 *              while(TRXEM_PORT_IN & TRXEM_SPI_MISO_PIN);
 *              ...[Depending on type of register access]
 *              trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len);
 *              TRXEM_SPI_END();
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
static void trxReadWriteBurstSingle(uint8 addr,uint8 *pData,uint16 len)
{
	uint16 i;
	/* Communicate len number of bytes: if RX - the procedure sends 0x00 to push bytes from slave*/
  if(addr&RADIO_READ_ACCESS)
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      for (i = 0; i < len; i++)
      {
          TRXEM_SPI_TX(0);            /* Possible to combining read and write as one access type */
          TRXEM_SPI_WAIT_DONE();
          *pData = TRXEM_SPI_RX();     /* Store pData from last pData RX */
          pData++;
      }
    }
    else
    {
      TRXEM_SPI_TX(0);
      TRXEM_SPI_WAIT_DONE();
      *pData = TRXEM_SPI_RX();
    }
  }
  else
  {
    if(addr&RADIO_BURST_ACCESS)
    {
      /* Communicate len number of bytes: if TX - the procedure doesn't overwrite pData */
      for (i = 0; i < len; i++)
      {
        TRXEM_SPI_TX(*pData);
        TRXEM_SPI_WAIT_DONE();
        pData++;
      }
    }
    else
    {
      TRXEM_SPI_TX(*pData);
      TRXEM_SPI_WAIT_DONE();
    }
  }
  return;
}

/******************************************************************************
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
*******************************************************************************/