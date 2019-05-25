//*****************************************************************************
//! @file    per_test_gui.c
//
//! @brief  Implementation of the menu system used in the per test.
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

#include "hal_types.h"
#include "menu_system.h"
#include "per_test.h"
#include "chip_detect.h"

/******************************************************************************
 * Local Functions
 */

uint8 perSlaveEditFrequencyApp(void** pDummy);
uint8 perMasterTxPowerConfigApp(void** pDummy);
uint8 perMasterPacketsConfigApp(void** pDummy);
uint8 perMasterSetLinkTypeModeApp(void** pDummy);
uint8 perMasterSetRetransmissionsApp(void** pDummy);
uint8 perConfigureDeviceModeMenuApp(void** pDummy);        
uint8 perSetDefaultLinkParametersApp(void** pDummy);    
uint8 perMasterSetDataRateApp(void** pDummy);         
uint8 perMasterSetLinkApp(void** pDummy);            
uint8 perMasterSetPacketLengthApp(void** pDummy);

                                           
/******************************************************************************
 * LOCAL VARIABLES AND CONSTANTS - needed for GUI information
 */

/* Data rate selection */
static float guiDataRate;
/* Packet length selection */
static int16 guiPacketLength;
/* Selected output power in dBm */
static int16 guiTxPower;   
/* Selected number of retransmits of a packet before evaluating if packet is 
 * lost 
 */
static int16 guiPacketRetransmits2Way; 

/******************************************************************************
 * MENUS
 */

 
/* Menus that are accessable from main and the PER core */

menu_t perSlaveMenu;
menu_t perHeadMenu;
menu_t perAbstractHeadMenu;
menu_t perCC1101ChipSelectMenu;
menu_t perCC1120ChipSelectMenu;

/* Menus in the PER TESTER GUI */

static menu_t perMasterEasyLinkConfigMenu;
static menu_t perMasterExpertLinkToplogyMenu;
static menu_t perMasterPerTestStartMenu;
static menu_t perMasterExpertOneWayLinkConfigMenu;
static menu_t perMasterExpertTwoWayLinkConfigMenu;
static menu_t perMasterCC1101DataRateMenu;
static menu_t perMasterCC1120DataRateMenu;
static menu_t perMasterCC1200DataRateMenu;
static menu_t perMasterCC1190DataRateMenu;
static menu_t perMasterModeMenuInfo;
static menu_t perMasterCC1101TxPowerMenu;
static menu_t perMasterCC1120TxPowerMenu;
static menu_t perMasterCC1200TxPowerMenu;
static menu_t perMasterNumberPacketsMenu;
static menu_t perMasterRetransmitMenu;
static menu_t perCC1101FrequencyMenu;
static menu_t perCC1190FrequencyMenu;
static menu_t perCC112xFrequencyMenu;
static menu_t perCC120xFrequencyMenu;
static menu_t perExpertModeMenu;
static menu_t perDeviceModeMenu;
static menu_t perMasterPacketLengthMenu;

static menu_t perMasterLinkBypassConfigMenu;
static menu_t perSlaveLinkBypassConfigMenu;

static menu_t perValueLineLinkConfigMenu;
static menu_t perCC11xLDataRateMenu;
static menu_t perCC11xLTxPowerMenu;
static menu_t perCC11xLFrequencyMenu;
static menu_t perChipSelectMenuInfo;


/* Configurator Mode INFO: 
 * - Add function that selects the correct smartRF configuration, link type etc
 * - Call function to change perDeviceModeMenu's menuItems list
 */

static const menuItem_t perCC1101ExpertModeMenuItems[] = 
{
  {M_DISABLED,0," Select Expert Mode",0,0,0,0,0},
  {0x00,"1","Manual Mode     ",0,&perCC1101FrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)EXPERT_MODE},
  {0x00,"2","Link bypass mode",0,&perCC1101FrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)LINK_BYPASS_MODE},
  /*{M_DISABLED,0,"Configurator Mode",0,0,0,0,0} */
};
static const menuItem_t perCC110LExpertModeMenuItems[] = 
{
  {M_DISABLED,0," Select Expert Mode",0,0,0,0,0},
  {0x00,"1","Manual Mode     ",0,&perCC11xLFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)EXPERT_MODE},
  {0x00,"2","Link bypass mode",0,&perCC11xLFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)LINK_BYPASS_MODE},
  /*{M_DISABLED,0,"Configurator Mode",0,0,0,0,0}*/ 
};

static const menuItem_t perCC112xExpertModeMenuItems[] = 
{
  {M_DISABLED,0," Select Expert Mode",0,0,0,0,0},
  {0x00,"1","Manual Mode     ",0,&perCC112xFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)EXPERT_MODE},
  {0x00,"2","Link Bypass Mode",0,&perCC112xFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)LINK_BYPASS_MODE},
  /*{M_DISABLED,0,"Configurator Mode",0,0,0,0,0} // Add pointer to Device Function menu when Configurator Mode is ready */
};

static const menuItem_t perCC120xExpertModeMenuItems[] = 
{
  {M_DISABLED,0," Select Expert Mode",0,0,0,0,0},
  {0x00,"1","Manual Mode     ",0,&perCC120xFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)EXPERT_MODE},
  {0x00,"2","Link Bypass Mode",0,&perCC120xFrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)LINK_BYPASS_MODE},
  /*{M_DISABLED,0,"Configurator Mode",0,0,0,0,0} // Add pointer to Device Function menu when Configurator Mode is ready */
};

static menu_t perExpertModeMenu =
{
  (menuItem_t*)perCC112xExpertModeMenuItems,    /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "1",                                          /* pTextMenuItems */
  3,                                            /* nMenuItems     */
  1,                                            /* nCurrentItem   */
  -1,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};

static const menuItem_t perCC112xFrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","169 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","434 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}, 
  {0x00,"3","868 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)2},
  {0x00,"4","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)3},
  {0x00,"5","955 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)4}  
};

static menu_t perCC112xFrequencyMenu =
{
  (menuItem_t*)perCC112xFrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "5",                                          /* pTextMenuItems */
  6,                                            /* nMenuItems     */
  3,                                            /* nCurrentItem   */
  3,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};

static const menuItem_t perCC120xFrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","169 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","434 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}, 
  {0x00,"3","868 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)2},
  {0x00,"4","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)3},
  {0x00,"5","955 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)4}  
};
static menu_t perCC120xFrequencyMenu =
{
  (menuItem_t*)perCC120xFrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "5",                                          /* pTextMenuItems */
  6,                                            /* nMenuItems     */
  3,                                            /* nCurrentItem   */
  3,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};


static const menuItem_t perCC1101FrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","315 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","434 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}, 
  {0x00,"3","868 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)2},
  {0x00,"4","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)3}
};
static menu_t perCC1101FrequencyMenu =
{
  (menuItem_t*)perCC1101FrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "4",                                          /* pTextMenuItems */
  5,                                            /* nMenuItems     */
  2,                                            /* nCurrentItem   */
  2,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};

static const menuItem_t perCC11xLFrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","315 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","434 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}, 
  {0x00,"3","868 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)2},
  {0x00,"4","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)3}
};
static menu_t perCC11xLFrequencyMenu =
{
  (menuItem_t*)perCC11xLFrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "4",                                          /* pTextMenuItems */
  5,                                            /* nMenuItems     */
  2,                                            /* nCurrentItem   */
  2,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};

static const menuItem_t perCC1120CC1190FrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","869 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}
};
static const menuItem_t perCC1101CC1190FrequencyMenuItems[] =
{
  {M_DISABLED,0,"  Select Frequency",0,0,0,0,0},
  {0x00,"1","869 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)0},
  {0x00,"2","915 MHz",0,&perDeviceModeMenu,0,&perSlaveEditFrequencyApp,(void **)1}
};
static menu_t perCC1190FrequencyMenu =
{
  (menuItem_t*)perCC1101CC1190FrequencyMenuItems,     /* pItems         */
  0,                                            /* pParentMenu    */
  0,                                            /* pMenuGraphics  */
  0,                                            /* pTextHeader    */
  "2",                                          /* pTextMenuItems */
  3,                                            /* nMenuItems     */
  1,                                            /* nCurrentItem   */
  1,                                            /* nSelectedItem  */
  0,                                            /* nScreen        */
  0                                             /* reservedAreas  */
};
/* Device mode menu and items depending on Easy/Expert Mode/Configurator Mode/Link Bypass Mode selection */
static const menuItem_t perEasyDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perMasterEasyLinkConfigMenu,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE},
  {0x00,"2","Slave",0,0,0,&perSlaveStartApp,0}
};
static const menuItem_t perExpertDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perMasterExpertLinkToplogyMenu,0,&perSetDefaultLinkParametersApp,(void **)EXPERT_MODE},
  {0x00,"2","Slave",0,0,0,&perSlaveStartApp,0}
};
/* In case of CONFIGURATOR_MODE all the neccessary link settings are already configured */ 
static const menuItem_t perConfiguratorDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perMasterPerTestStartMenu,0,0,0},
  {0x00,"2","Slave",0,0,0,&perSlaveStartApp,0}
};
/* In case of LINK_BYPASS_MODE the link part is skipped and slave and master node will 
 * begin test with smarRFConfiguration setting 0 at max output*/
static const menuItem_t perLinkBypassDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perMasterLinkBypassConfigMenu,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE},
  {0x00,"2","Slave" ,0,&perSlaveLinkBypassConfigMenu ,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE},
};

static const menuItem_t perCC113LDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perValueLineLinkConfigMenu,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE}
};

static const menuItem_t perCC115LDeviceModeMenuItems[] =
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Slave",0,&perValueLineLinkConfigMenu,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE}
};

/* Device mode menu for cc1190*/
static const menuItem_t perCC1190DeviceModeMenuItems[] =  
{
  {M_DISABLED,0," Select Device Mode",0,0,0,0,0},
  {0x00,"1","Master",0,&perMasterEasyLinkConfigMenu,0,&perSetDefaultLinkParametersApp,(void **)EASY_MODE},
  {0x00,"2","Slave",0,0,0,&perSlaveStartApp,0},
};
static menu_t perDeviceModeMenu =
{
  (menuItem_t*)perEasyDeviceModeMenuItems,  /* pItems         */
  0,                                        /* pParentMenu    */
  0,                                        /* pMenuGraphics  */
  0,                                        /* pTextHeader    */
  "2",                                      /* pTextMenuItems */
  3,                                        /* nMenuItems     */
  1,                                        /* nCurrentItem   */
  -1,                                       /* nSelectedItem  */
  0,                                        /* nScreen        */
  0                                         /* reservedAreas  */
}; 

/****************************** Slave menu ******************************
 *
 *- This menu is handled by the Slave Mode application to avoid 
 *  mixing menu system and applications
 */
static const menuItem_t perSlaveMenuItems[] =
{   
  {M_CENTER|M_DISABLED,0,"Slave Mode Status:",0,0,0,0,0},
  {M_DISABLED|M_CENTER|M_STRING,0,0,slaveStatus,0,0,0,0},
  {0x00,"1","Abort Slave Mode",0,0,0,0,0}
};
menu_t perSlaveMenu   = 
{
  (menuItem_t *)perSlaveMenuItems,     /* pItems         */
  0,                                   /* pParentMenu    */
  0,                                   /* pMenuGraphics  */
  "PER Test",                          /* pTextHeader    */
  "1",                                 /* pTextMenuItems */
  3,                                   /* nMenuItems     */
  2,                                   /* nCurrentItem   */
  -1,                                  /* nSelectedItem  */
  0,                                   /* nScreen        */
  0x6A                                 /* reservedAreas  */
};


/**********************************************************************/

static const menuItem_t perCC1101HeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC1101FrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Expert Mode",0,&perExpertModeMenu,0,0,0},
  {0x00,"3","Information",0,&perMasterModeMenuInfo,0,0,0}
};
static const menuItem_t perCC110LHeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC11xLFrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Expert Mode",0,&perExpertModeMenu,0,0,0},
  {0x00,"3","Information",0,&perMasterModeMenuInfo,0,0,0}
};
static const menuItem_t perCC113LHeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC11xLFrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Information",0,&perMasterModeMenuInfo,0,0,0}
};

static const menuItem_t perCC115LHeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC11xLFrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Information",0,&perMasterModeMenuInfo,0,0,0}
};
static const menuItem_t perHeadCC1190MenuItems[] =  
{
 {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
 {0x00,"1","Easy Mode"       ,0,&perCC1190FrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
 {0x00,"2","Link Bypass Mode",0,&perCC1190FrequencyMenu,0,&perConfigureDeviceModeMenuApp,(void **)LINK_BYPASS_MODE},
};
static const menuItem_t perCC112xHeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC112xFrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Expert Mode",0,&perExpertModeMenu,0,0,0},
  {0x00,"3","Information",0,&perMasterModeMenuInfo,0,0,0}
};
static const menuItem_t perCC120xHeadMenuItems[]= 
{
  {M_DISABLED,0,"    Select Mode",0,0,0,0,0},
  {0x00,"1","Easy Mode"  ,0,&perCC112xFrequencyMenu ,0,&perConfigureDeviceModeMenuApp,(void **)EASY_MODE},
  {0x00,"2","Expert Mode",0,&perExpertModeMenu,0,0,0},
  {0x00,"3","Information",0,&perMasterModeMenuInfo,0,0,0}
};

static menu_t perHeadMenu = 
{
  (menuItem_t*)perCC1101HeadMenuItems,      /* pItems         */ 
  0,                                        /* pParentMenu    */
  0,                                        /* pMenuGraphics  */
  0,                                        /* pTextHeader    */
  "3",                                      /* pTextMenuItems */
  4,                                        /* nMenuItems     */
  1,                                        /* nCurrentItem   */
  -1,                                       /* nSelectedItem  */
  0,                                        /* nScreen        */
  0                                         /* reservedAreas  */
};
/****************************************************************************/
static const menuItem_t perCC1101ChipSelectMenuItems[]=               
{
  {M_DISABLED,0," Select Radio Combo",0,0,0,0,0},
  {0x00,"1","CC1101 Stand Alone ",0,&perHeadMenu,0,&perInitApp,(void **)CC1101_SELECTED},
  {0x00,"2","CC1101+CC1190 Combo",0,&perHeadMenu,0,&perInitApp,(void **)CC1101_CC1190_SELECTED},
  {0x00,"3","Information",0,&perChipSelectMenuInfo,0,0,0} 
};
static menu_t perCC1101ChipSelectMenu = 
{
  (menuItem_t *)perCC1101ChipSelectMenuItems, /* pItems         */   
  0,                                          /* pParentMenu    */
  0,                                          /* pMenuGraphics  */
  0,                                          /* pTextHeader    */
  "3",                                        /* pTextMenuItems */
  4,                                          /* nMenuItems     */
  1,                                          /* nCurrentItem   */
  -1,                                         /* nSelectedItem  */
  0,                                          /* nScreen        */
  0                                           /* reservedAreas  */
};
static const menuItem_t perCC1120ChipSelectMenuItems[]=               
{
  {M_DISABLED,0," Select Radio Combo",0,0,0,0,0},
  {0x00,"1","CC1120 Stand Alone ",0,&perHeadMenu,0,&perInitApp,(void **)CC1120_SELECTED},
  {0x00,"2","CC1120+CC1190 Combo",0,&perHeadMenu,0,&perInitApp,(void **)CC1120_CC1190_SELECTED},
  {0x00,"3","Information",0,&perChipSelectMenuInfo,0,0,0} 
};

static menu_t perCC1120ChipSelectMenu = 
{
  (menuItem_t *)perCC1120ChipSelectMenuItems, /* pItems         */   
  0,                                          /* pParentMenu    */
  0,                                          /* pMenuGraphics  */
  0,                                          /* pTextHeader    */
  "3",                                        /* pTextMenuItems */
  4,                                          /* nMenuItems     */
  1,                                          /* nCurrentItem   */
  -1,                                         /* nSelectedItem  */
  0,                                          /* nScreen        */
  0                                           /* reservedAreas  */
};
/**************************** Link configuration menus ****************************/
/* Link configuration:  Easy Mode with subMenus and Items
 */

/* Different data rates/output power for CC1120 and CC1101 */
static const menuItem_t perMasterEasyLinkConfigCC1101MenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1101DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1101TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static const menuItem_t perMasterEasyLinkConfigCC110LMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perCC11xLTxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static const menuItem_t perMasterEasyLinkConfigCC1120MenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1120DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1120TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static const menuItem_t perMasterEasyLinkConfigCC1200MenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1200DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1200TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static const menuItem_t perMasterEasyLinkConfigCC1101CC1190MenuItems[] = 
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {M_DISABLED,0,"TX Power",0,0,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static const menuItem_t perMasterEasyLinkConfigCC1120CC1190MenuItems[] = 
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {M_DISABLED,0,"TX Power",0,0,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};
static menu_t perMasterEasyLinkConfigMenu =
{
  (menuItem_t*)perMasterEasyLinkConfigCC1101MenuItems,  /* pItems : Assigned after chip detect*/
  0,                                                    /* pParentMenu                        */
  0,                                                    /* pMenuGraphics                      */
  0,                                                    /* pTextHeader                        */
  "3",                                                  /* pTextMenuItems                     */
  6,                                                    /* nMenuItems                         */
  5,                                                    /* nCurrentItem                       */
  -2,                                                   /* nSelectedItem                      */
  0,                                                    /* nScreen                            */
  0                                                     /* reservedAreas                      */
};
                  
/* Selection of data rate in case CC1101 is inserted */
static const menuItem_t perMasterCC1101DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","  1.20 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2"," 38.38 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3","249.94 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2}
};

static menu_t perMasterCC1101DataRateMenu = 
{
  (menuItem_t*)perMasterCC1101DataRateMenuItems,     /* pItems           */
  &perMasterEasyLinkConfigMenu,                      /* pParentMenu      */
  0,                                                 /* pMenuGraphics    */
  0,                                                 /* pTextHeader      */
  "3",                                               /* pTextMenuItems   */
  4,                                                 /* nMenuItems       */
  0,                                                 /* nCurrentItem     */
  0,                                                 /* nSelectedItem    */
  0,                                                 /* nScreen          */
  0                                                  /* reservedAreas    */
};
/* Selection of data rate in case CC110L is inserted */
static const menuItem_t perCC11xLDataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","  1.20 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2"," 38.38 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3","249.94 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2}
};

static menu_t perCC11xLDataRateMenu = 
{
  (menuItem_t*)perCC11xLDataRateMenuItems,     /* pItems           */
  &perValueLineLinkConfigMenu,                       /* pParentMenu      */
  0,                                                 /* pMenuGraphics    */
  0,                                                 /* pTextHeader      */
  "3",                                               /* pTextMenuItems   */
  4,                                                 /* nMenuItems       */
  0,                                                 /* nCurrentItem     */
  0,                                                 /* nSelectedItem    */
  0,                                                 /* nScreen          */
  0                                                  /* reservedAreas    */
};
static const menuItem_t perMasterCC1120DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","  1.20 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2"," 50.00 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3","150.00 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2}
};
static const menuItem_t perMasterCC1125DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","  0.30 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2","  1.20 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3"," 50.00 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2}
};
static menu_t perMasterCC1120DataRateMenu = 
{
  (menuItem_t*)perMasterCC1120DataRateMenuItems,     /* pItems         */  
  &perMasterEasyLinkConfigMenu,                      /* pParentMenu    */  
  0,                                                 /* pMenuGraphics  */  
  0,                                                 /* pTextHeader    */  
  "3",                                               /* pTextMenuItems */  
  4,                                                 /* nMenuItems     */  
  2,                                                 /* nCurrentItem   */  
  2,                                                 /* nSelectedItem  */  
  0,                                                 /* nScreen        */  
  0                                                  /* reservedAreas  */  
};
static const menuItem_t perMasterCC1200DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","   1.20 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2","  50.00 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3"," 200.00 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2}
};
static menu_t perMasterCC1200DataRateMenu = 
{
  (menuItem_t*)perMasterCC1200DataRateMenuItems,     /* pItems         */  
  &perMasterEasyLinkConfigMenu,                      /* pParentMenu    */  
  0,                                                 /* pMenuGraphics  */  
  0,                                                 /* pTextHeader    */  
  "3",                                               /* pTextMenuItems */  
  4,                                                 /* nMenuItems     */  
  2,                                                 /* nCurrentItem   */  
  2,                                                 /* nSelectedItem  */  
  0,                                                 /* nScreen        */  
  0                                                  /* reservedAreas  */  
};
/* Selection of data rate in case CC1190 is selected */
static const menuItem_t perMaster869MHzCC1101CC1190DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","  1.20 kbit/s"     ,0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2","  1.20 kbit/s SENS",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3","  4.80 kbit/s"      ,0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2},
  {0x00,"4"," 38.38 kBit/s"      ,0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_3},
};
static const menuItem_t perMaster915MHzCC1101CC1190DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","   9.6 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2","  50.0 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
  {0x00,"3"," 150.0 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_2},
  {0x00,"4"," 300.0 kBit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_3},
};
static const menuItem_t perMasterCC1120CC1190DataRateMenuItems[] = 
{
  {M_DISABLED,0,"Select Data Rate",0,0,0,0,0},
  {0x00,"1","   1.2 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_0},
  {0x00,"2","  50.0 kbit/s",0,0,0,&perMasterSetDataRateApp,(void **)SMARTRF_CONFIGURATION_1},
};

static menu_t perMasterCC1190DataRateMenu = 
{
  (menuItem_t*)perMaster869MHzCC1101CC1190DataRateMenuItems, /* pItems           */
  &perMasterEasyLinkConfigMenu,                      /* pParentMenu      */
  0,                                                 /* pMenuGraphics    */
  0,                                                 /* pTextHeader      */
  "4",                                               /* pTextMenuItems   */
  5,                                                 /* nMenuItems       */
  4,                                                 /* nCurrentItem     */
  4,                                                 /* nSelectedItem    */
  0,                                                 /* nScreen          */
  0                                                  /* reservedAreas    */
};
/* Start PER test menu
 * -> The idea by not starting the PER test directly when linking devices is
 *    to be able to restart the test over and over again when the Master and
 *    Slave is separated. In case the one-way link type is selected, 
 *    the user is allowed to change the window size/number of packets that 
 *    will be used in calculating PER. In case the two-way link test is chosen
 *    the user is allowed to change the number of attempted communicated
 *    packets, i.e the Master will retransmit a packet up to N times if not
 *    receiving an ACK from the Slave. If no ACK is received after N 
 *    retransmissions, the packet is considered lost. The number N is also
 *    user configurable to be able to play with this type of link.  
 */
static const menuItem_t perMasterPerTestStartMenuItems[] = 
{
  {0x00,"1","Number of Packets",0,&perMasterNumberPacketsMenu,0,0,0},
  {0x00,"2","Start PER Test",0,0,0,&perMasterStartTestApp,0}
};
static menu_t perMasterPerTestStartMenu = 
{
  (menuItem_t*)perMasterPerTestStartMenuItems, /* pItems                                                             */
  &perMasterEasyLinkConfigMenu,                /* pParentMenu  - Will be manipulated runtime depending on traversing */
  0,                                           /* pMenuGraphics                                                      */
  0,                                           /* pTextHeader                                                        */
  "2",                                         /* pTextMenuItems                                                     */
  2,                                           /* nMenuItems                                                         */
  1,                                           /* nCurrentItem                                                       */
  -1,                                          /* nSelectedItem                                                      */
  0,                                           /* nScreen                                                            */
  0                                            /* reservedAreas                                                      */
};


/* Link configuration:  Expert Mode */

static const menuItem_t perMasterExpertLinkToplogyMenuItems[] =
{
  {M_DISABLED,0," Select Link Type",0,0,0,0,0},
  {0x00,"1","One-Way",0,&perMasterExpertOneWayLinkConfigMenu,0,&perMasterSetLinkTypeModeApp,(void **)LINK_1_WAY},
  {0x00,"2","Two-Way with ACK",0,&perMasterExpertTwoWayLinkConfigMenu,0,&perMasterSetLinkTypeModeApp,(void **)LINK_2_WAY},
  {M_DISABLED|M_DUMMY,0,"  and re-transmit",0,0,0,0,0}
};

static menu_t perMasterExpertLinkToplogyMenu = 
{
  (menuItem_t*)perMasterExpertLinkToplogyMenuItems,  /* pItems         */
  0,                                                 /* pParentMenu    */
  0,                                                 /* pMenuGraphics  */
  0,                                                 /* pTextHeader    */
  "2",                                               /* pTextMenuItems */
  4,                                                 /* nMenuItems     */
  0,                                                 /* nCurrentItem   */
  -1,                                                /* nSelectedItem  */
  0,                                                 /* nScreen        */
  0                                                  /* reservedAreas  */
};

/* Link Configuration : One-Way */
/* Currently the datarates offered for CC1120 and CC1101 are different leading to 2 different menus */
static const menuItem_t perMasterExpertOneWayCC1101LinkConfigMenuItems[] =
{ 
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1101DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"3","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"4","TX Power",0,&perMasterCC1101TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertOneWayCC110LLinkConfigMenuItems[] =
{ 
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"3","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"4","TX Power",0,&perCC11xLTxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertOneWayCC1120LinkConfigMenuItems[] =
{ 
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1120DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"3","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {M_DISABLED,0,0,0,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"4","TX Power",0,&perMasterCC1120TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertOneWayCC1200LinkConfigMenuItems[] =
{ 
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1200DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"3","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {M_DISABLED,0,0,0,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0}, 
 {M_DISABLED,0,0,0,0,0,0,0},
 {0x00,"4","TX Power",0,&perMasterCC1200TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static menu_t perMasterExpertOneWayLinkConfigMenu =
{
  (menuItem_t*)perMasterExpertOneWayCC1101LinkConfigMenuItems,  /* pItems - Decided runtime */
  0,                                                            /* pParentMenu              */
  0,                                                            /* pMenuGraphics            */
  0,                                                            /* pTextHeader              */
  "5",                                                          /* pTextMenuItems           */
  12,                                                           /* nMenuItems               */
  2,                                                            /* nCurrentItem             */
  -1,                                                           /* nSelectedItem            */
  0,                                                            /* nScreen                  */
  0                                                             /* reservedAreas            */
};

/* Link Configuration : Two-Way link with retransmit */
static const menuItem_t perMasterExpertTwoWayCC1101LinkConfigMenuItems[] =
{
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1101DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {0x00,"3","TX Power",0,&perMasterCC1101TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"4","Pkt re-transmits",0,&perMasterRetransmitMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"  [# Packets]",&guiPacketRetransmits2Way,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertTwoWayCC110LLinkConfigMenuItems[] =
{
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {0x00,"3","TX Power",0,&perCC11xLTxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"4","Pkt re-transmits",0,&perMasterRetransmitMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"  [# Packets]",&guiPacketRetransmits2Way,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertTwoWayCC1120LinkConfigMenuItems[] =
{
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1120DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {0x00,"3","TX Power",0,&perMasterCC1120TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"4","Pkt re-transmits",0,&perMasterRetransmitMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Packets]",&guiPacketRetransmits2Way,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static const menuItem_t perMasterExpertTwoWayCC1200LinkConfigMenuItems[] =
{
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1200DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Packet Length",0,&perMasterPacketLengthMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Bytes]",&guiPacketLength,0,0,0,0},
 {0x00,"3","TX Power",0,&perMasterCC1200TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"4","Pkt re-transmits",0,&perMasterRetransmitMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [# Packets]",&guiPacketRetransmits2Way,0,0,0,0},
 {0x00,"5","Link devices",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesApp,0}
};

static menu_t perMasterExpertTwoWayLinkConfigMenu =
{
  (menuItem_t*)perMasterExpertTwoWayCC1101LinkConfigMenuItems,  /* pItems - assigned runtime after chip detect */
  &perMasterExpertLinkToplogyMenu,                              /* pParentMenu                                 */
  0,                                                            /* pMenuGraphics                               */
  0,                                                            /* pTextHeader                                 */
  "5",                                                          /* pTextMenuItems                              */
  11,                                                           /* nMenuItems                                  */
  1,                                                            /* nCurrentItem                                */
  -1,                                                           /* nSelectedItem                               */
  0,                                                            /* nScreen                                     */
  0                                                             /* reservedAreas                               */
};

/* Menu for configuration of packet length */
static const menuItem_t perMasterPacketLengthMenuItems[] =
{
  {M_DISABLED,0,"Select Packet Length",0,0,0,0,0},
  {0x00," 1",":  3 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)3},
  {0x00," 2",":  5 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)5},
  {0x00," 3",": 10 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)10},
  {0x00," 4",": 15 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)15},
  {0x00," 5",": 20 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)20},
  {0x00," 6",": 25 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)25},
  {M_DISABLED,0,"Select Packet Length",0,0,0,0,0},
  {0x00," 7",": 30 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)30},
  {0x00," 8",": 35 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)35},
  {0x00," 9",": 40 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)40},
  {0x00,"10",": 45 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)45},
  {0x00,"11",": 50 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)50},
  {0x00,"12",": 55 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)55},
  {M_DISABLED,0,"Select Packet Length",0,0,0,0,0},
  {0x00,"13",": 60 Bytes",0,0,0,&perMasterSetPacketLengthApp,(void **)60},
  
};
static menu_t perMasterPacketLengthMenu = 
{
  (menuItem_t*)perMasterPacketLengthMenuItems,        /* pItems - assigned runtime after chip detect */
  0,                                                  /* pParentMenu                                 */
  0,                                                  /* pMenuGraphics                               */          
  0,                                                  /* pTextHeader                                 */
  "13",                                               /* pTextMenuItems                              */
  16,                                                 /* nMenuItems                                  */
  1,                                                  /* nCurrentItem                                */
  1,                                                  /* nSelectedItem                               */
  0,                                                  /* nScreen                                     */
  0                                                   /* reservedAreas                               */
};                                                    
                                                      
                                                      
/* Menu for configuring number of retransmissions */  
static const menuItem_t perMasterRetransmitMenuItems[] =
{                                                     
 {M_DISABLED,0,"Allowed re-transmits",0,0,0,0,0},
 {M_DISABLED|M_DUMMY,0,"    per packet",0,0,0,0,0},
 {0x00,"1",": 1 packet",0,0,0,&perMasterSetRetransmissionsApp,(void **)1}, 
 {0x00,"2",": 2 packets",0,0,0,&perMasterSetRetransmissionsApp,(void **)2}, 
 {0x00,"3",": 3 packets",0,0,0,&perMasterSetRetransmissionsApp,(void **)3}, 
 {0x00,"4",": 4 packets",0,0,0,&perMasterSetRetransmissionsApp,(void **)4}, 
 {0x00,"5",": 5 packets",0,0,0,&perMasterSetRetransmissionsApp,(void **)5}
};
static menu_t perMasterRetransmitMenu = 
{
  (menuItem_t*)perMasterRetransmitMenuItems,  /* pItems         */
  &perMasterExpertTwoWayLinkConfigMenu,       /* pParentMenu    */
  0,                                          /* pMenuGraphics  */
  0,                                          /* pTextHeader    */
  "5",                                        /* pTextMenuItems */
  7,                                          /* nMenuItems     */
  4,                                          /* nCurrentItem   */
  4,                                          /* nSelectedItem  */
  0,                                          /* nScreen        */
  0                                           /* reservedAreas  */
};




/* Power selection menus */
/* The number given as an argument to perMasterTxPowerConfigApp is the 
 * index corresponding to the wanted dBm value in each radio's power
 * tables.
 */
static const menuItem_t perMasterCC1101TxPowerMenuItems[] = 
{
  {M_DISABLED,0," Select output power",0,0,0,0,0},
  {0x00,"1"," 10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 1},      
  {0x00,"2","  7 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 7},      
  {0x00,"3","  5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 6},      
  {0x00,"4","  0 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 5},      
  {0x00,"5","-10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 4},      
  {0x00,"6","-15 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 3},      
  {M_DISABLED,0," Select output power",0,0,0,0,0},                        
  {0x00,"7","-20 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 2},      
  {0x00,"8","-30 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 0}    
};    

static const menuItem_t perCC11xLTxPowerMenuItems[] = 
{
  {M_DISABLED,0," Select output power",0,0,0,0,0},
  {0x00,"1"," 10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 1},      
  {0x00,"2","  7 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 7},      
  {0x00,"3","  5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 6},      
  {0x00,"4","  0 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 5},      
  {0x00,"5","-10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 4},      
  {0x00,"6","-15 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 3},      
  {M_DISABLED,0," Select output power",0,0,0,0,0},                        
  {0x00,"7","-20 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 2},      
  {0x00,"8","-30 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 0}    
}; 

static const menuItem_t perMasterCC1120TxPowerMenuItems[] = 
{
  {M_DISABLED,0," Select output power",0,0,0,0,0},
  {0x00,"1"," 14 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 1},   
  {0x00,"2"," 10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 6},  
  {0x00,"3","  5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 5},  
  {0x00,"4","  0 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 4},  
  {0x00,"5"," -5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 3},  
  {0x00,"6","-10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 2},  
  {M_DISABLED,0," Select output power",0,0,0,0,0},                    
  {0x00,"7","-16 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 0}   
};                                                                    

static const menuItem_t perMasterCC1200TxPowerMenuItems[] = 
{
  {M_DISABLED,0," Select output power",0,0,0,0,0},
  {0x00,"1"," 14 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 1},   
  {0x00,"2"," 10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 6},  
  {0x00,"3","  5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 5},  
  {0x00,"4","  0 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 4},  
  {0x00,"5"," -5 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 3},  
  {0x00,"6","-10 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 2},  
  {M_DISABLED,0," Select output power",0,0,0,0,0},                    
  {0x00,"7","-16 dBm",0,0,0,&perMasterTxPowerConfigApp,(void **) 0}   
}; 

static menu_t perMasterCC1101TxPowerMenu = 
{
  (menuItem_t*)perMasterCC1101TxPowerMenuItems,   /* pItems         */
  0,                                              /* pParentMenu    */
  0,                                              /* pMenuGraphics  */
  0,                                              /* pTextHeader    */
  "8",                                            /* pTextMenuItems */
  10,                                             /* nMenuItems     */
  4,                                              /* nCurrentItem   */
  4,                                              /* nSelectedItem  */
  0,                                              /* nScreen        */
  0                                               /* reservedAreas  */
};

static menu_t perCC11xLTxPowerMenu = 
{
  (menuItem_t*)perCC11xLTxPowerMenuItems,   /* pItems         */
  0,                                              /* pParentMenu    */
  0,                                              /* pMenuGraphics  */
  0,                                              /* pTextHeader    */
  "8",                                            /* pTextMenuItems */
  10,                                             /* nMenuItems     */
  4,                                              /* nCurrentItem   */
  4,                                              /* nSelectedItem  */
  0,                                              /* nScreen        */
  0                                               /* reservedAreas  */
};

static menu_t perMasterCC1120TxPowerMenu = 
{
  (menuItem_t*)perMasterCC1120TxPowerMenuItems,   /* pItems         */
  &perMasterEasyLinkConfigMenu,                   /* pParentMenu    */
  0,                                              /* pMenuGraphics  */
  0,                                              /* pTextHeader    */
  "7",                                            /* pTextMenuItems */
  9,                                              /* nMenuItems     */
  4,                                              /* nCurrentItem   */
  4,                                              /* nSelectedItem  */
  0,                                              /* nScreen        */
  0                                               /* reservedAreas  */
};
static menu_t perMasterCC1200TxPowerMenu = 
{
  (menuItem_t*)perMasterCC1200TxPowerMenuItems,   /* pItems         */
  &perMasterEasyLinkConfigMenu,                   /* pParentMenu    */
  0,                                              /* pMenuGraphics  */
  0,                                              /* pTextHeader    */
  "7",                                            /* pTextMenuItems */
  9,                                              /* nMenuItems     */
  4,                                              /* nCurrentItem   */
  4,                                              /* nSelectedItem  */
  0,                                              /* nScreen        */
  0                                               /* reservedAreas  */
};

/* Number of packets Menu */
static const menuItem_t perNumberPacketsItems[] = 
{
  {M_DISABLED,0,"  Select number of  ",0,0,0,0,0},
  {M_DISABLED,0,"      packets",0,0,0,0,0},
  {0x00,"1","100",0,0,0,&perMasterPacketsConfigApp,(void **)100},
  {0x00,"2","1000",0,0,0,&perMasterPacketsConfigApp,(void **)1000},
  {0x00,"3","10000",0,0,0,&perMasterPacketsConfigApp,(void **)10000},
  {0x00,"4","65000",0,0,0,&perMasterPacketsConfigApp,(void **)65000} 
};

static menu_t perMasterNumberPacketsMenu =
{
  (menuItem_t *)perNumberPacketsItems,  /* pItems         */
  &perMasterPerTestStartMenu,           /* pParentMenu    */
  0,                                    /* pMenuGraphics  */
  0,                                    /* pTextHeader    */
  "4",                                  /* pTextMenuItems */
  6,                                    /* nMenuItems     */
  3,                                    /* nCurrentItem   */
  3,                                    /* nSelectedItem  */
  0,                                    /* nScreen        */
  0                                     /* reservedAreas  */
};


/* Info menu - arranged it as a menu to save code space */
static const menuItem_t perMasterModeMenuInfoItems[] = 
{
  {0         ,"1","Easy Mode:"          ,0,0,0,0,0},
  {M_DISABLED,0  ,"- Few configuration ",0,0,0,0,0},
  {M_DISABLED,0  ,"  options"           ,0,0,0,0,0},
  {M_DISABLED,0  ,"- One-Way PER test"  ,0,0,0,0,0},
  {M_DISABLED,0  ,""                    ,0,0,0,0,0},
  {M_DISABLED,0  ,""                    ,0,0,0,0,0},
  {M_DISABLED,0  ,""                    ,0,0,0,0,0},
  
  
  {0         ,"2","Expert Mode:"        ,0,0,0,0,0},
  {M_DISABLED,0  ,"- More configuration",0,0,0,0,0},
  {M_DISABLED,0  ,"  options"           ,0,0,0,0,0},
  {M_DISABLED,0  ,"- One-Way PER test"  ,0,0,0,0,0},
  {M_DISABLED,0  ,"- Two-Way PER test"  ,0,0,0,0,0},
  {M_DISABLED,0  ,"  with re-transmit " ,0,0,0,0,0},
  {M_DISABLED,0  ,""                    ,0,0,0,0,0}
};

static menu_t perMasterModeMenuInfo   = 
{
  (menuItem_t *)perMasterModeMenuInfoItems,
  0,
  0,
  "PER Test",
  "2",
  14,
  0,
  -1,
  0,
  0
};

/* Info menu - arranged it as a menu to save code space */
static const menuItem_t perChipSelectMenuInfoItems[] = 
{
  {0         ,"1","Stand Alone:"          ,0,0,0,0,0},
  {M_DISABLED,0  ," - CC11xx Evaluation ",0,0,0,0,0},
  {M_DISABLED,0  ,"  Module ",0,0,0,0,0},
  {0         ,"2","Combo:"        ,0,0,0,0,0},
  {M_DISABLED,0  ," - CC11xx with CC1190"  ,0,0,0,0,0},
  {M_DISABLED,0  ,"   RF front end."          ,0,0,0,0,0}
};

static menu_t perChipSelectMenuInfo   = 
{
  (menuItem_t *)perChipSelectMenuInfoItems,
  0,
  0,
  "PER Test",
  "2",
  6,
  0,
  -1,
  0,
  0
};
/****************************************************************************
*  Menu's for Value Line configuration
*/
static const menuItem_t perSlaveValueLineLinkConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perCC11xLTxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartValueLineApp,0}
};

static const menuItem_t perMasterValueLineLinkConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesValueLineApp,0}
};



static menu_t perValueLineLinkConfigMenu =
{
  (menuItem_t*)perMasterValueLineLinkConfigMenuItems,  /* pItems : Assigned after chip detect*/
  0,                                                    /* pParentMenu                        */
  0,                                                    /* pMenuGraphics                      */
  0,                                                    /* pTextHeader                        */
  "2",                                                  /* pTextMenuItems                     */
  4,                                                    /* nMenuItems                         */
  3,                                                    /* nCurrentItem                       */
  -2,                                                   /* nSelectedItem                      */
  0,                                                    /* nScreen                            */
  0                                                     /* reservedAreas                      */
};

/****************************************************************************
*  Menu's for Link Bypass configuration
*/
static const menuItem_t perCC11xLMasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static const menuItem_t perCC1101MasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1101DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static const menuItem_t perCC112xMasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1120DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static const menuItem_t perCC1101CC1190MasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static const menuItem_t perCC1120CC1190MasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static const menuItem_t perCC120xMasterLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1200DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","Start test",0,&perMasterPerTestStartMenu,0,&perMasterConfigureDevicesLinkBypassApp,0}
};
static menu_t perMasterLinkBypassConfigMenu =
{
  (menuItem_t*)perCC1101MasterLinkBypassConfigMenuItems,  /* pItems : Assigned after chip detect*/
  0,                                                    /* pParentMenu                        */
  0,                                                    /* pMenuGraphics                      */
  0,                                                    /* pTextHeader                        */
  "2",                                                  /* pTextMenuItems                     */
  4,                                                    /* nMenuItems                         */
  3,                                                    /* nCurrentItem                       */
  -2,                                                   /* nSelectedItem                      */
  0,                                                    /* nScreen                            */
  0                                                     /* reservedAreas                      */
};

static const menuItem_t perCC11xLSlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perCC11xLDataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perCC11xLTxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};

static const menuItem_t perCC1101SlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1101DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1101TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};

static const menuItem_t perCC112xSlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1120DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1120TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};
static const menuItem_t perCC1101CC1190SlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {M_DISABLED,"2","TX Power",0,0,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};
static const menuItem_t perCC1120CC1190SlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1190DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {M_DISABLED,"2","TX Power",0,0,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};
static const menuItem_t perCC120xSlaveLinkBypassConfigMenuItems[] =
{   
 {M_DISABLED,0,"  Link Configuration",0,0,0,0,0},
 {0x00,"1","Data Rate",0,&perMasterCC1200DataRateMenu,0,0,0}, 
 {M_DISABLED|M_DUMMY|M_SPLIT|M_FLOAT2,0,"   [kbit/s]",&guiDataRate,0,0,0,0},
 {0x00,"2","TX Power",0,&perMasterCC1200TxPowerMenu,0,0,0},
 {M_DISABLED|M_DUMMY|M_SPLIT,0,"   [dBm]",&guiTxPower,0,0,0,0},
 {0x00,"3","Start test",0,0,0,&perSlaveStartLinkBypassApp,0}
};
static menu_t perSlaveLinkBypassConfigMenu =
{
  (menuItem_t*)perCC1101SlaveLinkBypassConfigMenuItems,  /* pItems : Assigned after chip detect*/
  0,                                                    /* pParentMenu                        */
  0,                                                    /* pMenuGraphics                      */
  0,                                                    /* pTextHeader                        */
  "3",                                                  /* pTextMenuItems                     */
  6,                                                    /* nMenuItems                         */
  5,                                                    /* nCurrentItem                       */
  -2,                                                   /* nSelectedItem                      */
  0,                                                    /* nScreen                            */
  0                                                     /* reservedAreas                      */
};

/******************************************************************************
 * Functions
 */


/******************************************************************************
 * @fn          perResetMenuVariables
 *
 * @brief       Function is called from the PER core. It resets menu variables,
 *              not any configurations, since that will override configurator 
 *              mode. Dependant on radio inserted.
 *              
 * input parameters
 *
 * output parameters
 *
 * @return      void
 */
void perResetMenuVariables(void)
{   
  /* Menu setup according to chip detect */
  if(perRadioChipType.deviceName == CHIP_TYPE_CC1101)
  {  
    perHeadMenu.pItems                         = (menuItem_t*)perCC1101HeadMenuItems;
    perHeadMenu.pTextMenuItems                 = "3";/* pTextMenuItems */
    perHeadMenu.nMenuItems                     = 4;         /* nMenuItems     */
    perExpertModeMenu.pItems                   = (menuItem_t*)perCC1101ExpertModeMenuItems;
    perCC1101FrequencyMenu.nCurrentItem        = 2;
    perCC1101FrequencyMenu.nSelectedItem       = 2;
    perMasterExpertOneWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertOneWayCC1101LinkConfigMenuItems;
    perMasterExpertTwoWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertTwoWayCC1101LinkConfigMenuItems;
    perMasterEasyLinkConfigMenu.pItems         = (menuItem_t*)perMasterEasyLinkConfigCC1101MenuItems;
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC1101MasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC1101SlaveLinkBypassConfigMenuItems;
    
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC110L)
  {
    perHeadMenu.pItems                         = (menuItem_t*)perCC110LHeadMenuItems;
    perHeadMenu.pTextMenuItems                 = "3";/* pTextMenuItems */
    perHeadMenu.nMenuItems                     = 4;         /* nMenuItems     */
    perExpertModeMenu.pItems                   = (menuItem_t*)perCC110LExpertModeMenuItems;
    perCC11xLFrequencyMenu.nCurrentItem        = 3;
    perCC11xLFrequencyMenu.nSelectedItem       = 3;
    
    perMasterExpertOneWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertOneWayCC110LLinkConfigMenuItems;
    perMasterExpertTwoWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertTwoWayCC110LLinkConfigMenuItems;
    perMasterEasyLinkConfigMenu.pItems         = (menuItem_t*)perMasterEasyLinkConfigCC110LMenuItems;
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC11xLMasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC11xLSlaveLinkBypassConfigMenuItems;
    
    perAbstractHeadMenu = perHeadMenu;
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC113L)
  {
    perHeadMenu.pItems                         = (menuItem_t*)perCC113LHeadMenuItems;
    perHeadMenu.pTextMenuItems                 = "2";/* pTextMenuItems */
    perHeadMenu.nMenuItems                     = 3;         /* nMenuItems     */
    perCC11xLFrequencyMenu.nCurrentItem        = 3;
    perCC11xLFrequencyMenu.nSelectedItem       = 3;
    
    /* Disable expert mode in perMasterModeSelectionMenu*/
    perDeviceModeMenu.pItems = (menuItem_t*)perCC113LDeviceModeMenuItems;
    perDeviceModeMenu.pTextMenuItems = "1";
    perDeviceModeMenu.nMenuItems =2;
    
    perAbstractHeadMenu = perHeadMenu;
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC115L)
  {
 
    perHeadMenu.pItems                         = (menuItem_t*)perCC115LHeadMenuItems;
    perHeadMenu.pTextMenuItems                 = "2";/* pTextMenuItems */
    perHeadMenu.nMenuItems                     = 3;         /* nMenuItems     */
    perCC11xLFrequencyMenu.nCurrentItem        = 3;
    perCC11xLFrequencyMenu.nSelectedItem       = 3;
        
    /* Disable expert mode in perMasterModeSelectionMenu*/
    perDeviceModeMenu.pItems = (menuItem_t*)perCC115LDeviceModeMenuItems;
    perDeviceModeMenu.pTextMenuItems = "1";
    perDeviceModeMenu.nMenuItems =2;
          
    /* Set link config menu*/
    perValueLineLinkConfigMenu.pItems          = (menuItem_t*)perSlaveValueLineLinkConfigMenuItems;
    perValueLineLinkConfigMenu.pTextMenuItems  = "3";/* pTextMenuItems                     */
    perValueLineLinkConfigMenu.nMenuItems      = 6; /* nMenuItems                         */
    perValueLineLinkConfigMenu.nCurrentItem    = 5;/* nCurrentItem                       */
    
    
    perAbstractHeadMenu = perHeadMenu;
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1101_CC1190) 
  {
    perHeadMenu.pItems = (menuItem_t*)perHeadCC1190MenuItems;
    perHeadMenu.pTextMenuItems =      "2";/* pTextMenuItems */
    perHeadMenu.nMenuItems = 2;         /* nMenuItems     */
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC1101CC1190MasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC1101CC1190SlaveLinkBypassConfigMenuItems;
    /* Disable expert mode in perMasterModeSelectionMenu*/
    perDeviceModeMenu.pItems = (menuItem_t*)perCC1190DeviceModeMenuItems;
    
    /* Set link config menu*/
    perMasterEasyLinkConfigMenu.pItems = (menuItem_t*)perMasterEasyLinkConfigCC1101CC1190MenuItems;
    
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1120_CC1190) 
  {
    perHeadMenu.pItems = (menuItem_t*)perHeadCC1190MenuItems;
    perHeadMenu.pTextMenuItems =      "2";/* pTextMenuItems */
    perHeadMenu.nMenuItems = 3;         /* nMenuItems     */
          
   perCC1190FrequencyMenu.pItems             =  (menuItem_t*)perCC1120CC1190FrequencyMenuItems;     /* pItems         */
   perCC1190FrequencyMenu.pTextMenuItems     =   "2";                                        /* pTextMenuItems */
   perCC1190FrequencyMenu.nMenuItems         =   3;                                            /* nMenuItems     */
   perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC1120CC1190MasterLinkBypassConfigMenuItems;
   perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC1120CC1190SlaveLinkBypassConfigMenuItems;
   
   /* Disable expert mode in perMasterModeSelectionMenu*/
    perDeviceModeMenu.pItems = (menuItem_t*)perCC1190DeviceModeMenuItems;
    
    /* Set link config menu*/
    perMasterEasyLinkConfigMenu.pItems = (menuItem_t*)perMasterEasyLinkConfigCC1120CC1190MenuItems;
    
  }
  else if((perRadioChipType.deviceName == CHIP_TYPE_CC1200)||
            (perRadioChipType.deviceName == CHIP_TYPE_CC1201))
  {
    perHeadMenu.pItems                         = (menuItem_t*)perCC120xHeadMenuItems;
    perExpertModeMenu.pItems                   = (menuItem_t*)perCC120xExpertModeMenuItems;
    perCC120xFrequencyMenu.nCurrentItem        = 3;
    perCC120xFrequencyMenu.nSelectedItem       = 3; 
    perMasterExpertOneWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertOneWayCC1200LinkConfigMenuItems;
    perMasterExpertTwoWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertTwoWayCC1200LinkConfigMenuItems;
    perMasterEasyLinkConfigMenu.pItems         = (menuItem_t*)perMasterEasyLinkConfigCC1200MenuItems;   
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC120xMasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC120xSlaveLinkBypassConfigMenuItems;    
    
      perAbstractHeadMenu = perHeadMenu;
    
  }
  else if(perRadioChipType.deviceName == CHIP_TYPE_CC1125)
  {
    perHeadMenu.pItems                         = (menuItem_t*)perCC112xHeadMenuItems;
    perExpertModeMenu.pItems                   = (menuItem_t*)perCC112xExpertModeMenuItems;
    perMasterCC1120DataRateMenu.pItems         = (menuItem_t*)perMasterCC1125DataRateMenuItems;
    perMasterCC1120DataRateMenu.nCurrentItem   = 2;
    perMasterCC1120DataRateMenu.nSelectedItem  = 2;
    perCC112xFrequencyMenu.nCurrentItem        = 3;
    perCC112xFrequencyMenu.nSelectedItem       = 3; 
    perMasterExpertOneWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertOneWayCC1120LinkConfigMenuItems;
    perMasterExpertTwoWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertTwoWayCC1120LinkConfigMenuItems;
    perMasterEasyLinkConfigMenu.pItems         = (menuItem_t*)perMasterEasyLinkConfigCC1120MenuItems;   
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC112xMasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC112xSlaveLinkBypassConfigMenuItems; 
    perAbstractHeadMenu                        = perHeadMenu;
  }
  else /*CHIP_TYPE_CC1120 & CHIP_TYPE_CC1121  */
  {
    perHeadMenu.pItems                         = (menuItem_t*)perCC112xHeadMenuItems;
    perExpertModeMenu.pItems                   = (menuItem_t*)perCC112xExpertModeMenuItems;
    perCC112xFrequencyMenu.nCurrentItem        = 3;
    perCC112xFrequencyMenu.nSelectedItem       = 3; 
    perMasterExpertOneWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertOneWayCC1120LinkConfigMenuItems;
    perMasterExpertTwoWayLinkConfigMenu.pItems = (menuItem_t*)perMasterExpertTwoWayCC1120LinkConfigMenuItems;
    perMasterEasyLinkConfigMenu.pItems         = (menuItem_t*)perMasterEasyLinkConfigCC1120MenuItems;   
    perMasterLinkBypassConfigMenu.pItems       = (menuItem_t*)perCC112xMasterLinkBypassConfigMenuItems;
    perSlaveLinkBypassConfigMenu.pItems        = (menuItem_t*)perCC112xSlaveLinkBypassConfigMenuItems;    
    
    // If cc1120 is detected, keep ChipSelectMenu as head menu
    if(perRadioChipType.deviceName != CHIP_TYPE_CC1120)
    {
      perAbstractHeadMenu = perHeadMenu;
    }
  }
  perMasterExpertLinkToplogyMenu.nCurrentItem  = 1;
  

  return;
}



/******************************************************************************
 * @fn          perEditSlaveStatusApp
 *
 * @brief       Function that is called by the menu system to change the
 *              frequency band. The function does not check for input 
 *              parameters bounds.
 *
 * input parameters                    
 *              
 * @param       pDummy - frequency band index casted to void**
 *
 * output parameters
 *
 * @return      PER_RETURN_SUCCESS
 */
uint8 perSlaveEditFrequencyApp(void** pDummy)
{
  uint16 freqBand = (uint16)*pDummy; 
  perSettings.frequencyBand = freqBand;                     
  return PER_RETURN_SUCCESS;
}

/******************************************************************************
 * @fn          perMasterTxPowerConfigApp
 *
 * @brief       Function is called by menu system to configure the test to use
 *              the selected TX power. The function does
 *              not check if the input parameter is a legal value. 
 *                
 * input parameters             
 *
 * @param       pDummy - index in tx output power table casted to void**
 *
 * output parameters
 *
 * @return      PER_RETURN_SUCCESS
 */
uint8 perMasterTxPowerConfigApp(void** pDummy)
{
  int16 index = (uint16) *pDummy;
  
  perSettings.txPower = index;
  guiTxPower = perRfApi.perGetGuiTxPower(index);
  return PER_RETURN_SUCCESS;
}
/******************************************************************************
 * @fn          perMasterPacketsConfigApp
 *
 * @brief       Function is called by menu system to configure the test to use
 *              the selected number of packets. The function does not check 
 *              if input parameter is in the valid range[1,61]
 *
 * input parameters                
 *              
 * @param       pDummy - number of packets casted to void**
 *
 * output parameters
 *
 * @return      PER_RETURN_SUCCESS
 */          
uint8 perMasterPacketsConfigApp(void** pDummy)
{
  uint16 v = (uint16)*pDummy;
  perSettings.totalNumPackets = v;
  return PER_RETURN_SUCCESS;
}

/******************************************************************************
 * @fn          perMasterSetLinkTypeModeApp
 *
 * @brief       Called from the menu system to configure link type and 
 *              as a cause of it manipulate the linked list of menus to 
 *              reflect the choice. It also defaults settings so that everything
 *              is consistent.
 *    
 * input parameters
 *          
 * @param       pDummy  - id value  casted to void**:
 *                        0 : easy mode, 1: expert mode - one way, 
 *                        2 : expert mode - two way 
 * 
 * output parameters
 *
 * @return      PER_RETURN_SUCCESS
 */
uint8 perMasterSetLinkTypeModeApp(void** pDummy) 
{
  uint16 param = (uint16) *pDummy;
  perSettings.linkTopology = param;
  return PER_RETURN_SUCCESS;

}
 
/******************************************************************************
 * @fn          perMasterSetRetransmissionsApp
 *
 * @brief       Called from the menu system to configure allowed packets in 
 *              two way PER test.
 *    
 * input parameters
 *          
 * @param       pDummy  - nr of allowed retransmits casted to void**
 *                        
 *                        
 * output parameters
 *
 * @return      PER_RETURN_SUCCESS
 */
uint8 perMasterSetRetransmissionsApp(void** pDummy)
{
  uint16 value = (uint16) *pDummy;
  perSettings.nAllowedRetransmits = value;
  guiPacketRetransmits2Way = value;
  return PER_RETURN_SUCCESS;
}
        
/***********************************************************************************
* @fn          cpyStatusString
*
* @brief       Copies a string
*               
*
* @param       pStringTo   - To
*              pStringFrom - From
*              length      - String length
*
* @return      void 
*              
*/
void cpyStatusString(char *pStringTo, const char *pStringFrom, uint8 length)
{
 uint8 i;
 for(i = 0;i<length-1;i++)
 { 
  *(pStringTo+i) = *(pStringFrom+i);
 }
 return;
}


/***********************************************************************************
* @fn          perConfigureDeviceModeMenuApp
*
* @brief       Set the correct menuItems in the perDeviceModeMenu menu
*               
* input parameters
*
* @param       
*
* output parameters
*
* @return      Computed packet rate 
*              
*/
uint8 perConfigureDeviceModeMenuApp(void** pDummy)
{
  uint16 param   = (uint16) *pDummy;  
  if(param == EASY_MODE)
  {
    // Different device menu for receivers and transmitters
    if(perRadioChipType.deviceName == CHIP_TYPE_CC115L)
    {
      perDeviceModeMenu.pItems = (menuItem_t*)perCC115LDeviceModeMenuItems;;
    }
    else if(perRadioChipType.deviceName == CHIP_TYPE_CC113L)
    {
      perDeviceModeMenu.pItems = (menuItem_t*)perCC113LDeviceModeMenuItems;;
    }    
    else
    {
      perDeviceModeMenu.pItems = (menuItem_t*)perEasyDeviceModeMenuItems;
    }
  }
  else if(param == EXPERT_MODE)
  {
    perDeviceModeMenu.pItems = (menuItem_t*)perExpertDeviceModeMenuItems;
  }
  else if(param == LINK_BYPASS_MODE)
  {
      perDeviceModeMenu.pItems = (menuItem_t*)perLinkBypassDeviceModeMenuItems;
  }
  else
  {
    perDeviceModeMenu.pItems = (menuItem_t*)perConfiguratorDeviceModeMenuItems;
  }
  return PER_RETURN_SUCCESS;
}
/***********************************************************************************
* @fn          perSetDefaultLinkParametersApp
*
* @brief       Sets default link parameters when traversing in the menu tree given 
*              detected chip type.
*               
* input parameters
*
* @param       EASY/EXPERT/CONFIGURATOR MODE selected
*
* output parameters
*
* @return      PER_RETURN_SUCCESS
*              
*/
uint8 perSetDefaultLinkParametersApp(void** pDummy)
{
  uint16 param   = (uint16) *pDummy;  
  if(param != CONFIGURATOR_MODE)
  {
    switch(perRadioChipType.deviceName)
    {
      case CHIP_TYPE_CC1101:
        guiPacketLength                           = CC1101_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC1101_DEFAULT_PACKETLENGTH-1); /* => 20 B packet for CC1101 in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 10 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 5; 
        perMasterPacketLengthMenu.nScreen         = 0;
        perMasterCC1101TxPowerMenu.nCurrentItem   = 1; /* 10 dBm */
        perMasterCC1101TxPowerMenu.nSelectedItem  = 1; /* 10 dBm */
        perMasterCC1101TxPowerMenu.nScreen        = 0; /* 10 dBm */
        perMasterCC1101DataRateMenu.nCurrentItem  = 1;
        perMasterCC1101DataRateMenu.nSelectedItem = 1; 
        
        guiTxPower                                = 10;  /* dBm */
        
        break;
     case CHIP_TYPE_CC110L:
        guiPacketLength                           = CC11xL_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC11xL_DEFAULT_PACKETLENGTH-1); /* => 20 B packet for CC110L in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 10 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 5; 
        perMasterPacketLengthMenu.nScreen         = 0;
        perCC11xLTxPowerMenu.nCurrentItem   = 1; /* 10 dBm */
        perCC11xLTxPowerMenu.nSelectedItem  = 1; /* 10 dBm */
        perCC11xLTxPowerMenu.nScreen        = 0; /* 10 dBm */
        perCC11xLDataRateMenu.nCurrentItem  = 1;
        perCC11xLDataRateMenu.nSelectedItem = 1; 
        
        guiTxPower                                = 10;  /* dBm */
        break;
     case CHIP_TYPE_CC113L:
        guiPacketLength                           = CC11xL_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC11xL_DEFAULT_PACKETLENGTH-1); /* => 20 B packet for CC113L in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 10 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 5; 
        perMasterPacketLengthMenu.nScreen         = 0;
        perCC11xLDataRateMenu.nCurrentItem  = 1;
        perCC11xLDataRateMenu.nSelectedItem = 1; 
       
        break;          
     case CHIP_TYPE_CC115L:
        guiPacketLength                           = CC11xL_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC11xL_DEFAULT_PACKETLENGTH-1); /* => 20 B packet for CC115L in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 10 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 5; 
        perMasterPacketLengthMenu.nScreen         = 0;
        perCC11xLTxPowerMenu.nCurrentItem   = 1; /* 10 dBm */
        perCC11xLTxPowerMenu.nSelectedItem  = 1; /* 10 dBm */
        perCC11xLTxPowerMenu.nScreen        = 0; /* 10 dBm */
        perCC11xLDataRateMenu.nCurrentItem  = 1;
        perCC11xLDataRateMenu.nSelectedItem = 1; 
        
        guiTxPower                                = 10;  /* dBm */
        break;        
      case CHIP_TYPE_CC1101_CC1190:                      
        guiPacketLength                           = CC1101_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC1101_DEFAULT_PACKETLENGTH-1); /* => 20 B packet for CC1101-CC1190 in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 20/26 */
        perMasterCC1190DataRateMenu.nCurrentItem  = 1;
        perMasterCC1190DataRateMenu.nSelectedItem = 1; 
        
        
        if(perSettings.frequencyBand == 0)
        {
            perMasterCC1190DataRateMenu.pItems = (menuItem_t*)perMaster869MHzCC1101CC1190DataRateMenuItems; /* pItems           */
            guiTxPower                                = 20;  /* dBm */
        }        
        else
        {
            perMasterCC1190DataRateMenu.pItems = (menuItem_t*)perMaster915MHzCC1101CC1190DataRateMenuItems;
            guiTxPower                                = 26; /* dBm */
        }
        
        break;
      case CHIP_TYPE_CC1120_CC1190:                      
        guiPacketLength                           = CC112X_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC112X_DEFAULT_PACKETLENGTH-1); /* => 3 B packet for CC1120-CC1190 in EASY MODE */  
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 20 */
        
        perMasterCC1190DataRateMenu.pItems = (menuItem_t*)perMasterCC1120CC1190DataRateMenuItems;
        perMasterCC1190DataRateMenu.nCurrentItem  = 1;
        perMasterCC1190DataRateMenu.nSelectedItem = 1; 
        perMasterCC1190DataRateMenu.pTextMenuItems = "2";  /* pTextMenuItems   */
        perMasterCC1190DataRateMenu.nMenuItems     =  3;   /* nMenuItems       */
        
        if(perSettings.frequencyBand == 0)
        {
           // 869 MHz
           guiTxPower                                = 27;  /* dBm */
        }        
        else
        {
          // 915 Mhz
          guiTxPower                                = 26; /* dBm */
        }

        
        break;        
    case CHIP_TYPE_CC1120:
    case CHIP_TYPE_CC1121:
        guiPacketLength                           = CC112X_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC112X_DEFAULT_PACKETLENGTH-1); /* => 3 B packet for CC112x in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 14 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 1; 
        perMasterPacketLengthMenu.nScreen         = 0; 
        perMasterCC1120TxPowerMenu.nCurrentItem   = 1; /* 14 dBm */
        perMasterCC1120TxPowerMenu.nSelectedItem  = 1; /* 14 dBm */
        perMasterCC1120TxPowerMenu.nScreen        = 0; /* 14 dBm */
        perMasterCC1120DataRateMenu.nCurrentItem  = 1;
        perMasterCC1120DataRateMenu.nSelectedItem = 1;
        
        guiTxPower                                = 14;  /* dBm */
       
        break;
        
    case CHIP_TYPE_CC1125: 
        guiPacketLength                           = CC112X_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC112X_DEFAULT_PACKETLENGTH-1); /* => 3 B packet for CC112x in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 14 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 1; 
        perMasterPacketLengthMenu.nScreen         = 0; 
        perMasterCC1120TxPowerMenu.nCurrentItem   = 3; /* 5 dBm */
        perMasterCC1120TxPowerMenu.nSelectedItem  = 3; /* 5 dBm */
        perMasterCC1120TxPowerMenu.nScreen        = 0; /* 5 dBm */
        perMasterCC1120DataRateMenu.nCurrentItem  = 1;
        perMasterCC1120DataRateMenu.nSelectedItem = 1;
        
        guiTxPower                                = 5;  /* dBm */
     
       break;
        
    case CHIP_TYPE_CC1200:
    case CHIP_TYPE_CC1201:
        guiPacketLength                           = CC120X_DEFAULT_PACKETLENGTH;
        guiDataRate                               = perRfApi.perGetDataRate(SMARTRF_CONFIGURATION_0);
        perSettings.payloadLength                 = (CC120X_DEFAULT_PACKETLENGTH-1); /* => 3 B packet for CC112x in EASY MODE */
        perSettings.smartRfConfiguration          = SMARTRF_CONFIGURATION_0; 
        perSettings.txPower                       = 1; /* Index in power table corresponding to 14 */
        perMasterPacketLengthMenu.nCurrentItem    = 1;
        perMasterPacketLengthMenu.nSelectedItem   = 1; 
        perMasterPacketLengthMenu.nScreen         = 0; 
        perMasterCC1200TxPowerMenu.nCurrentItem   = 1; /* 14 dBm */
        perMasterCC1200TxPowerMenu.nSelectedItem  = 1; /* 14 dBm */
        perMasterCC1200TxPowerMenu.nScreen        = 0; /* 14 dBm */
        perMasterCC1200DataRateMenu.nCurrentItem  = 1;
        perMasterCC1200DataRateMenu.nSelectedItem = 1;
        
        guiTxPower                                = 14;  /* dBm */
        
        break;        
      default:
        break;
    }
    
    /* Defaulting perSettings members */  
    perSettings.nAllowedRetransmits  = 3;    
    guiPacketRetransmits2Way         = 3;
    perSettings.rssiWindowSize       = DEFAULT_RSSI_AVERAGE_WIN_SIZE;    
    perMasterRetransmitMenu.nCurrentItem  = 4;
    perMasterRetransmitMenu.nSelectedItem = 4;
  }
  
  /* Even in configurator mode it will be nice to change the number of packets to communicate */ 
  /* perSettings.totalNumPackets must be consistent with the menu */
  perSettings.totalNumPackets              = 65000;
  perMasterNumberPacketsMenu.nSelectedItem = 5;
  perMasterNumberPacketsMenu.nCurrentItem  = 5;
  perMasterNumberPacketsMenu.nScreen       = 0;
  
  return PER_RETURN_SUCCESS;
}

/******************************************************************************
 * @fn          perSetDataRateApp
 *
 * @brief       Sets graphical datarate according to provided value of
 *              SMARTRF_CONFIGURATION_X, X = {0,1,2}
 *                
 *              
 * @param       pDummy -  (void **) SMARTRF_CONFIGURATION_X
 *
 * @return      PER_RETURN_SUCCESS
 */         
uint8 perMasterSetDataRateApp(void** pDummy)
{
  uint16 index = (uint16) *pDummy;
  perSettings.smartRfConfiguration = index;
  guiDataRate = perRfApi.perGetDataRate(index);
  return PER_RETURN_SUCCESS;
}     





/***********************************************************************************
* @fn          perMasterSetPacketLength
*
* @brief       Configures the packet length of the packets. The total total packet
*              will be payload length + 1
*               
* input parameters
*
* @param       Selected packet length
*
* output parameters
*
* @return      PER_RETURN_SUCCESS
*              
*/
uint8 perMasterSetPacketLengthApp(void** pDummy)
{
  uint16 tmp = (uint16) *pDummy;
  perSettings.payloadLength = tmp - 1;
  guiPacketLength          = tmp;
  return PER_RETURN_SUCCESS;
}

/***********************************************************************************
* @fn          perMasterSetLinkApp
*
* @brief       Sets the Link Type
*               
* input parameters
*
* @param       LINK_1_WAY/LINK_2_WAY
*
* output parameters
*
* @return      PER_RETURN_SUCCESS
*              
*/
uint8 perMasterSetLinkApp(void** pDummy)
{
  perSettings.linkTopology = (uint16) *pDummy;
  return PER_RETURN_SUCCESS;
};
