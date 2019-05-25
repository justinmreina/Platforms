//-----------------------------------------------------------------------------
// \file    evmomapl138_pb.c
// \brief   implementation of the pushbutton driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_pb.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static void convertPBToI2CGpioUI(uint8_t in_pb_num, uint16_t *in_pin_num);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the pushbuttons for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...pushbuttons ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t PB_init(void)
{
   uint32_t rtn = 0;

   // init experimenter and ui i/o expanders.
   rtn = I2CGPIO_init(I2C_ADDR_GPIO_EX);
   rtn |= I2CGPIO_init(I2C_ADDR_GPIO_UI);

   if (rtn)
      return (ERR_INIT_FAIL);
   else
      return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   returns whether the passed led is on (or not).
//
// \param   uint8_t in_pb_num - pushbutton to return state (0-8).
//
// \param   uint8_t *pb_val - pb data from expander
//                            0 -> pb is released
//                            1 -> pb is pressed
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...data returned in pb_val.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t PB_get(uint8_t in_pb_num, uint8_t *pb_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (in_pb_num < MAX_PB_NUM)
   {
      uint16_t pin_num;
      
      // are we getting pb data from ui or experimenter?
      if (PB_EX_1 == in_pb_num)
      {
         rtn = I2CGPIO_getInput(I2C_ADDR_GPIO_EX, I2C_GPIO_EX_PB1, pb_val);
         if (rtn != ERR_NO_ERROR)
            return (rtn);
      }
      else
      {
         convertPBToI2CGpioUI(in_pb_num, &pin_num);

         rtn = I2CGPIO_getInput(I2C_ADDR_GPIO_UI, pin_num, pb_val);
         if (rtn != ERR_NO_ERROR)
            return (rtn);
      }
      
      if (*pb_val)
      {
         // i2c gpio pin is high, pb is released.
         *pb_val = 0;
      }
      else
      {
         // i2c gpio pin is low, pb is pressed.
         *pb_val = 1;
      }
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   returns status of all pushbuttons...one pb per bit.
//
// \param   uint32_t *pb_val - pb data from expander
//                            0 -> pb is off
//                            1 -> pb is on
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...data returned in pb_val.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t PB_getAll(uint32_t *pb_val)
{
   uint32_t rtn;
   uint8_t tmp_val;
   uint16_t i2c_data;
   uint16_t i2c_bit;
   uint8_t ret_bit;
   
   *pb_val = 0;

   // get experimenter pb and stuff data into return pb_val.
   rtn = I2CGPIO_getInput(I2C_ADDR_GPIO_EX, I2C_GPIO_EX_PB1, &tmp_val);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   if (tmp_val)
      SETBIT(*pb_val, bitval_u32[0]);

   // get all ui pb data.
   rtn = I2CGPIO_getInputAll(I2C_ADDR_GPIO_UI, &i2c_data);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // convert i/o expander bits to pb bits.
   i2c_bit = 1 << I2C_GPIO_UI_PB1;
   for (ret_bit = 1; ret_bit < 9; ret_bit++)
   {
      if (i2c_data & i2c_bit)
         SETBIT(*pb_val, bitval_u32[ret_bit]);

      i2c_bit = i2c_bit >> 1;
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

void convertPBToI2CGpioUI(uint8_t in_pb_num, uint16_t *in_pin_num)
{
   switch (in_pb_num)
   {
      case PB_UI_1:
         *in_pin_num = I2C_GPIO_UI_PB1;
         break;

      case PB_UI_2:
         *in_pin_num = I2C_GPIO_UI_PB2;
         break;

      case PB_UI_3:
         *in_pin_num = I2C_GPIO_UI_PB3;
         break;

      case PB_UI_4:
         *in_pin_num = I2C_GPIO_UI_PB4;
         break;

      case PB_UI_5:
         *in_pin_num = I2C_GPIO_UI_PB5;
         break;

      case PB_UI_6:
         *in_pin_num = I2C_GPIO_UI_PB6;
         break;

      case PB_UI_7:
         *in_pin_num = I2C_GPIO_UI_PB7;
         break;

      case PB_UI_8:
         *in_pin_num = I2C_GPIO_UI_PB8;
         break;

      default:
         *in_pin_num = 0xFF;
   }
}
