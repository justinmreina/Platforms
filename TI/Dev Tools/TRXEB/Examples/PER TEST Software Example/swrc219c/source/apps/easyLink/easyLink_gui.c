//*****************************************************************************
//! @file   easyLink_gui.c
//
//! @brief  Implementation of the menu system used in the sniff mode test.
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

/******************************************************************************
 * INCLUDES
 */
#include "menu_system.h"
#include "menu_driver.h"
#include "easyLink.h"


/******************************************************************************
 * Local Functions
 */
/******************************************************************************
 * LOCAL VARIABLES AND CONSTANTS - needed for GUI information
 */

/******************************************************************************
 * MENUS
 */

menu_t easyLinkMainMenu;



static const menuItem_t easyLinkMainMenuItems[] =
{
  {M_DISABLED,0,"    Select Device",0,0,0,0,0},
  {0x00,"1","Master (TX)",0,0,0,&easyLinkMaster,0},
  {0x00,"2","Slave  (RX)",0,0,0,&easyLinkSlave,0},
};
static const menuItem_t easyLinkCC13LMainMenuItems[] =
{
  {M_DISABLED,0,"    Select Device",0,0,0,0,0},
  {0x00,"1","Slave  (RX)",0,0,0,&easyLinkSlave,0},
};
static const menuItem_t easyLinkCC115LMainMenuItems[] =
{
  {M_DISABLED,0,"    Select Device",0,0,0,0,0},
  {0x00,"1","Master (TX)",0,0,0,&easyLinkMaster,0},
};
menu_t easyLinkMainMenu = 
{
  (menuItem_t*)easyLinkMainMenuItems,  /* pItems          */
  0,                                    /* pParentMenu     */
  0,                                    /* pMenuGraphics   */
  "EasyLink Test",                     /* pTextHeader     */
  "2",                                  /* pTextMenuItems  */
  3,                                    /* nMenuItems      */
  1,                                    /* nCurrentItem    */
  -1,                                   /* nSelectedItem   */
  0,                                    /* nScreen         */
  0                                     /* reservedAreas   */
};

/******************************************************************************
 * Functions
 */
/******************************************************************************
 * @fn          easyLinkResetMenuVariables
 *
 * @brief       Function is called from the easyLink core. It resets menu variables.
 *              Dependant on radio inserted.
 *              
 * input parameters
 *
 * output parameters
 *
 * @return      void
 */
void easyLinkResetMenuVariables(void)
{
  if(pRadioChipType.deviceName == CHIP_TYPE_CC113L)
  {
    easyLinkMainMenu.pItems                         = (menuItem_t*)easyLinkCC13LMainMenuItems;
    easyLinkMainMenu.pTextMenuItems                 = "1";/* pTextMenuItems */
    easyLinkMainMenu.nMenuItems                     = 2;         /* nMenuItems     */
  }
  else if(pRadioChipType.deviceName == CHIP_TYPE_CC115L)
  {
    easyLinkMainMenu.pItems                         = (menuItem_t*)easyLinkCC115LMainMenuItems;
    easyLinkMainMenu.pTextMenuItems                 = "1";/* pTextMenuItems */
    easyLinkMainMenu.nMenuItems                     = 2;         /* nMenuItems     */
  }
  else{
    easyLinkMainMenu.pItems                         = (menuItem_t*)easyLinkMainMenuItems;
    easyLinkMainMenu.pTextMenuItems                 = "2";/* pTextMenuItems */
    easyLinkMainMenu.nMenuItems                     = 3;         /* nMenuItems     */
  }
}  
