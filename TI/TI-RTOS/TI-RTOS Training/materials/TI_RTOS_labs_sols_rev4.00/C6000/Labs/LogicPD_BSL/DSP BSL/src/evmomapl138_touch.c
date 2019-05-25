//-----------------------------------------------------------------------------
// \file    evmomapl138_touch.c
// \brief   implementation of the touch driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_led.h"
#include "evmomapl138_pmic.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_lcd_raster.h"
#include "evmomapl138_touch.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// \brief   Initialize the Touch Screen feature of the PMIC
//
// \param   none.
//
// \return  none.
//-----------------------------------------------------------------------------
uint32_t TOUCH_init(void)
{
	uint32_t	results = 0;
	uint8_t		temp;

	results = PMIC_init();
	if (results != ERR_NO_ERROR)
	{
		printf("error initializing pmic!\r\n");
		return (results);
	}

	// Activate the Touch interrupt
	PMIC_writeRegister(INT, PMIC_TOUCH_EN);

	// Put the PMIC touch controler in standby mode
	PMIC_writeRegister(TSCMODE, TSC_STANDBY);

	// Clear the initial interrupts
	results = 0;
	PMIC_readRegister(INT, &temp);
	while ((temp & PMIC_INT_TSC) != 0)
	{
		printf("\r\n Clearing initial touch interrupt:");
		USTIMER_delay(500000);
		PMIC_readRegister(INT, &temp);
		results++;
	}

	// Put the PMIC touch controler back in standby mode
	PMIC_writeRegister(TSCMODE, TSC_STANDBY);

	// Fire-up the LCD driver.
	RASTER_init();

	// Clear the LCD
	RASTER_clear(COLOR_WHITE);

	//Plot the initial cursor
	TOUCH_plotCursor(LCD_WIDTH / 2, LCD_HEIGHT / 2);

	return 0;
}

//-----------------------------------------------------------------------------
// \brief   Read a selected PMIC Touch ADC axis.
//
// \return  none.
//-----------------------------------------------------------------------------
void TOUCH_readAxis(uint8_t modeVal, uint8_t * p_resultL, uint8_t * p_resultH)
{
	uint32_t rtn;
	uint8_t temp;

	//Set the Mode
	PMIC_writeRegister(TSCMODE, modeVal);

	// Enable the ADC
	rtn		= PMIC_readRegister(ADCONFIG, &temp);
	temp	|= PMIC_AD_ENABLE;
	PMIC_writeRegister(ADCONFIG, temp);


	//Select DAC input
	rtn		= PMIC_readRegister(ADCONFIG, &temp);
	temp	&= ~PMIC_AD_INPUT_MASK;
	temp	|= PMIC_TSC_AD_INPUT;
	PMIC_writeRegister(ADCONFIG, temp);


	// Start Covnertsion
	rtn		= PMIC_readRegister(ADCONFIG, &temp);
	temp	|= PMIC_AD_START;
	PMIC_writeRegister(ADCONFIG, temp);


	//Wait for done
	do
	{
		rtn	= PMIC_readRegister(ADCONFIG, &temp);
	}while ((temp & PMIC_AD_START) != 0);

	// Read the Results
	rtn = PMIC_readRegister(ADRESULT1, p_resultL);
	rtn = PMIC_readRegister(ADRESULT2, p_resultH);	
	
	// This line is added to get rid of a compiler warning.
	rtn += 1;
}

//-----------------------------------------------------------------------------
// \brief   Draw the touch test cursor at the desired location.
//
// \return  none.
//-----------------------------------------------------------------------------
void TOUCH_plotCursor(uint16_t x, uint16_t y)
{
	uint8_t i, northLen, southLen, eastLen, westLen;

	westLen = TOUCH_CURSOR_LINE_LEN;
	if (x < TOUCH_CURSOR_LINE_LEN)
	{
		westLen = x + 1;
	}

	eastLen = TOUCH_CURSOR_LINE_LEN;
	if (x > (LCD_WIDTH - TOUCH_CURSOR_LINE_LEN))
	{
		eastLen = LCD_WIDTH - x;
	}

	northLen = TOUCH_CURSOR_LINE_LEN;
	if (y < TOUCH_CURSOR_LINE_LEN)
	{
		southLen = y + 1;
	}

	southLen = TOUCH_CURSOR_LINE_LEN;
	if (y > (LCD_HEIGHT - TOUCH_CURSOR_LINE_LEN))
	{
		northLen = LCD_HEIGHT - y;
	}

	for (i = 0; i < westLen; i++)
	{
		RASTER_plot(x - i, y, COLOR_BLACK);
	}

	for (i = 0; i < eastLen; i++)
	{
		RASTER_plot(x + i, y, COLOR_BLACK);
	}

	for (i = 0; i < northLen; i++)
	{
		RASTER_plot(x, y + i, COLOR_BLACK);
	}

	for (i = 0; i < southLen; i++)
	{
		RASTER_plot(x, y - i, COLOR_BLACK);
	}
}



//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
