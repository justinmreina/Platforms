//-----------------------------------------------------------------------------
// \file    evmomapl138_pmic.c
// \brief   implementation of a TI TPS65070 pmic driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_pmic.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// gpio defines.
#define GPIO_BANK_PMIC_PWR_ON    (GPIO_BANK2)
#define GPIO_PIN_PMIC_PWR_ON     (GPIO_PIN2)
#define PINMUX_PMIC_GPIO_REG     (6)
#define PINMUX_PMIC_GPIO_MASK    (0x00F00000)
#define PINMUX_PMIC_GPIO_VAL     (0x00800000)

// i2c defines.
#define I2C_PORT_PMIC         (I2C0)

// voltage defines.
#define PMIC_VOLTAGE_3P3	  (0x3F)
#define PMIC_VOLTAGE_1P8	  (0x25)
#define PMIC_VOLTAGE_1P2	  (0x13)
//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the pmic for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...pmic ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t PMIC_init(void)
{
   EVMOMAPL138_pinmuxConfig(PINMUX_GPIO_BUFF_OE_REG, PINMUX_GPIO_BUFF_OE_MASK, PINMUX_GPIO_BUFF_OE_VAL);
   GPIO_setDir(GPIO_BUFF_OE_BANK, GPIO_BUFF_OE_PIN, GPIO_OUTPUT);
   GPIO_setOutput(GPIO_BUFF_OE_BANK, GPIO_BUFF_OE_PIN, OUTPUT_LOW);

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   .
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...startup regs written.
//    else - something happened during i2c comm.
//-----------------------------------------------------------------------------
uint32_t PMIC_writeStartupRegs(void)
{
   uint32_t rtn;

   rtn = PMIC_writeRegister(DEFDCDC3_LOW, PMIC_VOLTAGE_1P2);
   rtn |= PMIC_writeRegister(0x0D, 0x5F); //set CON_CTRL_1 enable all DCDC & LDO

	//it is not safe to run io at 1.8v on alpha boards therefore DCDC2 will only be allowed
	//to output 1.8 if EN_3P3_1P8_IO macro is defined. 
#ifdef EN_3P3_1P8_IO		
   rtn |= PMIC_writeRegister(DEFDCDC2_LOW,  PMIC_VOLTAGE_1P8); //set DCDC2_LOW voltage to 1.8v
   rtn |= PMIC_writeRegister(DEFDCDC2_HIGH, PMIC_VOLTAGE_3P3); //set DCDC2_HIGH voltage to 3.3v
#else
   rtn |= PMIC_writeRegister(DEFDCDC2_LOW,  PMIC_VOLTAGE_3P3); //set DCDC2_LOW voltage to 3.3v
   rtn |= PMIC_writeRegister(DEFDCDC2_HIGH, PMIC_VOLTAGE_3P3); //set DCDC2_HIGH voltage to 3.3v	
#endif 

   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   .
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...regs printed to stdout.
//    else - something happened during i2c comm.
//-----------------------------------------------------------------------------
uint32_t PMIC_readPrintRegs(void)
{
   uint32_t rtn;
   uint8_t reg;
   uint8_t reg_num;
   
   for (reg_num = 0x0B; reg_num <= 0x14; reg_num++)
   {
      rtn = PMIC_readRegister(reg_num, &reg);
      if (rtn != ERR_NO_ERROR)
      {
         printf("\terror reading reg[0x%02X]: %u\r\n", reg_num, rtn);
         break;
      }
      else
      {
         printf("\treg[0x%02X]: 0x%02X\r\n", reg_num, reg);
      }
   }
   
   return (rtn);
}

//----------------------------------------------------------------------------
// \brief   reads a register from pmic.
//
// \return  none.
//-----------------------------------------------------------------------------
uint32_t PMIC_readRegister(uint8_t in_reg_addr, uint8_t *dest_buffer)
{
   uint32_t rtn;

   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_PMIC, I2C_ADDR_PMIC, &in_reg_addr, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the pmic register data.
   rtn = I2C_read(I2C_PORT_PMIC, I2C_ADDR_PMIC, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);
   
   return (rtn);
}

//----------------------------------------------------------------------------
// \brief   writes a register to pmic.
//
// \return  none.
//-----------------------------------------------------------------------------
uint32_t PMIC_writeRegister(uint8_t in_reg_addr, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];
   
   i2c_data[0] = in_reg_addr;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_PMIC, I2C_ADDR_PMIC, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}
