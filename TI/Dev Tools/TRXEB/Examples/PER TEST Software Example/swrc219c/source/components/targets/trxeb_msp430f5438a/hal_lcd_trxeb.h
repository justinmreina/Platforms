/******************************************************************************
  Filename:     hal_lcd_trxeb.h

  Description:  hal header for trxeb lcd module, EA DOGM128 LCD

******************************************************************************/

#ifndef HAL_LCD_TRXEB_H
#define HAL_LCD_TRXEB_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include <msp430.h>

/******************************************************************************
 * CONSTANTS
 */

/* Display characteristics */
#define LCD_PIXELS    8192
#define LCD_BYTES     1024
#define LCD_COLS      128
#define LCD_ROWS      64
#define LCD_PAGES     8
#define LCD_PAGE_ROWS 8

/* Font characteristics
 * CHAR_WIDTH: The space the display uses for each character
 * FONT_WIDTH: The actual width of the font's characters
 * The difference is the spacing between characters
 */
#define LCD_CHAR_WIDTH    6
#define LCD_FONT_WIDTH    5

/* Port to use for display */
#define LCD_PORT_OUT_CS_A0    P9OUT
#define LCD_PORT_DIR_CS_A0    P9DIR
#define LCD_PORT_SEL_CS_A0    P9SEL

#define LCD_PORT_OUT_RST      P7OUT
#define LCD_PORT_DIR_RST      P7DIR
#define LCD_PORT_SEL_RST      P7SEL

#define LCD_PORT_SEL_SPI      P9SEL

#define LCD_PORT_OUT_POWER    P7OUT
#define LCD_PORT_DIR_POWER    P7DIR
#define LCD_PORT_SEL_POWER    P7SEL
#define LCD_PORT_DS_POWER     P7DS

/* Signal to pin mapping (SPI and GPIO) */
#define LCD_CS     6  /* Chip Select (GPIO)                           */
#define LCD_SI     1  /* Slave Input (SPI MOSI)                       */
#define LCD_SO     2  /* Slave Output (SPI MISO), not in use          */
#define LCD_SCL    3  /* SPI Clock                                    */
#define LCD_A0     7  /* Controls if bit-stream is data/command (GPIO)*/
#define LCD_RST    3  /* Reset LCD (GPIO)                             */
#define LCD_POWER  7  /* LCD power (GPIO)                             */

/* Condition for SPI TX-buffer to be ready */
#define TX_BUF_READY (UCB2IFG & UCTXIFG)

enum{NO_MOTION = 0, SLIDE_RIGHT = 1, SLIDE_LEFT = 2};

/*******************************************************************************
 * Extern variables
 */
extern char halLcdBuffer[LCD_BYTES];


/*******************************************************************************
 * FUNCTION PROTOTYPES
 */

void halLcdInit(void);
void halLcdUninit(void);
void halLcdSpiInit(void);
void halLcdGotoXY(char x,char page);
void halLcdSetContrast(char contrast);

void halLcdPrintString(char *pBuffer, char *pStr,char x,char page);
void halLcdPrintStringCentered(char *pBuffer, char *pStr,char page);
void halLcdPrintInt(char *pBuffer, int32 number,char x,char page);
void halLcdPrintFloat(char *pBuffer, float number, char numOfDecimals, char x, char page);

uint8 halLcdStringLength(char *pStr);
uint8 halLcdIntLength(int32 number);
uint8 halLcdFloatLength(float number, char numOfDecimals);

void halLcdInvertPage(char *pBuffer,char x_from, char x_to, char page);
void halLcdInvert(char *pBuffer,char x_from, char y_from, char x_to, char y_to);

void halLcdClear(char *pBuffer);
void halLcdClearPage(char *pBuffer,char page);
void halLcdClearBufferPart(char *pBuffer,char xFrom, char xTo, char pageFrom, char pageTo);
void halLcdWriteBufferDirectly(const char *pData, uint16 size);

void halLcdGetBuffer(char *pBuffer);
void halLcdSendBuffer(char *pBuffer);
void halLcdSendBufferPart(char *pBuffer,char xFrom, char xTo, char pageFrom, char pageTo);

void halLcdSendBufferAnimated(char *pToBuffer,char *pFromBuffer,char motion);

void halLcdStr2hex(char *pStr, char *pHex);
void halLcdInt2str(int32 in,char *pOut);

void halLcdSetHLine(char *pBuffer,char x_from,char x_to,char y);
void halLcdClearHLine(char *pBuffer,char x_from,char x_to,char y);
void halLcdSetVLine(char *pBuffer,char x,char y_from,char y_to);
void halLcdClearVLine(char *pBuffer,char x,char y_from,char y_to);
void halLcdLine(char *pBuffer,char x_from,char y_from,char x_to,char y_to);

void halLcdVArrow(char *pBuffer,char x,char y_from,char y_to);
void halLcdHArrow(char *pBuffer,char x_from,char x_to,char y);

void halLcdSetPx(char *pBuffer,char x,char y);
void halLcdClearPx(char *pBuffer,char x,char y);

#ifdef  __cplusplus
}
#endif

/******************************************************************************
  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
*******************************************************************************/


#endif /* HAL_LCD_TRXEB_H */