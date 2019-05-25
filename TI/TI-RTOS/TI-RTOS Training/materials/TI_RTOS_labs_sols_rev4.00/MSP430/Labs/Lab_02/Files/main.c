//---------------------------------------------------------------------------
// Project: Blink MSP430 using MSP430ware (CCS Lab) (STARTER)
//
// Author: Eric Wilbur
//
// Date: May 2015
//
//---------------------------------------------------------------------------


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




// *****************************************************************************************
// UNUSED_HWI_ISR()
//
// The default linker command file created by CCS links all interrupt vectors to their
// specified address location. This gives you a warning for vectors that are not associated
// with an ISR function. The following function (and pragma's) handles all interrupt
// vectors.
//
// Just make sure you comment out the vector pragmas handled by your own code. For example,
// you will receive a "program will not fit into" error if you do not comment out the WDT
// vector below. This occurs since the linker tries to fit both of the vector addresses
// into the same memory locations ... and they won't fit.
// *****************************************************************************************
#pragma vector=ADC12_VECTOR
#pragma vector=COMP_B_VECTOR
#pragma vector=DMA_VECTOR
#pragma vector=PORT1_VECTOR
#pragma vector=PORT2_VECTOR
#pragma vector=RTC_VECTOR
#pragma vector=SYSNMI_VECTOR
#pragma vector=TIMER0_A0_VECTOR
#pragma vector=TIMER0_A1_VECTOR
#pragma vector=TIMER0_B0_VECTOR
#pragma vector=TIMER0_B1_VECTOR
#pragma vector=TIMER1_A0_VECTOR
#pragma vector=TIMER1_A1_VECTOR
#pragma vector=TIMER2_A0_VECTOR
#pragma vector=TIMER2_A1_VECTOR
#pragma vector=UNMI_VECTOR
#pragma vector=USB_UBM_VECTOR
#pragma vector=USCI_A0_VECTOR
#pragma vector=USCI_A1_VECTOR
#pragma vector=USCI_B0_VECTOR
#pragma vector=USCI_B1_VECTOR
#pragma vector=WDT_VECTOR
__interrupt void UNUSED_HWI_ISR (void)
{
	__no_operation();
}
