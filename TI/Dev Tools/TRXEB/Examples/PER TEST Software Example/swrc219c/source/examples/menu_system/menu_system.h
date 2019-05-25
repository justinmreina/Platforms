//****************************************************************************
//! @file menu_system.h
//
//! @brief  header file for a generic menu system with sub menus.
//          The menu system itself is hardware independent but a separate 
//          menu driver will be needed to display the menu on a given LCD 
//          display.
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

#ifndef __menu_system_h
#define __menu_system_h

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/*****************************************************************************
 * CONSTANTS
 */

  
#define M_DISABLED  0x8000 
#define M_EXTEND    0x4000 
#define M_SWAP      0x2000 
#define M_PVALUE_2  0x1000 
#define M_PVALUE_1  0x0800 
#define M_PVALUE_0  0x0400 
#define M_ALIGN_1   0x0200 
#define M_ALIGN_0   0x0100 
#define M_CAT_7     0x0080 
#define M_CAT_6     0x0040 
#define M_CAT_5     0x0020 
#define M_CAT_4     0x0010 
#define M_CAT_3     0x0008 
#define M_CAT_2     0x0004 
#define M_CAT_1     0x0002 
#define M_CAT_0     0x0001 
#define M_DUMMY     (M_EXTEND + M_DISABLED)
#define M_RIGHT      M_ALIGN_0                  
#define M_CENTER     M_ALIGN_1   
#define M_SPLIT     (M_ALIGN_1 + M_ALIGN_0)
#define M_FLOAT1    M_PVALUE_0 
#define M_FLOAT2    M_PVALUE_1
#define M_FLOAT3   (M_PVALUE_1 + M_PVALUE_0)
#define M_FLOAT4    M_PVALUE_2 
#define M_FLOAT5   (M_PVALUE_2 + M_PVALUE_0)
#define M_FLOATA   (M_PVALUE_2 + M_PVALUE_1)
#define M_STRING    (M_PVALUE_2 + M_PVALUE_1 + M_PVALUE_0)

  
/*****************************************************************************
 * TYPEDEFS
 */

typedef uint8 (*pAppFunction)(void *argv[]);

struct menuItem;
struct menu;
struct graphics;

/*****************************************************************************
 * Description:  
 *  menuItem is specified and a type menuItem_t of menuItem is declared. The
 *  menuItem essentially holds all info to be printed to the LCD. 
 *
 * Members:
 *  - flags:            Bits to set options for each menuItem. Bits can be set
 *                      with enums starting with M_
 *                        bit15: disable item
 *                        bit14: extend selection area onto this item
 *                        bit13: swap position of pValue and pDescription
 *                        bit12: pValue datatype bit 2
 *                        bit11: pValue datatype bit 1
 *                        bit10: pValue datatype bit 0
 *                        bit9 : align bit 1
 *                        bit8 : align bit 0
 *                        bit7 : category 7
 *                        bit6 : category 6
 *                        bit5 : category 5
 *                        bit4 : category 4
 *                        bit3 : category 3
 *                        bit2 : category 2
 *                        bit1 : category 1
 *                        bit0 : category 0
 *
 *                      Two or more menus can share a linked list of items if
 *                      they are similiar. Menus can then load the linked list
 *                      of items with different options active. 
 *
 *                      When the "disable item" bit is set, the menuItem can not
 *                      be selected. The application developer must 
 *                      make sure that such item's is not selected at startup.
 *                      This flag can be set with the M_DISABLED enum.
 *                      
 *                      The "extend selection area" flag can be used to create
 *                      a dummy item if the developer wants to use more than one
 *                      line for a menu choice. i.e. if everything doesn't fit
 *                      on a single line. The developer creates a menuItem with
 *                      an application and/or a submenu as usual. Then he/she
 *                      creates one or more dummy items right after the master
 *                      item by setting the "extend selection area" flag and the
 *                      "disabled" flag (use M_DUMMY to do this). The developer
 *                      can then put text on all of the dummy items as well. It
 *                      might be a good idea to use some spaces or aligning on
 *                      the dummy items to avoid confusing them with other items.
 *
 *                      pValue is a pointer to void but may actually point to an
 *                      integer, a c-string or a float depending on these two
 *                      bits. If it's pointing to a float, different number of
 *                      decimals to be shown can be selected. The bits works
 *                      as follows (MSB first):
 *                          000 - point to integer                 (default)
 *                          001 - point to float, show 1 decimal  (M_DOUBLE1)
 *                          010 - point to float, show 2 decimals (M_DOUBLE2)
 *                          011 - point to float, show 3 decimals (M_DOUBLE3)
 *                          100 - point to float, show 4 decimals (M_DOUBLE4)
 *                          101 - point to float, show 5 decimals (M_DOUBLE5)
 *                          110 - point to float, auto decimals   (M_DOUBLEA) 
 *                          111 - point to c-string                (M_STRING)
 *
 *                      Alignment of the menu items can be set with the
 *                      alignment bits as follows (MSB first):
 *                          00 - left aligned   (default)
 *                          01 - right aligned  (M_RIGHT)
 *                          10 - centered       (M_CENTER)
 *                          11 - split          (M_SPLIT)
 *                      In split alignment pValue will be right aligned while
 *                      the number and description will be left aligned.
 *              					 			 
 *  - pTextNumber:      Pointer to the number text that goes in front of
 *                      *pTextDescription. This is concatenated to be shown on
 *                      display.
 *  - pText//! @brief  Pointer to the text to be displayed for this item
 *                      without the number in front. Will also be used as
 *                      headers when entering sub menus
 *  - pValue:           Points to a value to be displayed at the line. May be 
 *                      useful to point at a string, float or a signed int 
 *                      to show live data on the LCD. See the pValue datatype  
 *                      bits and the align bits in the flag. If signed int, 
 *                      this int must be 16 or 32 bits.
 *  - pSubMenu:         Pointer to sub menu -> must check for NULL pointer
 *  - pItemGraphics:    Pointer to logo/picture graphical structure if 
 *                      applicable
 *  - pApplication:     Pointer to function that is executed when selected.
 *  - pAppArgs:         X arguments of type void*. Any pointer can be cast 
 *                      to void and back again without loss of information. 
 *                      External data can also be used. A perSettingsStruct 
 *                      can i.e. be external. This will allow for less 
 *                      arguments in function.
 */
typedef struct menuItem
{
  uint16                  flags;
  char                    *pTextNumber; 
  char                    *pTextDescription;
  void                    *pValue;
  struct menu             *pSubMenu;
  struct menuGraphics     *pItemGraphics;             
  pAppFunction            pApplication;
  void                    *pAppArgs;
} menuItem_t;

/*****************************************************************************
 * Description:  
 *  menu is specified and a type tMenu of menu is declared. The menu serves as a
 *  container for all menu items.
 *
 * Members:
 *  - pItems:         Pointer to first element of this menu's array of items.
 *  - pParentMenu:    Pointer to this menus parent menu. Pupose: Backwards
 *                    navigation.
 *  - pMenuGraphics:  Pointer to possible menu graphics if applicable.
 *  - pTextHeader:    A pointer to a c-string which can be used to override the
 *                    default header/title in the menu. The default one picks
 *                    the item description on the item entered in the top menu
 *                    to get to the current menu. Example: if you start in the
 *                    main menu, then enters "Config" the title is "Config".
 *                    It will remain the same in all submenus of the "Config"-
 *                    menu. The default title in the top menu is "Main Menu".
 *                    To use the default value: set this to zero.
 *  - pTextMenuItems: A c-string containing the number of elements in this menu.
 *  - nMenuItems      Number of menu items
 *  - nCurrentItem:   a integer telling which item is the current one, meaning
 *                    which one is marked with an inverted area.
 *  - nSelectedItem:  Some menus may be option-menus where each item is a
 *                    selectable option. Options/items are selected by entering
 *                    them. nSelectedItem tells which item is selected. The user
 *                    may change positions (up or down) without affecting which
 *                    item is selected. To disable this feature, set nSelected
 *                    to -1.
 *  - nScreen:        This member tells which menu screen to display. The menu
 *                    system and menu driver will handle this by itself if
 *                    initialized correctly.
 *  - reservedAreas:  Contains 8 bits to reserve the menu system from writing
 *                    on one or several of 8 areas when displaying this menu.
 *                    This can be useful for example if the user wants to
 *                    integrate an application or add-on into the menu system.
 *                    For the EA DOGM128 display these 8 areas are equivalent
 *                    with the 8 pages. An example of usage can be if the user
 *                    wants to implement a graph inside a menu. Then the user
 *                    might have the header on page/area 0 and reserve pages
 *                    1-4 to display the graph there. The menu items will only
 *                    be placed on pages/areas 5-7. The user can then operate
 *                    freely on these pages with the hardware abstraction layer.
 *                    The uppermost area (page 0) correspond to LSB (bit 0) and
 *                    so on. Note that reserving page 0 for own use removes the
 *                    header completely.
 */
typedef struct menu
{
  struct menuItem    *pItems;
  struct menu        *pParentMenu;
  struct graphics    *pMenuGraphics; 
  char               *pTextHeader;
  char               *pTextMenuItems;
  uint8              nMenuItems;
  uint8              nCurrentItem;
  int8               nSelectedItem;
  uint8              nScreen;
  uint8              reservedAreas;
} menu_t;


typedef struct graphics
{
  char *pImage;         // String of characters that make up a picture.
                        // Must match with bound given below:
  uint8 upperLeftX;     // |(upperLeftX,upperLeftY)                    |
  uint8 upperLeftY;     // |                                           |
  uint8 lowerRightX;    // |                                           |
  uint8 lowerRightY;    // |                  (lowerRightX,lowerRightY)|
} graphics_t;


/*****************************************************************************
 * FUNCTION PROTOTYPES
 */
void menuPositionTop(menu_t *pMenu);
uint8 menuUp(menu_t *pMenu);
uint8 menuDown(menu_t *pMenu);
menu_t* menuBack(menu_t *pMenu);
menu_t* menuEnter(menu_t *pMenu);
menu_t* menuTop(const menu_t *pMenu);


#ifdef  __cplusplus
}
#endif

#endif //__menu_system_h