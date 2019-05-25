/******************************************************************************
*  Filename: cc112x_freq_scan_mode_rx.c
*
*  Description: This program demonstrates fast frequency scanning that enables
*               narrow band systems to operate under FCC 15.247 (frequency 
*               hopping) at full power.
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include "msp430.h"
#include "lcd_dogm128_6.h"    
#include "stdlib.h"
#include "bsp.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "io_pin_int.h"

#include "hal_timer.h"
#include "radio_drv.h"

/*******************************************************************************
* DEFINES
*/


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8  packetSemaphore;
static uint32 packetCounter = 0;

/*******************************************************************************
* @fn          initMCU
*
* @brief       Initialize MCU and board peripherals
*                
* @param       none
*
* @return      none
*/
static void initMCU(void) {
  
  /* Init clocks and I/O */
  bspInit(BSP_SYS_CLK_8MHZ);
  
  /* Init LEDs */
  bspLedInit(); 
  
  /* Init buttons */
  bspKeyInit(BSP_KEY_MODE_POLL);
  
  /* Initialize SPI interface to LCD (shared with SPI flash) */
  bspIoSpiInit(BSP_FLASH_LCD_SPI, BSP_FLASH_LCD_SPI_SPD);  
  
  /* Init LCD */
  lcdInit();
  
  /* Enable global interrupt */
  _BIS_SR(GIE);
}

/*******************************************************************************
* @fn          updateLcd
*
* @brief       Updates LCD buffer and sends bufer to LCD module
*                
* @param       none
*
* @return      none
*/
void updateLcd(radio_info_t *pkt_info) {
  
  /* Update LDC buffer and send to screen */
  lcdBufferClear(0);
  lcdBufferPrintString(0, "RX Freq Scan Mode", 0, eLcdPage0);
  lcdBufferSetHLine(0, 0, LCD_COLS - 1, 7); 
  lcdBufferPrintString(0, "Rx'd pkts :", 0, eLcdPage2);
  lcdBufferPrintInt(0, packetCounter, 70, eLcdPage2);
  lcdBufferPrintString(0, "Rssi/Noise:", 0, eLcdPage3);
  lcdBufferPrintInt(0, pkt_info->rssi_pkt, 70, eLcdPage3);
  lcdBufferPrintInt(0, -pkt_info->rssi_noise, 100, eLcdPage3);
  lcdBufferPrintString(0, "Channel   :", 0, eLcdPage4);
  lcdBufferPrintInt(0, pkt_info->channel, 70, eLcdPage4);
  lcdBufferPrintString(0, "Freq Error:", 0, eLcdPage5);
  lcdBufferPrintInt(0, pkt_info->freq_err_est, 70, eLcdPage5);
  
#ifdef RADIO_FULL_CAL
    lcdBufferPrintString(0, "RX - Full Calibration" , 0, eLcdPage7);
#else
    lcdBufferPrintString(0, "RX - Min Calibration" , 0, eLcdPage7);
#endif
    
  lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
  lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
  lcdSendBuffer(0);
}


/******************************************************************************
* @fn          runRX
*
* @brief       puts radio in RX and waits for packets. Function assumes
*              that status bytes are appended in the RX_FIFO
*              Update packet counter and display for each packet received.
*                
* @param       none
*
* @return      none
*/
static void runRX(void) {
  
  uint8 rxBytes;
  int32 freq_error_est;
  radio_info_t pkt_info;
  
  /* Initialize packet buffer of size TOT_PKTLEN + 1 */
  uint8 rxBuffer[128+1];
         
  /* Connect ISR function to GPIO0 */
  ioPinIntRegister(IO_PIN_PORT_1, GPIO0, &radioRxISR_GDO0);
  
  /* Interrupt on falling edge */
  ioPinIntTypeSet(IO_PIN_PORT_1, GPIO0, IO_PIN_FALLING_EDGE);
  
  /* Enable interrupt */
  ioPinIntEnable(IO_PIN_PORT_1, GPIO0);
  
  /* Connect ISR function to GPIO2 */
  ioPinIntRegister(IO_PIN_PORT_1, GPIO2, &radioRxISR_GDO2);
  
  /* Interrupt on falling edge */
  ioPinIntTypeSet(IO_PIN_PORT_1, GPIO2, IO_PIN_RISING_EDGE);
  
  /* Enable interrupt */
  ioPinIntEnable(IO_PIN_PORT_1, GPIO2);
  
  /* Enable rf scan timer */
  hal_timer_init(RF_RX_SCAN_TIMER);
  
  /* register the isr with the timer function to enable call backs*/
  timer_isr_register(radioRxISR_timer);
  
  /* turn on the receiver */
  radio_receive_on();

  while(1) { 
    
    /* enter low power mode */
    /* both the timer and end-of-packet can cause an exit from low power mode */
    radio_wait_for_idle();
    
    /* check for a end-of-packet */
    if( radio_read_info(&pkt_info) == 1) {
     
      /* read the packet and check if the CRC was good */
      if(radio_read(rxBuffer, &rxBytes) == 1) {
        
        /* increment counter if the CRC was good */
        packetCounter++;
        
      }
    
      /* Update LCD */
      updateLcd(&pkt_info);
      
      /* restart the receiver */
      radio_receive_on();
      
    }
  }
}

/*******************************************************************************
* @fn          main
*
* @brief       Runs the main routine
*                
* @param       none
*
* @return      none
*/
void main(void){
  
  /* Initialize MCU and peripherals */
  initMCU();
  
  /* Initialize the LED's */
  bspLedInit();
  
  /* initilize the radio */
  radio_init();
  
  /* Enter runRX, never coming back */
  runRX();
}
