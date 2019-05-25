//---------------------------------------------------------------------------
// Project: Blink MSP430 using BIOS (STARTER)
// Author: Eric Wilbur
// Date: May 2015
//
// Follow these steps to create this project in CCSv6.0:
// 1. Project -> New CCS Project
// 2. Select Template:
//    - TI-RTOS for MSP430 -> Driver Examples -> LP -> Example -> Empty
//    - this template DOES contain UIA/RTOS Analyzer support
//    - And it contains MSP430Ware links and include search paths
// 3. Delete the following files:
//    - Board.h, empty.c, EK_TM4C123GXL.c/h, empty_readme.txt
// 4. Add main.c from TI-RTOS Workshop Solution file for this lab
// 5. Edit as needed (to add/subtract) BIOS services, delete given Task
// 6. Build, load, run...
//---------------------------------------------------------------------------


//----------------------------------------
// BIOS header files
//----------------------------------------
#include <xdc/std.h>  						//mandatory - have to include first, for BIOS types
#include <ti/sysbios/BIOS.h> 				//mandatory - if you call APIs like BIOS_start()
#include <xdc/runtime/Log.h>				//needed for any Log_info() call
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles


//-----------------------------------------
// MSP430 Header Files
//-----------------------------------------
#include <driverlib.h> 


//-----------------------------------------
// MSP430 MCLK frequency settings
// Used to set MCLK frequency
// Ratio = MCLK/FLLREF = 8192KHz/32KHz
//-----------------------------------------
#define MCLK_DESIRED_FREQUENCY_IN_KHZ  8000                            // 8MHz
#define MCLK_FLLREF_RATIO              MCLK_DESIRED_FREQUENCY_IN_KHZ / ( UCS_REFOCLK_FREQUENCY / 1024 )    // Ratio = 250

#define GPIO_ALL	GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3| \
					GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7


//-----------------------------------------
// Prototypes
//-----------------------------------------
void hardware_init(void);
void ledToggle(void);
void delay(void);


//-----------------------------------------
// Globals
//-----------------------------------------
volatile int16_t i16ToggleCount = 0;


//---------------------------------------------------------------------------
// main()
//---------------------------------------------------------------------------
void main(void)
{

   hardware_init();							// init hardware via Xware

   while(1)									// forever loop
   {
	   ledToggle();							// toggle LED

	   delay();								// create a delay of ~1/2sec

	   i16ToggleCount += 1;					// keep track of #toggles
   }

}


//-----------------------------------------------------------------------------
// hardware_init()
//-----------------------------------------------------------------------------
void hardware_init(void)					//called by main
{
	// Disable the Watchdog Timer (important, as this is enabled by default)
	WDT_A_hold( WDT_A_BASE );


    // Set MCLK frequency to 8192 KHz
	// First, set DCO FLLREF to 32KHz = REF0
	UCS_initClockSignal(UCS_FLLREF, UCS_REFOCLK_SELECT, UCS_CLOCK_DIVIDER_1);

	// Second, Set Ratio and Desired MCLK Frequency = 8192KHz and initialize DCO
	UCS_initFLLSettle(MCLK_DESIRED_FREQUENCY_IN_KHZ, MCLK_FLLREF_RATIO);

	// Set GPIO ports to low-level outputs
    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P3, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P5, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_P6, GPIO_ALL );
    GPIO_setAsOutputPin( GPIO_PORT_PJ, GPIO_ALL );

    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P3, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P5, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_P6, GPIO_ALL );
    GPIO_setOutputLowOnPin( GPIO_PORT_PJ, GPIO_ALL );


    // Configure I/O's that we're using (redundant, based on the previous settings)
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 );
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 );

}

//---------------------------------------------------------------------------
// ledToggle()
//
// Toggle LED via GPIO pin
// Add "+ GPIO_PIN1 to GPIO_setOutput call to add blue X LED
//---------------------------------------------------------------------------
void ledToggle(void)
{
	GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );

}



// *****************************************************************************************
// delay()
//
// This intrinsic delays exactly the number of clock cycles (MCLK) shown
// Note: this routine burns power - other routines can be used that burn almost no power
// *****************************************************************************************
void delay(void)
{
	__delay_cycles(4096000);			// ~1/2 second delay
}

