//-----------------------------------------------------------------------------
// \file    evmomapl138_led.c
// \brief   implementation of the led driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_led.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static void convertLedToI2CGpio(uint8_t in_led_num, uint16_t *in_pin_num);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the leds for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...leds ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t LED_init(void)
{
   uint32_t rtn;

   // init experimenter i/o expander.
   rtn = I2CGPIO_init(I2C_ADDR_GPIO_EX);
   
   // turn all leds off.
   rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_EX, I2C_GPIO_EX_LED1, 1);
   rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_EX, I2C_GPIO_EX_LED2, 1);

   if (rtn)
      return (ERR_INIT_FAIL);
   else
      return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   returns whether the passed led is on (or not).
//
// \param   uint8_t in_led_num - led number to check.
//
// \param   uint8_t *led_status - led data from expander
//                            0 -> led is off
//                            1 -> led is on
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds...data returned in led_status.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t LED_isOn(uint8_t in_led_num, uint8_t *led_status)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   
   if (in_led_num < MAX_LED_NUM)
   {
      uint16_t pin_num;

      convertLedToI2CGpio(in_led_num, &pin_num);

      rtn = I2CGPIO_getInput(I2C_ADDR_GPIO_EX, pin_num, led_status);
      if (rtn != ERR_NO_ERROR)
         return (rtn);
      
      if (*led_status)
      {
         // i2c gpio pin is high, led is off.
         *led_status = 0;
      }
      else
      {
         // i2c gpio pin is low, led is on.
         *led_status = 1;
      }
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   turns on one led.
//
// \param   uint8_t in_led_num - led to turn on.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, led turned off.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t LED_turnOn(uint8_t in_led_num)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (in_led_num < MAX_LED_NUM)
   {
      uint16_t pin_num;

      convertLedToI2CGpio(in_led_num, &pin_num);

      // leds are active low...set to 0 to turn on.
      rtn = I2CGPIO_setOutput(I2C_ADDR_GPIO_EX, pin_num, 0);
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   turns on off led.
//
// \param   uint8_t in_led_num - led to turn off.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, led turned on.
//    ERR_INVALID_PARAMETER - input out of bounds.
//    else - something happened with i2c comm.
//-----------------------------------------------------------------------------
uint32_t LED_turnOff(uint8_t in_led_num)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (in_led_num < MAX_LED_NUM)
   {
      uint16_t pin_num;

      convertLedToI2CGpio(in_led_num, &pin_num);

      // leds are active low...set to 1 to turn off.
      rtn = I2CGPIO_setOutput(I2C_ADDR_GPIO_EX, pin_num, 1);
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   toggles on one led.
//
// \param   uint8_t in_led_num - led to toggle.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, led toggled.
//    ERR_INVALID_PARAMETER - input out of bounds.
//-----------------------------------------------------------------------------
uint32_t LED_toggle(uint8_t in_led_num)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   uint8_t led_status;
   
   rtn = LED_isOn(in_led_num, &led_status);

   if (led_status)
   {
      rtn = LED_turnOff(in_led_num);
   }
   else
   {
      rtn = LED_turnOn(in_led_num);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

void convertLedToI2CGpio(uint8_t in_led_num, uint16_t *in_pin_num)
{
   switch (in_led_num)
   {
      case LED_1:
         *in_pin_num = I2C_GPIO_EX_LED1;
         break;

      case LED_2:
         *in_pin_num = I2C_GPIO_EX_LED2;
         break;
   }
}
