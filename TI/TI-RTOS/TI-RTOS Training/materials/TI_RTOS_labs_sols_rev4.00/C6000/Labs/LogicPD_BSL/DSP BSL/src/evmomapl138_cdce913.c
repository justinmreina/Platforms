//-----------------------------------------------------------------------------
// \file    evmomapl138_cdce913.c
// \brief   implementation of a TI cdce913 clock driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_cdce913.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// i2c defines.
#define I2C_PORT_CDCE913         (I2C0)
#define BYTE_READ_WRITE_BIT      (0x80)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the clock synthesizer for use.
//          Y1 (camera) = 27MHz
//          Y2 (ADC) = 	user selectable
//          Y3 (VIDEO) =  27MHz
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...clock is in default configuration.
//    ERR_INIT_FAIL - something happened during initialization.
//	  ERR_NO_UI_BOARD - the UI can not be found
//-----------------------------------------------------------------------------
uint32_t CDCE913_init(void)
{
   // make sure the ui board is connected before proceeding.
   if (!UTIL_isUIBoardAttached())
      return (ERR_NO_UI_BOARD);

   CDCE913_writeByte(0x14, 0xED);
   CDCE913_writeByte(0x18, 0x00);
   CDCE913_writeByte(0x19, 0x40);
   CDCE913_writeByte(0x1A, 0x02);
   CDCE913_writeByte(0x1B, 0x08);
  
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief	Read a register from the CDCE913.
//
// \param   uint8_t in_offset: address of register to be read
//
// \param 	uint8_t * dest_buffer: buffer for retrieved data t obe written to 
 
// \return  uint32_t
//    ERR_NO_ERROR - register read correctly.
//
//-----------------------------------------------------------------------------
uint32_t CDCE913_readByte(uint8_t in_offset, uint8_t *dest_buffer)
{
   uint32_t rtn;
   
   // set bit to indicate this is a byte read.
   SETBIT(in_offset, BYTE_READ_WRITE_BIT);

   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_CDCE913, I2C_ADDR_CDCE913, &in_offset, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the register data.
   rtn = I2C_read(I2C_PORT_CDCE913, I2C_ADDR_CDCE913, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief	Write a register to the CDCE913.
//
// \param   uint8_t in_offset: address of register to be written to
//
// \param 	uint8_t * data: data to be written to register 
 
// \return  uint32_t
//    ERR_NO_ERROR - register written to correctly.
//
//-----------------------------------------------------------------------------
uint32_t CDCE913_writeByte(uint8_t in_offset, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];
   
   // set bit to indicate this is a byte write.
   i2c_data[0] = in_offset | BYTE_READ_WRITE_BIT;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_CDCE913, I2C_ADDR_CDCE913, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief	Set an output divider.
//
// \param   cdce913_output_e output: output divider to set
//
// \param 	uint16_t divide_by: value to divide by. Note that divider 1 is 10 bits and 
//				dividers 2/3 are 7 bits. The value of divide_by will be truncated accordingly  
//
// \return  uint32_t
//    ERR_NO_ERROR - output set correctly.
//	  ERR_FAIL	 - invalid output specified.
//
//-----------------------------------------------------------------------------
uint32_t CDCE913_setOutput(cdce913_output_e output, uint16_t divide_by)
{
	uint32_t retVal = 0;
	uint8_t regVal = 0; 
	
	switch(output)
	{
		case cdce913_output_1: 
				//the upper 5 bits of PDIV_1H are shared we need to make sure not to change 
				//the value of these bits when we write to the register.
				CDCE913_readByte(PDIV_1H, &regVal);
				regVal &= 0xF8;								//keep upper 5 bits of current value
				regVal |= (uint8_t)((divide_by>>8) & 0x07); 	//use lower 3 bits of msB of divide_by
				CDCE913_writeByte(PDIV_1H, regVal);
				//now write the lower byte
				regVal = (uint8_t)divide_by;
				CDCE913_writeByte(PDIV_1L, regVal);
				break;
				
		case cdce913_output_2:
				CDCE913_readByte(PDIV_2, &regVal);
				regVal &= 0x80; 							//keep bit 7
				regVal |= (uint8_t)(divide_by & 0x7F);		
				CDCE913_writeByte(PDIV_2, regVal);		
				break;
				
		case cdce913_output_3:
				regVal = (uint8_t)(divide_by & 0x7F);		//bit 7 reserved, allways write 0
				CDCE913_writeByte(PDIV_3, regVal);
				break;
				
		default:
			return ERR_FAIL;
		
	}
	return retVal;
}
