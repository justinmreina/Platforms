/******************************************************************************
  Filename:     hal_lcd_trxeb.h

  Description:  hal for trxeb lcd module, EA DOGM128 LCD

******************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include  <msp430.h>
#include "hal_lcd_trxeb.h"
#include "hal_timer_32k.h"
#include "alphabet.c"
#include "hal_types.h"

/******************************************************************************
 * CONSTANTS
 */

/* LCD initialization command sequence */
static const char init[] = {0x40,    /*Display start line 0                    */
                            0xa1,    /*ADC reverse, 6 oclock viewing direction */
                            0xc0,    /*Normal COM0...COM63                     */
                            0xa6,    /*Display normal, not mirrored            */
                            0xa2,    /*Set Bias 1/9 (Duty 1/65)                */
                            0x2f,    /*Booster, Regulator and Follower On      */
                            0xf8,    /*Set internal Booster to 4x              */
                            0x00,    /*                                        */
                            0x27,    /*Contrast set                            */
                            0x81,    /*                                        */
                            0x16,    /* <- use value from LCD-MODULE .doc guide*/
                                     /*    for better contrast (not 0x10)      */
                            0xac,    /*No indicator                            */
                            0x00,    /*                                        */
                            0xaf,    /*Display on                              */
                            0xb0,    /*Page 0 einstellen                       */
                            0x10,    /*High-Nibble of column address           */
                            0x00     /*Low-Nibble of column address            */
                           };
/******************************************************************************
 * LOCAL VARIABLES
 */

char halLcdBuffer[LCD_BYTES];

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

static void halLcdSendArray(char* pArray, uint16 size);
static void halLcdSendCommand(char* pCommand, char size);
static void halLcdSendData(char* pData, uint16 size);

/******************************************************************************
 * @fn          halLcdSendArray
 *
 * @brief       Sends an array of length i of either data or commands (depending
 *              on A0) on SPI to the LCD controller. Used as in the
 *              functions halLcdSendCommand and halLcdSendData.
 *
 * input parameters
 *
 * @param       pArray  - The array of data or commands to be sent to the LCD
 * @param       size    - Number of elements/bytes in pArray
 *
 * output parameters
 *
 * @return      none
 */
static void halLcdSendArray(char* pArray, uint16 size)
{
  while(size)
  {
    while (!TX_BUF_READY);     /* Transmit buffer ready?                   */
    UCB2TXBUF = *pArray;       /* Send data control                        */
    pArray++;                  /* Increment pointer to data                */
    size--;                    /* Decrement bytes left                     */
  }                            /*                                          */
  while (!TX_BUF_READY);       /* Wait for transmit buffer to become empty */
}

/******************************************************************************
 * @fn          halLcdSendCommand
 *
 * @brief       Sends an array of length size of commands to the LCD
 *              controller. Used in several halLcd functions.
 *
 * input parameters
 *
 * @param       pCommand  - The array of commands to be sent to the LCD
 * @param       size      - Number of elements/bytes in command
 *
 * output parameters
 *
 * @return      none
 */
static void halLcdSendCommand(char* pCommand, char size)
{
  LCD_PORT_OUT_CS_A0 &= ~(1<<LCD_CS);    /* Chip Select LOW   */
  LCD_PORT_OUT_CS_A0 &= ~(1<<LCD_A0);    /* A0 LOW => command */
  halLcdSendArray(pCommand, size);       /* Send command      */
  LCD_PORT_OUT_CS_A0 |=  (1<<LCD_CS);    /* Chip Select HIGH  */
}

/******************************************************************************
 * @fn          halLcdSendData
 *
 * @brief       Sends an array of length i of data to be displayed on the LCD.
 *              Used in several halLcd functions.
 *
 * input parameters
 *
 * @param       data  - The array of data to be displayed on the LCD
 * @param       size  - Number of elements in data
 *
 * output parameters
 *
 * @return      none
 */
static void halLcdSendData(char* pData, uint16 size)
{
  LCD_PORT_OUT_CS_A0 &= ~(1<<LCD_CS);    /* Chip Select LOW       */
  LCD_PORT_OUT_CS_A0 |=  (1<<LCD_A0);    /* A0 HIGH => data       */
  halLcdSendArray(pData, size);          /* Send data             */
  LCD_PORT_OUT_CS_A0 &= ~(1<<LCD_A0);    /* A0 LOW => default low */
  LCD_PORT_OUT_CS_A0 |=  (1<<LCD_CS);    /* Chip Select HIGH      */
}


/******************************************************************************
 * @fn          halLcdInit
 *
 * @brief       Initializes the SPI interface, the port used for the display and
 *              the display itself. This must be run before you can use the LCD.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halLcdInit(void)
{

  halLcdSpiInit();  // Initialize SPI interface (USCI controller + MOSI, CLK pins)

  /* IO pins */
  LCD_PORT_SEL_CS_A0 &= ~((1<<LCD_CS) | (1<<LCD_A0));
  LCD_PORT_SEL_RST   &= ~(1<<LCD_RST);
  LCD_PORT_SEL_POWER &= ~(1<<LCD_POWER);

  /* Output */
  LCD_PORT_DIR_CS_A0 |=   (1<<LCD_CS) | (1<<LCD_A0);
  LCD_PORT_DIR_RST   |=   (1<<LCD_RST);
  LCD_PORT_DIR_POWER |=   (1<<LCD_POWER);

  /* set LCD_POWER=1 with high drive strength */
  LCD_PORT_OUT_POWER |=   (1<<LCD_POWER);
  LCD_PORT_DS_POWER  |=   (1<<LCD_POWER);

  /* set RSTn=0, A0=1, CSn=1 */
  LCD_PORT_OUT_CS_A0 |=   (1<<LCD_CS) | (1<<LCD_A0);
  LCD_PORT_OUT_RST   &=   ~(1<<LCD_RST);

  __delay_cycles(1600000);  /* wait ~ 100 ms (@ 16 MHz) */
  LCD_PORT_OUT_RST   |=   (1<<LCD_RST);

  halLcdSendCommand((char*)init, sizeof(init));       /* Send init commands */
}

/******************************************************************************
 * @fn          halLcdUninit
 *
 * @brief       Uninitializes the SPI interface, the port used for the display and
 *              the display itself.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halLcdUninit(void)
{
    /* Uninitialize SPI interface */
    LCD_PORT_DIR_CS_A0 |= (1<<LCD_CS);                  // CSn as output high
    UCB2CTL1 |= UCSWRST;                                // Disable SPI controller
    LCD_PORT_SEL_SPI &= ~((1<<LCD_SI) | (1<<LCD_SCL));  // SI, SCL as GPIO

    /* Turn off LCD power */
    LCD_PORT_OUT_POWER &= ~(1<<LCD_POWER); // Pull low
    LCD_PORT_DIR_POWER &= ~(1<<LCD_POWER); // Input
    LCD_PORT_DS_POWER  &= ~(1<<LCD_POWER); // Back to low drive strength

    /* Uninitialize LCD reset pin */
    LCD_PORT_OUT_RST   &= ~(1<<LCD_RST);    // Pull low
    LCD_PORT_DIR_RST   &= ~(1<<LCD_RST);    // Input


    /* Uninitialize LCD mode pin (A0) */
    LCD_PORT_DIR_CS_A0 &= ~(1<<LCD_A0);     // Input

} // halLcdUninit


/******************************************************************************
 * @fn          halLcdSpiInit
 *
 * @brief       Initializes the LCD SPI interface. Quicker for reconfiguring
 *              the SPI interface than halLcdInit() if the LCD display has
 *              already been initialized.
 *
 * input parameters
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSpiInit(void)
{

  /* Configure USCI B2 for SPI master */
  UCB2CTL1 |= UCSWRST;                          /* Disable SPI interface        */
  UCB2CTL0  = UCSYNC | UCMST | UCCKPL | UCMSB;  /* Synchronous mode, 3-pin SPI, */
                                                /* master, 8-bit, MSB first,    */
                                                /* clock pol=1(inactive=high),  */
                                                /* cloch pha=0(setup=first edge)*/
  UCB2CTL1 |= UCSSEL1 | UCSSEL0;                /* Set clock source = 11 = SMCLK*/

  UCB2BR0  = 1;   /* Bit rate control registers (set clock divider = 1)  */
  UCB2BR1  = 0;   /* The maximal clock speed for the LCD is 20MHz        */

  /* Configure ports and pins */
  /* SPI pins */
  LCD_PORT_SEL_SPI |=   (1<<LCD_SI) | (1<<LCD_SCL);

  UCB2CTL1 &= ~UCSWRST; /* Enable SPI interface */

} // halLcdSpiInit


/******************************************************************************
 * @fn          halLcdGotoXY
 *
 * @brief       Set the cursor on a specified place on the display.
 *              When data is sent the data will start printing where the cursor
 *              is.
 *
 * input parameters
 *
 * @param x     Cursor column (possible values: 0 - 127)
 * @param y     Cursor page (possible values: 0 - 7), 8px per page
 *
 * output parameters
 *
 * @return      none
 */
void halLcdGotoXY(char x, char y)
{
  char set[] = {0xb0, 0x10, 0x00}; /* Command array                           */
  set[0] = set[0] + y;             /* Add Y position to command               */
  set[2] = set[2] + (x & 0x0f);    /* Add X position (low nibble) to command  */
  set[1] = set[1] + (x >> 4);      /* Add X position (high nibble) to command */
  halLcdSendCommand(set, 3);
}

/******************************************************************************
 * @fn          halLcdSetContrast
 *
 * @brief       Sets the displays contrast
 *
 * input parameters
 *
 * @param       contrast  - Contrast (possible values: 0-63)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSetContrast(char contrast)
{
  char set[] = {0x81, 0x00};  /* Command from data sheet                         */
  set[1] += (contrast & 0x3f);/* Limit contrast value if out of bounds (max=0x3f)*/
  halLcdSendCommand(set, 2);  /* Send command                                    */
}

/******************************************************************************
 * @fn          halLcdPrintString
 *
 * @brief       Prints a text string on the display at specified column and
 *              page.
 *
 * input parameters
 *
 * @param       pBuffer - Pointer to the output buffer
 * @param       pStr    - Pointer to the string to be printed
 * @param       x       - The x-position (column) to begin printing (0-127)
 * @param       page    - The page to print on (0-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdPrintString(char *pBuffer, char *pStr,char x,char page)
{
  uint8 strSize = halLcdStringLength(pStr);
  uint16 firstPos = page*LCD_COLS+x;

  /* Running through each letter in input string */
  for(uint8 i=0;i<strSize;i++)
  {
    char *pBuf; /* Pointer to output buffer */
    pBuf = halLcdBuffer;
    if(pBuffer) pBuf = pBuffer; /* pBuf points to pBuffer if set */
    if(pStr[i]==' ')
    {
      for(uint8 j=0;j<LCD_CHAR_WIDTH;j++){
        *(pBuf + (firstPos+LCD_CHAR_WIDTH*i+j)) = 0x00; /* Space character */
      }
    }
    else
    {
      /* The index to the beginning of the current letter in alphabet[] */
      uint16 firstIndex = ((uint16)(pStr[i])-33)*LCD_FONT_WIDTH;

      /* Stores each vertical column of the current letter in the result */
      for(uint8 j=0;j<LCD_FONT_WIDTH;j++){
        *(pBuf + (firstPos+LCD_CHAR_WIDTH*i+j)) = alphabet[firstIndex+j];
      }
      *(pBuf + (firstPos+LCD_CHAR_WIDTH*i+LCD_FONT_WIDTH)) = 0x00; /* Spacing after letter */
    }
  }
}

/******************************************************************************
 * @fn        halLcdPrintCentered
 *
 * @brief     Prints a text string in the middle of the specified page.
 *
 * input parameters
 *
 * @param     pBuffer - Pointer to target buffer
 * @param     pStr    - Pointer to the string to be printed
 * @param     page    - The page to print on (0-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdPrintStringCentered(char *pBuffer, char *pStr,char page)
{
  uint8 strSize = halLcdStringLength(pStr);
  uint8 x = LCD_COLS/2-strSize*LCD_CHAR_WIDTH/2;
  halLcdPrintString(pBuffer,pStr,x,page);
}

/******************************************************************************
 * @fn          halLcdPrintInt
 *
 * @brief       Prints a number of datatype int on the display at specified
 *              column and page. Use this function instead of performing a
 *              int to c-string conversion and then using halLcdPrint.
 *
 * input parameters
 *
 * @param       pBuffer - Pointer to target buffer
 * @param       number  - The number to be printed
 * @param       x       - The x-position (column) to begin printing (0-127)
 * @param       page    - The page to print on (0-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdPrintInt(char *pBuffer, int32 number,char x,char page){

  uint16 firstPos = page*LCD_COLS+x;
  char *pBuf;
  pBuf = halLcdBuffer; /* pBuf points to halLcdBuffer[0] */
  if(pBuffer) pBuf = pBuffer;

  /* if number is negative:
   * write a minus at first position, increment first position by one character
   * and multiply number by (-1).
   */
  if(number<0){
    for(uint8 j=0;j<LCD_FONT_WIDTH;j++)
    {
      *(pBuf + (firstPos + j)) = alphabet[12*LCD_FONT_WIDTH+j];
    }
    *(pBuf + (firstPos + LCD_FONT_WIDTH)) = 0x00;   /* Spacing */
    firstPos += LCD_CHAR_WIDTH;
    number *= (-1);
  }

  /* Finding numbers of digits in number (except for minus character) */
  int8 numOfDigits=halLcdIntLength(number);

  /* running through each digit, starting with MSD, and writing to buffer */
  for(int8 i=numOfDigits-1;i>=0;i--){
    int32 temp = number/10;
    int32 digit = number-temp*10;
    int32 firstIndex = (digit+15)*LCD_FONT_WIDTH;
    for(uint8 j=0;j<LCD_FONT_WIDTH;j++)
    {
      *(pBuf + (firstPos + LCD_CHAR_WIDTH*i + j)) = alphabet[firstIndex+j];
    }
    *(pBuf + (firstPos + LCD_CHAR_WIDTH*i + LCD_FONT_WIDTH)) = 0x00;  /* Spacing */

    number = temp;
  }
}

/******************************************************************************
 * @fn          halLcdPrintDouble
 *
 * @brief       Prints a number of datatype float on the display at specified
 *              column and page. Use this function instead of performing a
 *              float to c-string conversion and then using halLcdPrint.
 *
 * input parameters
 *
 * @param       pBuffer       - Pointer to target buffer if not default
 * @param       number        - The number to be printed
 * @param       numOfDecimals - The number of decimals to be printed, MAX = 10
 * @param       x             - The x-position (column) to begin printing (0-127)
 * @param       page          - The page to print on (0-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdPrintFloat(char *pBuffer, float number, char numOfDecimals, char x,char page)
{

  int32 integerPart;
  uint8 decimalArray[11];
  uint8 numNeg = 0;
  char *pBuf;

  pBuf = halLcdBuffer; /* pBuf initially points to the default LCD buffer. */
  /* Checking if pBuffer is set, in that case pBuf should point to pBuffer */
  if(pBuffer) pBuf = pBuffer;

  float threshold = -0.5;          /* The threshold which defines how low a    */
  for(uint8 i=0;i<numOfDecimals;i++)/* float must be to be considered negative.*/
  {                                 /* I.e. if a float is -0.001 and the number*/
    threshold *= 0.1;               /* of decimals is 2 then the number will be */
  }                                 /* considered as 0.00 and not -0.00.        */
  if(number <= threshold)
  {
   number *= -1;
   numNeg = 1;
  }

  /* Extracting integer part */
  integerPart = (int32)number;

  /* Storing numOfDecimals+1 decimals in an array */
  int32 integer1;
  for(uint8 i=0;i<numOfDecimals+1; i++)
  {
    number   = number*10;
    integer1 = (int32)number;
    integer1 = integer1 % 10;
    decimalArray[i] = integer1;
  }

  /* Perform upwards rounding: This can correct the truncation error that the
   * MSP does when passing a float argument that is generated by division. ex:
   * (59/100)*100 = 58.9999961. If printing with 2 decimals, this will give
   * 59.00. This also indicates that many decimals should not be used...
   */
  uint8 roundUp;
  if(decimalArray[numOfDecimals] > 4)
  {
    roundUp = 1;
    for(int8 i=numOfDecimals-1; i>=0; i--)
    {
       decimalArray[i] = decimalArray[i] + roundUp;
       if(decimalArray[i] == 10)
       {
         decimalArray[i] = 0;
       }
       else
       {
         roundUp = 0;
       }
    }
    if(roundUp == 1)
    {
      integerPart++;
    }
  }
  /* Printing the decimal number to screen
   * - print sign if applicable
   * - print integer number
   * - print comma
   * - print number of decimals
   */
  if(numNeg == 1)
  {
    halLcdPrintString(pBuf,"-",x,page);
    x += LCD_CHAR_WIDTH;
  }
  halLcdPrintInt(pBuf, integerPart,x,page);
  x += halLcdIntLength(integerPart)*LCD_CHAR_WIDTH;
  halLcdPrintString(pBuf,".",x,page);
  x += LCD_CHAR_WIDTH;
  for(uint8 i = 0; i< numOfDecimals; i++)
  {
    halLcdPrintInt(pBuf,decimalArray[i],x,page);
    x += LCD_CHAR_WIDTH;
  }

}

/******************************************************************************
 * @fn          halLcdClear
 *
 * @brief       Empties the provided screen buffer
 *
 * input parameters
 *
 * @param       pBuffer - Pointer to target buffer
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClear(char *pBuffer)
{
  char *pBuf;
  pBuf = halLcdBuffer;
  if(pBuffer) pBuf = pBuffer;
  for(uint16 i=0;i<LCD_BYTES;i++)
  {
    *(pBuf + i) = 0x00;
  }
}

/******************************************************************************
 * @fn          halLcdSetHLine
 *
 * @brief       Writes a horizontal line from (x_from,y) to (x_to,y)
 *
 * input parameters
 *
 * @param       pBuffer - Pointer to target buffer
 * @param       x_from  - x start/from
 * @param       x_to    - x end/to
 * @param       y       - display row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSetHLine(char *pBuffer,char x_from,char x_to,char y)
{
  char page = y/LCD_PAGE_ROWS;
  char bit  = y%LCD_PAGE_ROWS;
  char bitmask = 1<<bit;

  char *pBuf = halLcdBuffer;  /* Default buffer is halLcdBuffer       */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer is set, pBuf points to it */

  for(char i=x_from;i<=x_to;i++)
  {
    *(pBuf + (page*LCD_COLS+i)) |= bitmask;
  }
}

/******************************************************************************
 * @fn          halLcdSetVLine
 *
 * @brief       Writes a vertical line from (x,y_from) to (x,y_to)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x        - display column
 * @param       y_from   - display row start
 * @param       y_to     - display row end
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSetVLine(char *pBuffer, char x,char y_from,char y_to)
{

  char firstPage = y_from/LCD_PAGE_ROWS;
  char lastPage = y_to/LCD_PAGE_ROWS;
  char *pBuf = halLcdBuffer; /* Pointer to destination buffer (default val.) */

  if(pBuffer) pBuf = pBuffer; /* If pBuffer is set, pBuf should point to it */

  /*  Finds the bitmask to use with the first page */
  uint8 firstPageMask = 0xFF;
  uint8 pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_from-firstPage*LCD_PAGE_ROWS>i)
    {
      firstPageMask -= pow;
      pow *= 2;
    }
  }

  /* Finds the bitmask to use with the last page */
  uint8 lastPageMask = 0x00;
  pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_to-lastPage*LCD_PAGE_ROWS>=i)
    {
      lastPageMask += pow;
      pow *= 2;
    }
  }

  if(lastPage==firstPage)
  {
    firstPageMask &= lastPageMask;
    lastPageMask = firstPageMask;
  }
  *(pBuf + (firstPage*LCD_COLS+x)) |= firstPageMask;
  for(uint8 page=firstPage+1;page<=lastPage-1;page++)
  {
    *(pBuf + (page*LCD_COLS+x)) |= 0xFF;
  }
  *(pBuf + (lastPage*LCD_COLS+x)) |= lastPageMask;
}

/******************************************************************************
 * @fn          halLcdClearVLine
 *
 * @brief       Clears a vertical line from (x,y_from) to (x,y_to)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x        - column that will have parts cleared
 * @param       y_from   - start row
 * @param       y_to     - end row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClearVLine(char *pBuffer,char x,char y_from,char y_to)
{
  char firstPage = y_from/LCD_PAGE_ROWS;
  char lastPage = y_to/LCD_PAGE_ROWS;
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer is set, pBuf points to it */

  /* Finds the bitmask to use with the first page */
  uint8 firstPageMask = 0xFF;
  uint8 pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_from-firstPage*LCD_PAGE_ROWS>i)
    {
      firstPageMask -= pow;
      pow *= 2;
    }
  }

  /* Finds the bitmask to use with the last page */
  uint8 lastPageMask = 0x00;
  pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_to-lastPage*LCD_PAGE_ROWS>=i)
    {
      lastPageMask += pow;
      pow *= 2;
    }
  }

  if(lastPage==firstPage)
  {
    firstPageMask &= lastPageMask;
    lastPageMask = firstPageMask;
  }

  *(pBuf + (firstPage*LCD_COLS+x)) &= ~firstPageMask;
  for(uint8 page=firstPage+1;page<=lastPage-1;page++)
  {
    *(pBuf + (page*LCD_COLS+x)) &= 0x00;
  }
  *(pBuf + (lastPage*LCD_COLS+x)) &= ~lastPageMask;
}

/******************************************************************************
 * @fn          halLcdSetPx
 *
 * @brief       Sets a pixel on (x,y)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x        - Column
 * @param       y        - Row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSetPx(char *pBuffer,char x,char y)
{
  char page = y/LCD_PAGE_ROWS;
  char bit  = y%LCD_PAGE_ROWS;
  char bitmask = 1<<bit;
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */

  *(pBuf + (page*LCD_COLS+x)) |= bitmask;
}

/******************************************************************************
 * @fn          halLcdClearPx
 *
 * @brief       Clears a pixel on (x,y)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x        - Column
 * @param       y        - Row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClearPx(char *pBuffer,char x,char y)
{
  char page = y/LCD_PAGE_ROWS;
  char bit  = y%LCD_PAGE_ROWS;
  char bitmask = 1<<bit;
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */
  *(pBuf + (page*LCD_COLS+x)) &= ~bitmask;
}

/******************************************************************************
 * @fn          halLcdClearHLine
 *
 * @brief       Clears a horizontal line from (x_from,y) to (x_to,y)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x_from   - Column start
 * @param       x_to     - Column end
 * @param       y        - Row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClearHLine(char *pBuffer,char x_from,char x_to,char y)
{
  char page = y/LCD_PAGE_ROWS;
  char bit  = y%LCD_PAGE_ROWS;
  char bitmask = 1<<bit;
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */
  for(char i=x_from;i<=x_to;i++)
  {
    *(pBuf + (page*LCD_COLS+i)) &= ~bitmask;
  }
}

/******************************************************************************
 * @fn          halLcdHArrow
 *
 * @brief       Draws an horizontal arrow from (x_from,y) to (x_to,y)
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x_from   - Column start
 * @param       x_to     - Column end
 * @param       y        - Row
 *
 * output parameters
 *
 * @return      none
 */
void halLcdHArrow(char *pBuffer,char x_from,char x_to,char y)
{
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */

  if(x_to>x_from)
  {
    halLcdSetHLine(pBuf,x_from,x_to,y);
    halLcdSetVLine(pBuf,x_to-1,y-1,y+1);
    halLcdSetVLine(pBuf,x_to-2,y-2,y+2);
  }
  else if(x_to<x_from)
  {
    halLcdSetHLine(pBuf,x_to,x_from,y);
    halLcdSetVLine(pBuf,x_to+1,y-1,y+1);
    halLcdSetVLine(pBuf,x_to+2,y-2,y+2);
  }
}

/******************************************************************************
 * @fn          halLcdLine
 *
 * @brief       Draws a line from (x_from,y_from) to (x_to,y_to)
 *              Uses Bresenham's line algorithm
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x_from   - Column from
 * @param       x_to     - Column to
 * @param       y_from   - Row from
 * @param       y_to     - Row to
 *
 * output parameters
 *
 * @return      none
 */
void halLcdLine(char *pBuffer,char x_from,char y_from,char x_to,char y_to)
{
  int8 x, y, deltay, deltax, d;
  int8 x_dir, y_dir;
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */

  if(x_from==x_to)          /* Vertical Line */
  {
    halLcdSetVLine(pBuf,x_from,y_from,y_to);
  }
  else if(y_from==y_to)     /* Horizontal Line */
  {
    halLcdSetHLine(pBuf,x_from,x_to,y_from);
  }
  else                      /* Diagonal Line => Bresenham's algorithm */
  {

    if (x_from > x_to)  x_dir = -1;
    else                x_dir = 1;
    if (y_from > y_to)  y_dir = -1;
    else                y_dir = 1;

    x = x_from;
    y = y_from;

    deltay = y_to - y_from;
    deltax = x_to - x_from;
    if(deltay<0)  deltay *= -1;   /* absolute value */
    if(deltax<0)  deltax *= -1;   /* absolute value */

    if (deltax >= deltay)
    {
      d = (deltay << 1) - deltax;
      while (x != x_to)
      {
        halLcdSetPx(pBuf,x,y);
        if ( d < 0 )
          d += (deltay << 1);
        else
        {
          d += ((deltay - deltax) << 1);
          y += y_dir;
        }
        x += x_dir;
      }
    }
    else
    {
      d = (deltax << 1) - deltay;
      while (y != y_to)
      {
        halLcdSetPx(pBuf,x,y);
        if ( d < 0 )
          d += (deltax << 1);
        else
        {
          d += ((deltax - deltay) << 1);
          x += x_dir;
        }
        y += y_dir;
      }
    }
  }
}

/******************************************************************************
 * @fn          halLcdVArrow
 *
 * @brief       Draws a vertical arrow from (x,y_from) to (x,y_to)
 *
 * input parameters
 *
 * @param       pBuffer  - The target buffer
 * @param       x        - Column
 * @param       y_from   - Row from
 * @param       y_to     - Row to
 *
 * output parameters
 *
 * @return      none
 */
void halLcdVArrow(char *pBuffer,char x,char y_from,char y_to)
{
  char *pBuf = halLcdBuffer;  /* Default: pBuf points to halLcdBuffer */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it  */
  if(y_to>y_from)
  {
    halLcdSetVLine(pBuf,x,y_from,y_to);
    halLcdSetHLine(pBuf,x-1,x+1,y_to-1);
    halLcdSetHLine(pBuf,x-2,x+2,y_to-2);
  }
  else if(y_to<y_from)
  {
    halLcdSetVLine(pBuf,x,y_to,y_from);
    halLcdSetHLine(pBuf,x-1,x+1,y_to+1);
    halLcdSetHLine(pBuf,x-2,x+2,y_to+2);
  }
}

/******************************************************************************
 * @fn          halLcdClearBufferPart
 *
 * @brief       Clears the pixels in a given piece of a page. Resolution is given
 *              in coulmns [0-127] and pages[0-7]. The function assumes
 *              xFrom <= xTo and pageFrom <= pageTo.
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       xFrom    - The lowest x-position (column) to be cleared (0-127)
 * @param       xTo      - The highest x-position (column) to be cleared (xFrom-127)
 * @param       pageFrom - The page index where clearing will begin (0-7)
 * @param       pageTo   - The page index where clearing will end (pageFrom-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClearBufferPart(char *pBuffer,char xFrom, char xTo, char pageFrom, char pageTo)
{
  uint16 xFirstPos;
  uint8 xRange,yRange;
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */

  /* Expecting yFrom <= yTo */
  xRange = xTo-xFrom;
  /* Expecting yFrom <= yTo */
  yRange = pageTo - pageFrom;

  for(uint8 y=0;y<=yRange;y++)
  {
    xFirstPos = (pageFrom+y)*LCD_COLS+xFrom;
    for(uint8 i=0;i<=xRange;i++)
    {
      *(pBuf + (xFirstPos+i)) = 0x00;
    }
  }
}

/******************************************************************************
 * @fn          halLcdInvertPage
 *
 * @brief       Inverts the pixels in a given piece of a page
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x_from   - The first x-position (column) to be inverted (0-127)
 * @param       x_to     - The last x-position (column) to be inverted (0-127)
 * @param       page     - The page to invert on
 *
 * output parameters
 *
 * @return      none
 */
void halLcdInvertPage(char *pBuffer,char x_from, char x_to, char page)
{

  uint16 firstPos = page*LCD_COLS+x_from;
  uint8 range = x_to-x_from;
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */

  for(uint8 i=0;i<=range;i++)
  {
    *(pBuf + (firstPos+i)) ^= 0xFF;
  }
}

/******************************************************************************
 * @fn          halLcdInvert
 *
 * @brief       Inverts the pixels in a given region of the display
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 * @param       x_from   - The first x-position (column) to be inverted (0-127)
 * @param       y_from   - The first y-position (row) to be inverted (0-63)
 * @param       x_to     - The last x-position (column) to be inverted (0-127)
 * @param       y_to     - The last y-position (row) to be inverted (0-63)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdInvert(char *pBuffer,char x_from, char y_from, char x_to, char y_to)
{
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */

  /* Finds the first and last page to invert on */
  uint8 firstPage = y_from / LCD_PAGE_ROWS;
  uint8 lastPage = y_to / LCD_PAGE_ROWS;

  /* Finds the bitmask to invert with on first page */
  uint8 firstPageMask = 0xFF;
  uint8 pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_from-firstPage*LCD_PAGE_ROWS>i)
    {
      firstPageMask -= pow;
      pow *= 2;
    }
  }

  /* Finds the bitmask to invert with on the last page */
  uint8 lastPageMask = 0x00;
  pow = 1;
  for(uint8 i=0;i<LCD_PAGE_ROWS;i++)
  {
    if(y_to-lastPage*LCD_PAGE_ROWS>=i)
    {
      lastPageMask += pow;
      pow *= 2;
    }
  }

  /* Preventive Action (PA/CA) to prevent error if firstPage==lastPage :-) */
  if(firstPage==lastPage)
  {
    lastPageMask ^= 0xFF;
  }

  /* Inverts the given part of the first page   */
  for(uint8 i=x_from;i<=x_to;i++)
  {
    *(pBuf + (firstPage*LCD_COLS+i)) ^= firstPageMask;
  }
  /* Inverts the pages between first and last in the given section */
  for(uint8 i=firstPage+1;i<=lastPage-1;i++)
  {
    for(uint8 j=x_from;j<=x_to;j++)
    {
      *(pBuf + (i*LCD_COLS+j)) ^= 0xFF;
    }
  }
  /* Inverts the given part of the last page */
  for(uint8 i=x_from;i<=x_to;i++)
  {
    *(pBuf + (lastPage*LCD_COLS+i)) ^= lastPageMask;
  }

}

/******************************************************************************
 * @fn          halLcdSendBuffer
 *
 * @brief       Sends the LCD buffer to the display. None of the other halLcd
 *              functions will actually update the display, only the buffer.
 *
 * input parameters
 *
 * @param       pBuffer  - Pointer to target buffer
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSendBuffer(char *pBuffer)
{
  uint8 page;
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */

  for(page=0; page<8; page++)   /* Page counter */
  {
    halLcdGotoXY(0, page);      /* Set pointer to start of row/page */
    halLcdSendData(pBuf + (page*LCD_COLS), LCD_COLS); /* Send one page */
  }
}

/******************************************************************************
 * @fn          halLcdSendBufferPart
 *
 * @brief       Function will send the specfied part of the LCD buffer to the
 *              corresponding part on the LCD. Function assumes xFrom <= xTo and
 *              pageFrom <= pageTo. Resolution is given in coulmns [0-127]
 *              and pages[0-7].
 *
 *
 * @param       pBuffer  - The buffer to send
 * @param       xFrom    - The lowest x-position (column) to be written (0-127)
 * @param       xTo      - The highest x-position (column) to be written (xFrom-127)
 * @param       pageFrom - The page index where writing will begin (0-7)
 * @param       pageTo   - The page index where writing will end (pageFrom-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSendBufferPart(char *pBuffer,char xFrom, char xTo, char pageFrom, char pageTo){

  uint8 xRange;
  uint8 yOffset,yRange;
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */

  /* Assuming xFrom <= xTo:^*/
  xRange = xTo-xFrom+1;

  /* Assuming pageFrom <= page To: */
  yRange     = pageTo - pageFrom;

  for(uint8 y=0;y<=yRange;y++)
  {
    yOffset = pageFrom+y;
    halLcdGotoXY(xFrom,yOffset);                     /* Set pointer to lower x position at given page */
    halLcdSendData(pBuf+(yOffset*LCD_COLS)+xFrom, xRange);  /* Send selected bytes*/
  }
}


/******************************************************************************
 * @fn          halLcdStringLength
 *
 * @brief       Returns the length a given c-string in number of characters.
 *              Multiply by LCD_CHAR_WIDTH to get length in pixels.
 *
 * input parameters
 *
 * @param       pStr  - c-string
 *
 * output parameters
 *
 * @return      length of pStr
 */

uint8 halLcdStringLength(char *pStr)
{

  uint8 i=0;
  while(pStr[i]!='\0') i++;
  return i;
}

/******************************************************************************
 * @fn          halLcdIntLength
 *
 * @brief       Returns the textlength an integer will use on the display with
 *              the halLcdPrintInt function. Examples; 215 returns 3 and -145
 *              returns 4 (add one for the minus character). Multiply result by
 *              LCD_CHAR_WIDTH to get length in pixels.
 *
 * input parameters
 *
 * @param       number  - number to get length of
 *
 * output parameters
 *
 * @return      length of number
 */
uint8 halLcdIntLength(int32 number)
{
  if(number==0)
  {
    return 1; /* the character zero also takes up one place */
  }
  uint8 numOfDigits=0;
  if(number<0)
  {
    number *= (-1);
    numOfDigits++;  /* add one character to length due to minus sign */
  }
  while(number>=1)
  {
    number /= 10;
    numOfDigits++;
  }
  return numOfDigits;

}

/******************************************************************************
 * @fn          halLcdDoubleLength
 *
 * @brief       Returns the textlength a float will use on the display with
 *              the halLcdPrintDouble function. The numOfDecimals must be provided
 *              to limit the number of decimals.
 *
 * input parameters
 *
 * @param       number  - number to get length of
 *              numOfDeceimals - the wanted number of decimals to use
 *
 * output parameters
 *
 * @return      text character length of number
 */
uint8 halLcdFloatLength(float number, char numOfDecimals)
{
  uint8 numOfDigits = 0;

  float threshold = -0.5;          /* The threshold which defines how low a     */
  for(uint8 i=0;i<numOfDecimals;i++)/* float must be to be considered negative. */
  {                                 /* I.e. if a float is -0.001 and the number */
    threshold *= 0.1;               /* of decimals is 2 then the number will be  */
  }                                 /* considered as 0.00 and not -0.00.         */

  if(number<=threshold) /* Add one character for minus sign if negative.   */
  {                     /* This can not be done in halLcdIntLength because */
    numOfDigits++;      /* -0.5 i.e. would cast to 0 and lose its sign.    */
    number *= (-1);     /* Work only with positive part afterwards.        */
  }

  numOfDigits += halLcdIntLength((int32)number);    /* integer part  */
  if(numOfDecimals) numOfDigits++;                  /* comma         */
  numOfDigits += numOfDecimals;                     /* decimal part  */
  return numOfDigits;
}

/******************************************************************************
 * @fn          halLcdGetBuffer
 *
 * @brief       returns the LCD buffer. Useful for animating the display
 *
 * input parameters
 *
 * @param       pBuffer - pointerto a lcd buffer
 *
 * output parameters
 *
 * @return      none
 */
void halLcdGetBuffer(char *pBuffer)
{

  //pBuffer = halLcdBuffer;
  for(uint16 i=0;i<LCD_BYTES;i++)
  {
    pBuffer[i] = halLcdBuffer[i];
  }
}

/******************************************************************************
 * @fn          halLcdClearPage
 *
 * @brief       Clears a page from a LCD buffer.
 *
 * input parameters
 *
 * @param       pBuffer  - The target buffer
 * @param       page     - page to clear (0-7)
 *
 * output parameters
 *
 * @return      none
 */
void halLcdClearPage(char *pBuffer,char page)
{
  char *pBuf = halLcdBuffer;  /* Default: Point to halLcdBuffer      */
  if(pBuffer) pBuf = pBuffer; /* If pBuffer isset, pBuf points to it */
  for(uint8 i=0;i<LCD_COLS;i++)
  {
    *(pBuf + (page*LCD_COLS+i)) = 0x00;
  }
}

/******************************************************************************
 * @fn          halLcdSendBufferAnimated
 *
 * @brief       While halLcdSendBuffer just sends the LCD buffer "as is" and
 *              updates the screen instantanously, this function can be used to
 *              create an animated transition between two displays.
 *
 * usage:       When changing the image on the display with halLcd functions,
 *              the buffer is changed immediately. The changes will take affect
 *              on the LCD when halLcdSendBuffer or halLcdAnimate is used.
 *              halLcdSendBuffer will change the display to show the new buffer
 *              instantanously. halLcdAnimate on the other side, will make a
 *              smooth transition into showing the new buffer. In order for
 *              halLcdAnimate to know what to animate from, it takes in an
 *              argument pFromBuffer. It should point to an address containing
 *              what was stored on the LCD buffer last time halLcdSendBuffer or
 *              halLcdAnimate was used, or in other words, what is presently on
 *              the display. This way, the halLcdAnimate will not take any memory
 *              unless used. Example of how to think:
 *
 *              1. halLcdSendBuffer or halLcdAnimate executed
 *              2. store whats on the LCD with halLcdGetBuffer
 *              3. execute one or more halLcd functions to get to the new display
 *              4. run halLcdAnimate to update display with smooth transitions
 *
 *              Two animations exists. SLIDE_LEFT and SLIDE_RIGHT which says that
 *              the new screen slides in leftwards or rightwards respectively.
 *              Send them in as the motion input.
 *
 * input parameters
 *
 * @param       pFromBuffer  - address to variable containing what's on screen
 * @param       motion        - which animation to use for transition
 *
 * output parameters
 *
 * @return      none
 */
void halLcdSendBufferAnimated(char *pToBuffer, char *pFromBuffer, char motion)
{

  char *pToBuf = halLcdBuffer;       /* Default: pBuf points to halLcdBuffer            */
  if (pToBuffer) pToBuf = pToBuffer; /* If to buffer is set, pToBuf points to pToBuffer */

  char pageData[LCD_COLS];

  for(char offset = 0; offset<=LCD_COLS; offset+=4)
  {
    for(char pageIndex=0; pageIndex<8; pageIndex++)   /* Page counter*/
    {

      /*  Assigning data to this page from both buffers */
      for(uint16 i=0;i<LCD_COLS;i++)
      {
        if(motion==SLIDE_LEFT)
        {
          if(i+offset < LCD_COLS)
          {
              pageData[i] = *(pFromBuffer + (pageIndex*LCD_COLS + i+offset));
          }
          else
          {
              pageData[i] = *(pToBuf + (pageIndex*LCD_COLS + i+offset - LCD_COLS));
          }
        }
        else /* SLIDE_RIGHT */
        {
          if(i-offset > 0)
          {
              pageData[i] = *(pFromBuffer + (pageIndex*LCD_COLS + i-offset));
          }
          else
          {
            pageData[i] = *(pToBuf + (pageIndex*LCD_COLS + i-offset + LCD_COLS));
          }
        }
      }

      halLcdGotoXY(0, pageIndex);             /* Set pointer to start of row/page */
      halLcdSendData(&pageData[0], LCD_COLS); /* Send one page                    */
    }
    halTimer32kMcuSleepTicks(100);
  }
}

/*****************************************************************************
 * @fn          halLcdWriteBufferDirectly
 *
 * @brief       Writes provided bytes from start of buffer and potentially
 *              to the end of it(up to LCD_BYTES bytes)
 *
 * input parameters
 *
 * @param       pData - pointer to data array
 *              size  - number of bytes to be transferred
 * output parameters
 *
 * @return      none
 */
void halLcdWriteBufferDirectly(const char *pData, uint16 size)
{
  uint16 i;
  i = 0;
  while((i<size) && (i<LCD_BYTES))
  {
    halLcdBuffer[i] = *pData;
    pData++;
    i++;
  }
}

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