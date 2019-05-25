//-----------------------------------------------------------------------------
// \file    evmomapl138_aic3106.c
// \brief   implementation of a aic3106 driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_aic3106.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// i2c defines.
#define I2C_PORT_AIC3106         (I2C0)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// /brief Initialize the audio codec 
// 
// /param none
//
// /return uint32_t ERR_NO_ERROR on sucess
//
//-----------------------------------------------------------------------------
uint32_t AIC3106_init(void)
{
   // select page 0 and reset codec.
   AIC3106_writeRegister(AIC3106_REG_PAGESELECT, 0);
   AIC3106_writeRegister(AIC3106_REG_RESET, 0x80);

   // config codec regs. please see AIC3106 documentation for explination. 
   // Document Num: TLV320AIC3106
   AIC3106_writeRegister(3, 0x22);
   AIC3106_writeRegister(4, 0x20);
   AIC3106_writeRegister(5, 0x6E);
   AIC3106_writeRegister(6, 0x23);
   AIC3106_writeRegister(7, 0x0A);
   AIC3106_writeRegister(8, 0x00);
   AIC3106_writeRegister(9, 0x00);
   AIC3106_writeRegister(10, 0x00);
   AIC3106_writeRegister(15, 0);
   AIC3106_writeRegister(16, 0);
   AIC3106_writeRegister(19, 0x04);
   AIC3106_writeRegister(22, 0x04);
   AIC3106_writeRegister(27, 0);
   AIC3106_writeRegister(30, 0);
   AIC3106_writeRegister(37, 0xE0);
   AIC3106_writeRegister(38, 0x10);
   AIC3106_writeRegister(43, 10);
   AIC3106_writeRegister(44, 10);
   AIC3106_writeRegister(58, 0);
   AIC3106_writeRegister(72, 0);
   AIC3106_writeRegister(82, 0x80);
   AIC3106_writeRegister(86, 0x09);
   AIC3106_writeRegister(92, 0x80);
   AIC3106_writeRegister(93, 0x09);
   AIC3106_writeRegister(101, 0x01);
   AIC3106_writeRegister(102, 0);	
   AIC3106_writeRegister(43, 0x28);			//turn down the L DAC gain
   AIC3106_writeRegister(44, 0x28);			//turn down the R DAC gain

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// /brief Read data from a register on the AIC3106.
// 
// /param uint8_t in_reg_addr: The address of the register to be read from.
//
// /param uint8_t * dest_buffer: Pointer to buffer to store retrieved data.
//
// /return uint32_t ERR_NO_ERROR on sucess
//
//-----------------------------------------------------------------------------
uint32_t AIC3106_readRegister(uint8_t in_reg_addr, uint8_t *dest_buffer)
{
   uint32_t rtn;

   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_AIC3106, I2C_ADDR_AIC3106, &in_reg_addr, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the register data.
   rtn = I2C_read(I2C_PORT_AIC3106, I2C_ADDR_AIC3106, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// /brief Write a register on the AIC3106.
// 
// /param uint8_t in_reg_addr: The address of the register to be written to.
//
// /param uint8_t data: Data to be written to the register
//
// /return uint32_t ERR_NO_ERROR on sucess
//
//-----------------------------------------------------------------------------
uint32_t AIC3106_writeRegister(uint8_t in_reg_addr, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];
   
   i2c_data[0] = in_reg_addr;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_AIC3106, I2C_ADDR_AIC3106, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
