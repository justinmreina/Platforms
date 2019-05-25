
//Libraries
#include <stdint.h>

//INCLUDES
#include <msp430.h>
#include "hal_types.h"
#include "hal_defs.h"
#include "trx_rf_spi.h"
#include "hal_digio2.h"
#include "lcd_dogm128_6.h"
#include "hal_timer_32k.h"

#include "bsp.h"
#include "bsp_key.h"

#include "sniff_mode.h"
#include "cc112x_spi.h"
#include "cc120x_spi.h"

#include "cc112x_sniff_mode_api.h"
#include "cc120x_sniff_mode_api.h"

#include "jmr_temp.h"
#include "jmr_cc1200.h"

#include "bsp/clocks.h"
#include "bsp/gpio.h"

//DEFINES
#define DCO_MULT_16MHZ          488
#define DCORSEL_16MHZ           DCORSEL_5
#define VCORE_16MHZ             PMMCOREV_1
#define NEW_COMM_ARRAY_SIZE     (128)                                       /* note: 128 is max                                     */
#define PKT_INIT_VAL            (45)
#define JMR_PKTLEN              NEW_COMM_ARRAY_SIZE

//Functions
extern void (*sniffModeRegConfig)(uint16_t);
extern void (*manualCalibration)(void);
extern uint8 (*freqConfig)(uint16 index);
extern uint8 (*masterStartApp)(void);
extern uint8 (*slaveStartApp)(void);
extern uint8 cc120xFsCfgs[5];
extern uint8 freqSettings40Mhz[5][3];
extern uint8 buttonPressed;
extern uint8 cc120x_gpioConfigMaster[];

//Global Vars
extern const registerSetting_t simpleLinkTestSniffCC120x[];


//Vars
radioChipType_t sniffRadioChipType;
uint32  pkt;                                                                /* id number in the sent message                        */
uint8_t newComArray[NEW_COMM_ARRAY_SIZE];                                   /* data to tx                                           */


/************************************************************************************************************************************/
/** @fcn        void jmr_cc1200_bspInit(void)
 *  @brief      initialize board on boot
 *  @details    x
 *
 *  @src     bspInit()
 *
 *  @section    Opens
 *      port clk cfg code too
 *      check leds work
 */
/************************************************************************************************************************************/
void jmr_cc1200_bspInit(void) {

    uint16_t ui16IntState;

    // Stop watchdog timer (prevent timeout reset)
    WDTCTL = WDTPW + WDTHOLD;

    // Disable global interrupts
    ui16IntState = __get_interrupt_state();
    __disable_interrupt();

    clocks_init();

    gpio_init();

    cc1200_spi_init();

    // Return to previous interrupt state
    __set_interrupt_state(ui16IntState);

    return;
}


/************************************************************************************************************************************/
/** @fcn        void cc1200_spi_init(void)
 *  @brief      initialize the CC1200's SPI interface
 *  @details    x
 *
 * @src     direct copy trx_rf_spi.c:trxRfSpiInterfaceInit(0x10)
 *
 *  @param      [in]    (uint8) clockDivider - SMCLK/clockDivider gives SCLK frequency (0x10) <- <temp> 0x10
 *
 */
/************************************************************************************************************************************/
void cc1200_spi_init(void) {

    const uint8_t clockDivider = 0x10;                                      /* original value used in demo source                   */

    //******************************************************************************************************************************//
    //                                                  INIT UCB0 (SPI)                                                             //
    //******************************************************************************************************************************//
    // RF SPI0 CS as GPIO output high                                       /* (from bspInit() call) - config CS's GPIO             */
    P3SEL &= ~BIT0;
    P3OUT |=  BIT0;
    P3DIR |=  BIT0;

    /* Keep peripheral in reset state*/
    UCB0CTL1 |= UCSWRST;

    /* Configuration
     * -  8-bit
     * -  Master Mode
     * -  3-pin
     * -  synchronous mode
     * -  MSB first
     * -  Clock phase select = captured on first edge
     * -  Inactive state is low
     * -  SMCLK as clock source
     * -  Spi clk is adjusted corresponding to systemClock as the highest rate
     *    supported by the supported radios: this could be optimized and done
     *    after chip detect.
     */
    UCB0CTL0  =  0x00+UCMST + UCSYNC + UCMODE_0 + UCMSB + UCCKPH;
    UCB0CTL1 |=  UCSSEL_2;
    UCB0BR1   =  0x00;

    UCB0BR0 = clockDivider;

    /* Configure port and pins
     * - MISO/MOSI/SCLK GPIO controlled by peripheral
     * - CS_n GPIO controlled manually, set to 1
     */
    TRXEM_PORT_SEL |= TRXEM_SPI_MOSI_PIN + TRXEM_SPI_MISO_PIN + TRXEM_SPI_SCLK_PIN;
    TRXEM_PORT_SEL &= ~TRXEM_SPI_SC_N_PIN;
    TRXEM_PORT_OUT |= TRXEM_SPI_SC_N_PIN + TRXEM_SPI_MISO_PIN;/* Pullup on MISO */


    TRXEM_PORT_DIR |= TRXEM_SPI_SC_N_PIN;
    /* In case not automatically set */
    TRXEM_PORT_DIR |= TRXEM_SPI_MOSI_PIN + TRXEM_SPI_SCLK_PIN;
    TRXEM_PORT_DIR &= ~TRXEM_SPI_MISO_PIN;

    /* Release for operation */
    UCB0CTL1 &= ~UCSWRST;

    //******************************************************************************************************************************//
    //                                                  INIT VARS                                                                   //
    //******************************************************************************************************************************//
    //Init Packet Count
    pkt = PKT_INIT_VAL;

    return;
}



/************************************************************************************************************************************/
/** @fcn        uint8_t jmr_cc1200_sniffInitApp(void)
 *  @brief      x
 *  @details    x
 *
 *  @src     direct copy per test/apps/sniff_mode/sniff_mode.c:sniffInitApp()
 */
/************************************************************************************************************************************/
uint8_t jmr_cc1200_sniffInitApp(void) {

      sniffModeRegConfig = &cc120x_sniffModeRegConfig;
      freqConfig = &cc120x_FreqConfig;
      masterStartApp = &cc120x_masterStartApp;
      slaveStartApp = &cc120x_slaveStartApp;

      //Configure radio registers
      sniffModeRegConfig(sniffRadioChipType.deviceName);
      uint8 writeByte;
      uint16 i;

      //Reset radio
      trxSpiCmdStrobe(CC120X_SRES);

      //Write registers to radio
      for(i=0; i<50; i++) {
        writeByte =  simpleLinkTestSniffCC120x[i].data;
        cc120xSpiWriteReg( simpleLinkTestSniffCC120x[i].addr, &writeByte, 1);
      }

      // Put radio in powerdown to save power
      trxSpiCmdStrobe(CC120X_SPWD);

      //Return success to the menu
      return SNIFF_RETURN_SUCCESS;
}


/************************************************************************************************************************************/
/** @fcn        uint8_t jmr_cc1200_freqConfig(uint16_t index)
 *  @brief      x
 *  @details    x
 *
 *  @src     direct copy per test/apps/sniff_mode/sniff_mode.c:cc120x_FreqConfig()
 *
 *  @section    FS_CFGS ref - Band select setting for LO divider
 *      //Band select setting for LO divider
 *      uint8 cc120xFsCfgs[5] =
 *      {
 *        0x0A, // 169 MHz
 *        0x04, // 434 MHz
 *        0x02, // 868 MHz              //B1 ONLY, 820.0 - 960.0 MHz band (LO divider = 4), OOL Detector Disabled
 *        0x02, // 915 MHz
 *        0x02, // 955 MHz
 *      };
 *
 *   @section VCO Frequency Selection                                   [Sec 9.12]
 *      The VCO frequency is given by the 24 bit (unsigned) frequency word FREQ located in the FREQ2, FREQ1, and FREQ0 registers
 *
 *      uint8 freqSettings40Mhz[5][3] =
 *      {
 *        {0x54, 0xC1, 0x89},  // 169.5125 MHz
 *        {0x56, 0xCC, 0xCC},  // 433 MHz
 *        {0x56, 0xCC, 0xCC},  // 868 MHz                                   // set VCO to 868 MHz                                   //
 *        {0x5B, 0x80, 0x00},  // 915 MHz
 *        {0x5F, 0x80, 0x00}   // 955 MHz
 *      };

 *
 */
/************************************************************************************************************************************/
uint8_t jmr_cc1200_freqConfig(uint16_t index) {

    uint8_t read_vals[3] = {0xFF, 0xFF, 0xFF};

    //FS_CFG - Frequency Synthesizer Configuration                      [Sec 12, p.91]
    //cc120xFsCfgs - Band select setting for LO divider
    uint8_t fs_cfgs_val = cc120xFsCfgs[2];                                  /* select LO divider=4, 820-960 MHz                     */

    //FREQ2-0 - Frequency Configuration                                 [Sec 12, p.100]
    uint8_t *freq_vals  = freqSettings40Mhz[2];

    //Read original value
    cc120xSpiReadReg(CC120X_FS_CFG, &read_vals[0], 1);

    //Write Value
    cc120xSpiWriteReg(CC120X_FS_CFG, &fs_cfgs_val, 1);                      /* FS_CFG                                               */
    cc120xSpiWriteReg(CC120X_FREQ2, freq_vals, 3);                          /* FREQ_2, FREQ_1. FREQ_0 - burst                       */

    //Validate Value Written
    cc120xSpiReadReg(CC120X_FS_CFG, &read_vals[1], 1);
    cc120xSpiReadReg(CC120X_MODCFG_DEV_E, &read_vals[2], 1);                /* also read mod format, to confirm                     */

    // Put radio in powerdown to save power
    trxSpiCmdStrobe(CC120X_SPWD);

    return SNIFF_RETURN_SUCCESS;

}


/************************************************************************************************************************************/
/** @fcn        void jmr_cc1200_masterStartApp(void)
 *  @brief      x
 *  @details    x
 *
 *  @src     direct copy per test/apps/sniff_mode/cc120x_sniff_mode_api .cc120x_masterStartApp()
 */
/************************************************************************************************************************************/
uint8_t jmr_cc1200_masterStartApp(void) {
    static uint8 marcState;
    // Set first packet number
    pkt = 1;

    // Set up GPIO pins. For debug
    cc112xSpiWriteReg(CC120X_IOCFG3,&cc120x_gpioConfigMaster[0],4);

    // Calibrate radio
    trxSpiCmdStrobe(CC120X_SCAL);

    // Wait for calibration to be done (radio back in IDLE state)
    do {
      cc120xSpiReadReg(CC120X_MARCSTATE, &marcState, 1);
    } while (marcState != 0x41);

    while(1) {

        if(buttonPressed == BSP_KEY_LEFT) {
          // Left button pressed. Abort function
          // Put radio in powerdown to save power
          trxSpiCmdStrobe(CC120X_SPWD);

          return SNIFF_RETURN_FAILURE;
        }
        else {//                                            if (buttonPressed == BSP_KEY_RIGHT) {

          // Update packet counter
          pkt++;

          jmr_cc1200_tx_demo();
        }
    }
}


/************************************************************************************************************************************/
/** @fcn        void jmr_cc1200_tx_demo(void)
 *  @brief      perform the tx packet from the TrxEB demo
 *  @details    x
 *
 *  @src     direct copy per test/apps/sniff_mode/cc120x_sniff_mode_api .c:cc120x_masterStartApp() critical section
 *
 */
/************************************************************************************************************************************/
void jmr_cc1200_tx_demo(void) {

    uint32_t i;
    uint32_t loop_ct = 0;


    //******************************************************************************************************************************//
    //                                          STEP 1 - Fill Array                                                                 //
    //******************************************************************************************************************************//
    for(i=0; i<NEW_COMM_ARRAY_SIZE; i++) {
        newComArray[i] = 0x55;                                              // (i>(NEW_COMM_ARRAY_SIZE/2)) ? 0xEE:0;
    }


    //******************************************************************************************************************************//
    //                                          STEP 2 - Transmit                                                                   //
    // P4.6 - Yellow                                                                                                                //
    // P4.7 - Red                                                                                                                   //
    //  @section    Opens                                                                                                           //
    //      - Code freezes without delays at end of loop                                                                            //
    //                                                                                                                              //
    //  @section    BIT7 Measurements       BIT5,     BIT7                                                                          //
    //      NEW_COMM_ARRAY_SIZE: 128,       t=100ms,  101ms                                                                         //
    //  @section    SNOP Description                                                                                                //
    //      0x50 -> b6b4 -> [6:4] = 101 -> State:SETTLING (likely TXRX_SWITCH)                                                      //
    //      0x06 -> xxxx -> [6:4] = 000 -> State:IDLE                                                                               //
    //                                                                                                                              //
    //******************************************************************************************************************************//
    for(;;) {
        // Strobe IDLE and fill TX FIFO
        trxSpiCmdStrobe(CC120X_SIDLE);

        // wait for radio to enter IDLE state
        while((trxSpiCmdStrobe(CC112X_SNOP)& 0xF0) != 0x00);

        P4OUT |= BIT7;                                                      /* BIT7 pulse: 101ms                                    */

        cc112xSpiWriteTxFifo(newComArray, NEW_COMM_ARRAY_SIZE);             /* Send the message                                     */

        P4OUT |= BIT5;                                                      /* BIT5 pulse: 100ms                                    */

        // Send packet
        trxSpiCmdStrobe(CC120X_STX);

        // Wait for radio to finish sending packet
        while((trxSpiCmdStrobe(CC120X_SNOP)& 0xF0) != 0x00);                /* Starts as 0x50, ends on 0x06                         */

        P4OUT &= ~(BIT5|BIT7);

        // Put radio in powerdown to save power
        trxSpiCmdStrobe(CC120X_SPWD);

        loop_ct++;

        P4OUT |= BIT6;                                                      /* BIT6 pulse: 7.48ms                                   */

        for(i = 0; i <10000; i++) {
            asm(" NOP");
            asm(" NOP");
            asm(" NOP");
            asm(" NOP");
        }

        P4OUT &= ~BIT6;
    }
}


