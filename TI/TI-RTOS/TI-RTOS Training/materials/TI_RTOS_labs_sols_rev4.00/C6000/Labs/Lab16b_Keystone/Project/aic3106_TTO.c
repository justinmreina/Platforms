//-----------------------------------------------------------------------------
// Title:   Technical Training Organization (TTO) SYS/BIOS 1.5-DAY Workshop 0.80
// File:	aic3106_TTO.c 
// Rev:		0.80
// Date:	07-25-2011
// Author:	Eric Wilbur (ericw@ti.com)
// Ref:		C6748 BSL, Experimenter Test Example (audio, mcasp)
//
// Brief:	Definitions, prototypes and externs
//
// Notes:	1. Refer to main.h for key definitions
//-----------------------------------------------------------------------------
#include "main.h"


//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

// i2c defines.
#define I2C_PORT_AIC3106         (I2C0)
#define FS_48KHZ
//#define FS_8KHZ
//#define FS_10KHz


//-----------------------------------------------------------------------------
// AIC3106_Init_TTO()
//-----------------------------------------------------------------------------

void AIC3106_Init_TTO(void)
{
   // select page 0 and reset codec.
   AIC3106_writeRegister(AIC3106_REG_PAGESELECT, 0);
   AIC3106_writeRegister(AIC3106_REG_RESET, 0x80);

   // config codec regs. please see AIC3106 documentation for explanation. 
   // Document Num: TLV320AIC3106

   #ifdef FS_48KHZ		// 48kHz sampling rate
	   AIC3106_writeRegister(3, 
				(0 << 7) |	// PLL is disabled
				(4 << 3) |	// PLL Q value = 4
				(2 << 0));	// PLL P value = 2
				//0x22);	// PLL disabled, Q=4, P=2
	   AIC3106_writeRegister(2, 
				(0 << 4) | 	// ADC fs = fs(ref)/1
				(0 << 0));	// DAC fs = fs(ref)/1
				//0x00);	//ADC/DAC sample rate = f(s)/1 = MCLK/(128*Q)/1
   #endif

   #ifdef FS_8KHz
	   AIC3106_writeRegister(3, 
				(0 << 7) |	// PLL is disabled
				(4 << 3) |	// PLL Q value = 4
				(2 << 0));	// PLL P value = 2
	   //AIC3106_writeRegister(3, 0x22);	// PLL disabled, Q=4, P=2

	   AIC3106_writeRegister(2, 
				(10 << 4) | 	// ADC fs = fs(ref)/6
				(10 << 0));	// DAC fs = fs(ref)/6
	   //AIC3106_writeRegister(2, 0xAA);	//ADC/DAC sample rate = f(s)/6 = MCLK/(128*Q)/6
   #endif

   #ifdef FS_10KHz	// it's actually 9.6kHz
	   AIC3106_writeRegister(3, 
				(0 << 7) |	// PLL is disabled
				(10 << 3) |	// PLL Q value = 10
				(2 << 0));	// PLL P value = 2
	   //AIC3106_writeRegister(3, (0 << 7) | (10 << 3) | (2 << 0));	// PLL disabled, Q=10, P=2
	   
	   AIC3106_writeRegister(2, 
				(2 << 4) | 	// ADC fs = fs(ref)/6
				(2 << 0));	// DAC fs = fs(ref)/6
	   //AIC3106_writeRegister(2, 0x22);	//ADC/DAC sample rate = f(s)/2 = MCLK/(128*Q)/2
   #endif

	
   AIC3106_writeRegister(7, 
   				(0 << 7) |	// fs(ref) = 48kHz (needed only for AGC time constants, not used)
   				(0 << 6) |	// ADC dual rate mode is disabled
   				(0 << 5) |	// DAC dual rate mode is disabled
   				(1 << 3) |	// left DAC datapath plays left channel input data
				(1 << 1) |	// right DAC datapath plays right channel input data
				(0 << 0));	// reserved
   				//0x0A);
   
   AIC3106_writeRegister(8,	
				(0 << 7) |	// BCLK is input (use "1" for output)
				(0 << 6) |	// WCLK is input (use "1" for output)
				(0 << 5) |	// do no place DOUT in high-z when inactive
				(0 << 4) |	// BCLK & WCLK disabled in master mode if code powered down
				(0 << 3) |	// reserved
				(0 << 2) |	// disable 3D effect
				(0 << 0));	// digital mic support disabled
				//0xC0);	// BCLK and WCLK are output
				
   AIC3106_writeRegister(9, 
				(0 << 6) |	// serial data bus in i2s mode
				(3 << 4) |	// audio word length 32 bits
				(0 << 3) |	// continuous transfer mode
				(0 << 2) |	// don't resync DAC w/ group delay variation
				(0 << 1) |	// don't resync ADC w/ group delay variation
				(0 << 0));	// resync w/o soft muting
				//0x30);	// I2S mode, 32-bit data words, continous xfer mode
							
   //AIC3106_writeRegister(10, 0x00);		// data word offset
   
   #if 1	// turn input gain off
   // PGA setting, 0 means 0dB gain
   AIC3106_writeRegister(15, 
				(0 << 7) |	// left ADC PGA is not muted
				(0 << 0)); 	// left ADC PGA gain setting = 0 dB
				//(0);
   
   AIC3106_writeRegister(16, 
				(0 << 7) |	// right ADC PGA is not muted
				(0 << 0)); 	// right ADC PGA gain setting = 0 dB
				//(0);   

	#else		// turn input gain on
   AIC3106_writeRegister(15, 
				(0 << 7) |	// left ADC PGA is not muted
				(44 << 0)); 	// left ADC PGA gain setting = 22 dB
				//(0);
   
   AIC3106_writeRegister(16, 
				(0 << 7) |	// right ADC PGA is not muted
				(44 << 0)); 	// right ADC PGA gain setting = 22 dB
	#endif		
    
   AIC3106_writeRegister(19, 0x04);	// left ADC is powered up
   AIC3106_writeRegister(22, 0x04);	// right ADC is powered up

   AIC3106_writeRegister(27, 0);	// left AGC maximum gain allowed is 0dB (AGC not used)
   AIC3106_writeRegister(30, 0);	// right AGC maximum gain allowed is 0dB (AGC not used)
   
   AIC3106_writeRegister(37, 
				(1 << 7) |	// left DAC powered up
				(1 << 6) |	// right DAC powered up
				(2 << 4) |	// HPLCOM configured as independent single-ended output (not used here)
				(0 << 0));	// reserved
				//0xE0);
				
   // set the DAC gain   
   AIC3106_writeRegister(43, 
				(0 << 7) |	// left DAC channel is not muted
				(0 << 0));		// left DAC gain setting = 0dB
				
   AIC3106_writeRegister(44, 
				(0 << 7) |	// right DAC channel is not muted
				(0 << 0));		// left DAC gain setting = 0dB

	#if 0
	// set the DAC gain   
   AIC3106_writeRegister(43, 
				(0 << 7) |	// left DAC channel is not muted
				(0x28 << 0));		// left DAC gain setting = -20dB
				
   AIC3106_writeRegister(44, 
				(0 << 7) |	// right DAC channel is not muted
				(0x28 << 0));		// left DAC gain setting = -20dB

	#endif
				   
   AIC3106_writeRegister(82, 
				(1 << 7) |	// DAC_L1 is routed to LEFT_LOP/M
				(0 << 0));	// DAC_L1 to LEFT_LOP/M analog volum control
				//0x80);			
   AIC3106_writeRegister(86, 
				(4 << 4) |	// LEFT_LOP/M output level control = 0dB
				(1 << 3) | 	// LEFT_LOP/M is not muted
				(0 << 2) | 	// reserved, read only
				(0 << 1) |	// read only
				(1 << 0));	// read only (must write 1 for some reason)
				// 0x09);

   AIC3106_writeRegister(92, 
				(1 << 7) |	// DAC_R1 is routed to RIGHT_LOP/M
				(0 << 0));	// DAC_R1 to RIGHT_LOP/M analog volum control
				//0x80);			

   AIC3106_writeRegister(93, 
				(4 << 4) |	// RIGHT_LOP/M output level control = 0dB
				(1 << 3) | 	// RIGHT_LOP/M is not muted
				(0 << 2) | 	// reserved, read only
				(0 << 1) |	// read only
				(1 << 0));	// read only (must write 1 for some reason)
				// 0x09);
   
   AIC3106_writeRegister(101, 
				(0 << 6) |	// read only
				(0 << 5) |	// MFP3 pin as GPI disabled
				(0 << 3) |	// read only
				(0 << 2) |	// MFP2 pin as GPO disabled
				(0 << 1) |	// MFP2 drives low when configured as GPO
				(1 << 0));	// CODEC_CLKIN uses CLKDIV_OUT
				//0x01);
				
   AIC3106_writeRegister(102, 
				(0 << 6) |	// CLKDIV_IN uses MCLK
				(0 << 4) |	// PLLCLK_IN uses MCLK
				(0 << 0));	// PLL clock divider N = 16
				//0);	// CLKDIV_IN uses MCLK
      

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



