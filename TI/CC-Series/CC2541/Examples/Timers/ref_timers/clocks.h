/************************************************************************************************************************************/
/** @file       clocks.h
 *  @brief      x
 *  @details	x
 *
 *  @target     Texas Instruments CC2541F128
 *  @board      all
 *
 *  @author     Justin Reina, Firmware Engineer
 *  @created	6/22/17
 *  @last rev	6/22/17
 *
 *  @section 	BLE Stack Vocabulary
 *      XOSC - Internal 32 MHz Xtal
 *      HOSC - Internal 16 MHz RC
 *      ARC - Advanced Remote Control
 *      ST - Sensor Tag
 *
 *  @section    Clock Reference
 *              Register CLKCONCMD (0xC6)
 *              See Section 4.4 of User's Guide - 'Oscillators and Clocks'              (swru191f - p.66)
 *                  Section 4.4.4 of User's Guide - 'Oscillator and Clock Registers'    (p.68)
 *
 *              The device has one internal system clock, or main clock. The source for the system clock can be either the 16-MHz RC 
 *              oscillator or the 32-MHz crystal oscillator. Clock control is performed using the CLKCONCMD SFR register.
 *                      XOSC - Int. Xtal & 32 MHz
 *                      HOSC - Int. RC @ 16 MHz? (RCOSC?)
 *
 *  @section    Oscillator Content from Example Code
 *              //ioCC2541.h
 *              	SFR(CLKCONCMD, 0xC6) // Clock Control Command 
 *              
 *              //hal_board_cfg.h  // Setting Clocks 
 *              	HAL_BOARD_INIT()
 *              	  START_HSOSC_XOSC();
 *              	  SET_OSC_TO_HSOSC();				                        // HOSC is Int. 16 MHz RCOSC
 *              	  SET_32KHZ_OSC();
 *              	  SET_OSC_TO_XOSC();				                        // XOSC is Int. 32 MHz Xtal
 *              	  STOP_HSOSC();
 *
 *              //hal_board_cfg.h  // Setting Clocks 
 *              
 *              	// switch to the 16MHz HSOSC and wait until it is stable
 *              	#define SET_OSC_TO_HSOSC() {
 *              	  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_16MHZ;
 *              	  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_16MHZ );
 *              	}
 *              
 *              	// switch to the 32MHz XOSC and wait until it is stable
 *              	#define SET_OSC_TO_XOSC() {
 *              	  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_32MHZ;
 *              	  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_32MHZ );
 *              	}
 *              
 *              	// set 32kHz OSC and wait until it is stable
 *              	#define SET_32KHZ_OSC() {
 *              	  CLKCONCMD = (CLKCONCMD & ~0x80) | OSC_32KHZ;
 *              	  while ( (CLKCONSTA & 0x80) != OSC_32KHZ );
 *              	}
 *              
 *              	#define START_HSOSC_XOSC() {
 *              	  SLEEPCMD &= ~OSC_PD;                                                  // start 16MHz RCOSC & 32MHz XOSC 
 *              	  while (!(SLEEPSTA & XOSC_STB));                                       // wait for stable 32MHz XOSC     
 *              	}
 *              
 *              	#define STOP_HSOSC() {
 *              	  SLEEPCMD |= OSC_PD;                                                   // stop 16MHz RCOSC 				
 *              	}
 *              	...
 *              	  CLKCONCMD = 0x80;  // Select 32-kHz RC OSC & 32-MHz clk src. 
 *                // No dummy wait here - safe to assume 32-kHz calibration finishes before WatchDog is used. 
 *
 *              //hal_mcu.h 	// CLKCONCMD bit definitions 
 *              #define OSC              BV(6) 		                                        // #define BV(n)      (1 << (n)) 
 *              #define TICKSPD(x)       (x << 3)
 *              #define CLKSPD(x)        (x << 0)
 *              #define CLKCONCMD_32MHZ  (0)
 *              #define CLKCONCMD_16MHZ  (CLKSPD(1) | TICKSPD(1) | OSC)
 *
 *  @section	Legal Disclaimer
 *      All contents of this source file and/or any other related source files are the explicit property of Justin Reina. Do not 
 *      distribute. Do not copy.
 */
/************************************************************************************************************************************/
#ifndef _CLOCKS_H_
#define _CLOCKS_H_




#endif  /* _CLOCKS_H_ */

