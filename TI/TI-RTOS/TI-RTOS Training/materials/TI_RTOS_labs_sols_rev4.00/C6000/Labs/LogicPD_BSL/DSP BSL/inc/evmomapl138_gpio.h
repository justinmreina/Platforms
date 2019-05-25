//-----------------------------------------------------------------------------
// \file    evmomapl138_gpio.h
// \brief   OMAP-L138 general purpose io registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_GPIO_H
#define EVMOMAPL138_GPIO_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t DIR;
   volatile uint32_t OUT_DATA;
   volatile uint32_t SET_DATA;
   volatile uint32_t CLR_DATA;
   volatile uint32_t IN_DATA;
   volatile uint32_t SET_RIS_TRIG;
   volatile uint32_t CLR_RIS_TRIG;
   volatile uint32_t SET_FAL_TRIG;
   volatile uint32_t CLR_FAL_TRIG;
   volatile uint32_t IRQ_STAT;
} gpio_regs_t;

// define all the available gpio peripherals for the processor.
#define GPIO_BANK01              ((gpio_regs_t *)GPIO_BANK01_BASE)
#define GPIO_BANK23              ((gpio_regs_t *)GPIO_BANK23_BASE)
#define GPIO_BANK45              ((gpio_regs_t *)GPIO_BANK45_BASE)
#define GPIO_BANK67              ((gpio_regs_t *)GPIO_BANK67_BASE)
#define GPIO_BANK8_              ((gpio_regs_t *)GPIO_BANK8_BASE)

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------
#define GPIO_BANK0            (0)
#define GPIO_BANK1            (1)
#define GPIO_BANK2            (2)
#define GPIO_BANK3            (3)
#define GPIO_BANK4            (4)
#define GPIO_BANK5            (5)
#define GPIO_BANK6            (6)
#define GPIO_BANK7            (7)
#define GPIO_BANK8            (8)
#define MAX_GPIO_BANK_NUM     (9)

#define GPIO_PIN0             (0)
#define GPIO_PIN1             (1)
#define GPIO_PIN2             (2)
#define GPIO_PIN3             (3)
#define GPIO_PIN4             (4)
#define GPIO_PIN5             (5)
#define GPIO_PIN6             (6)
#define GPIO_PIN7             (7)
#define GPIO_PIN8             (8)
#define GPIO_PIN9             (9)
#define GPIO_PIN10            (10)
#define GPIO_PIN11            (11)
#define GPIO_PIN12            (12)
#define GPIO_PIN13            (13)
#define GPIO_PIN14            (14)
#define GPIO_PIN15            (15)
#define MAX_GPIO_PIN_NUM      (16)

#define GPIO_OUTPUT           (0)
#define GPIO_INPUT            (1)

#define OUTPUT_LOW            (0)
#define OUTPUT_HIGH           (1)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t GPIO_setDir(uint32_t in_bank, uint8_t in_pin_num, uint8_t in_dir);
uint32_t GPIO_getInput(uint32_t in_bank, uint8_t in_pin_num, uint8_t *gpio_val);
uint32_t GPIO_setOutput(uint32_t in_bank, uint8_t in_pin_num, uint8_t in_val);

void GPIO_toggleLoop(uint32_t *in_bank, uint8_t *in_pin_num, uint8_t in_num_pins);

#endif
