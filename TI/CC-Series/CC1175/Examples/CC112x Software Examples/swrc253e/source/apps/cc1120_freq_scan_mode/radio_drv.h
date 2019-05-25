/******************************************************************************
*  Filename: radio_drv.h
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
#ifndef RADIO_DRV
#define RADIO_DRV

/* calibrate entire band at the cost of slower scanning performance, to use */
/* this option the band of operation is required to be less than 4MHz total */
/* bandwidth for all channels used by the system as defined below */
// #define RADIO_FULL_CAL

/* default narrow band setting is to use 25KHz, however if ULTRA_NARROW_BAND */
/* is defined then the system will be configured for 12.5KHz RX bandwidth    */
/* the scan rates will decrease and the requirements on the frequency        */
/* accuracy will increase                                                    */
// #define ULTRA_NARROW_BAND

/* this setting will enable the use of range extender for both RX and TX side */
// #define ENABLE_RANGE_EXTENDER

/* system wide configuration defines */
#define RF_START_FREQ           902750  /* start frequency defined in khz */
#define RF_STEPSIZE_FREQ        50      /* frequency stepsize defined in khz */
#define RF_NUM_CHANNELS         50      /* number of channels in system */
#define RANDOM_SEED             250     /* used to generate hop sequence */
#define RF_XTAL_FREQ            32000   /* XTAL frequency, given in 1kHz steps */
#define LF_OSC                  32768   /* defines the Low frequency osc freq. */
#define RF_LO_DIVIDER           4       /* the value of the hardware LO divider */
#define RF_CAL_TIMEOUT          500     /* approximately 500s timeout*/

/* define hardware pins the various tranciever GPIOs are attached to */
#define GPIO3                   BIT2              /* P1.2 on TRXEB */
#define GPIO3_PORT              IO_PIN_PORT_1
#define GPIO2                   BIT3              /* P1.3 on TRXEB */
#define GPIO2_PORT              IO_PIN_PORT_1
#define GPIO0                   BIT7              /* P1.7 on TRXEB */
#define GPIO0_PORT              IO_PIN_PORT_1

/* define hardware pins for the optional CC1190 range extender */
#define PAEN_PIN                BIT4              /* P1.4 on TRXEB */
#define PAEN_PORT               P1OUT
#define PAEN_DIR                P1DIR
#define LNAEN_PIN               BIT4              /* P3.4 on TRXEB */
#define LNAEN_PORT              P3OUT
#define LNAEN_DIR               P3DIR

/* recieve specific system configuration defines */
#define RF_RX_SCAN_TIMER        8        /* set a timer tick to ~250us */
#define RF_PACKET_TIMEOUT       2048     /* approximately 500ms timeout */
#define RF_RSSI_TIMEOUT         40       /* approximately 10ms timeout */
#define RF_PQT_TIMEOUT          120      /* approximately 30ms timeout */
#define RF_SCAN_RSSI_LIMIT     -110      /* initial limit, used by moving avg */
#define RF_RSSI_ABOVE_AVG       8        /* use signal above moving average */
#define RSSI_OFFSET             99       /* constant defined in datasheet */

/* transmit specific system configuration defines*/
#define RF_TX_BURST_TIMER       16384   /* send a packet every 500ms */

/* possible states of state machine */
#define RF_START                2
#define RF_NEXT_CHAN            3
#define RF_CARRIER_TO_PKT       4
#define RF_CARRIER_TO_PQT       5
#define RF_PQT                  6
#define RF_PKT                  7
#define RF_SUCCESS              8
#define RF_SEND                10

/* type definition used to get packet information after each reception */
typedef struct {
	int16 rssi_pkt;
	uint8 rssi_noise;
	uint8 channel;
	uint32 freq_err_est;
        uint32 pktCounter;
} radio_info_t;

/* Initialize the radio hardware */
uint8 radio_init(void);

/* Compensate for frequency error by modifing the frequency offset register */
uint8 radio_set_freq_offset(int16 freq_offset);

/* Idle the radio, used when leaving low power modes (below)*/
uint8 radio_idle(void);

/* Prepare & transmit a packet in same call (slightly worse timing jitter) */
uint8 radio_send(uint8 *payload, uint16 payload_len);

/* Enter recieve mode */
uint8 radio_receive_on(void);

/* Read a received packet into a buffer */
uint8 radio_read(uint8 *payload, uint8 *payload_len);

/* Wait for radio to become idle (with timer based timeout) */
uint8 radio_wait_for_idle(void);

/* Check if the radio driver has just received a packet */
uint8 radio_read_info(radio_info_t *pkt_info);

/* Force a recalibration of all entries in frequency table */
uint8 radio_pre_calibrate(void);

/* Main scanning function, this needs to be called approximately every 200us */
void radioRxISR_timer(void);
void radioRxISR_GDO0(void);
void radioRxISR_GDO2(void);

#endif