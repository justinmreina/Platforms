//-----------------------------------------------------------------------------
// \file    evmomapl138_gpio.c
// \brief   implementation of the gpio driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
#define GPIO_ODD_BANK_PIN_ADDER     (16)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static gpio_regs_t * getRegisterBankAndBit(uint32_t in_bank, uint8_t in_pin_num, uint32_t *reg_bit);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// \brief   sets the direction (input/output) of the given gpio pin.
//
// \param   uint32_t in_bank - gpio bank for desired pin.
//
// \param   uint8_t in_pin_num - gpio pin within bank to be set.
//
// \param   uint8_t in_dir - input or output.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, gpio direction set.
//    ERR_INVALID_PARAMETER - input out of bounds.
//-----------------------------------------------------------------------------
uint32_t GPIO_setDir(uint32_t in_bank, uint8_t in_pin_num, uint8_t in_dir)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if ((in_bank < MAX_GPIO_BANK_NUM) && (in_pin_num < MAX_GPIO_PIN_NUM))
   {
      gpio_regs_t *gpio_bank;
      uint32_t gpio_bit;

      // get the register bank and bitmask for the input bank and pin.
      gpio_bank = getRegisterBankAndBit(in_bank, in_pin_num, &gpio_bit);

      if (GPIO_OUTPUT == in_dir)
      {
         // clear the bit to config pin for output.
         CLRBIT(gpio_bank->DIR, gpio_bit);
      }
      else
      {
         // set the bit to config pin for input.
         SETBIT(gpio_bank->DIR, gpio_bit);
      }
      
      rtn = ERR_NO_ERROR;
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   returns the current input of the given gpio pin.
//
// \param   uint32_t in_bank - gpio bank for desired pin.
//
// \param   uint8_t in_pin_num - gpio pin within bank to get input.
//
// \param   uint8_t *gpio_val - gpio pin data
//                      0 - pin is cleared.
//                      1 - pin is set.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, data returned in gpio_val.
//    ERR_INVALID_PARAMETER - input out of bounds.
//-----------------------------------------------------------------------------
uint32_t GPIO_getInput(uint32_t in_bank, uint8_t in_pin_num, uint8_t *gpio_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if ((in_bank < MAX_GPIO_BANK_NUM) && (in_pin_num < MAX_GPIO_PIN_NUM))
   {
      gpio_regs_t *gpio_bank;
      uint32_t gpio_bit;

      // get the register bank and bitmask for the input bank and pin.
      gpio_bank = getRegisterBankAndBit(in_bank, in_pin_num, &gpio_bit);

      if (CHKBIT(gpio_bank->IN_DATA, gpio_bit))
      {
         // in data is set, return 1.
         *gpio_val = 1;
      }
      else
      {
         // in data is clear, return 0.
         *gpio_val = 0;
      }
      
      rtn = ERR_NO_ERROR;
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   sets the given gpio pin output.
//
// \param   uint32_t in_bank - gpio bank for desired pin.
//
// \param   uint8_t in_pin_num - gpio pin within bank to be set.
//
// \param   uint8_t in_val - 0/1 to set or clear the pin.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, gpio pin set.
//    ERR_INVALID_PARAMETER - input out of bounds.
//-----------------------------------------------------------------------------
uint32_t GPIO_setOutput(uint32_t in_bank, uint8_t in_pin_num, uint8_t in_val)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if ((in_bank < MAX_GPIO_BANK_NUM) && (in_pin_num < MAX_GPIO_PIN_NUM))
   {
      gpio_regs_t *gpio_bank;
      uint32_t gpio_bit;

      // get the register bank and bitmask for the input bank and pin.
      gpio_bank = getRegisterBankAndBit(in_bank, in_pin_num, &gpio_bit);

      if (in_val)
         SETBIT(gpio_bank->OUT_DATA, gpio_bit);
      else
         CLRBIT(gpio_bank->OUT_DATA, gpio_bit);

      rtn = ERR_NO_ERROR;
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   sits in a forever loop toggling a given set of pins.
//
// \param   uint32_t *in_bank - gpio bank for desired pins
//
// \param   uint8_t *in_pin_num - gpio pins within bank to be set.
//
// \param   uint8_t in_num_pins - number of pins to be toggled.
//
// \return  none.
//-----------------------------------------------------------------------------
void GPIO_toggleLoop(uint32_t *in_bank, uint8_t *in_pin_num, uint8_t in_num_pins)
{
   uint8_t pin_val = 0;
   uint8_t i;

   for (i = 0; i < in_num_pins; i++)
   {
      GPIO_setDir(in_bank[i], in_pin_num[i], GPIO_OUTPUT);
   }
   
   while (1)
   {
      for (i = 0; i < in_num_pins; i++)
      {
         GPIO_setOutput(in_bank[i], in_pin_num[i], pin_val);
      }
      USTIMER_delay(5000);

      if (pin_val)
         pin_val = 0;
      else
         pin_val = 1;
   }
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// converts a bank (0-8) / pin (0-15) to a register bank (0-3) and bit (0-31).
//-----------------------------------------------------------------------------
static gpio_regs_t * getRegisterBankAndBit(uint32_t in_bank, uint8_t in_pin_num, uint32_t *reg_bit)
{
   gpio_regs_t *rtn;

   switch (in_bank)
   {
      case GPIO_BANK0:
         rtn = GPIO_BANK01;
         break;

      case GPIO_BANK1:
         rtn = GPIO_BANK01;
         in_pin_num += GPIO_ODD_BANK_PIN_ADDER;
         break;
      
      case GPIO_BANK2:
         rtn = GPIO_BANK23;
         break;
         
      case GPIO_BANK3:
         rtn = GPIO_BANK23;
         in_pin_num += GPIO_ODD_BANK_PIN_ADDER;
         break;
      
      case GPIO_BANK4:
         rtn = GPIO_BANK45;
         break;
         
      case GPIO_BANK5:
         rtn = GPIO_BANK45;
         in_pin_num += GPIO_ODD_BANK_PIN_ADDER;
         break;
      
      case GPIO_BANK6:
         rtn = GPIO_BANK67;
         break;
         
      case GPIO_BANK7:
         rtn = GPIO_BANK67;
         in_pin_num += GPIO_ODD_BANK_PIN_ADDER;
         break;

      case GPIO_BANK8:
         rtn = GPIO_BANK8_;
         break;
   }
   
   *reg_bit = bitval_u32[in_pin_num];

   return (rtn);
}
