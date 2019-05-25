//-----------------------------------------------------------------------------
// \file    evmomapl138_dip.c
// \brief   implementation of the dip switch driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_dip.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static void convertDipToI2CGpio(uint8_t in_dip_num, uint16_t *in_pin_num);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the dip switches for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...dips ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t DIP_init(void)
{
   uint32_t rtn = 0;

   // init experimenter i/o expander.
   rtn = I2CGPIO_init(I2C_ADDR_GPIO_EX);

   if (rtn)
      return (ERR_INIT_FAIL);
   else
      return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   returns whether the passed dip is on (or not).
//
// \param   uint8_t in_dip_num - dip switch to return state (0-3).
//
// \param   uint8_t *dip_val - dip data from expander
//                               0 -> dip is off
//                               1 -> dip is on
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...data returned in dip_val.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t DIP_get(uint8_t in_dip_num, uint8_t *dip_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (in_dip_num < MAX_DIP_NUM)
   {
      uint16_t pin_num;
      
      convertDipToI2CGpio(in_dip_num, &pin_num);

      rtn = I2CGPIO_getInput(I2C_ADDR_GPIO_EX, pin_num, dip_val);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      if (*dip_val)
      {
         // i2c gpio pin is high, dip is off.
         *dip_val = 0;
      }
      else
      {
         // i2c gpio pin is low, dip is on.
         *dip_val = 1;
      }
   }
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   returns status of all dip switches...one switch per bit.
//
// \param   uint32_t *dip_val - dip data from expander
//                            0 -> dip is off
//                            1 -> dip is on
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...data returned in dip_val.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t DIP_getAll(uint32_t *dip_val)
{
   uint32_t rtn;
   uint16_t i2c_data;
   
   rtn = I2CGPIO_getInputAll(I2C_ADDR_GPIO_EX, &i2c_data);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // convert i/o expander bits to dip bits.
   *dip_val = (uint32_t)((i2c_data & I2C_GPIO_EX_DIP_MASK) >> I2C_GPIO_EX_DIP_SHIFT);

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

void convertDipToI2CGpio(uint8_t in_dip_num, uint16_t *in_pin_num)
{
   switch (in_dip_num)
   {
      case DIP_1:
         *in_pin_num = I2C_GPIO_EX_SW1;
         break;

      case DIP_2:
         *in_pin_num = I2C_GPIO_EX_SW2;
         break;

      case DIP_3:
         *in_pin_num = I2C_GPIO_EX_SW3;
         break;

      case DIP_4:
         *in_pin_num = I2C_GPIO_EX_SW4;
         break;

      case DIP_5:
         *in_pin_num = I2C_GPIO_EX_SW5;
         break;

      case DIP_6:
         *in_pin_num = I2C_GPIO_EX_SW6;
         break;

      case DIP_7:
         *in_pin_num = I2C_GPIO_EX_SW7;
         break;

      case DIP_8:
         *in_pin_num = I2C_GPIO_EX_SW8;
         break;

      default:
         *in_pin_num = 0xFF;
   }
}
