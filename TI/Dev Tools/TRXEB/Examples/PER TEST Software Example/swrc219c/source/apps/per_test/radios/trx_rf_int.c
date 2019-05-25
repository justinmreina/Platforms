//*****************************************************************************
//! @file trx_rf_int.c  
//    
//! @brief  Implementation file for radio interrupt interface 
//          functions on Port 1, pin 7. The ISR is defined elsewhere
//          and connected to the interrupt vector real time. 
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/


/******************************************************************************
 * INCLUDES
 */
#include <msp430.h>
#include "hal_types.h"
#include "hal_defs.h"
#include "trx_rf_int.h"
#include "trx_rf_spi.h"
#include "hal_digio2.h"

/******************************************************************************
* CONSTANTS
*/

/* Interrupt port and pin */
#define TRXEM_INT_PORT 1
#define TRXEM_INT_PIN  7
#define TRXEM_INT_PORT_IN P1IN

/******************************************************************************
 * FUNCTIONS
 */
 
/*******************************************************************************
 * @fn          trxIsrConnect
 *
 * @brief       Connects an ISR function to PORT1 interrupt vector and 
 *              configures the interrupt to be a high-low transition. 
 * 
 * input parameters
 *
 * @param       pF  - function pointer to ISR
 *
 * output parameters
 *
 * @return      void
 */ 
void trxIsrConnect(ISR_FUNC_PTR pF)
{
  uint8 pin_bitmask;
  digio io;
  io.port = TRXEM_INT_PORT; 
  io.pin  = TRXEM_INT_PIN;
  /* Assigning ISR function */
  halDigio2IntConnect(io, pF);

  /* Setting high-> low interrupt */
  pin_bitmask = 1<<io.pin;
  P1IES |= pin_bitmask;
  return;
}

/*******************************************************************************
 * @fn          trxClearIntFlag
 *
 * @brief       Clears sync interrupt flag
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */
void trxClearIntFlag(void)
{
  digio io;
  io.port = TRXEM_INT_PORT; 
  io.pin  = TRXEM_INT_PIN;
  halDigio2IntClear(io);
  return;
}

/*******************************************************************************
 * @fn          trxEnableInt
 *
 * @brief       Enables sync interrupt 
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */ 
void trxEnableInt(void)
{
  digio io;
  io.port = TRXEM_INT_PORT; 
  io.pin  = TRXEM_INT_PIN;
  halDigio2IntEnable(io);
  return;
}

/*******************************************************************************
 * @fn          trxDisableInt
 *
 * @brief       Disables sync interrupt
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @return      void
 */ 
void trxDisableInt(void)
{
  digio io;
  io.port = TRXEM_INT_PORT; 
  io.pin  = TRXEM_INT_PIN;
  halDigio2IntDisable(io);
  return;
}

/******************************************************************************
 * @fn          trxSampleSyncPin
 *
 * @brief       Reads the value of the sync pin. 
 *                 
 * input parameters
 *   
 * @param       none
 *
 * output parameters
 *
 * @return      uint8
 */
uint8 trxSampleSyncPin(void)
{
  return ((TRXEM_INT_PORT_IN & (0x01<<TRXEM_INT_PIN))>>TRXEM_INT_PIN);
}
