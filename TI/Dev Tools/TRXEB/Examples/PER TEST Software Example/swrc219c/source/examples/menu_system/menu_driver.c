//****************************************************************************
//! @file        menu_driver.c
//
//! @brief       Implementation file for the menu driver. The menu driver is
//               the layer that is between the menu system and the LCD HAL.
//               DOGM128LCD: 
//               To avoid confusion the term page is used when talking about 
//               the physical section of the LCD called a page. That is, the 
//               LCD is divided into 8 pages with 8px of height each. The LCD 
//               is 128px wide wich then gives the resolution of 128x64px. 
//               The term screen is used when talking about what would seem to 
//               be one "page" in the menu system. For example, a picture
//               displaying the first 7 menuItems is one screen. The next 
//               screen contains the next 7 menuItems, and so on. 
//               Thus, one screen consists of 8 pages.
//
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/

/*****************************************************************************
 * INCLUDES
 */
#include "menu_driver.h"
#include "menu_system.h"
#include "lcd_dogm128_6.h"
#include "hal_types.h"
/*****************************************************************************
 * LOCAL AUXILLARY FUNCTION PROTOTYPES
 */

static uint8 printNavNumbers(const menu_t *pMenu);
static void printItem(const menu_t *pMenu, uint8 nItem, tLcdPage page, uint8 nrSize);
static void printHeader(const menu_t *pMenu);
/*****************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
static void menuWriteBuffer(const menu_t *pMenu);
static uint8 determineDecimals(float number);
static uint8 determineItemsPerScreen(const menu_t *pMenu);
static uint8 getNextPage(const menu_t *pMenu, uint8 page);
/* Deprecated */
/* static char getGreatestNrSize(const menu_t *pMenu);*/

/*****************************************************************************
 * @fn        menuDisplay
 *
 * @brief     Displays the menu provided by pMenu
 *
 * input parameters
 *
 * @param     pMenu   - The menu to be shown on the display
 */
void menuDisplay(const menu_t *pMenu){

#ifdef MENU_ANIMATED

  static menu_t *pPrevMenu = 0; /* previously displayed menu */
  tLcdMotion motion;
  char buff[LCD_BYTES];
  
  if(pMenu->pParentMenu==pPrevMenu)
  { /* user went into a submenu: slide left */
    motion = eLcdSlideLeft;
  }
  else if(pPrevMenu->pParentMenu==pMenu) 
  { /* user went back to parent menu: slide right */
    motion = eLcdSlideRight;
  }
  else
  { /* none of the above */
    motion = eLcdNoMotion;
  }
  
  if(motion)
  {
    lcdBufferCopy(0, buff);
    //lcdGetBuffer(buff);  
  }
  
#endif /* MENU_ANIMATED */
  
  menuWriteBuffer(pMenu);

#ifdef MENU_ANIMATED
  
  if(motion)
  {
    lcdSendBufferAnimated(0,buff,motion);
  }
  /* will be remembered to next function call due to static */
  pPrevMenu = (menu_t*)pMenu;  
  
#endif // MENU_ANIMATED
  
#ifndef MENU_DMA
  /* Not implemeted*/
  lcdSendBuffer(0);
  
#endif /* not defined MENU_DMA */
  
}

/*****************************************************************************
 * @fn        menuWriteBuffer
 *
 * @brief     Writes the provided pMenu to the LCD Buffer
 *
 * input parameters
 *
 * @param     pMenu   - The menu to be shown on the display
 */
void menuWriteBuffer(const menu_t *pMenu)
{
  
  /* Print the header on the first line/page on the LCD if not reserved */
  if(!(pMenu->reservedAreas & 1))
  {
    lcdBufferClearPage(0,eLcdPage0);
    printHeader(pMenu);
  }
  
  /* Which screen and which item is the current on */
  int8 nCurrentItem = pMenu->nCurrentItem;
  int8 screen = pMenu->nScreen;

  /* Finding first item on screen */
  uint8 itemsPerScreen = determineItemsPerScreen(pMenu);
  uint8 nItem = screen*itemsPerScreen;
  menuItem_t *pItem = &(pMenu->pItems[nItem]);
 
  /* Iterating through items, printing one by one */
  tLcdPage page=(tLcdPage)getNextPage(pMenu,0);   /* finds first page */
  while(nItem<pMenu->nMenuItems && page)
  {
    /* write the current item */
    lcdBufferClearPage(0,page);
    printItem(pMenu,nItem,page,0);
    
    
    /* Invert region around item if the item is selected */
    if(pItem==&(pMenu->pItems[nCurrentItem]))
    {
      lcdBufferSetHLine(0,0,LCD_COLS-1,page*LCD_PAGE_ROWS-1);
      lcdBufferInvertPage(0,0,LCD_COLS-1,page);
    }
    else
    {
      if(pMenu->reservedAreas & (1<<(page-1)))
      {
        /* if the previous page is reserved, and this page isn't marked,
         * make sure horizontal line doesn't stick.
         */
        lcdBufferClearHLine(0,0,LCD_COLS-1,page*LCD_PAGE_ROWS-1);
      }
    }
    /* NOTE: if the page over this (marked) item is reserved, the item will
     * still use 9 pixels of height for the invertion area so that it "steals"
     * 1 px of height from the reserved page.
     */
        
    
    /* If this is a dummy item, invert the region around it if the
     * master item is selected
     */
    if(pItem->flags & M_EXTEND){
      int8 masterNumber = nItem - 1;
      while(pMenu->pItems[masterNumber].flags & M_EXTEND)
      {
        masterNumber--;
      }
      if(masterNumber==nCurrentItem)
      {
        lcdBufferInvert(0,0,page*LCD_PAGE_ROWS,LCD_COLS-1,(page+1)*LCD_PAGE_ROWS-1);
      }
    }
   
    /* Iterate */
    page = (tLcdPage)getNextPage(pMenu,page);
    nItem++;
    pItem = &(pMenu->pItems[nItem]);
  }
  
  /* clearing unused pages that's not reserved */
  while(page)
  {
    lcdBufferClearPage(0,page);
    if(pMenu->reservedAreas & (1<<(page-1)))
    {
      /* if the previous page is reserved, and this page is empty,
       * make sure horizontal line doesn't stick.
       */
      lcdBufferClearHLine(0,0,LCD_COLS-1,page*LCD_PAGE_ROWS-1);
    }
    page = (tLcdPage)getNextPage(pMenu,page);
  }
  
}
/*****************************************************************************
 * @fn        printItem
 *
 * @brief     Prints a given menuItem on the specified line/page in the buffer.
 *            Used as an auxillary function for displayMenu to improve
 *            readability of code. An item consists of three fields to be
 *            printed in the following order:
 *
 *                +----------+-------------------------+------------+
 *                | Item nr  | Item description        | Item value |
 *                +----------+-------------------------+------------+
 *
 *            The whole block can be left aligned (default), right aligned
 *            (M_RIGHT), centered (M_CENTER) or it can be splitted such
 *            that item nr and item description is left aligned and item value
 *            is right aligned (M_SPLIT). The description and value can also
 *            swap places with M_SWAP. If nrSize is 0 the nr field will use
 *            only the necessary space. In other words: if some numbers are
 *            one digit and some ar two digits they will not be aligned.
 *            The nrSize input allows for a specified size at i.e. two digits.
 *
 * input parameters
 *
 * @param     pItem    - The item to print on the display
 * @param     page     - Which page to print the item on
 * @param     selected - Whether or not this item is the selected one
 * @param     nrSize   - The size of the number field (in pixels).
 */
static void printItem(const menu_t *pMenu, uint8 nItem, tLcdPage page, uint8 nrSize)
{
  /* Finding current item and wether or not it is selected (with an arrow) */
  menuItem_t *pItem = &(pMenu->pItems[nItem]);
  int8 selected = (nItem==pMenu->nSelectedItem);
  
  /* Setting empty fields to a dummystring '\0' */
  char dummyString = '\0';
  char *pTextNumber = &dummyString;
  char *pTextDescription = &dummyString;
  void *pValue = &dummyString;
  
  /* Finding textfields for non-empty fields */
  if(pItem->pTextNumber) pTextNumber = pItem->pTextNumber;
  if(pItem->pTextDescription) pTextDescription = pItem->pTextDescription;
  if(pItem->pValue) pValue = pItem->pValue;
  
  /* Implicit typecasting of pValue to pointers of other applicable datatypes.
   * This is needed some places
   */
  float *pValueFloat;
  pValueFloat = pValue;
  /* fix: using int32 casting gives sign errors */
  int *pValueInt = (int*) pValue;
  /* Masking out flags to make comparing easier */
  uint16 alignFlags = pItem->flags & (M_ALIGN_0 | M_ALIGN_1);
  uint16 pValueFlags = pItem->flags & (M_PVALUE_0 | M_PVALUE_1 | M_PVALUE_2);
  if(pValue==&dummyString)
  {
    /* If no value is presented, pValue points to the dummystring.
     * Continue working with it as if it's a string regardless of what's
     * actually in the flag.
     */
    pValueFlags = M_STRING;
  }  
  
  /* Calculate the size (in pixels) that number and description needs */
  if(nrSize==0)
  { /* nrSize is not fixed. Determine it. */
    nrSize = lcdGetStringLength(pTextNumber)*LCD_CHAR_WIDTH;
  }
  
  uint8 descSize = lcdGetStringLength(pTextDescription)*LCD_CHAR_WIDTH;
  uint8 selSize = (pMenu->nSelectedItem!=-1)*LCD_CHAR_WIDTH;
 
  
  /* Calculate the size (in pixels) that value needs */
  uint8 valSize;
  uint8 valDecimals;
  switch(pValueFlags)
  {
  case M_FLOAT1:
    valSize = lcdGetFloatLength(*pValueFloat,1)*LCD_CHAR_WIDTH;
    break;
  case M_FLOAT2:
    valSize = lcdGetFloatLength(*pValueFloat,2)*LCD_CHAR_WIDTH;
    break;
  case M_FLOAT3:
    valSize = lcdGetFloatLength(*pValueFloat,3)*LCD_CHAR_WIDTH;
    break;
  case M_FLOAT4:
    valSize = lcdGetFloatLength(*pValueFloat,4)*LCD_CHAR_WIDTH;
    break;
  case M_FLOAT5:
    valSize = lcdGetFloatLength(*pValueFloat,5)*LCD_CHAR_WIDTH;
    break;
  case M_FLOATA:
    valDecimals = determineDecimals(*pValueFloat);
    valSize = lcdGetFloatLength(*pValueFloat,valDecimals)*LCD_CHAR_WIDTH;
    break;
  case M_STRING:
    valSize = lcdGetStringLength(pValue)*LCD_CHAR_WIDTH;
    break;
  default: /* integer */
    valSize = lcdGetIntLength(*pValueInt)*LCD_CHAR_WIDTH;    
    break;
  }
  
  /* The number of margins/spaces between fields and the total size of the line */
  int8 numOfMargins = -1;
  if(nrSize) numOfMargins++;
  if(descSize) numOfMargins++;
  if(valSize) numOfMargins++; 
  int16 totalSize = nrSize+descSize+valSize+numOfMargins*LCD_CHAR_WIDTH;
  if(numOfMargins==-1) numOfMargins=0;
  
  /* Calculates position for the number field */
  int16 nrPos, descPos, valPos, selPos;
  switch(alignFlags)
  {
  case M_RIGHT:
    nrPos = (LCD_COLS - MENU_MARGIN - totalSize);
    break;
  case M_CENTER:
    nrPos = (LCD_COLS - MENU_MARGIN - totalSize)/2;
    break;
  default:  /* note: number is left aligned for both left and splitted alignment */
    nrPos = MENU_MARGIN;
    break;
  }  

  /* Calculates position for the selected item mark */
  selPos = nrPos+nrSize;
  
  if(pItem->flags & M_SWAP) /* description and value swaps place */
  {
    /* Calculates position for the value field (after nr field) */
    valPos = nrPos + nrSize;
    if(nrSize||selSize) valPos += LCD_CHAR_WIDTH; /* add a space if item nr is present */
    
    /* Calculates position for the description field */
    if(alignFlags==M_SPLIT)
    {
      /* Splitted alignment: description is right aligned */
      descPos = LCD_COLS - MENU_MARGIN - descSize;
    }
    else
    {
      /* Normal alignment (left/center/right): description is placed after value */
      descPos = valPos + valSize;
      if(valSize) descPos += LCD_CHAR_WIDTH; /* add a space i descr. is present */
    }
  }
  else  /* Normal constellation of fields */
  {
    /* Calculates position for the description field (after nr field) */
    descPos = nrPos + nrSize;
    if(nrSize||selSize) descPos += LCD_CHAR_WIDTH; /* add a space if item nr is present */
    
    /* Calculates position for the value field */
    if(alignFlags==M_SPLIT)
    {
      /* Splitted alignment: value is right aligned */
      valPos = LCD_COLS - MENU_MARGIN - valSize;
    }
    else
    {
      /* Normal alignment (left/center/right): value is placed after description */
      valPos = descPos + descSize;
      if(descSize) valPos += LCD_CHAR_WIDTH; /* add a space i descr. is present */
    }
  }

  
  /* Print number and description */
  lcdBufferPrintString(0,pTextNumber,nrPos,page);
  lcdBufferPrintString(0,pTextDescription,descPos,page);
  
  /* Print selection mark if selected */
  if(selected)
  {
    lcdBufferPrintString(0,"~",selPos,page);
  }
  
  /* write value to buffer */
  switch(pValueFlags)
  {
  case M_STRING:
    lcdBufferPrintString(0,pValue,valPos,page);
    break;
  case M_FLOAT1:
    lcdBufferPrintFloat(0,*pValueFloat,1,valPos,page);
    break;
  case M_FLOAT2:
    lcdBufferPrintFloat(0,*pValueFloat,2,valPos,page);
    break;
  case M_FLOAT3:
    lcdBufferPrintFloat(0,*pValueFloat,3,valPos,page);
    break;
  case M_FLOAT4:
    lcdBufferPrintFloat(0,*pValueFloat,4,valPos,page);
    break;
  case M_FLOAT5:
    lcdBufferPrintFloat(0,*pValueFloat,5,valPos,page);
    break;
  case M_FLOATA:
    lcdBufferPrintFloat(0,*pValueFloat,valDecimals,valPos,page);
    break;
  default:
    lcdBufferPrintInt(0,*pValueInt,valPos,page);
    break;
  }
}
/*****************************************************************************
 * @fn        printHeader
 *
 * @brief     Prints the header of the menu system on the top page/line on the
 *            LCD. This includes title and navigation item numbers in right 
 *            corner. Also prints an underline.
 *
 * input parameters
 *
 * @param     pMenu   - The menu struct which is going to be displayed
 */
static void printHeader(const menu_t *pMenu)
{
  /* displaying navigation data in the top
   * each of them returns the amount of pixels they occupie
   */
   
  uint8 occupied = printNavNumbers(pMenu);   /* numbers in top right corner */

  /* determining prefered title depending on where in the menu the user is */
  char *prefTitle;
  if(pMenu->pTextHeader)
  {
    prefTitle = pMenu->pTextHeader;
  }
  else if(pMenu->pParentMenu)
  {
    menu_t *pTopMenu = menuTop(pMenu);
    int8 nCurrentItem = pTopMenu->nCurrentItem;
    prefTitle=pTopMenu->pItems[nCurrentItem].pTextDescription;
  }
  else
  {
    prefTitle = "Main Menu";
  }

  /* calculating maximal width for menu title and the threshold to when the
   * title is too big to be centered and must be skeewed. Both are specified
   * in number of characters, not pixels.
   */
  uint8 maxWidth = (LCD_COLS-occupied)/LCD_CHAR_WIDTH-1;
  uint8 skewThreshold = (LCD_COLS-2*occupied)/LCD_CHAR_WIDTH-2;
  
  /* truncates title as necessary */
  char title[22];
  uint8 prefTitleLen = lcdGetStringLength(prefTitle);
  uint8 width = ( prefTitleLen<maxWidth ? prefTitleLen : maxWidth );
  for(uint8 i=0;i<width;i++)
  {
    title[i]=prefTitle[i];
  }
  title[width] = '\0';

  /* writes title */
  if(width<=skewThreshold)
  { 
    /* title's not too big to be centered */
    lcdBufferPrintStringAligned(0, title,eLcdAlignCenter,eLcdPage0);
    //lcdBufferPrintStringCentered(0,title,0);
  }
  else
  {
    /* title's too big to be centered. Skew title */
    int8 pos = LCD_COLS-occupied-LCD_CHAR_WIDTH-width*LCD_CHAR_WIDTH;
    lcdBufferPrintString(0,title,pos,eLcdPage0);
  }

  /* header underline */
  lcdBufferSetHLine(0,0,LCD_COLS-1,7);

}


/*****************************************************************************
 * @fn        printNavNumbers
 *
 * @brief     writes the navigation numbers in the top right corner that tells
 *            the user which item is selected and how many items the menu has
 *            got in total (i.e. 4/15). Used as an auxillary function for
 *            displayMenu to improve readability of code. Its writing to the
 *            LCD buffer.           
 *
 * input parameters
 *
 * @param     pMenu   - The menu to be shown on the display
 *
 * output parameters
 *
 * @result    Returns the number of pixels the navigation numbers occupies on
 *            the right end of the line.
 */
static uint8 printNavNumbers(const menu_t *pMenu)
{
  if(pMenu->pTextMenuItems)
  {
    uint8 nCurrentItem  = pMenu->nCurrentItem;
    char *totalNr      = pMenu->pTextMenuItems;
    char *currentNr    = pMenu->pItems[nCurrentItem].pTextNumber;
    
    uint8 margin       = MENU_MARGIN-(LCD_CHAR_WIDTH-LCD_FONT_WIDTH);
    uint8 totalNrLen   = lcdGetStringLength(totalNr);
    uint8 currentNrLen = lcdGetStringLength(currentNr);
      
    uint8 totalNrPos   = LCD_COLS-margin-totalNrLen*LCD_CHAR_WIDTH;
    uint8 slashPos     = totalNrPos-LCD_CHAR_WIDTH;
    uint8 currentNrPos = slashPos-currentNrLen*LCD_CHAR_WIDTH;
    
    lcdBufferPrintString(0,totalNr,totalNrPos,eLcdPage0);
    lcdBufferPrintString(0,(char*)"/",slashPos,eLcdPage0);
    lcdBufferPrintString(0,currentNr,currentNrPos,eLcdPage0);
    
    /* returns the maximal amount of occupied pixels on the right
     * uses totalNrLen twice instead of currentNrLen to keep constant
     * within one menu.
     */
    return margin + 2*totalNrLen*LCD_CHAR_WIDTH + LCD_CHAR_WIDTH + 1;
  }
  
  return 0; /* If there's no "total number of items" text, don't write */
}

/*****************************************************************************
 * @fn        determineDecimals
 *
 * @brief     Determines the number of decimals needed to print a float.
 *            i.e. one would need one decimal to represent 1.500 and three
 *            to represent 1.5030. Maximal precision is 5 decimals. That
 *            means that 1.123456789 needs 5 decimals and that 1.20000000008
 *            needs 1 decimal since 5 decimals isn't enough to get to the last
 *            decimal anyway.
 *
 * input parameters
 *
 * @param     number  - The number to calculate needed decimals for
 *
 * output parameters
 *
 * @result    Returns the number of decimals needed.
 */
static uint8 determineDecimals(float number)
{
  if(number<0) number *= (-1);  /* remove sign */
  number -= (int32)number;      /* only decimal part */
  
  /* Determining decimals needed by the use of truncation when typecasting to 
   * integers 
   */
  int32 decimal_5 = (int32)(100000.0*number)*1      ;
  int32 decimal_4 = (int32)( 10000.0*number)*10     ;
  int32 decimal_3 = (int32)(  1000.0*number)*100    ;
  int32 decimal_2 = (int32)(   100.0*number)*1000   ;
  int32 decimal_1 = (int32)(    10.0*number)*10000  ;
  int32 decimal_0 = (int32)(     1.0*number)*100000 ;
  
  /* correcting round off errors*/
  if(100000.0*number-(float)decimal_5>0.5) decimal_5++;
  if( 10000.0*number-(float)decimal_4>0.5) decimal_4++;
  if(  1000.0*number-(float)decimal_3>0.5) decimal_3++;
  if(   100.0*number-(float)decimal_2>0.5) decimal_2++;
  if(    10.0*number-(float)decimal_1>0.5) decimal_1++;
  if(     1.0*number-(float)decimal_0>0.5) decimal_0++;
  
        if(decimal_5!=decimal_4)  return 5;
  else  if(decimal_4!=decimal_3)  return 4;
  else  if(decimal_3!=decimal_2)  return 3;
  else  if(decimal_2!=decimal_1)  return 2;
  else  if(decimal_1==decimal_0)  return 0;
  else                            return 1;
}

/*****************************************************************************
 * @fn        determineItemsPerScreen
 *
 * @brief     Determines the number of items that fit on one screen of the
 *            menu pMenu. Might be different for different menus because of 
 *            the reserved area functionality.
 *
 * input parameters
 *
 * @param     pMenu - The menu to process
 *
 * output parameters
 *
 * @result    number of items that fit on one screen
 *            
 */
static uint8 determineItemsPerScreen(const menu_t *pMenu)
{
  uint8 itemsPerScreen = MENU_ITEMS_PER_SCREEN;
  uint8 reservedAreas = pMenu->reservedAreas;
  
  /* NB: bit 0 is the header so don't subtract off that one */
  for(uint8 bit=1;bit<=7;bit++)
  {
    if( reservedAreas & (1<<bit) )
    {
      itemsPerScreen--;   /* decrement by one for each reserved page */
    }
  }
  return itemsPerScreen;
}

/*****************************************************************************
 * @fn        getNextPage
 *
 * @brief     Takes in a page as an argument and finds the next page that is
 *            available, that is, not reserved by the user in the reservedArea
 *            flag. Kind of like an iterator. If the argument is 0 the
 *            function returns the first available. The position for the header
 *            is not considered as an available header and zero will be returned
 *            if no available position on screen is present.
 *
 * input parameters
 *
 * @param     pMenu   - Menu which needs an available item to place
 * @param     page    - The page to iterate from
 *
 * output parameters
 *
 * @result    The next available page
 */
static uint8 getNextPage(const menu_t *pMenu, uint8 page)
{
  uint8 reservedAreas = pMenu->reservedAreas;
  
  do
  {
    page++;
    if(page>MENU_ITEMS_PER_SCREEN)
    {
      return eLcdPage0;
    }
  } while( reservedAreas & (1<<page) );
  
  return page;
  
}

/*****************************************************************************
 * @fn        getGreatestNrSize
 *
 * @brief     Takes in a menu and determines how many characters the greatest
 *            number field is. Useful if a fixed width is prefered in the
 *            printItem function.
 *
 * input parameters
 *
 * @param     pMenu   - Menu to find fixed number field width of
 *
 * output parameters
 *
 * @result    Greatest number field size in characters
 */
/* Deprecated */
/*
static char getGreatestNrSize(const menu_t *pMenu)
{
  char nrSize = 0;
  for(char i=0;i<pMenu->nMenuItems;i++)
  {
    menuItem_t *pItem = &(pMenu->pItems[i]);
    char currentSize = halLcdStringLength(pItem->pTextNumber);
    if(currentSize>nrSize)
    {
      nrSize = currentSize;
    }
  }
  return nrSize;
}
*/
/*****************************************************************************
 * @fn        menuGetScreen
 *
 * @brief     Returns which screen a given item is on. The menu system needs
 *            this to keep track of which screen is shown to avoid skipping
 *            a screen with disabled items only.
 *
 * input parameters
 *
 * @param     pMenu   - Menu which the item is on
 * @param     nItem   - The item number to return placement screen of
 *
 * output parameters
 *
 * @result    Greatest number field size in characters
 */
uint8 menuGetScreen(const menu_t *pMenu, uint8 nItem)
{
  uint8 itemsPerScreen = determineItemsPerScreen(pMenu);
  return nItem/itemsPerScreen;
}


/*****************************************************************************
 * @fn        menuClearReservedArea
 *
 * @brief     A function that easily lets the user clear every reserved area
 *            of the LCD. Note that the last pixel of a reserved page isn't
 *            reserved if an item is present right below it. This is because the
 *            selection area around the item steals one pixel. That means,
 *            if you reserve two pages of 8 pixels in a row, you would get 15
 *            pixels free, not 16.
 *
 * input parameters
 *
 * @param     pMenu   - Menu which you have reserved areas from
 */
void menuClearReservedArea(const menu_t *pMenu)
{
  uint8 reservedAreas = pMenu->reservedAreas;
  for(uint8 bit=1;bit<=7;bit++)
  {
    if( reservedAreas & (1<<bit) )
    {
      /* Clear page if reserved, but only clear last line of page if the next
       * page is reserved as well. This is because the inverted selection area
       * steals one pixel of height from the page above it.
       */
      
      if( (reservedAreas & (1<<(bit+1))) || bit>7 )
      {
        lcdBufferClearPage(0,(tLcdPage)bit);
      }
      else
      {
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+0);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+1);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+2);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+3);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+4);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+5);
        lcdBufferClearHLine(0,0,LCD_COLS-1,(tLcdPage)bit*8+6);
      }
    }
  }      
}