/**************************************************************************//**
*
* @file     lcd_dogm128.h
*
* @brief    Device driver header file for DOGM128 LCD display.
*           The DOGM128W-6 LCD display is a 128x64 dot matrix and is divided
*           into 8 pages (LCD_PAGE_0 through LCD_PAGE_7), each 8 px high.
*
*           The (x,y) coordinate system
*           used in this device driver is as described below:
*           <pre>
            + ----->   x
            | ***********************************************
            | |(0,0)              PAGE 0             (127,0)|
            V |                   PAGE 1                    |
              |                    ...                      |
            y |                    ...                      |
              |                    ...                      |
              |                    ...                      |
              |                    ...                      |
              |(0,63)             PAGE 7            (127,63)|
              ***********************************************
            </pre>
*
******************************************************************************/
#ifndef LCD_DOGM128_H
#define LCD_DOGM128_H

/******************************************************************************
* INCLUDES
*/
#include "bsp.h"


/******************************************************************************
* DEFINES
*/
// Display characteristics
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

enum {
    LCD_SLIDE_RIGHT = 1,
    LCD_SLIDE_LEFT = 2
};

enum {
    LCD_ALIGN_LEFT = 0,
    LCD_ALIGN_CENTER = 1,
    LCD_ALIGN_RIGHT = 2
};

enum {
    LCD_PAGE_0 = 0,
    LCD_PAGE_1 = 1,
    LCD_PAGE_2 = 2,
    LCD_PAGE_3 = 3,
    LCD_PAGE_4 = 4,
    LCD_PAGE_5 = 5,
    LCD_PAGE_6 = 6,
    LCD_PAGE_7 = 7
};

/******************************************************************************
* EXTERNAL VARIABLES
*/
extern char lcdDefaultBuffer[LCD_BYTES];


/******************************************************************************
* FUNCTION PROTOTYPES
*/
void lcdInit(void);

// Functions accessing LCD
void lcdClear(void);
void lcdGotoXY(char x, char y);
void lcdSendCommand(char *pCmd, char length);
void lcdSendData(char *pData, unsigned short length);
void lcdSendBuffer(char *pBuffer);
void lcdSendBufferPart(char *pBuffer, char xFrom, char xTo, char pageFrom, char pageTo);
void lcdSendBufferAnimated(char *pToBuffer, char *pFromBuffer, char motion);

// Buffer manipulation functions
void lcdBufferClear(char *pBuffer);
void lcdBufferClearPage(char *pBuffer, char page);
void lcdBufferClearPart(char *pBuffer, char xFrom, char xTo, char pageFrom, char pageTo);
void lcdBufferInvert(char *pBuffer, char x_from, char y_from, char x_to, char y_to);
void lcdBufferInvertPage(char *pBuffer, char x_from, char x_to, char page);
unsigned char lcdGetStringLength(char *pStr);
unsigned char lcdGetIntLength(signed long number);
unsigned char lcdGetFloatLength(float number, char numOfDecimals);
void lcdBufferPrintString(char *pBuffer, char *pStr, char x, char page);
void lcdBufferPrintStringAligned(char *pBuffer, char *pStr, char alignment, char page);
void lcdBufferPrintInt(char *pBuffer, signed long number, char x, char page);
void lcdBufferPrintIntAligned(char *pBuffer, signed long number, char alignment, char page);
void lcdBufferPrintFloat(char *pBuffer, float number, char numOfDecimals, char x, char page);
void lcdBufferPrintFloatAligned(char *pBuffer, float number, char numOfDecimals, char alignment, char page);
void lcdBufferSetLine(char *pBuffer, char x_from, char y_from, char x_to, char y_to);
void lcdBufferClearLine(char *pBuffer, char x_from, char y_from, char x_to, char y_to);
void lcdBufferSetHLine(char *pBuffer, char x_from, char x_to, char y);
void lcdBufferClearHLine(char *pBuffer, char x_from, char x_to, char y);
void lcdBufferSetVLine(char *pBuffer, char x, char y_from, char y_to);
void lcdBufferClearVLine(char *pBuffer, char x, char y_from, char y_to);
void lcdBufferHArrow(char *pBuffer, char x_from, char x_to, char y);
void lcdBufferVArrow(char *pBuffer, char x, char y_from, char y_to);
void lcdBufferSetPx(char *pBuffer, char x, char y);
void lcdBufferClearPx(char *pBuffer, char x, char y);

#endif /* #ifndef LCD_DOGM128_H */