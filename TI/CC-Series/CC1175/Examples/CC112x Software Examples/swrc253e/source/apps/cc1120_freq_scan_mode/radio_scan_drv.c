/******************************************************************************
*  Filename: radio_scan_drv.c
*
*  Description: Implementation file for basic frequency hopping transmit
*               and frequency scanning receiver solution
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
#include "stdlib.h"
#include "cc112x_freq_scan_mode_reg_config.h"
#include "io_pin_int.h"
#include "bsp_led.h"

#include "radio_drv.h"

/*******************************************************************************
* DEFINES
*/
/* semaphores used by ISR to communicate with main loop statemachine */
#define ISR_IDLE        0x00
#define ISR_PIN_GDO0    0x01
#define ISR_PIN_GDO2    0x02

/* control to exit isr */
#define NOT_DONE        0x11
#define ISR_DONE        0x12

/* Bit fields in the chip status byte */
#define RADIO_INVERT_SIGNAL     0x40
#define RADIO_PQT_REACHED       0x02
#define RADIO_SYNC_FOUND        0x80
#define ISR_PKT_VALID           0x06
#define ISR_PQT_VALID             12
#define ISR_RSSI_VALID            13
#define RSSI_VALID_BM           0x01
#define RX_FIFO_ERROR           0x11
#define LOW_POWER_MODE          LPM1_bits

/******************************************************************************
 * GLOBALS -  Storage for the rf calibration values used to make hopping faster 
 */
uint8 freq0[RF_NUM_CHANNELS];                     /* table for frequency reg */
uint8 freq1[RF_NUM_CHANNELS];                     /* table for frequency reg */
uint8 freq2[RF_NUM_CHANNELS];                     /* table for frequency reg */
uint8 freqTable[RF_NUM_CHANNELS];                 /* frequency hopping table */
uint8 avg_rssi_table[RF_NUM_CHANNELS];            /* table of RSSI averages */
uint8 packetSemaphore;                            /* packet flag container */
uint8 rfState;                                    /* statemachine container */
uint16 pqt_timeout, rssi_timeout, pkt_timeout;    /* packet timeout counters */
uint32 cal_timeout;                               /* calibration timeout */
uint8 rfChanNm;                                   /* current channel number */

#ifdef RADIO_FULL_CAL                             /* calibration tables */
  uint8 fs_chp_cal[RF_NUM_CHANNELS];
  uint8 fs_vco2_cal[RF_NUM_CHANNELS];
  uint8 fs_vco4_cal[RF_NUM_CHANNELS];
#else
  uint8 fs_chp_cal[2];
  uint8 fs_vco2_cal[2];
  uint8 fs_vco4_cal[2];
#endif

/* Statemachine container */
static uint8 rfState;

/******************************************************************************
 * @fn          radio_calc_freq
 *
 * @brief       Calculate the required frequency registers and send the using 
 *              serial connection to the RF tranceiver.
 *              
 * input parameters
 *
 * @param       freq         -  frequency word provided in [kHz] resolution
*  @param       freq0        -  pointer to frequency register 0 table
*  @param       freq1        -  pointer to frequency register 1 table
*  @param       freq2        -  pointer to frequency register 2 table
*  @param       num_char     -  number of channels to use
 *
 * output parameters
 *
 * @return      void
 */
void generate_hopping_table(uint8 *hop_table, uint8 length_table, 
                            int16 rand_seed) {
  
  int channel_found, ii, jj;
  unsigned int chan_int;
  
  /* generate a PN hopping table start by placing the seed */
  srand(rand_seed);
  
  /* ii is the table index */
  for (ii=0; ii<length_table; ii++)
  {
    /* we have to look for overlaps */
    channel_found = 1;
    while(channel_found > 0)
    {
      /* find a new random number */
      chan_int =  length_table * (rand()>>7);
      hop_table[ii] =  chan_int >> 8;
      channel_found = 0;
      
      /* check to see if already exists */
      for (jj=0; jj<ii; jj++)
      {
        if(hop_table[ii] == hop_table[jj])
        {
          channel_found++;
        }
      }
    }
  }
  return;
}

/******************************************************************************
 * @fn          calculate_freq_error_est
 *
 * @brief       Convert the frequency error estimate from two complement coded 
 *              data to frequency error in Hertz
 *              
 * input parameters
 *
 * @param       freq_reg_error -  two complement formatted data from tranceiver
 *
 * output parameters
 *
 * @return      freq_error     - 32 bit signed integer value representing
                                 frequency error in Hertz
 */
int32 calculate_freq_error_est(uint16 freq_reg_error) {
  
  uint32 freq_error_est;
  int32 freq_error_est_int;
  uint8 sign;
  
  /* the incoming data is 16 bit two complement format, separate "sign" */
  if (freq_reg_error > 32768) {
    freq_error_est = -(freq_reg_error - 65535);
    sign = 1;
  } else {
    freq_error_est = freq_reg_error;
    sign = 0;
  }
  
  /* convert the data to hertz format in two steps to avoid integer overuns */
  freq_error_est = (freq_error_est * (RF_XTAL_FREQ/RF_LO_DIVIDER)) >> 8;
  freq_error_est = (freq_error_est * 1000) >> 10;
  
  /* re-assign the "sign" */
  if(sign == 1) {
    freq_error_est_int = -freq_error_est;
  } else {
    freq_error_est_int = freq_error_est;
  }
  
  return freq_error_est_int;
}

/******************************************************************************
* @fn          radio_calc_freq
*
* @brief       Calculate the required frequency registers and send the using 
*              serial connection to the RF tranceiver.
*              
* input parameters
*
* @param       freq         -  frequency word provided in [kHz] resolution
*  @param       freq0        -  pointer to frequency register 0 table
*  @param       freq1        -  pointer to frequency register 1 table
*  @param       freq2        -  pointer to frequency register 2 table
*  @param       num_char     -  number of channels to use
*
* output parameters
*
* @return      void
*/
void radio_calc_freq(uint32 freq, uint8 *freq0, uint8 *freq1, uint8 *freq2, uint8 num_chan) {
  
  uint8 cc;
  uint32 freq_regs_uint32;
  uint8 freq_regs_uint8;
  uint32 freq_offset;
  float f_vco;
  
  /* loop for all channels requested */  
  for(cc=0; cc<num_chan; cc++){
    
    freq_offset = RF_STEPSIZE_FREQ * cc;
    
    /* Radio frequency -> VCO frequency */
    f_vco = (freq + freq_offset) * RF_LO_DIVIDER;
    
    /* Divide by oscillator frequency */
    f_vco = f_vco * (1/(float)RF_XTAL_FREQ);     
    
    /* Multiply by 2^16 */
    f_vco = f_vco * 65536;                       
    
    /* Convert value into uint32 from float */
    freq_regs_uint32 = (uint32) f_vco;
    
    /* return the frequency word */
    freq0[cc] = ((uint8*)&freq_regs_uint32)[0];
    freq1[cc] = ((uint8*)&freq_regs_uint32)[1];
    freq2[cc] = ((uint8*)&freq_regs_uint32)[2];
  }
  
  return;
}

/******************************************************************************
* @fn          radio_set_freq
*
* @brief       Calculate the required frequency registers and send the using 
*              serial connection to the RF tranceiver.
*              
* input parameters
*
* @param       freq   -  frequency word provided in [kHz] resolution
*
* output parameters
*
* @return      void
*/
void radio_set_freq(uint32 freq) {
  
  uint8 freq_regs[3];
  
  /* calculate the frequency word required */
  radio_calc_freq(freq, &freq_regs[2], &freq_regs[1], &freq_regs[0], 1);
  
  /* write the frequency word to the transciever */
  cc112xSpiWriteReg(CC112X_FREQ2, freq_regs, 3);
  
  return;
}

/******************************************************************************
* @fn          pre_callibrate_rf
*
* @brief       Pre calibrate all the RF frequencies to enable faster hopping
*             
* input parameters
*
* @param       *fs_chp_cal   -  pointer to the table storing fs_chp cal data
* @param       *fs_vco2_cal  -  pointer to the table storing fs_vco2 cal data
* @param       *fs_vco4_cal  -  pointer to the table storing fs_vco4 cal data
* @param        num_char     -  number of channels to use
*
* output parameters
*
* @return      void
*/
void pre_callibrate_rf(uint8 *fs_chp_cal, uint8 *fs_vco2_cal, uint8 *fs_vco4_cal,
                       uint8 num_chan) {
  uint8 cc;
  uint8 marcState;
  
#ifdef RADIO_FULL_CAL
  for(cc=0; cc<num_chan; cc++){

    /* set the transciever operating frequency */
    radio_set_freq(RF_START_FREQ + (RF_STEPSIZE_FREQ * cc));

    /* Calibrate radio */
    trxSpiCmdStrobe(CC112X_SCAL);
  
    /* Wait for calibration to be done (radio back in IDLE state) */
    do {
      cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
    } while (marcState != 0x41);
    
    cc112xSpiReadReg(CC112X_FS_CHP, &fs_chp_cal[cc], 1);
    cc112xSpiReadReg(CC112X_FS_VCO2, &fs_vco2_cal[cc], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &fs_vco4_cal[cc], 1);
    
    /* force the radio into idle */
    trxSpiCmdStrobe(CC112X_SIDLE);
  }
#else
    /* set the transciever operating frequency at the center frequency */
    radio_set_freq(RF_START_FREQ + (RF_STEPSIZE_FREQ * (num_chan/2)));

    /* Calibrate radio */
    trxSpiCmdStrobe(CC112X_SCAL);
  
    /* Wait for calibration to be done (radio back in IDLE state) */
    do {
      cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
    } while (marcState != 0x41);
    
    cc112xSpiReadReg(CC112X_FS_CHP, &fs_chp_cal[0], 1);
    cc112xSpiReadReg(CC112X_FS_VCO2, &fs_vco2_cal[0], 1);
    cc112xSpiReadReg(CC112X_FS_VCO4, &fs_vco4_cal[0], 1);
    
    /* force the radio into idle */
    trxSpiCmdStrobe(CC112X_SIDLE);
#endif  
  return;
}

/******************************************************************************
* @fn          use_pre_callibrate_rf
*
* @brief       use pre calibrated frequencies which enables faster hopping
*             
* input parameters
*
* @param       fs_chp_cal   -  frequency calibration value "fs_chp"
* @param       fs_vco2_cal  -  frequency calibration value "fs_vco2"
* @param       fs_vco4_cal  -  frequency calibration value "fs_vco4"
* @param        num_char     -  number of channels to use
*
* output parameters
*
* @return      void
*/
void use_pre_callibrate_rf(uint8 *fs_chp_cal, uint8 *fs_vco2_cal,
                           uint8 *fs_vco4_cal, uint8 index) {
    
#ifdef RADIO_FULL_CAL
    cc112xSpiWriteReg(CC112X_FS_CHP, &fs_chp_cal[index], 1);
    cc112xSpiWriteReg(CC112X_FS_VCO2, &fs_vco2_cal[index], 1);
    cc112xSpiWriteReg(CC112X_FS_VCO4, &fs_vco4_cal[index], 1);
#endif

    return;
}

/******************************************************************************
* @fn          radio_set_freq_pre_cal
*
* @brief       Set the frequency of the transciever to a new frequency and use
*              pre calibrated frequencies which enables faster hopping
*             
* input parameters
*
* @param       freq0        -  pointer to frequency register 0 table
* @param       freq1        -  pointer to frequency register 1 table
* @param       freq2        -  pointer to frequency register 2 table
* @param       num_char     -  number of channels to use
*
* output parameters
*
* @return      void
*/
void radio_set_freq_pre_cal(uint8 *freq0, uint8 *freq1, uint8 *freq2, uint8 rfChanNm) {
  
  uint8 freq_regs_uint8[3];
  
  /* write the frequency word to the transciever one byte at time */
  freq_regs_uint8[2] = freq0[rfChanNm];
  freq_regs_uint8[1] = freq1[rfChanNm];
  freq_regs_uint8[0] = freq2[rfChanNm];
  
  cc112xSpiWriteReg(CC112X_FREQ2, freq_regs_uint8, 3);
  
  return;
}

/*******************************************************************************
* @fn          registerConfig
*
* @brief       Write register settings as given by SmartRF Studio found in
*              cc1200_rx_sniff_mode_reg_config.h
*
* @param       none
*
* @return      none
*/
static void registerConfig(void) {
  
  uint8 writeByte;
  
  /* Reset radio */
  trxSpiCmdStrobe(CC112X_SRES);
  
  /* Write registers to radio */
  for(uint16 i = 0; i < (sizeof(preferredSettings)/sizeof(registerSetting_t)); i++) {  
    writeByte = preferredSettings[i].data; 
    cc112xSpiWriteReg(preferredSettings[i].addr, &writeByte, 1);
  }
  return;
}

/******************************************************************************
 * @fn          calculate_rssi
 *
 * @brief       Calculate RSSI from unsigned char value from Radio
 *              
 * input parameters
 *
 * @param       rssi   -  two complement formatted data from tranceiver
 *
 * output parameters
 *
 * @return      rssi   - 16 bit signed integer value representing rssi in [dB]
 */
int16 calculate_rssi(uint8 cc_rssi) {
  
  int16 rssi;
  
  rssi = cc_rssi;
  if (rssi >= 128) {
    rssi = rssi - 256;
  }
  rssi = rssi - RSSI_OFFSET;
  return rssi; 
}

/******************************************************************************
* @fn          init_rssi_table
*
* @brief       Initialized the rssi table to the threshold value
*              
* input parameters
*
* @param       *avg_rssi_table - pointer to moving average table of rssi values
* @param       rssi            - rssi value to add to table
* @param       num_chan        -  number of channels to use
*
* output parameters
*
* @return      void
*/
void init_rssi_table(uint8 *avg_rssi_table, int16 rssi , uint8 num_chan) {
  
  uint8 cc;
  
  /* loop for all channels requested */  
  for(cc=0; cc<num_chan; cc++){
    
    /* store updated value to the table */
    avg_rssi_table[cc] = -rssi;
    
  }
  
  return;
}

/******************************************************************************
* @fn          add_rssi_to_avg
*
* @brief       Calculate the moving average of rssi values  
*              
* input parameters
*
* @param       *avg_rssi_table - pointer to moving average table of rssi values
* @param       rssi            - rssi value to add to table
* @param       index           - use this index
*
* output parameters
*
* @return      void
*/
void add_rssi_to_avg(uint8 *avg_rssi_table, int16 rssi , uint8 index) {
  
  uint8  uint8_rssi;
  uint16 uint16_rssi;
  
  /* convert rssi value to possitive value */
  uint8_rssi = -rssi;                  
  
  /* get the current value from the table */
  uint16_rssi = avg_rssi_table[index];
  
  /* multiply by 8 as we are doing 8 times averaging */
  uint16_rssi = uint16_rssi<<3;
  
  /* subtract the current average value and add the new rssi value */
  uint16_rssi = uint16_rssi - avg_rssi_table[index] + uint8_rssi;
  
  /* divide by 8 as we are doing 8 times averaging */
  uint16_rssi = uint16_rssi>>3;
  
  /* store updated value to the table */
  avg_rssi_table[index] = (uint8)uint16_rssi;
  
  return;
}

/******************************************************************************
* @fn         range_extender_rxon
*
* @brief      Initialize the radio hardware
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
void range_extender_rxon(void) {
  
  /* configure txon */
  PAEN_PORT &= ~PAEN_PIN;
  LNAEN_PORT |= LNAEN_PIN;
  
  return;
}


/******************************************************************************
* @fn         range_extender_txon
*
* @brief      Initialize the radio hardware
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
void range_extender_txon(void) {
  
  /* configure txon */
  PAEN_PORT |= PAEN_PIN;
  LNAEN_PORT &= ~LNAEN_PIN;
  
  return;
}


/******************************************************************************
* @fn         range_extender_idle
*
* @brief      Initialize the radio hardware
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
void range_extender_idle(void) {
  
  /* configure idle */
  PAEN_PORT &= ~PAEN_PIN;
  LNAEN_PORT &= ~LNAEN_PIN;
  
  return;
}


/******************************************************************************
* @fn         range_extender_init
*
* @brief      Initialize the radio hardware
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
void range_extender_init(void) {
    
  uint8 regs_uint8;
  
  /* lower the maximum output of the transciever to 10dBm */
  regs_uint8 = 0x74;
  cc112xSpiWriteReg(CC112X_PA_CFG2, &regs_uint8, 1);

  /* initialize the IO */
  PAEN_DIR |= PAEN_PIN;
  LNAEN_DIR |= LNAEN_PIN;
  
  /* initialize idle */
  range_extender_idle();

  return;
}

/*******************************************************************************
* @fn          radioRxISR
*
* @brief       ISR for packet handling in TX. Sets packet semaphore 
*              and clears interrupt flag
*
* @param       none
*
* @return      none
*/
void radioRxISR_GDO0(void) {
  
  /* Set packet semaphore */
  packetSemaphore |= ISR_PIN_GDO0;  
  
  /* Clear ISR flag */
  ioPinIntClear(GPIO0_PORT, GPIO0);
  
  /* run the radio isr */
  radioRxISR_timer();
}

/*******************************************************************************
* @fn          radioRxISR_GDO2
*
* @brief       ISR for packet handling in TX. Sets packet semaphore 
*              and clears interrupt flag
*
* @param       none
*
* @return      none
*/
void radioRxISR_GDO2(void) {
  
  /* Set packet semaphore */
  packetSemaphore |= ISR_PIN_GDO2;  
  
  /* Clear ISR flag */
  ioPinIntClear(GPIO2_PORT, GPIO2);

  /* run the radio isr */
  radioRxISR_timer();

}


/******************************************************************************
* @fn         radioRxISR_timer
*
* @brief      This ISR implements the main functions of the RX scanning 
*             reciever. It will modify a 
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
* global parameters   
*
* @param      rfState - determines the state of the statemachine
*             
*                               
*/
void radioRxISR_timer(void) {
  
  uint8 marcState, done;
  uint8 regState, regState1;
  int16  rssi_int;
  uint8 rxBytes;
  
  done = NOT_DONE;
  
  while(done == NOT_DONE) {
    
    switch (rfState) {
      
      /* start the scanning process by setting up the radio to output RSSI  */
      /* updates on the GDO pin                                             */
    case RF_START:      
      
      /* start the reciever at channel 0 */
      rfChanNm = 0;
      
      /* Reset calibration timeout counter */
      cal_timeout = 0;
      pqt_timeout = 0;
      rssi_timeout = 0;
      pkt_timeout = 0;
      
      /* idle the radio */
      trxSpiCmdStrobe(CC112X_SIDLE);
      
      /* change the frequency using precalculated values*/
      radio_set_freq_pre_cal(freq0, freq1, freq2, rfChanNm);
      
      /* use precalibrated RF values */
      use_pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, rfChanNm);      
      
      /* start the RX chain */
      trxSpiCmdStrobe(CC112X_SRX);
      
      /* move to next section of the scanning algorithm */
      rfState = RF_CARRIER_TO_PKT; 
      
      /* Reset packet semaphore */
      packetSemaphore = ISR_IDLE;
      
      bspLedClear(BSP_LED_ALL);
      done = ISR_DONE;
      break;
      
      /* no valid RSSI value at current channel, move to next channel       */
    case RF_NEXT_CHAN:
      
      bspLedSet(BSP_LED_1);
      
      /* reset the packet error timerout function */
      pqt_timeout = 0;
      rssi_timeout = 0;
      pkt_timeout = 0;
      
      /* idle the radio and move to the next RF frequency */
      if(rfChanNm++ == RF_NUM_CHANNELS) rfChanNm = 0;
      
      /* change the frequency using precalculated values*/
      radio_set_freq_pre_cal(freq0, freq1, freq2, rfChanNm);
      
      /* use precalibrated RF values */
      use_pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, rfChanNm);      
      
      /* start the RX chain */
      trxSpiCmdStrobe(CC112X_SRX);
      
      rfState = RF_CARRIER_TO_PKT;
      
      /* Reset packet semaphore */
      packetSemaphore = ISR_IDLE;
      done = ISR_DONE;
      
      bspLedClear(BSP_LED_1);
      
      break;
      
      /* a valid RSSI value has been found, check value and select next steps */
    case RF_CARRIER_TO_PKT:
      
      bspLedSet(BSP_LED_2);
      
      /* Wait for RSSI valid indicator from transciever */
      if(packetSemaphore & ISR_PIN_GDO2) {
        
        /* check to see if we have an RSSI value above the threshold  */
        cc112xSpiReadReg(CC112X_RSSI1, &regState, 1);
        rssi_int = calculate_rssi(regState);
        
        /* Reset packet semaphore */
        packetSemaphore &= ~ISR_PIN_GDO2;
        
        /* check to see if we have found a signal above moving average */
        if(rssi_int > (-avg_rssi_table[rfChanNm] + RF_RSSI_ABOVE_AVG)) {
          
          /* there is a signal lets move to PKT reception */
          rfState = RF_PKT;
          pqt_timeout = 0;
          
        } else {
          
          /* there is no signal lets move to next channel */
          rfState = RF_NEXT_CHAN;   
          
          /* store the current rssi value into an array of moving averages */
          add_rssi_to_avg(avg_rssi_table, rssi_int , rfChanNm);
          
        }
        
      }  else {
        /* use the timer ticks to check for timeouts */
        /* check to see if RSSI has been found in time */
        if(rssi_timeout++ > RF_RSSI_TIMEOUT ) {
          
          rssi_timeout = 0;
          
          /* there is no signal lets move to next channel */
          rfState = RF_NEXT_CHAN;   
        }
        done = ISR_DONE;
      }
      
      /* exit the isr here and wait for the next cycle */
      bspLedClear(BSP_LED_2);
      break;
      
      /* a good rssi has been found. try to get the rest of the packet */
    case RF_PKT:
      
      bspLedToggle(BSP_LED_4);
      
      /* Wait for packet received interrupt  */
      if(packetSemaphore & ISR_PIN_GDO0) {
        
        /* Read number of bytes in RX FIFO */
        cc112xSpiReadReg(CC112X_NUM_RXBYTES, &rxBytes, 1);
        
        /* restart the search engine by restarting the statemachine  */
        rfState = RF_START;
        
        /* Check that we have bytes in FIFO */
        if(rxBytes != 0) {
          
          /* Read marcstate to check for RX FIFO error */
          cc112xSpiReadReg(CC112X_MARCSTATE, &marcState, 1);
          
          /* Mask out marcstate bits and check if we have a RX FIFO error */
          if((marcState & 0x1F) == RX_FIFO_ERROR) {
            
            /* Flush RX FIFO */
            trxSpiCmdStrobe(CC112X_SFRX);
            
          } else {
            
            /* Indicate that we found a good packet */
            bspLedSet(BSP_LED_3);
            
            /* restart the search engine by restarting the statemachine  */
            rfState = RF_SUCCESS;
            
          }
          
          /* Reset packet semaphore */
          packetSemaphore = ISR_IDLE;
          
          /* if no packet interrupt is found engage both PQT & PKT timeouts */
        }
      } else {
        
        /* use the timer ticks to check for timeouts */
        /* check to see if PQT has been found in time */
        if(pqt_timeout++ > RF_PQT_TIMEOUT ) {
          
          pqt_timeout = 0;
          
          /* check the PQT status register to validate positive PQT */
          cc112xSpiReadReg(CC112X_MODEM_STATUS1, &regState, 1);
          if((regState & (RADIO_SYNC_FOUND + RADIO_PQT_REACHED)) == 0) {
            
            /* there is no signal, move to next channel */
            rfState = RF_NEXT_CHAN; 
            
          }
        }
        /* use the timer ticks to check for timeouts */
        /* check to see if missed the packet for some reason */
        if (pkt_timeout++ > RF_PACKET_TIMEOUT) {
          
          /* Reset packet semaphore */
          packetSemaphore = ISR_IDLE;
          
          /* restart the search engine by restarting the statemachine  */
          rfState = RF_NEXT_CHAN;
        }
      }
      done = ISR_DONE;
      break;
      
      /* a good rssi has been found. try to get the rest of the packet */
    case RF_SUCCESS:
      done = ISR_DONE;
      bspLedToggle(BSP_LED_3);
      break;
    default:
      
      /* Reset packet semaphore */
      packetSemaphore = ISR_IDLE;
      
      /* restart the search engine by restarting the statemachine  */
      done = ISR_DONE;
      rfState = RF_START;      
      break;
    }
  }
  
  /* Recalibrate the RF tranciever with an appropriate cadence */
  
  if(cal_timeout++ > RF_CAL_TIMEOUT * (LF_OSC / RF_RX_SCAN_TIMER)) {
    pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, RF_NUM_CHANNELS);
    cal_timeout = 0;
  }
  
  return;
}

/******************************************************************************
* @fn         radio_init
*
* @brief      Initialize the radio hardware
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_init(void) {
  
  uint8 regState;
  
  /* Instantiate transceiver RF SPI interface to SCLK ~ 4 MHz */
  /* Input parameter is clockDivider */
  /* SCLK frequency = SMCLK/clockDivider */
  trxRfSpiInterfaceInit(2);
  
  /* download the configuration to the radio */
  registerConfig();
  
  /* pre_calibration the rf transciever for each of the 50 channels */
  pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, RF_NUM_CHANNELS);
  
  /* pre calculate the frequency words for each of the 50 channels */
  radio_calc_freq(RF_START_FREQ , freq0, freq1, freq2, RF_NUM_CHANNELS);
 
  /* initialize the rssi table for each of the 50 channels */
  init_rssi_table(avg_rssi_table, RF_SCAN_RSSI_LIMIT , RF_NUM_CHANNELS);
     
  /* Generate a pseudo random hopping sequence in TX mode only */  
  generate_hopping_table(freqTable, RF_NUM_CHANNELS, RANDOM_SEED); 
  
  /* reset the channel counter */
  rfChanNm = 0;
  
  /* enable range extender */
#ifdef ENABLE_RANGE_EXTENDER
  range_extender_init();
#endif
      
  return 0;
}

/******************************************************************************
* @fn         radio_set_freq_offset
*
* @brief      modify the frequency offset variable globally
*             
*              
* input parameters
*
* @param      int16 freq_offset
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_set_freq_offset(int16 freq_offset) {
 
  float f_vco;
  int16 freq_off_regs;
  uint8 freq_regs[2];
    
  /* Radio frequency -> VCO frequency */
  f_vco = freq_offset * RF_LO_DIVIDER;
  
  /* Divide by oscillator frequency */
  f_vco = f_vco * (1/(float)RF_XTAL_FREQ);     
  
  /* Multiply by 2^18 */
  f_vco = f_vco * 262144;                       
  
  /* Convert value into uint32 from float */
  freq_off_regs = (int16) f_vco;
  
  /* return the frequency word */
  freq_regs[0] = ((uint8*)&freq_off_regs)[1];
  freq_regs[1] = ((uint8*)&freq_off_regs)[0];
   
  /* write the frequency offset word to the transciever */
  cc112xSpiWriteReg(CC112X_FREQOFF1, freq_regs, 2);
  
  return 0;
}


/******************************************************************************
* @fn         radio_idle
*
* @brief      Idle the radio, used when leaving low power modes (below)
*             
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_idle(void){
  
  /* Strobe IDLE to for idle tranciever */
  trxSpiCmdStrobe(CC112X_SIDLE);
  
  /* Idle range extender */
  range_extender_idle();
  
  rfChanNm = 0;
  
  return 0;
}

/******************************************************************************
* @fn         radio_send
*
* @brief      Prepare & transmit a packet in same call
*             
*              
* input parameters
*
* @param       unsigned char *payload
*              unsigned short payload_len
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_send(unsigned char *payload, unsigned short payload_len) {
   uint32 tx_freq;
 
    /* Write packet to TX FIFO */
    cc112xSpiWriteTxFifo(payload, payload_len);
    
    /* Lookup the next channel number using the index and change the frequency */
    tx_freq = RF_START_FREQ + (RF_STEPSIZE_FREQ * freqTable[rfChanNm]);
    radio_set_freq(tx_freq);

    /* use precalibrated RF values */
    use_pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, rfChanNm);
    
    /* loop back at the end of the array */
    if(rfChanNm++ == RF_NUM_CHANNELS) rfChanNm = 0;

    /* Range extender in TX mode */
    range_extender_txon();

    /* Strobe TX to send packet */
    trxSpiCmdStrobe(CC112X_STX);
    
    /* configure state-machine for TX operation */
    rfState = RF_SEND;
    
    return 0;
}

/******************************************************************************
* @fn         radio_receive_on
*
* @brief      Initiate the RX chain and reset state machine variables
*              
* input parameters
*
* @param       void
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_receive_on(void) {
  
    uint8 regState;
  
    /* Set the rssi valid signal to appear on IOCFG2 */
    regState = ISR_RSSI_VALID;
    cc112xSpiWriteReg(CC112X_IOCFG2, &regState, 1);
  
    /* set the required preamble bytes to two */
    regState = 0x10;
    cc112xSpiWriteReg(CC112X_PREAMBLE_CFG1, &regState, 1);

    /* set the required sync bytes to two */
    regState = 0x08;
    cc112xSpiWriteReg(CC112X_SYNC_CFG0, &regState, 1);

    /* Range extender in RX mode */
    range_extender_rxon();

    /* Strobe RX to initiate the recieve chain */
    trxSpiCmdStrobe(CC112X_SRX);
    
    /* Reset RX packet semaphore and statemachine */
    packetSemaphore = ISR_IDLE;
    rfState = RF_START;

    return 0;
}

/******************************************************************************
* @fn         radio_read
*
* @brief      Read a received packet into a buffer
*              
* input parameters
*
* @param       unsigned char *payload
*              unsigned short payload_len
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_read(uint8 *payload, uint8 *payload_len) {
  uint8 ret;
  
  /* Read number of bytes in RX FIFO */
  cc112xSpiReadReg(CC112X_NUM_RXBYTES, payload_len, 1);
  
  /* Read buf_len bytes from RX FIFO */
  cc112xSpiReadRxFifo(payload, *payload_len);  
  
  /* Check CRC ok (CRC_OK: bit7 in second status byte) */
  /* This assumes status bytes are appended in RX_FIFO */
  /* (PKT_CFG1.APPEND_STATUS = 1.)                     */
  /* If CRC is disabled the CRC_OK field will read 1   */
  if(payload[*payload_len - 1] & 0x80) {
    ret = 1;
  } else {
    ret = 0;
  }
  return ret;
}
                             

/******************************************************************************
* @fn         radio_wait_for_idle
*
* @brief      Wait for radio to become idle 
*
* input parameters
*
* @param       unsigned char *payload
*              unsigned short payload_len
*
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_wait_for_idle(void) {

  /* Enter Low Power mode and wait */
  _BIS_SR(LOW_POWER_MODE + GIE);             

  return 0;
}

/******************************************************************************
* @fn         radio_read_info
*
* @brief      Wait for radio to become idle 
*
* input parameters
*
* @param       int32 *freq_error_est
*             
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_read_info(radio_info_t *pkt_info) {
  
  uint8 regState,regState1;

  pkt_info->channel = freqTable[rfChanNm];
  
  /* In TX mode store the following information in the array */
  if(rfState == RF_SEND) {
    pkt_info->rssi_pkt = 15;
    pkt_info->freq_err_est = RF_START_FREQ  + (RF_STEPSIZE_FREQ * freqTable[rfChanNm]);
  }
  
  /* For a good packet overwrite the data with RX data */
  if(rfState == RF_SUCCESS) {
  
    /* Read marcstate to check for frequency error estimate */
    cc112xSpiReadReg( CC112X_FREQOFF_EST0, &regState, 1);
    cc112xSpiReadReg( CC112X_FREQOFF_EST1, &regState1, 1);
        
    /* Calculate the frequency error in Hz */
    pkt_info->freq_err_est = calculate_freq_error_est(((regState1 << 8) + regState)); 
    
    /* Extract the average RSSI values seen in the active channel */
    pkt_info->rssi_noise = avg_rssi_table[rfChanNm];
    
    /* Extract the average RSSI values seen in the active channel */
    cc112xSpiReadReg(CC112X_RSSI1, &regState, 1);
    pkt_info->rssi_pkt = calculate_rssi(regState);
    
  }

  return (rfState == RF_SUCCESS);
}

/******************************************************************************
* @fn         radio_pre_calibrate
*
* @brief      Force a manual recalibration of all frequencies in the list
*
* input parameters
*
* @param       void
*             
* output parameters
*
* @return      void
*
*                               
*/
uint8 radio_pre_calibrate(void) {
  
  pre_callibrate_rf(fs_chp_cal, fs_vco2_cal, fs_vco4_cal, RF_NUM_CHANNELS);
  
  return 0;

}