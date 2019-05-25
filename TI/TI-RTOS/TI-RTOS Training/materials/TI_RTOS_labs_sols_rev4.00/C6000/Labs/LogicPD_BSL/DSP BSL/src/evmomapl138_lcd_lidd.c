//-----------------------------------------------------------------------------
// \file    evmomapl138_lcd_lidd.c
// \brief   implementation of the lidd LCD driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_cdce913.h"
#include "evmomapl138_lcdc.h"
#include "evmomapl138_lcd_lidd.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_emac.h"


//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------
const uint8_t CHAR_LCD_PINS[8] =
{
   CHAR_LCD_DB0, CHAR_LCD_DB1, CHAR_LCD_DB2, CHAR_LCD_DB3,
   CHAR_LCD_DB4, CHAR_LCD_DB5, CHAR_LCD_DB6, CHAR_LCD_DB7
};

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the lcd controller for raster use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...lcdc ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t LIDD_init(void)
{
   uint32_t rtn = ERR_NO_ERROR;

	/* Enable the LCD Hardware */
	EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_LCDC, PSC_ENABLE);

	/* Set MUX_MODE on UI GPIO expander to LCD */
	EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_0_REG, PINMUX_LCD_D_BUS_0_MASK, PINMUX_LCD_D_BUS_0_VALUE);
	EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_1_REG, PINMUX_LCD_D_BUS_1_MASK, PINMUX_LCD_D_BUS_1_VALUE);
	EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_2_REG, PINMUX_LCD_D_BUS_2_MASK, PINMUX_LCD_D_BUS_2_VALUE);
	EVMOMAPL138_pinmuxConfig(PINMUX_LCD_CONTROL_0_REG, PINMUX_LCD_CONTROL_0_MASK, PINMUX_LCD_CONTROL_0_VALUE);
	EVMOMAPL138_pinmuxConfig(PINMUX_LCD_CONTROL_1_REG, PINMUX_LCD_CONTROL_1_MASK, PINMUX_LCD_CONTROL_1_VALUE);

	/*
	 *  LCD Panel
	 *  8.824 MHz, raster mode pixel clock, 480-272 viewable frame
	 */
	LCD_RASTER_CTRL			&= 0xfffffffe;		// Turn raster controller off

	LCD_LIDD_CTRL			= 0x0000000C;		// LIDD mode 0x4

	LCD_LCD_STAT			= 0x000003ff;		// Clear status bits
    LCD_LCD_CTRL			= 0x00002500;		// PCLK = 150MHz / 37 = 4.054 MHz, LIDD mode

	LCD_LIDD_CS0_CONF		= 0xF7DCE3BA;
							//F     7   D     C   E    3    B     A
						    //1111 0111 1101 1100 1110 0011 1011 1010
							//gggg gfff fffe eeed dddd cccc ccbb bbaa


	//////////////////////////////////////////////////
	// Enable the Char LCD
	//////////////////////////////////////////////////
	// enable video (active low) via gpio expander and disable rmii phy to
	// ensure we have exclusive access to the bus.
	// TODO: need to update for new board rev.
	rtn = I2CGPIO_init(I2C_ADDR_GPIO_UI);
	if (rtn != ERR_NO_ERROR)
		return (ERR_NO_UI_BOARD);

	// make sure we are getting a 27MHz clock.
	rtn = CDCE913_init();

	rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_ETHER_PGM_ENn, 1);
	rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELA, 0);
	rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELB, 0);
	rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELC, 0);

	return (rtn);
}

//----------------------------------------------------------------------------
// \brief   write a byte to the character lcd.
//
// \param   uint8_t registerDest - .
//
// \param   uint8_t dataToWrite - .
//
// \return  none.
//-----------------------------------------------------------------------------
uint32_t LIDD_writeByte(uint8_t registerDest, uint8_t dataToWrite)
{

	if (registerDest == LCD_INST_REG)
	{
		LCD_LIDD_CS1_DATA	= dataToWrite;
	}
	else
	{
		 LCD_LIDD_CS1_ADDR	= dataToWrite;
	}

	return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   write a string to the character lcd.
//
// \param   uint8_t* charBuffer - .
//
// \param   uint16_t bufferLen - .
//
// \param   uint32_t charDelay - .
//
// \return  none.
//-----------------------------------------------------------------------------
uint32_t LIDD_writeString(uint8_t* charBuffer, uint16_t bufferLen, uint32_t charDelay)
{
   uint16_t bufferIndex;

	for (bufferIndex = 0; bufferIndex < bufferLen; bufferIndex++)
	{
		LIDD_writeByte(LCD_DATA_REG, charBuffer[bufferIndex]);

		if (charDelay > 0)
		{
			USTIMER_delay(charDelay);
		}
	}

	return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
