//-----------------------------------------------------------------------------
// \file    evmomapl138_usb.c
// \brief   implementation of the usb host and otg driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_sysconfig.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_usb.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
#define USB_CTRLR     *( volatile uint32_t* )( 0x01e00004 )
#define USB_STATR     *( volatile uint32_t* )( 0x01e00008 )
#define USB_DEVCTRL   *( volatile uint32_t* )( 0x01e00460 )

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
#define USB_OTG_POWER_HSEN			0x20
#define USB_OTG_POWER_ENSUSPM		0x01
#define USB_OTG_POWER_SOFTCONN		0x40

#define USB_OTG_DEVCTL_SESSION		0x01


//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the usb host and otg for use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...pmic ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t USB_init(void)
{
	uint32_t rtn = ERR_NO_ERROR;
   
	// Unlock bootcfg registers
	SYSCONFIG->KICKR[0] = KICK0R_UNLOCK;
	SYSCONFIG->KICKR[1] = KICK1R_UNLOCK;

	// Wake up USB module
	// TODO: Check this against a 300MHz
	SYSCONFIG->CFGCHIP[2] = 0x00002872;  // Internal USB clock, 24MHz system clock, out of reset, force host

	// Reset usb otg module
	USB_OTG->CTRLR = 0x00000001;

	// Wait for power off
	printf("Waiting for VBUS low...\n");
	USTIMER_delay(500000);
	while ((SYSCONFIG->CFGCHIP[2] & 0x00010000) != 0);

	// Start session
	SETBIT(USB_OTG_DEVCTL, 0x00000001);
	USTIMER_delay(500000);

	printf("--> USB power enabled\n\nWaiting for VBUS high...\n");

	if ((SYSCONFIG->CFGCHIP[2] & 0x00010000) == 0)
	{
	   rtn = ERR_INIT_FAIL;  // Fail
	}

	printf("--> VBUS is high.\n");

	return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   get the OHCI Revision Number
//
// \param   none.
//
// \return  uint8_t OHCI Revisino Number (0x10)
//-----------------------------------------------------------------------------
uint8_t USB_getHostRevision(void)
{
   uint32_t hcrevision = USB_HOST->HCREVISION;

   return((uint8_t) hcrevision);
}


//----------------------------------------------------------------------------
// \brief   Configure the USB module for OTG mode.
//
// \return  none.
//-----------------------------------------------------------------------------
void USB_OTG_init()
{
	uint16_t I;

	// **************************************************************************
	// Configure DRVVBUS Pin to be used for USB
	// **************************************************************************
	// MAKE SURE WRITE ACCESS KEY IS INITIALIZED PRIOR TO ACCESSING ANY OF THE
	// BOOTCFG REGISTERS.
	//SYSCONFIG->KICKR[0] = KICK0R_UNLOCK;
	//SYSCONFIG->KICKR[1] = KICK1R_UNLOCK;

	/* CONFIGURE THE DRVVBUS PIN HERE.*/
	/* See your device-specific System Reference Guide for more information on how to set up the
	pinmux. */

	// Power up the USB HW.
	EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_USB0, PSC_ENABLE);


	// Reset the USB controller:
	USB_OTG->CTRLR |= 0x00000001;

	//Wait until controller is finished with Reset. When done, it will clear the RESET bit field.
	while ((USB_OTG->CTRLR & 0x1) == 1);

	// RESET: Hold PHY in Reset
	SYSCONFIG->CFGCHIP[2] |= 0x00008000; // Hold PHY in Reset

	// Drive Reset for few clock cycles
	for (I=0; I < 50; I++);

	// RESET: Release PHY from Reset
	SYSCONFIG->CFGCHIP[2] &= 0xFFFF7FFF; // Release PHY from Reset



	/* Configure PHY with the Desired Operation */
	// OTGMODE
	SYSCONFIG->CFGCHIP[2] &= 0xFFFF9FFF; // 00= > Do Not Override PHY Values
	//SYSCONFIG->CFGCHIP[2] |= 0x00002000; // 11= > Override PHY Values, force host, VBUS low

	// PHYPWDN
	SYSCONFIG->CFGCHIP[2] &= 0xFFFFFBFF; // 1/0 = > PowerdDown/ NormalOperation

	// OTGPWRDN
	SYSCONFIG->CFGCHIP[2] &= 0xFFFFFDFF; // 1/0 = > PowerDown/ NormalOperation

	// DATAPOL
	SYSCONFIG->CFGCHIP[2] |= 0x00000100; // 1/0 = > Normal/ Reversed

	// SESNDEN
	SYSCONFIG->CFGCHIP[2] |= 0x00000020; // 1/0 = > NormalOperation/ SessionEnd

	// VBDTCTEN
	SYSCONFIG->CFGCHIP[2] |= 0x00000010; // 1/0 = > VBUS Comparator Enable/ Disable

	/* Configure PHY PLL use and Select Source */
	// REF_FREQ[3:0]
	SYSCONFIG->CFGCHIP[2] |= 0x00000002; // 0010b = > 24MHz Input Source
	
	// USB2PHYCLKMUX: Select External Source
	SYSCONFIG->CFGCHIP[2] &= 0xFFFFF7FF; // 1/0 = > Internal/External(Pin)
	
	// PHY_PLLON: On Simulation PHY PLL is OFF
	SYSCONFIG->CFGCHIP[2] |= 0x00000040; // 1/0 = > On/ Off



	/* Wait Until PHY Clock is Good */
	while ((SYSCONFIG->CFGCHIP[2] & 0x00020000) == 0); // Wait Until PHY Clock is Good.
	
	#ifndef HS_ENABLE

	// Disable high-speed
	//CSL_FINS(USB_OTG->POWER,USB_OTG_POWER_HSEN,0);
	USB_OTG->POWER &= ~USB_OTG_POWER_HSEN;
	#else
	
	// Enable high-speed
	//CSL_FINS(usbRegs->POWER,USB_OTG_POWER_HSEN,1);
	USB_OTG->POWER |= USB_OTG_POWER_HSEN;

	#endif
	
	// Enable Interrupts
	// Enable interrupts in OTG block
	USB_OTG->CTRLR &= 0xFFFFFFF7; // Enable PDR2.0 Interrupt
	USB_OTG->INTRTXE = 0x1F; // Enable All Core Tx Endpoints Interrupts + EP0 Tx/Rx interrupt
	USB_OTG->INTRRXE = 0x1E; // Enable All Core Rx Endpoints Interrupts
	
	// Enable all interrupts in OTG block
	USB_OTG->INTMSKSETR = 0x01FF1E1F;
	
	// Enable all USB interrupts in MUSBMHDRC
	USB_OTG->INTRUSBE = 0xFF;
	
	// Enable SUSPENDM so that suspend can be seen UTMI signal
	//CSL_FINS(USB_OTG->POWER,USB_OTG_POWER_ENSUSPM,1);
	USB_OTG->POWER |= USB_OTG_POWER_ENSUSPM;
	
	//Clear all pending interrupts
	USB_OTG->INTCLRR = USB_OTG->INTSRCR;

	// Start a session
	//CSL_FINS(USB_OTG->DEVCTL,USB_OTG_DEVCTL_SESSION,1);
	//USB_OTG->DEVCTL |= USB_OTG_DEVCTL_SESSION;

}



//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
