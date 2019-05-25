//*****************************************************************************
//! @file    chip_detect.c 
//  
//! @brief  Implementation file for radio chip type detection 
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

#include "chip_detect.h"
#include "freq_xosc_detect.h"
#include "trx_rf_spi.h"
#include "hal_timer_32k.h"
#include "bsp.h"
/******************************************************************************
 * CONSTANTS
 */

/* Minimum definitions for reading chip ID and chip VERSION */
#define CC1101_WRITE_BURST              0x40
#define CC1101_IOCFG2                   0x00      // IOCFG2 - GDO2 output pin configuration
#define CC1101_SNOP                     0x3D      // SNOP   - No operation. Returns status byte
#define CC1101_SRES                     0x30      // SRES   - Reset chip


#define CC112X_WRITE_BURST              0x40
#define CC112X_EXT_MEM_ACCESS           0x2F
#define CC112X_IOCFG3                   0x00
#define CC112X_IOCFG2                   0x01
#define CC112X_ECG_CFG                  0x04     // Register addres inside extended register space
#define CC112X_SNOP                     0x3D     // SNOP - No operation. Returns status byte.
#define CC112X_SRES                     0x30     // SRES - Reset chip.

/******************************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 trxDetectCC1101Crystal(void);
static uint8 trxDetectCC112xCrystal(void);

static void cc1101InitTimerA(void);
static void cc112xInitTimerA(void);
static void disableTimerA(void);

void timerAIntConnect(ISR_FUNC_PTR isr);
static void cc1101TimerISR(void);
static void cc112xTimerISR(void);
static void timeOutISR(void);

/******************************************************************************
* LOCAL VARIABLES
*/
static ISR_FUNC_PTR fptr;


static float floatingEstimate;
static uint8  xoscFreqEstimate;
static uint8  timerSemaphore;
static uint8  timeoutSemaphore;
static int16  capturePeriod;
static uint16 captureTable[2];

/******************************************************************************   
 * @fn          trxDetectRfCrystalFrequency()                                       
 *                                                                                
 * @brief       This function estimates the frequency of the crystal on the 
 *              evaluation module (EM). The function currently supports the
 *              following EMs:
 *              CC112x EMs (stand alone and combos)
 *              CC1101 EMs (stand alone and combos)
 *              CC110L (Value Line)
 *              CC113L (Value Line)
 *              CC115L (Value Line) 
 *              CC2500 
 *           
 * input parameters
 *      
 * @param       pRadioChipType  - holds data about a detected radio chip
 *        
 * output parameters  
 *
 * @return      uint8  - xoscFreqEstimate: xosc freq in MHz
 *                       xoscFreqEstimate = 0: xosc freq not detected
 */         
uint8 trxDetectRfCrystalFrequency(radioChipType_t *pRadioChipType)
{
   // Extract radio type info
   uint8 id  = pRadioChipType->id;
   uint8 ver = pRadioChipType->ver;
   
   // If CC1101 or ValueLine radios are detected
   if(id == 0x00 && ver != 0x00)
   {
     xoscFreqEstimate = trxDetectCC1101Crystal();
   }
   // CC2500 radio detected
   else if(id == 0x80)
   {
     // Use same method as for CC1101.
     xoscFreqEstimate = trxDetectCC1101Crystal();
   }
   // CC112x or CC120x radio detected
   else if( id >= 0x10 && id <= 0x5A)
   {
     xoscFreqEstimate = trxDetectCC112xCrystal();
   }
   // No supported radio detected, return none
   else
   {
     xoscFreqEstimate = XOSC_FREQ_NONE;
   }

   // Save estimated crystal frequency to pRadioChipType
   pRadioChipType->xoscFreq = xoscFreqEstimate;
   
   return xoscFreqEstimate;
}
/******************************************************************************   
 * @fn          trxDetectCC1101Crystal()                                    
 *                                                                                
 * @brief       This function estimates the crystal frequency if a NextGen 
 *              radio is detected.
 *              SPI init must be applied before this function   
 *              can be called.
 *
 * @param       none
 *          
 * @return      none
 */
static uint8 trxDetectCC1101Crystal()
{
  
 // Write CLOCK_XOSC/192 to GDO2
  uint8 writeByte = 0x3F;
  trx8BitRegAccess(CC1101_WRITE_BURST, CC1101_IOCFG2, &writeByte, 1);
  
  //Wait for crystal to be stable (CHIP_RDYn)
  while((trxSpiCmdStrobe(CC1101_SNOP)& 0xF0) != 0x00);
  
  //Get current system clock frequency
  uint32_t systemClockBak = bspSysClockSpeedGet();
  
  //set system clock frequency up to 25 MHz for accurate sampling
  bspSysClockSpeedSet(BSP_SYS_CLK_25MHZ);
  
  // initialize timerA to capture rising edges on CLOCK XOSC
  cc1101InitTimerA();
  
  // Setting up time out in case we hang wating for capture interrupt
  halTimer32kIntConnect(&timeOutISR);
  halTimer32kSetIntFrequency(1);  // 1 sec timeout
  halTimer32kIntEnable();
  
  // wait for interrupt on timer capture or timeout
  while((!timerSemaphore) && (!timeoutSemaphore));
  
  // stop timeuot timer
  halTimer32kIntDisable();
    
  // stop timer
  disableTimerA();
 
  if(timerSemaphore)
  {
     // assuming 50% duty cycle. Period time = time between rising and
    // falling edge x 2
    capturePeriod = (captureTable[1]  - captureTable[0])*2;
  
    //check for negative number and set absolute value
    capturePeriod= (capturePeriod<0)?(0-capturePeriod):capturePeriod;
  
    // Claculate XOSC frequency in  MHz:
    // system clock frequency / capturePeriod
    // times clock xosc divider (192)
    floatingEstimate = (((25.0*192.0)/capturePeriod));
  
    //Round up/down estimated frequency and truncate to int
    xoscFreqEstimate = (uint8) (floatingEstimate + 0.5);
  }
  else
  {
    xoscFreqEstimate =  XOSC_FREQ_NONE;
  }
  
  //set system clock frequency back to standard
  bspSysClockSpeedSet(systemClockBak);
    
  //reset radio
  trxSpiCmdStrobe(CC1101_SRES);
  
  return xoscFreqEstimate;
}
/******************************************************************************   
 * @fn          trxDetectCC112xCrystal()                                     
 *                                                                                
 * @brief       This function estimates the crystal frequency if a CC112x 
 *              is detected.
 *              SPI init must be applied before this function   
 *              can be called.
 *
 * @param       none
 *          
 * @return      none
 */
static uint8 trxDetectCC112xCrystal()
{  
  // Write EXT CLOCK to IOCFG3 and IOCFG2
  uint8 writeBytes1[2] = {0x31, 0x31};
  trx8BitRegAccess(CC112X_WRITE_BURST, CC112X_IOCFG3, writeBytes1, 2);
  // set external clock divider to 32
  writeBytes1[0] = 0x00;
  trx16BitRegAccess(CC112X_WRITE_BURST,CC112X_EXT_MEM_ACCESS,CC112X_ECG_CFG,writeBytes1,1);
  
  //wait for crystal to be stable (CHIP_RDYn)
  while((trxSpiCmdStrobe(CC112X_SNOP)& 0xF0) != 0x00);
  
  //get system clock frequency
  uint32_t systemClockBak = bspSysClockSpeedGet();
  
  //set system clock frequency up to 25 MHz for accurate sampling
  bspSysClockSpeedSet(BSP_SYS_CLK_25MHZ);
  
  // initialize timerA to capture rising and falling edges on EXT CLOCK
  cc112xInitTimerA();
  
  // Setting up time out in case we hang wating for capture interrupt
  halTimer32kIntConnect(&timeOutISR);
  halTimer32kSetIntFrequency(1); // 1 sec timeout
  halTimer32kIntEnable();
  
  // wait for interrupt on timer capture or timeout
  while((!timerSemaphore) && (!timeoutSemaphore));
  
  // stop timeuot timer
  halTimer32kIntDisable();
  
  // stop timer
  disableTimerA();
 
  if(timerSemaphore)
  {
    // assuming 50% duty cycle. Period time = time between rising and
    // falling edge x 2
    capturePeriod = (captureTable[1]  - captureTable[0])*2;
  
    //check for negative number and set absolute value
    capturePeriod= (capturePeriod<0)?(0-capturePeriod):capturePeriod;
  
    // Claculate XOSC frequency in  MHz: 
    // system clock frequency / capturePeriod
    // times external clock divider (32)
    // times digital clock divider (2)
    floatingEstimate = (((25.0*32.0*2.0)/capturePeriod));
  
    //Round up/down estimated frequency and truncate to int
    xoscFreqEstimate = (uint8) (floatingEstimate + 0.5);
  }
  else
  {
    xoscFreqEstimate =  XOSC_FREQ_NONE;
  }
  
  //set system clock frequency back to standard
  bspSysClockSpeedSet(systemClockBak);
  
  //reset radio
  trxSpiCmdStrobe(CC112X_SRES);
  
  return xoscFreqEstimate;
}
/******************************************************************************   
 * @fn        cc112xInitTimerA()                                                 
 *                                                                                
 * @brief     Initializes timerA for capture on CCR1 and CCR2 and connects 
 *            ISR function to the timer interrupt vector.
 *      
 * @param    none     
 *
 * @return   none
 */ 
static void cc112xInitTimerA(void)
{
  // Connect ISR function to interrupt vector
  timerAIntConnect(&cc112xTimerISR);
  
  // Set P1.2 and P1.3 on MCU as peripheral input   
  P1DIR &= ~(BIT2 | BIT3);
  P1SEL |=(BIT2 |BIT3); // Select high for peripheral function
    
  //Setup CCR1 to capture on rising edge
  TA0CCTL1 = CM_1+CCIS_0+SCS+CAP+CCIE; // CCR1 used for timer capture
                                        // capture on rising edge,
                                       // interrupt enable
  
  //Setup CCR2 to capture on rising edge
  TA0CCTL2 = CM_2+CCIS_0+SCS+CAP+CCIE; // CCR2 used for timer capture
                                        // capture on falling edge,
                                        // Enable timer interrupt
  // Set clock source on timer a
  TA0CTL = TASSEL_2 + MC_2+TAIE;         // SCLK, continuous mode, clear TAR
                                         // Enable interrrupt
}
/******************************************************************************   
 * @fn        nextgenRadioInitTimerA()                                                 
 *                                                                                
 * @brief     Initializes timerA for capture on CCR2 and connects 
 *            ISR function to the timer interrupt vector.
 *      
 * @param    none     
 *
 * @return   none
 */ 
static void cc1101InitTimerA(void)
{
  // Connect ISR function to interrupt vector
  timerAIntConnect(&cc1101TimerISR);
  
  // Set P1.3 on MCU as peripheral input   
  P1DIR &= ~BIT3;
  P1SEL |=  BIT3; // Select high for peripheral function
  
  //Setup CCR2 to capture on rising edge
  TA0CCTL2 = CM_3+CCIS_0+SCS+CAP+CCIE; // CCR2 used for timer capture
                                        // capture on rising and falling edge,
                                        // Enable timer interrupt

  // Set clock source on timer a
  TA0CTL = TASSEL_2 + MC_2+TAIE;         // SCLK, continuous mode, clear TAR
                                         // Enable interrrupt

}
/******************************************************************************   
 * @fn         cc1101TimerISR                                        
 *                                                                                
 * @brief      timer interrupt service routine for next gen radios 
 *      
 * @param         
 *
 * @return      
 */
static void cc1101TimerISR(void)
{
  static uint8 index;

  switch(__even_in_range(TA0IV,14))
  {   
    case  TA0IV_TA0CCR2:// CCR2
     // clear flag
     TA0CCTL2 &= ~(CCIFG);
     TA0CTL &= ~TAIFG;
     // Save time values to table
     captureTable[index] = TA0CCR2;
     // increment table index
     index++;
     // We have two readings, set semaphore and disable timer interrupts
     if(index == 2)
     {
       index = 0;
       // Set semaphore
       timerSemaphore =1;
       //disable interrupt on CCR2
       TA0CCTL2 &= ~CCIE;
       // disable timer interrupt
       TA0CTL &= ~TAIE;
     }
    default: 
      break; 
  }   
}
/******************************************************************************   
 * @fn         cc112xTimerISR                                        
 *                                                                                
 * @brief      timer interrupt service routine for cc112x radios 
 *      
 * @param         
 *
 * @return      
 */ 
static void cc112xTimerISR(void)
{
  switch(__even_in_range(TA0IV,14))
  {                    
    case  TA0IV_TA0CCR1:                          // CCR1
      // rising edge detected
      //disable interupt and remove flag on CCR1
      TA0CCTL1 &= ~(CCIFG | CCIE);     
      break;    
    case  TA0IV_TA0CCR2:                        // CCR2
      // clear flag
      TA0CCTL2 &= ~CCIFG;
      TA0CTL &= ~TAIFG;
      // falling edge detected
      //check that we have capture value on both capture registers
      //set semaphore and save values
      if((TA0CCR1 !=0) && (TA0CCR2 != 0))
      {
       // Save capture times on rising and falling edges to array
       captureTable[0] = TA0CCR1;
       captureTable[1] = TA0CCR2;
       // Set semaphore
       timerSemaphore =1;
       //disable interrupt on CCR2
       TA0CCTL2 &= ~CCIE;
       // disable timer interrupt
       TA0CTL &= ~TAIE;
      }
      break;  
    default: 
      break; 
  }  
}
/******************************************************************************   
 * @fn         disableTimerA                                            
 *                                                                                
 * @brief      Stops timer
 *      
 * @param      none  
 *
 * @return     none
 */ 
static void disableTimerA(void)
{
  //stop timerA, remove flags and disable interrupt
	//TA0CTL &= ~(MC_2 | TAIFG | TAIE);
  TA0CTL = 0x00;
  asm(" nop");
  // Clear TAxR
  TA0R =0x00;
  asm(" nop");
  // Clear TA0CCR1
  TA0CCR1 = 0x00;
  asm(" nop");
  // Clear TA0CCR2
  TA0CCR2 = 0x00;
  asm(" nop");
  // Clear TA0CCTL1
  TA0CCTL1 = 0x00;
  asm(" nop");
  // Clear TA0CCTL2
  TA0CCTL2 = 0x00;
  asm(" nop");
}
/******************************************************************************   
 * @fn         timeOutISR                                           
 *                                                                                
 * @brief      Sets timeout semaphore
 *      
 * @param      none  
 *
 * @return     none
 */ 
static void timeOutISR(void)
{
  // Set timeout semaphore
  timeoutSemaphore = 1;
}
/*******************************************************************************
 * @fn         timerAIntConnect
 *              
 * @brief       Connect function to timer interrupt
 *   
 * input parameters
 *
 * @param       isr  - pointer to function
 *
 * output parameters
 *
 * @return      none
 */
void timerAIntConnect(ISR_FUNC_PTR isr)
{
  uint16_t ui16IntState;
  ui16IntState = __get_interrupt_state();
  __disable_interrupt();
  
  fptr = isr;
  
  //HAL_INT_UNLOCK(key);
  /*HAL replace*/
  __set_interrupt_state(ui16IntState);
}  
/*******************************************************************************
* @brief  Timer_A1 Interrupt Vector (TAIV) handler
* 
* @param  none
* 
* @return none
*******************************************************************************/
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
  if (fptr != NULL)
  {
      (*fptr)();
  }
  __low_power_mode_off_on_exit();
}
