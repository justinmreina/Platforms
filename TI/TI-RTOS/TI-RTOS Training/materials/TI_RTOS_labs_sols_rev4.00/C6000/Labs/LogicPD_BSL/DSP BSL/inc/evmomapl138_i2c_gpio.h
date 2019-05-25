//-----------------------------------------------------------------------------
// \file    evmomapl138_i2c_gpio.h
// \brief   OMAP-L138 i2c gpio definitions and function prototypes.
//          driver for TI TCA6416 I/O expander.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_I2C_GPIO_H
#define EVMOMAPL138_I2C_GPIO_H

#include "evmomapl138_i2c.h"

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------

// pin definitions from schematic.
#define I2C_GPIO_UI_TP25            (0)
#define I2C_GPIO_UI_TP24            (1)
#define I2C_GPIO_UI_TP23            (2)
#define I2C_GPIO_UI_TP22            (3)
#define I2C_GPIO_UI_ETHER_PGM_ENn   (4)
#define I2C_GPIO_UI_SELC 		    (5)
#define I2C_GPIO_UI_SELB	        (6)
#define I2C_GPIO_UI_SELA            (7)
#define I2C_GPIO_UI_PB8             (8)
#define I2C_GPIO_UI_PB7             (9)
#define I2C_GPIO_UI_PB6             (10)
#define I2C_GPIO_UI_PB5             (11)
#define I2C_GPIO_UI_PB4             (12)
#define I2C_GPIO_UI_PB3             (13)
#define I2C_GPIO_UI_PB2             (14)
#define I2C_GPIO_UI_PB1             (15)

#define I2C_GPIO_EX_TP25            (0)
#define I2C_GPIO_EX_TP24            (1)
#define I2C_GPIO_EX_TP23            (2)
#define I2C_GPIO_EX_TP22            (3)
#define I2C_GPIO_EX_TP21            (4)
#define I2C_GPIO_EX_PB1             (5)
#define I2C_GPIO_EX_LED2            (6)
#define I2C_GPIO_EX_LED1            (7)
#define I2C_GPIO_EX_SW1             (8)
#define I2C_GPIO_EX_SW2             (9)
#define I2C_GPIO_EX_SW3             (10)
#define I2C_GPIO_EX_SW4             (11)
#define I2C_GPIO_EX_SW5             (12)
#define I2C_GPIO_EX_SW6             (13)
#define I2C_GPIO_EX_SW7             (14)
#define I2C_GPIO_EX_SW8             (15)
#define I2C_GPIO_EX_DIP_MASK        (0xFF00)
#define I2C_GPIO_EX_DIP_SHIFT       (8)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t I2CGPIO_init(uint16_t in_addr);
uint32_t I2CGPIO_getInput(uint16_t in_addr, uint8_t in_pin_num, uint8_t *data);
uint32_t I2CGPIO_getInputAll(uint16_t in_addr, uint16_t *data);
uint32_t I2CGPIO_setOutput(uint16_t in_addr, uint8_t in_pin_num, uint16_t in_val);
uint32_t I2CGPIO_setOutputAll(uint16_t in_addr, uint16_t in_val);

#endif

