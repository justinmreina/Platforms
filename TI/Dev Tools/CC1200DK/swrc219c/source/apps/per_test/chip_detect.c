//*****************************************************************************
//! @file    chip_detect.c 
//  
//! @brief  Implementation file for radio chip type detection 
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

#include "chip_detect.h"
#include "trx_rf_spi.h"

/******************************************************************************
* CONSTANTS
*/

/* Minimum definitions for reading chip ID and chip VERSION */
#define CC1101_READ_BURST               0xC0
#define CC1101_PARTNUM_ADDR             0x30
#define CC1101_VERSION_ADDR             0x31

#define CC1120_READ_BURST               0xC0
#define CC1120_EXT_MEM_ACCESS           0x2F
#define CC1120_PARTNUMBER_ADDR          0x8F
#define CC1120_PARTVERSION_ADDR         0x90



/******************************************************************************
* LOCAL FUNCTIONS
*/


/******************************************************************************   
* @fn          chipDetectRadio()                                       
*                                                                                
* @brief       This function detects if a chip is present on the EM
*              socket. SPI initialization must be done before this function  
*              can be called.  
*              Note: Currently able to detect CC1101, CC110L, CC113L, CC115L, CC2500
* 
* input parameters
*
* @param       pRadioChipType - pointer to radioChipType_t struct
*
* output parameters
*          
* @return      2 byte chip type
*/
static uint16 trxChipDetectRadio(radioChipType_t *pRadioChipType)
{
  volatile uint8 id;
  volatile uint8 ver;
  volatile uint16 type;
  
  // Relies on automatic POR   
  // Pull CSn low and wait for MISO goes low => clock ready 
  TRXEM_SPI_BEGIN();
  TRXEM_SPI_WAIT_MISO_LOW(id);                             // Wait for MISO Low
  if(id == 0) return CHIP_TYPE_NONE;                       // Return if failed 
  
  TRXEM_SPI_TX(CC1101_READ_BURST | CC1101_PARTNUM_ADDR);   // [7:6] = READ_BURST, [5:0] = part number address 
  TRXEM_SPI_WAIT_DONE();
  TRXEM_SPI_TX(0x00);
  TRXEM_SPI_WAIT_DONE();
  id = TRXEM_SPI_RX();
  
  
  TRXEM_SPI_TX(CC1101_READ_BURST | CC1101_VERSION_ADDR);    // [7:0] = ADDR 
  TRXEM_SPI_WAIT_DONE();
  TRXEM_SPI_TX(0x00);
  TRXEM_SPI_WAIT_DONE();
  ver = TRXEM_SPI_RX();
  TRXEM_SPI_END();
  
  if(id == 0x00 )
  {  
    switch(ver)
    {
    case 0x04:
      type = CHIP_TYPE_CC1101;
      break; 
    case 0x07:
      type = CHIP_TYPE_CC110L;
      break;
    case 0x08:
      type = CHIP_TYPE_CC113L;
      break;
    case 0x09:
      type = CHIP_TYPE_CC115L;
      break;
    default:
      type = CHIP_TYPE_NONE;
    }      
  }
  else if(id == 0x80 )
  {  
    switch(ver)
    {
    case 0x03:
      type = CHIP_TYPE_CC2500;
      break;
    default:
      type = CHIP_TYPE_NONE;
    }
  }
  else
  {
    type = CHIP_TYPE_NONE;
  }
  
  // Populating the global radio device struct if specific radio was detected 
  if(type != CHIP_TYPE_NONE)
  {
    pRadioChipType->id = id;
    pRadioChipType->ver = ver;
    pRadioChipType->deviceName = type;
  }
  
  return type;
}


/******************************************************************************   
* @fn          trxChipDetectCC112x()                                       
*                                                                                
* @brief       This function detects if a CC112x chip is present in the EM 
*              socket. SPI initialization must be ensured before this function 
*              can be called.  
*              
* input parameters
*
* @param       pRadioChipType - pointer to radioChipType_t struct
*          
* output parameters
*
* @return      2 byte chip type
*/
static uint16 trxChipDetectCC112x(radioChipType_t *pRadioChipType)
{
  volatile uint8 id;
  volatile uint8 ver;
  volatile uint16 type;
  
  // Pin reset 
  RF_RESET_N_PORT_SEL &= ~RF_RESET_N_PIN;
  RF_RESET_N_PORT_DIR |= RF_RESET_N_PIN;
  RF_RESET_N_PORT_OUT &= ~RF_RESET_N_PIN; // keep reset pin low 
  __delay_cycles(1000000);
  RF_RESET_N_PORT_OUT |= RF_RESET_N_PIN; // Release reset 
  
  // Pull CSn low and wait for MISO goes low => clock ready 
  TRXEM_SPI_BEGIN();
  TRXEM_SPI_WAIT_MISO_LOW(id);                             // Wait for MISO Low
  if(id == 0) return CHIP_TYPE_NONE;                       // Return if failed 
  
  TRXEM_SPI_TX(CC1120_READ_BURST | CC1120_EXT_MEM_ACCESS); // [7:6] = READ_BURST, [5:0] = extended memory access address 
  TRXEM_SPI_WAIT_DONE();
  
  TRXEM_SPI_TX(CC1120_PARTNUMBER_ADDR);                    // [7:0] = Partnumber address 
  TRXEM_SPI_WAIT_DONE();
  
  TRXEM_SPI_TX(0x00);
  TRXEM_SPI_WAIT_DONE();
  id = TRXEM_SPI_RX();
  
  TRXEM_SPI_TX(0x00);
  TRXEM_SPI_WAIT_DONE();
  ver = TRXEM_SPI_RX();
  
  TRXEM_SPI_END();
  
  switch(id)
  {
  case 0x40: //CC1121
    type = CHIP_TYPE_CC1121;
    break;
  case 0x48: // CC1120
    type = CHIP_TYPE_CC1120;
    break;
  case 0x58: //CC1125
    type = CHIP_TYPE_CC1125;
    break;
  case 0x5A: //CC1175
    type = CHIP_TYPE_CC1175;
    break;
  case 0x20://CC1200
    type = CHIP_TYPE_CC1200;
    break;
  case 0x21://CC1201
    type = CHIP_TYPE_CC1201;
    break;   
    
  default:
    type  = CHIP_TYPE_NONE;
  }
  
  // Populating the global radio device struct if specific radio was detected 
  if(type != CHIP_TYPE_NONE)
  {
    pRadioChipType->id = id;
    pRadioChipType->ver = ver;
    pRadioChipType->deviceName = type;
  }
  
  return type;
}

/******************************************************************************   
* @fn          detectChipType()                                       
*                                                                                
* @brief       This function detects if a supported radio chip is present in  
*              the EM socket.SPI init must be applied before this function   
*              can be called. 
*           
* input parameters
*      
* @param       pRadioChipType  - holds data about a detected radio chip
*        
* output parameters  
*
* @return      uint8  - 1: radio chip detected
*                       0: radio chip not detected
*/         
uint8 trxDetectChipType(radioChipType_t *pRadioChipType)
{
  uint16 chipType = CHIP_TYPE_NONE;
  chipType = trxChipDetectRadio(pRadioChipType);
  
  // Detect if CC112x is present if CC1101 isn't
  if(chipType == CHIP_TYPE_NONE)
  {
    chipType = trxChipDetectCC112x(pRadioChipType);
  }
  if(chipType == CHIP_TYPE_NONE)
  {
    // Defaulting the struct values if a radio chip is not detected 
    pRadioChipType->id  = 0x00; 
    pRadioChipType->ver = 0x00;
    pRadioChipType->deviceName =  CHIP_TYPE_NONE;
    return 0;
  }
  else
  {
    return 1;
  }
}  