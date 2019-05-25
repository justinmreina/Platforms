//-----------------------------------------------------------------------------
// \file    evmomapl138_usb.h
// \brief   OMAP-L138 usb host and otg registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_TOUCH_H
#define EVMOMAPL138_TOUCH_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
#define TSC_STANDBY		0x05
#define TSC_XPOS		0x00
#define TSC_YPOS		0x01
#define TSC_PRESSURE	0x02

#define PMIC_AD_INPUT_MASK		0x0F

#define PMIC_TSC_AD_INPUT		0x0E

#define PMIC_AD_ENABLE			0x80
#define PMIC_AD_START			0x40
#define PMIC_INT_TSC			0x08
#define PMIC_TOUCH_EN			0x40

#define DEFAULT_XSCALE			0.5096
#define DEFAULT_YSCALE			0.3166

#define DEFAULT_XOFFSET			-20
#define DEFAULT_YOFFSET			-20

#define TOUCH_HOLD_LIMIT		0x000F

#define TOUCH_CURSOR_LINE_LEN		20


//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t TOUCH_init(void);
void TOUCH_plotCursor(uint16_t x, uint16_t y);
void TOUCH_readAxis(uint8_t modeVal, uint8_t * p_resultL, uint8_t * p_resultH);


#endif






