//-----------------------------------------------------------------------------
// \file    evmomapl138_i2c_gpio.c
// \brief   implementation of a TI TCA6416 I/O expander driver for OMAP-L138.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_i2c_gpio.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
#define I2C_PORT_GPIO         (I2C0)
#define I2C_GPIO_PIN_MAX      (16)
// config input/output pins (1 -> input, 0 -> output).
#define I2C_GPIO_CONFIG0_EX   (0x3F)
#define I2C_GPIO_CONFIG1_EX   (0xFF)
#define I2C_GPIO_CONFIG0_UI   (0x0F)
#define I2C_GPIO_CONFIG1_UI   (0xFF)
// TCA6416 command byte defines.
#define CMD_BYTE_INPUT0       (0x00)
#define CMD_BYTE_INPUT1       (0x01)
#define CMD_BYTE_OUTPUT0      (0x02)
#define CMD_BYTE_OUTPUT1      (0x03)
#define CMD_BYTE_POLARITY0    (0x04)
#define CMD_BYTE_POLARITY1    (0x05)
#define CMD_BYTE_CONFIG0      (0x06)
#define CMD_BYTE_CONFIG1      (0x07)

//-----------------------------------------------------------------------------
// Private Static Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the i2c I/O expander.
//
// \param   uint16_t in_addr - desired expander i2c address.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...i2c gpio ready to use.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_init(uint16_t in_addr)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   uint8_t i2c_data[3];

   if ((I2C_ADDR_GPIO_EX == in_addr) || (I2C_ADDR_GPIO_UI == in_addr))
   {
      // make sure polarity is not inverted.
      i2c_data[0] = CMD_BYTE_POLARITY0;
      i2c_data[1] = 0;
      i2c_data[2] = 0;
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, i2c_data, 3, SET_STOP_BIT_AFTER_WRITE);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      // set config regs on I/O expander.
      if (I2C_ADDR_GPIO_EX == in_addr)
      {
         i2c_data[0] = CMD_BYTE_CONFIG0;
         i2c_data[1] = I2C_GPIO_CONFIG0_EX;
         i2c_data[2] = I2C_GPIO_CONFIG1_EX;
      }
      else
      {
         i2c_data[0] = CMD_BYTE_CONFIG0;
         i2c_data[1] = I2C_GPIO_CONFIG0_UI;
         i2c_data[2] = I2C_GPIO_CONFIG1_UI;
      }
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, i2c_data, 3, SET_STOP_BIT_AFTER_WRITE);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   get gpio input from one pin of the i2c I/O expander.
//
// \param   uint16_t in_addr - desired expander i2c address.
//
// \param   uint8_t in_pin_num - pin on expander to be read.
//
// \param   uint8_t *data - gpio data from expander
//                            0 -> pin is clear
//                            1 -> pin is set
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...gpio state returned in data.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_getInput(uint16_t in_addr, uint8_t in_pin_num, uint8_t *data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   // check address and pin number.
   if (((I2C_ADDR_GPIO_EX == in_addr) || (I2C_ADDR_GPIO_UI == in_addr)) &&
      (in_pin_num < I2C_GPIO_PIN_MAX) &&
      (data != NULL))
   {
      uint8_t i2c_data;
      uint8_t gpio_bit = 0;
      
      // set command byte to read appropriate input.
      if (in_pin_num < 8)
      {
         i2c_data = CMD_BYTE_INPUT0;
         gpio_bit = 1 << in_pin_num;
      }
      else
      {
         i2c_data = CMD_BYTE_INPUT1;
         gpio_bit = 1 << (in_pin_num - 8);
      }
      
      // send i2c command.
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_STOP_BIT_AFTER_WRITE);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      // read the gpio data.
      rtn = I2C_read(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_BUSY_BIT_CHECK);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      // check the input pin value and set var.
      if (i2c_data & gpio_bit)
         *data = 1;
      else
         *data = 0;
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   get gpio input from all pins of the i2c I/O expander.
//
// \param   uint16_t in_addr - desired expander i2c address.
//
// \param   uint16_t *data - gpio data from expander.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...gpio state returned in data.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_getInputAll(uint16_t in_addr, uint16_t *data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   
   if ((I2C_ADDR_GPIO_EX == in_addr) || (I2C_ADDR_GPIO_UI == in_addr) &&
      (data != NULL))
   {
      uint8_t i2c_data;
      
      // send i2c command to read input0.
      i2c_data = CMD_BYTE_INPUT0;
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_STOP_BIT_AFTER_WRITE);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // read the gpio data for input0.
      rtn = I2C_read(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_BUSY_BIT_CHECK);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // copy gpio data into var.
      *data = i2c_data;

      // send i2c command to read input1.
      i2c_data = CMD_BYTE_INPUT1;
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_STOP_BIT_AFTER_WRITE);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // read the gpio data for input1.
      rtn = I2C_read(I2C_PORT_GPIO, in_addr, &i2c_data, 1, SKIP_BUSY_BIT_CHECK);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // copy gpio data into var.
      *data += (i2c_data << 8);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   set gpio output for one pin of the i2c I/O expander.
//
// \param   uint16_t in_addr - desired expander i2c address.
//
// \param   uint8_t in_pin_num - pin on expander to be read.
//
// \param   uint16_t in_val - 0/1 to set or clear the pin.
//
// \return  uint32_t
//    ERR_NO_ERROR - pin set successfully.
//    ERR_INVALID_PARAMETER - invalid pin number.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_setOutput(uint16_t in_addr, uint8_t in_pin_num, uint16_t in_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (((I2C_ADDR_GPIO_EX == in_addr) || (I2C_ADDR_GPIO_UI == in_addr)) &&
      (in_pin_num < I2C_GPIO_PIN_MAX))
   {
      uint8_t i2c_data[2];
      uint8_t gpio_bit = 0;

      // set command byte to read appropriate output, so we do not change
      // any data that we do not want to.
      if (in_pin_num < 8)
      {
         i2c_data[0] = CMD_BYTE_OUTPUT0;
         gpio_bit = 1 << in_pin_num;
      }
      else
      {
         i2c_data[0] = CMD_BYTE_OUTPUT1;
         gpio_bit = 1 << (in_pin_num - 8);
      }

      // send i2c command.
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, i2c_data, 1, SKIP_STOP_BIT_AFTER_WRITE);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // read the gpio data.
      rtn = I2C_read(I2C_PORT_GPIO, in_addr, &i2c_data[1], 1, SKIP_BUSY_BIT_CHECK);
      if (rtn != ERR_NO_ERROR)
         return (rtn);

      // update the data to set/clr bit for pin num.
      if (in_val)
         SETBIT(i2c_data[1], gpio_bit);
      else
         CLRBIT(i2c_data[1], gpio_bit);

      // write the gpio data back to the I/O expander.
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   set gpio output for all pins of the i2c I/O expander.
//
// \param   uint16_t in_addr - desired expander i2c address.
//
// \param   uint16_t in_val - pattern data to set I/O expander pins.
//
// \return  uint32_t
//    ERR_NO_ERROR - pins set successfully.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_setOutputAll(uint16_t in_addr, uint16_t in_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if ((I2C_ADDR_GPIO_EX == in_addr) || (I2C_ADDR_GPIO_UI == in_addr))
   {
      uint8_t i2c_data[3];

      // load up the array with the cmd and input data.
      i2c_data[0] = CMD_BYTE_OUTPUT0;
      i2c_data[1] = (uint8_t) (in_val & 0x00FF);
      i2c_data[2] = (uint8_t) (in_val >> 8);

      // write the gpio data to the I/O expander.
      rtn = I2C_write(I2C_PORT_GPIO, in_addr, i2c_data, 3, SET_STOP_BIT_AFTER_WRITE);
   }
     
   return (rtn);
}
