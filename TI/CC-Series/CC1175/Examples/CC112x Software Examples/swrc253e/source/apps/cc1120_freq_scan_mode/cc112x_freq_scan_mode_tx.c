/******************************************************************************
*  Filename: cc112x_freq_scan_mode_tx.c
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

/******************************************************************************
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

/* set the packet length used by the transmitter */
#define PKTLEN   20        /* 1 < (PKTLEN) < 126 */

/*******************************************************************************
* @fn          createPacket
*
* @brief       This function is called before a packet is transmitted. It fills
*              the txBuffer with a packet consisting of a length byte, two
*              bytes packet counter and n random bytes
*
*              The packet format is as follows:
*              |--------------------------------------------------------------|
*              |           |           |           |         |       |        |
*              | pktLength | pktCount1 | pktCount0 | rndData |.......| rndData|
*              |           |           |           |         |       |        |
*              |--------------------------------------------------------------|
*               txBuffer[0] txBuffer[1] txBuffer[2]  ......... txBuffer[PKTLEN]
*                
* @param       Pointer to start of txBuffer
*
* @return      none
*/
static void createPacket(uint8 txBuffer[], uint32 packetCounter) {
  
  uint8 i = 0;
  
  /* Length byte */
  txBuffer[i++] = PKTLEN;                      
  
  /* MSB of packetCounter */
  txBuffer[i++] = (uint8) (packetCounter >> 8);   
  
  /* LSB of packetCounter */
  txBuffer[i++] = (uint8) packetCounter;         
  
  /* Fill rest of buffer with random bytes */
  for(; i < (PKTLEN + 1); i++) {
    txBuffer[i] = (uint8)rand();
  }
  return;
}


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
  lcdBufferPrintString(0, "TX Freq Scan Mode", 0, eLcdPage0);
  lcdBufferSetHLine(0, 0, LCD_COLS - 1, 7); 
  lcdBufferPrintString(0, "Tx'd pkts :", 0, eLcdPage2);
  lcdBufferPrintInt(0, pkt_info->pktCounter, 70, eLcdPage2);
  lcdBufferPrintString(0, "TX power  :", 0, eLcdPage3);
  lcdBufferPrintInt(0, pkt_info->rssi_pkt, 70, eLcdPage3);
  lcdBufferPrintString(0, "Channel   :", 0, eLcdPage4);
  lcdBufferPrintInt(0, pkt_info->channel, 70, eLcdPage4);
  lcdBufferPrintString(0, "TX Freq   :", 0, eLcdPage5);
  lcdBufferPrintInt(0, pkt_info->freq_err_est, 70, eLcdPage5);
  
#ifdef RADIO_FULL_CAL
    lcdBufferPrintString(0, "TX - Full Calibration" , 0, eLcdPage7);
#else
    lcdBufferPrintString(0, "TX - Min Calibration" , 0, eLcdPage7);
#endif
  
  lcdBufferSetHLine(0, 0, LCD_COLS - 1, 55);
  lcdBufferInvertPage(0, 0, LCD_COLS, eLcdPage7);
  lcdSendBuffer(0);
}

/*******************************************************************************
* @fn          runTX
*
* @brief       Transmits a packet every time a button is pushed. A packet 
*              counter is incremented for each packet sent and the LCD is
*              updated
*                
* @param       none
*
* @return      none
*/
static void runTX(void) {
  
  static uint8 marcState;
  uint8 regState;
  uint32 tx_freq;
  radio_info_t pkt_info;
  uint32 cal_timeout = 0;
  int16 freq_offset = 0;
  uint8 key;
  
  /* Initialize packet buffer of size TOT_PKTLEN + 1 */
  uint8 txBuffer[PKTLEN + 1];
  
  /* Enable burst timer */
  hal_timer_init(RF_TX_BURST_TIMER);
  
  /* initialize packet counter */
  pkt_info.pktCounter = 0;
  
  /* Infinite loop */
  while(1) {
    
    /* check for up button push */
    key = bspKeyPushed(BSP_KEY_ALL);
    if(key == BSP_KEY_DOWN) {
      freq_offset = freq_offset - 1;
      radio_set_freq_offset(freq_offset);
    }
    if(key == BSP_KEY_UP) {
       freq_offset = freq_offset + 1;
       radio_set_freq_offset(freq_offset);
    }

    /* Wait timer to expire */
    hal_timer_expire();
    
    /* Indicate TX activity using LED */
    bspLedSet(BSP_LED_3);

    /* Recalibrate the RF tranciever with an appropriate cadence */
    if(cal_timeout++ > RF_CAL_TIMEOUT * (int)(LF_OSC / RF_TX_BURST_TIMER)){
      
      radio_pre_calibrate();
      
      cal_timeout = 0;
    }
      
    /* Create a random packet with PKTLEN + 2 byte packet counter + n x bytes */
    createPacket(txBuffer, pkt_info.pktCounter++);
    
    /* Send packet */
    radio_send(txBuffer, PKTLEN+1);
    
    /* get packet inforamtion from driver */
    radio_read_info(&pkt_info);
    pkt_info.freq_err_est +=  freq_offset;
    
    /* Update LCD */
    updateLcd(&pkt_info);
    
    /* Indicate TX activity using LED */
    bspLedClear(BSP_LED_3);
    
  }
}

/*******************************************************************************
* @fn          main
*
* @brief       Runs the main routine. 
*                
* @param       none
*
* @return      none
*/
void main(void) {
  
  /* Initialize MCU and peripherals */
  initMCU();
  
  /* Initialize the LED's */
  bspLedInit();
  
  /* initilize the radio */
  radio_init();
  
  /* Enter runTX, never coming back */
  runTX();
}
