//****************************************************************************
//! @file menu_driver.h
//
//! @brief       Header file for the menu driver. The menu driver is
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

#ifndef menu_driver_h
#define menu_driver_h

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * INCLUDES
 */
#include "menu_system.h"

/*****************************************************************************
 * MACROS
 */
#define MENU_ITEMS_PER_SCREEN 7    /* How many menu items fit on one screen */
#define MENU_MARGIN 2             /* How many pixels of margin on left/right*/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 */
void menuDisplay(const menu_t *currentMenu);
uint8 menuGetScreen(const menu_t *pMenu, uint8 nItem);
void menuClearReservedArea(const menu_t *pMenu);

#ifdef  __cplusplus
}
#endif

#endif //menu_driver_h