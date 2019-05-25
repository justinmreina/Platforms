//-----------------------------------------------------------------------------
// \file    evmomapl138_lcd_lidd.h
// \brief   OMAP-L138 lcd lidd function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_LCD_LIDD_H
#define EVMOMAPL138_LCD_LIDD_H

#include "types.h"

//-----------------------------------------------------------------------------
// Defines and Macros
//-----------------------------------------------------------------------------
#define DELAY_1_MS						(1000)    // in us
#define DELAY_5_MS     					(5000)    // in us
#define DELAY_200_MS					(200000)    // in us


/* ------------------------------------------------------------------------ *
 *  LCD Controller                                                          *
 * ------------------------------------------------------------------------ */
#define LCD_BASE                0x01E13000
#define LCD_REVID               *( volatile uint32_t* )( LCD_BASE + 0x00 )
#define LCD_LCD_CTRL            *( volatile uint32_t* )( LCD_BASE + 0x04 )
#define LCD_LCD_STAT            *( volatile uint32_t* )( LCD_BASE + 0x08 )
#define LCD_LIDD_CTRL           *( volatile uint32_t* )( LCD_BASE + 0x0c )
#define LCD_LIDD_CS0_CONF       *( volatile uint32_t* )( LCD_BASE + 0x10 )
#define LCD_LIDD_CS0_ADDR       *( volatile uint32_t* )( LCD_BASE + 0x14 )
#define LCD_LIDD_CS0_DATA       *( volatile uint32_t* )( LCD_BASE + 0x18 )
#define LCD_LIDD_CS1_CONF       *( volatile uint32_t* )( LCD_BASE + 0x1c )
#define LCD_LIDD_CS1_ADDR       *( volatile uint32_t* )( LCD_BASE + 0x20 )
#define LCD_LIDD_CS1_DATA       *( volatile uint32_t* )( LCD_BASE + 0x24 )
#define LCD_RASTER_CTRL         *( volatile uint32_t* )( LCD_BASE + 0x28 )
#define LCD_RASTER_TIMING_0     *( volatile uint32_t* )( LCD_BASE + 0x2c )
#define LCD_RASTER_TIMING_1     *( volatile uint32_t* )( LCD_BASE + 0x30 )
#define LCD_RASTER_TIMING_2     *( volatile uint32_t* )( LCD_BASE + 0x34 )
#define LCD_RASTER_SUBPANEL     *( volatile uint32_t* )( LCD_BASE + 0x38 )
#define LCD_LCDDMA_CTRL         *( volatile uint32_t* )( LCD_BASE + 0x40 )
#define LCD_LCDDMA_FB0_BASE     *( volatile uint32_t* )( LCD_BASE + 0x44 )
#define LCD_LCDDMA_FB0_CEILING  *( volatile uint32_t* )( LCD_BASE + 0x48 )
#define LCD_LCDDMA_FB1_BASE     *( volatile uint32_t* )( LCD_BASE + 0x4c )
#define LCD_LCDDMA_FB1_CEILING  *( volatile uint32_t* )( LCD_BASE + 0x50 )


#define PINMUX_LCD_D_BUS_0_REG		16
#define PINMUX_LCD_D_BUS_1_REG		17
#define PINMUX_LCD_D_BUS_2_REG		18
#define PINMUX_LCD_CONTROL_0_REG	18
#define PINMUX_LCD_CONTROL_1_REG	19

#define PINMUX_LCD_D_BUS_0_MASK		(0xFFFFFF00)
#define PINMUX_LCD_D_BUS_1_MASK		(0xFFFFFFFF)
#define PINMUX_LCD_D_BUS_2_MASK		(0x000000FF)
#define PINMUX_LCD_CONTROL_0_MASK	(0xFF000000)
#define PINMUX_LCD_CONTROL_1_MASK	(0x000000FF)

#define PINMUX_LCD_D_BUS_0_VALUE	(0x22222200)
#define PINMUX_LCD_D_BUS_1_VALUE	(0x22222222)
#define PINMUX_LCD_D_BUS_2_VALUE	(0x00000022)
#define PINMUX_LCD_CONTROL_0_VALUE	(0x22000000)
#define PINMUX_LCD_CONTROL_1_VALUE	(0x00000022)

#define FRAMEBUF_BASE               0xc0000000

#define LCD_WIDTH					480
#define LCD_HEIGHT					272

/* ------------------------------------------------------------------------ *
 *  CHAR LCD Controller                                                     *
 * ------------------------------------------------------------------------ */

#define PINMUX_REG_16					16
#define PINMUX_REG_17					17
#define PINMUX_REG_18                   18
#define PINMUX_REG_19					19

#define PINMUX_REG_16_MASK				(0xFFFFFF00)
#define PINMUX_REG_17_MASK				(0x000000FF)
#define PINMUX_REG_18_MASK              (0xFF000000)
#define PINMUX_REG_19_MASK				(0x0F0000FF)

#define PINMUX_REG_16_VALUE				(0x88888800)
#define PINMUX_REG_17_VALUE				(0x00000088)
#define PINMUX_REG_18_VALUE             (0x88000000)
#define PINMUX_REG_19_VALUE				(0x08000088)

#define CHAR_LCD_GPIO_BANK_EN			GPIO_BANK8      //GPIO_BANK6
#define CHAR_LCD_GPIO_BANK_BUS			GPIO_BANK7
#define CHAR_LCD_GPIO_BANK_CONTROL		GPIO_BANK8


#define CHAR_LCD_ENn					GPIO_PIN10      //GPIO_PIN0

#define CHAR_LCD_DB0					GPIO_PIN8
#define CHAR_LCD_DB1					GPIO_PIN9
#define CHAR_LCD_DB2					GPIO_PIN10
#define CHAR_LCD_DB3					GPIO_PIN11
#define CHAR_LCD_DB4					GPIO_PIN12
#define CHAR_LCD_DB5					GPIO_PIN13
#define CHAR_LCD_DB6					GPIO_PIN14
#define CHAR_LCD_DB7					GPIO_PIN15

#define CHAR_LCD_RS						GPIO_PIN8   //VSYNC
#define CHAR_LCD_RnW					GPIO_PIN9	//HSYNC

#define LED_1           				(0)
#define LED_2           				(1)
#define MAX_LED_NUM     				(2)


#define LCD_SETUP_BYTE_0				0x60
#define LCD_SETUP_BYTE_1				0x38
#define LCD_SETUP_BYTE_2				0x08
#define LCD_SETUP_BYTE_3				0x01
#define LCD_SETUP_BYTE_4				0x04

#define LCD_SETUP_BYTE_5				0x0F

#define LCD_CLEAR_DISP					0x01

#define LCD_INST_REG           			(0)
#define LCD_DATA_REG           			(1)

#define LCD_COMMAND_SET_ADDR			0x80

uint32_t LIDD_init(void);
uint32_t LIDD_writeByte(uint8_t registerDest, uint8_t dataToWrite);
uint32_t LIDD_writeString(uint8_t* charBuffer, uint16_t bufferLen, uint32_t charDelay);


#endif

