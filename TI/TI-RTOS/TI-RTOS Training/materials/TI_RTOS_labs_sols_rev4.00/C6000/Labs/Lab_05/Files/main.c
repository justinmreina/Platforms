//-----------------------------------------------------------------------------
// Project: Blink LED on C6748 LCDK Using Hwi (STARTER)
// Author:  Eric Wilbur
// Date:    June 2014
//
// Follow these steps to create this project in CCSv6.0:
// 1. Project -> New CCS Project
// 2. Select Template:
//    - TI-RTOS for C6000 -> Kernel Examples -> TI Target Ex -> Minimal ->
//    - this template does NOT contain UIA/RTOS Analyzer support
// 3. Add UIA from available products
// 4. Add main.c from TI-RTOS Workshop Solution file for this lab
// 5. Edit as needed (to add/subtract) BIOS services, delete given Task
// 6. Add include search path for PDK
// 7. Build, load, run...
//-----------------------------------------------------------------------------



//-------------------------------------------------------
// BIOS header files
//-------------------------------------------------------
#include <xdc/std.h>  						//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 				//mandatory - if you call APIs like BIOS_start()
#include <xdc/runtime/Log.h>				//needed for any Log_info() call
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles
#include <xdc/runtime/Timestamp.h> 			//when using Timestamp APIs (TSCL/H), 32bit, 64bit


//-------------------------------------------------------
// MCSDK-PDK-CSLR Include Files
//
// Note: to use these header files, please add the
// following path to your Include Search Path:
// "C:\TI\pdk_OMAPL138_1_01_00_02\packages"
//
// Or similar - based on your actual install
//-------------------------------------------------------
#include <stdio.h>
#include <c6x.h>
#include <ti/csl/soc_C6748.h>
#include <ti/csl/cslr_gpio.h>
#include <ti/csl/cslr_syscfg0_C6748.h>
#include <ti/csl/cslr_psc_C6748.h>
#include <ti/csl/cslr_tmr.h>


//-------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------
void hardware_init(void);
void ledToggle (void);
void gpioPowerOn(void);
void delay(Uint32 count);


//-------------------------------------------------------
// Globals
//-------------------------------------------------------
volatile int16_t i16ToggleCount = 0;			// keep track of # LED toggles

// sys config registers overlay
CSL_SyscfgRegsOvly   sysRegs  = (CSL_SyscfgRegsOvly)(CSL_SYSCFG_0_REGS);
// Psc register overlay
CSL_PscRegsOvly      psc1Regs = (CSL_PscRegsOvly)(CSL_PSC_1_REGS);
// Gpio register overlay
CSL_GpioRegsOvly     gpioRegs = (CSL_GpioRegsOvly)(CSL_GPIO_0_REGS);
// Timer register overlay
CSL_TmrRegsOvly tmr0Regs = (CSL_TmrRegsOvly)CSL_TMR_0_REGS;



//---------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------
void main(void)
{

   hardware_init();							// init hardware via Xware

   BIOS_start();							// start BIOS Scheduler (never returns)

}



//-----------------------------------------------------------------------------
// hardware_init()
//-----------------------------------------------------------------------------
void hardware_init(void)						//called by main
{

// PINMUX SETUP ---------------------------------------------

// Key to be written to enable the pin mux registers for write
    sysRegs->KICK0R = 0x83e70b13;
    sysRegs->KICK1R = 0x95A4F1E0;

// Enable the pinmux for the GPIO bank 0 pin 9 (tied to LED on LCDK)
    sysRegs->PINMUX0 = ((CSL_SYSCFG_PINMUX0_PINMUX0_27_24_GPIO0_9)
                        << (CSL_SYSCFG_PINMUX0_PINMUX0_27_24_SHIFT));

// mux between Timer 0/1 and GPIO[8:11], enable Timer0/1
    sysRegs->PINMUX4 = CSL_FMKT(SYSCFG_PINMUX4_PINMUX4_3_0,TM64P0_OUT12);


// GPIO SETUP ---------------------------------------------

// First, enable the GPIO Module in the PSC
    gpioPowerOn();

// Configure GPIO0_9 (GPIO0_9_PIN) as an output
    CSL_FINS(gpioRegs->BANK[0].DIR,GPIO_DIR_DIR9,0);


// TIMER_1 3:4 SETUP --------------------------------------

// Select 32 bit unchained mode, take Timer0 out of reset
// Set prescalar value for Timer0 3:4 to 0x1
// Note: TTIM12RS must be set to NO_RESET also for code
// to work properly even though Timer0 3:4 is being used

    tmr0Regs->TGCR = CSL_FMKT(TMR_TGCR_TIMMODE,32BIT_UNCHAIN)
		                 | CSL_FMKT(TMR_TGCR_TIM12RS,NO_RESET)
                         | CSL_FMKT(TMR_TGCR_TIM34RS,NO_RESET)
                         | CSL_FMK(TMR_TGCR_PSC34,0x1);

// Set timer0 PRD3:4 to ~ 1/2 second
    CSL_FINS(tmr0Regs->PRD34,TMR_PRD34_PRD34,0x7A0000);

// Enable/Start TIMER0 3:4 - continuous mode
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE34,EN_CONT);

}


//-----------------------------------------------------------------------------
// ledToggle() ISR (called by BIOS Hwi, see app.cfg)
//-----------------------------------------------------------------------------
void ledToggle(void)							//called by main
{
	static Uint32 LED_state = 0;				// used to toggle LED state

	if (LED_state == 1)							// if LED_state is "1" - ON, turn LED ON via GPIO
	{
	    CSL_FINS(gpioRegs->BANK[0].OUT_DATA,GPIO_OUT_DATA_OUT9,1);
 	}
	else										// LED_state is "0" - OFF, turn LED OFF via GPIO
	{
		CSL_FINS(gpioRegs->BANK[0].OUT_DATA,GPIO_OUT_DATA_OUT9,0);
	}

	LED_state ^= 1;								// toggle LED state

	delay(8000000);								// create a delay of ~1/2sec

	i16ToggleCount += 1;						// keep track of #toggles

	Log_info1("LED TOGGLED [%u] times", i16ToggleCount);

}


void delay(Uint32 count)
{
    volatile Uint32 tempCount = 0;

    for (tempCount = 0; tempCount < count; tempCount++)
    {
        /* dummy loop to wait for some time  */
    }
}



void gpioPowerOn(void)
{
    volatile Uint32 pscTimeoutCount = 10240u;
    Uint32 temp = 0;

// power on GPIO module in PSC & enable GPIO module
    psc1Regs->MDCTL[CSL_PSC_GPIO] = ((psc1Regs->MDCTL[CSL_PSC_GPIO]
                                        & 0xFFFFFFE0)
                                     | CSL_PSC_MDSTAT_STATE_ENABLE);

// Kick start the Enable command
    temp = psc1Regs->PTCMD;
    temp = ((temp & CSL_PSC_PTCMD_GO0_MASK)
            | (CSL_PSC_PTCMD_GO0_SET << CSL_PSC_PTCMD_GO0_SHIFT));

    psc1Regs->PTCMD |= temp;

// Delay enough time for power state transition to occur (status not checked, this is the defn of HOPE)
    while (((psc1Regs->PTSTAT & (CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION)) != 0)
        && (pscTimeoutCount>0))
    {
        pscTimeoutCount--;
    }

}

