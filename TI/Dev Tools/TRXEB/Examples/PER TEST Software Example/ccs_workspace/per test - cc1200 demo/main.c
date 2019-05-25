
//Includes
#include  <msp430.h>
#include "trx_rf_spi.h"
#include "chip_detect.h"
#include "freq_xosc_detect.h"
#include "per_test.h"
#include "main_graphics.h"
#include "menu_system.h"
#include "menu_driver.h"
#include "sniff_mode.h"
#include "chip_information.h"
#include "easyLink.h"
#include "lcd_dogm128_6.h"

#include "bsp.h"
#include "bsp_key.h"
#include "bsp_led.h"

//Jmr-dev includes
#include "jmr/jmr_temp.h"
#include "jmr/jmr_cc1200.h"

//<NEW>
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

#include "jmr/jmr_temp.h"
#include "jmr/jmr_cc1200.h"

#include "bsp/clocks.h"
#include "bsp/gpio.h"

#include "smartrf_CC1200.h"


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
//</NEW>


//Locals
void radio_configure(void);
void read_Config(uint8_t column);


//Variables
uint8 marcState;
uint8_t reg_read_vals[2][30] = {0x00};

uint8_t target_vals[] = {
                         0x0C,
                         0x26,
                         0x0B,
                         0x06,
                         0x1B,
                         0x00,
                         0x40,                                              /* PKT_CFG0                                             */
                         0x00,
                         0xA0,                                              /* [8]: SR_E | SR_M[19:16]                              */
                         0x00,                                              /* 0xA00000 for SR_E|SR_N : 25.6us                      */
                         0x00,                                              /* need E2E help to progress further                    */
                         0x00,
                         0x02,
                         0x00,
                         0x3F,
                         0x7C,
                         0x7F,
                         0x56,
                         0xCC,
                         0xCC,
                         0xAC,
                        };


/************************************************************************************************************************************/
/** @fcn        void main(void)
 *  @brief      x
 *  @details    x
 */
/************************************************************************************************************************************/
void main(void) {


    //******************************************************************************************************************************//
    // INIT MCU                                                                                                                     //
    //******************************************************************************************************************************//
    jmr_cc1200_bspInit();


    //******************************************************************************************************************************//
    // RESET RADIO                                                                                                                  //
    //******************************************************************************************************************************//
    trxSpiCmdStrobe(CC120X_SRES);                                           /* Reset radio                                          */


    //******************************************************************************************************************************//
    // INIT RADIO CONFIG                                                                                                            //
    //******************************************************************************************************************************//
    radio_configure();                                                     /* configure the radio for use                           */


    //******************************************************************************************************************************//
    // CALIBRATE                                                                                                                    //
    //******************************************************************************************************************************//
    trxSpiCmdStrobe(CC120X_SCAL);

    // Wait for calibration to be done (radio back in IDLE state)
    do {
      cc120xSpiReadReg(CC120X_MARCSTATE, &marcState, 1);
    } while (marcState != 0x41);


    //******************************************************************************************************************************//
    // TRANSMIT                                                                                                                     //
    //******************************************************************************************************************************//
    jmr_cc1200_tx_demo();
}


/************************************************************************************************************************************/
/** @fcn        void radio_configure(void)
 *  @brief      configure the radio for UHF-ASK demo
 *  @details    x
 *
 *  @pre        radio was reset
 *  @post       radio is ready for transmission by Tx Buffer
 *
 *  @section    Operation
 *      x
 *
 *  @section    Opens
 *      Why is FS_SPARE write required for operation here?
 *      [X] Mod Depth is full                               Bug to be resolved over E2E (writes have no effect here)
 *      [X] Symbol Size is correct                          Can't resolve, get E2E's help
 *      Continuous Transmissions                        Can't resolve, get E2E's help. Try swrc253e port first though
 */
/************************************************************************************************************************************/
void radio_configure(void) {

    uint8_t write_val, i;

#ifdef OLD_WAY


    // Reset radio
    trxSpiCmdStrobe(CC120X_SRES);

    // Put radio in powerdown to save power
    trxSpiCmdStrobe(CC120X_SPWD);

    uint8_t *freq_vals  = freqSettings40Mhz[2];

    //Init Reads
    read_Config(0);

    //******************************************************************************************************************************//
    // WRITE                                                                                                                        //
    // - at first, reg by reg, reading each time
    // - what is map?
    // @note {0xE0, 0x7A, 0xA3} are default symbol vals (i think)
    // @note {0x2A, 0xA9, 0x43} is what spec lists
    //
    //******************************************************************************************************************************//
    //Direct
    //0x00-0x03 GPIO3:0          Set up GPIO pins. For debug
    cc112xSpiWriteReg(CC120X_IOCFG3, &cc120x_gpioConfigMaster[0], 4);

    //0x0B  MODCFG_DEV_E          Modulation Format and Freq Dev Cfg      - [0x1B - Normal Modem mode, ASK/OOK]
    write_val = 0x1B;
    cc120xSpiWriteReg(CC120X_MODCFG_DEV_E, &write_val, 1);

    //0x0D  PREAMBLE_CFG1         Preamble Length Configuration Reg. 1    - [0x00 - No Preamble]
    write_val = 0x00;
    cc120xSpiWriteReg(CC120X_PREAMBLE_CFG1, &write_val, 1);

    //0x0E  PREAMBLE_CFG0         Preamble Length Configuration Reg. 0    - [0x00 - No Preamble Detection or Use]
    write_val = target_vals[6];
    cc120xSpiWriteReg(CC120X_PREAMBLE_CFG0, &write_val, 1);

    //0x13  MDMCFG0               General Modem Param Cfg Reg. 0          - [0x00 - Transparent Mode disabled]
    write_val = 0x00;
    cc120xSpiWriteReg(CC120X_MDMCFG0, &write_val, 1);

    //0x14  SYMBOL_RATE2          Symbol Rate Cfg Exp and Mantissa        - [!!!! - Set to Calc Val]
    write_val = target_vals[8];
    cc120xSpiWriteReg(CC120X_SYMBOL_RATE2, &write_val, 1);

    //0x15  SYMBOL_RATE1          Symbol Rate Cfg Mantissa                - [!!!! - Set to Calc Val]
    write_val = target_vals[9];
    cc120xSpiWriteReg(CC120X_SYMBOL_RATE1, &write_val, 1);

    //0x16  SYMBOL_RATE0          Symbol Rate Cfg Mantissa                - [!!!! - Set to Calc Val]
    write_val = target_vals[10];
    cc120xSpiWriteReg(CC120X_SYMBOL_RATE0, &write_val, 1);

    //0x1E  FIFO_CFG              FIFO Configuration                      - [0x00 - 127 bytes allocated to TX FIFO, 1 to RX FIFO]
    write_val = 0x00;
    cc120xSpiWriteReg(CC120X_FIFO_CFG, &write_val, 1);

    //0x21  FS_CFG                Frequency Synthesizer Configuration     - [!!!! - Set LO divider]
    write_val  = cc120xFsCfgs[2];                                           /* select LO divider=4, 820-960 MHz                     */
    cc120xSpiWriteReg(CC120X_FS_CFG, &write_val, 1);

    //0x28 PKT_CFG0
    //Variable packet length mode. Packet length configured by the first byte received after sync word??
    write_val =  target_vals[13];                                      //fixed packet length (switch to infinite in continuous)
    cc120xSpiWriteReg(CC120X_PKT_CFG0, &write_val, 1);

    //0x2C PA_CFG1 - Ramp Config   Power Amplifier Configuration Reg. 1   - [0x3F - Set Ramp & Pwr Levels]
    write_val =  0x3F;                                                      /* set correct ramp shape & power levels                */
    cc120xSpiWriteReg(CC120X_PA_CFG1, &write_val, 1);

    //0x2D PA_CFG0 - Ramp Config   Power Amplifier Configuration Reg. 0   - [0x3F - Mod Depth & Upsampler]
    write_val =  0x7C;                                                      /* Issue- we're looking for full depth here             */
    cc120xSpiWriteReg(CC120X_PA_CFG0, &write_val, 1);

    //0x2E  PKT_LEN               Packet Length Configuration             - [127 - Packet Size (might not apply to cont tx mode)]
    write_val = target_vals[16];                                            /* 127 is max packet size                               */
    cc120xSpiWriteReg(CC120X_PKT_LEN, &write_val, 1);


    //Extended
    //0x0C  FREQ2                 Frequency Configuration[23:16]          - value for freq synth (Mantissa & Exponent)
    //0x0D  FREQ1                 Frequency Configuration[15:8]           - ""
    //0x0E  FREQ0                 Frequency Configuration[7:0]            - ""
    cc120xSpiWriteReg(CC120X_FREQ2, freq_vals, 3);                          /* FREQ_2, FREQ_1. FREQ_0 - burst                       */

    //0x2C FS_SPARE               Frequency Synthesizer Spare             - not sure why but required for succesful operation       */
    write_val =  0xAC;
    cc120xSpiWriteReg(CC120X_FS_SPARE, &write_val, 1);


    //Read Vals
    read_Config(1);

    //Confirm writes
    for(i=0; i <21; i++) {
        if(reg_read_vals[1][i] != target_vals[i]) {
            for(;;);
        }
    }
#else
//<ORIG>

    // Reset radio
    trxSpiCmdStrobe(CC120X_SRES);

    // Put radio in powerdown to save power
    trxSpiCmdStrobe(CC120X_SPWD);

    uint8_t *freq_vals  = freqSettings40Mhz[2];

    //Init Reads
    read_Config(0);
//</ORIG>

// SMARTRF_SETTING_IOCFG2           0x06
     write_val = SMARTRF_SETTING_IOCFG2;
     cc112xSpiWriteReg(CC120X_IOCFG2, &write_val, 1);

// SMARTRF_SETTING_SYNC_CFG1        0xAB
     write_val = SMARTRF_SETTING_SYNC_CFG1;
     cc112xSpiWriteReg(CC120X_IOCFG1, &write_val, 1);

// SMARTRF_SETTING_DEVIATION_M      0x47
     write_val = SMARTRF_SETTING_DEVIATION_M;
     cc112xSpiWriteReg(CC120X_DEVIATION_M, &write_val, 1);

// SMARTRF_SETTING_MODCFG_DEV_E     0x1F
     write_val = SMARTRF_SETTING_MODCFG_DEV_E;
     cc112xSpiWriteReg(CC120X_MODCFG_DEV_E, &write_val, 1);

// SMARTRF_SETTING_DCFILT_CFG       0x1E
     write_val = SMARTRF_SETTING_DCFILT_CFG;
     cc112xSpiWriteReg(CC120X_DCFILT_CFG, &write_val, 1);

// SMARTRF_SETTING_PREAMBLE_CFG0    0x8A
     write_val = SMARTRF_SETTING_PREAMBLE_CFG0;
     cc112xSpiWriteReg(CC120X_PREAMBLE_CFG0, &write_val, 1);

// SMARTRF_SETTING_IQIC             0x00
     write_val = SMARTRF_SETTING_IQIC;
     cc112xSpiWriteReg(CC120X_IQIC, &write_val, 1);

// SMARTRF_SETTING_CHAN_BW          0x01
     write_val = SMARTRF_SETTING_CHAN_BW;
     cc112xSpiWriteReg(CC120X_CHAN_BW, &write_val, 1);

// SMARTRF_SETTING_MDMCFG1          0x42
     write_val = SMARTRF_SETTING_MDMCFG1;
     cc112xSpiWriteReg(CC120X_MDMCFG1, &write_val, 1);

// SMARTRF_SETTING_MDMCFG0          0x05
     write_val = SMARTRF_SETTING_MDMCFG0;
     cc112xSpiWriteReg(CC120X_MDMCFG0, &write_val, 1);

// SMARTRF_SETTING_SYMBOL_RATE2     0xB0
     write_val = SMARTRF_SETTING_SYMBOL_RATE2;
     cc112xSpiWriteReg(CC120X_SYMBOL_RATE2, &write_val, 1);

// SMARTRF_SETTING_SYMBOL_RATE1     0x62
     write_val = SMARTRF_SETTING_SYMBOL_RATE1;
     cc112xSpiWriteReg(CC120X_SYMBOL_RATE1, &write_val, 1);

// SMARTRF_SETTING_SYMBOL_RATE0     0x4E
     write_val = SMARTRF_SETTING_SYMBOL_RATE0;
     cc112xSpiWriteReg(CC120X_SYMBOL_RATE0, &write_val, 1);

// SMARTRF_SETTING_AGC_REF          0x2F
     write_val = SMARTRF_SETTING_AGC_REF;
     cc112xSpiWriteReg(CC120X_AGC_REF, &write_val, 1);

// SMARTRF_SETTING_AGC_CS_THR       0xF8
     write_val = SMARTRF_SETTING_AGC_CS_THR;
     cc112xSpiWriteReg(CC120X_AGC_CS_THR, &write_val, 1);

// SMARTRF_SETTING_AGC_CFG3         0x31
     write_val = SMARTRF_SETTING_AGC_CFG3;
     cc112xSpiWriteReg(CC120X_AGC_CFG3, &write_val, 1);

// SMARTRF_SETTING_AGC_CFG2         0x60
     write_val = SMARTRF_SETTING_AGC_CFG2;
     cc112xSpiWriteReg(CC120X_AGC_CFG2, &write_val, 1);

// SMARTRF_SETTING_AGC_CFG1         0x12
     write_val = SMARTRF_SETTING_AGC_CFG1;
     cc112xSpiWriteReg(CC120X_AGC_CFG1, &write_val, 1);

// SMARTRF_SETTING_AGC_CFG0         0x84
     write_val = SMARTRF_SETTING_AGC_CFG0;
     cc112xSpiWriteReg(CC120X_AGC_CFG0, &write_val, 1);

// SMARTRF_SETTING_FIFO_CFG         0x00
     write_val = SMARTRF_SETTING_FIFO_CFG;
     cc112xSpiWriteReg(CC120X_FIFO_CFG, &write_val, 1);

// SMARTRF_SETTING_FS_CFG           0x12
     write_val = // SMARTRF_SETTING_FS_CFG;
     cc112xSpiWriteReg(CC120X_FS_CFG, &write_val, 1);

// SMARTRF_SETTING_PKT_CFG2         0x00
     write_val = SMARTRF_SETTING_PKT_CFG2;
     cc112xSpiWriteReg(CC120X_PKT_CFG2, &write_val, 1);

// SMARTRF_SETTING_PKT_CFG0         0x20
     write_val = SMARTRF_SETTING_PKT_CFG0;
     cc112xSpiWriteReg(CC120X_PKT_CFG0, &write_val, 1);

// SMARTRF_SETTING_PA_CFG0          0x51
     write_val = SMARTRF_SETTING_PA_CFG0;
     cc112xSpiWriteReg(CC120X_PA_CFG0, &write_val, 1);

// SMARTRF_SETTING_ASK_CFG          0x3F
     write_val = SMARTRF_SETTING_ASK_CFG;
     cc112xSpiWriteReg(CC120X_ASK_CFG, &write_val, 1);

// SMARTRF_SETTING_PKT_LEN          0xFF
     write_val = SMARTRF_SETTING_PKT_LEN;
     cc112xSpiWriteReg(CC120X_PKT_LEN, &write_val, 1);

// SMARTRF_SETTING_FREQOFF_CFG      0x23
     write_val = SMARTRF_SETTING_FREQOFF_CFG;
     cc112xSpiWriteReg(CC120X_FREQOFF_CFG, &write_val, 1);

// SMARTRF_SETTING_MDMCFG2          0x00
     write_val = SMARTRF_SETTING_MDMCFG2;
     cc112xSpiWriteReg(CC120X_MDMCFG2, &write_val, 1);

// SMARTRF_SETTING_FREQ2            0x56
     write_val = SMARTRF_SETTING_FREQ2;
     cc112xSpiWriteReg(CC120X_FREQ2, &write_val, 1);

// SMARTRF_SETTING_FREQ1            0xCC
     write_val = SMARTRF_SETTING_FREQ1;
     cc112xSpiWriteReg(CC120X_FREQ1, &write_val, 1);

// SMARTRF_SETTING_FREQ0            0xCC
     write_val = SMARTRF_SETTING_FREQ0;
     cc112xSpiWriteReg(CC120X_FREQ0, &write_val, 1);

// SMARTRF_SETTING_IF_ADC1          0xEE
     write_val = SMARTRF_SETTING_IF_ADC1;
     cc112xSpiWriteReg(CC120X_IF_ADC2, &write_val, 1);

// SMARTRF_SETTING_IF_ADC0          0x10
     write_val = SMARTRF_SETTING_IF_ADC0;
     cc112xSpiWriteReg(CC120X_IF_ADC0, &write_val, 1);

// SMARTRF_SETTING_FS_DIG1          0x04
     write_val = SMARTRF_SETTING_FS_DIG1;
     cc112xSpiWriteReg(CC120X_FS_DIG1, &write_val, 1);

// SMARTRF_SETTING_FS_DIG0          0xA3
     write_val = SMARTRF_SETTING_FS_DIG0;
     cc112xSpiWriteReg(CC120X_FS_DIG0, &write_val, 1);

// SMARTRF_SETTING_FS_CAL1          0x40
     write_val = SMARTRF_SETTING_FS_CAL1;
     cc112xSpiWriteReg(CC120X_FS_CAL1, &write_val, 1);

// SMARTRF_SETTING_FS_CAL0          0x0E
     write_val = SMARTRF_SETTING_FS_CAL0;
     cc112xSpiWriteReg(CC120X_FS_CAL0, &write_val, 1);

// SMARTRF_SETTING_FS_DIVTWO        0x03
     write_val = SMARTRF_SETTING_FS_DIVTWO;
     cc112xSpiWriteReg(CC120X_FS_DIVTWO, &write_val, 1);

// SMARTRF_SETTING_FS_DSM0          0x33
     write_val = SMARTRF_SETTING_FS_DSM0;
     cc112xSpiWriteReg(CC120X_FS_DSM0, &write_val, 1);

// SMARTRF_SETTING_FS_DVC1          0xF7
     write_val = SMARTRF_SETTING_FS_DVC1;
     cc112xSpiWriteReg(CC120X_FS_DVC1, &write_val, 1);

// SMARTRF_SETTING_FS_DVC0          0x0F
     write_val = SMARTRF_SETTING_FS_DVC0;
     cc112xSpiWriteReg(CC120X_FS_DVC0, &write_val, 1);

// SMARTRF_SETTING_FS_PFD           0x00
     write_val = SMARTRF_SETTING_FS_PFD;
     cc112xSpiWriteReg(CC120X_FS_PFD, &write_val, 1);

// SMARTRF_SETTING_FS_PRE           0x6E
     write_val = SMARTRF_SETTING_FS_PRE;
     cc112xSpiWriteReg(CC120X_FS_PRE, &write_val, 1);

// SMARTRF_SETTING_FS_REG_DIV_CML   0x1C
     write_val = SMARTRF_SETTING_FS_REG_DIV_CML;
     cc112xSpiWriteReg(CC120X_FS_REG_DIV_CML, &write_val, 1);

// SMARTRF_SETTING_FS_SPARE         0xAC
     write_val = SMARTRF_SETTING_FS_SPARE;
     cc112xSpiWriteReg(CC120X_FS_SPARE, &write_val, 1);

// SMARTRF_SETTING_FS_VCO0          0xB5
     write_val = SMARTRF_SETTING_FS_VCO0;
     cc112xSpiWriteReg(CC120X_FS_VCO0, &write_val, 1);

// SMARTRF_SETTING_IFAMP            0x0D
     write_val = SMARTRF_SETTING_IFAMP;
     cc112xSpiWriteReg(CC120X_IFAMP, &write_val, 1);

// SMARTRF_SETTING_XOSC5            0x0E
     write_val = SMARTRF_SETTING_XOSC5;
     cc112xSpiWriteReg(CC120X_XOSC5, &write_val, 1);

// SMARTRF_SETTING_XOSC1            0x03
     write_val = SMARTRF_SETTING_XOSC1;
     cc112xSpiWriteReg(CC120X_XOSC1, &write_val, 1);

//<ORIG>
     //Extended
     //0x0C  FREQ2                 Frequency Configuration[23:16]          - value for freq synth (Mantissa & Exponent)
     //0x0D  FREQ1                 Frequency Configuration[15:8]           - ""
     //0x0E  FREQ0                 Frequency Configuration[7:0]            - ""
//     cc120xSpiWriteReg(CC120X_FREQ2, freq_vals, 3);                          /* FREQ_2, FREQ_1. FREQ_0 - burst                       */

     //0x2C FS_SPARE               Frequency Synthesizer Spare             - not sure why but required for successful operation       */
//     write_val =  0xAC;
//     cc120xSpiWriteReg(CC120X_FS_SPARE, &write_val, 1);

     //Read Vals
     read_Config(1);

     //Confirm writes
     for(i=0; i <21; i++) {
         if(reg_read_vals[1][i] != target_vals[i]) {
             asm(" NOP");                                                   //for(;;);
         }
     }
//</ORIG>

#endif

    return;
}


/************************************************************************************************************************************/
/** @fcn        void read_Config(uint8_t column)
 *  @brief      Read our register values
 *  @details    x
 *
 *
 *  @param      [in] (uint8_t) column - which column to store to
 */
/************************************************************************************************************************************/
void read_Config(uint8_t column) {

    cc120xSpiReadReg(CC120X_IOCFG3,        &reg_read_vals[column][0],  1);
    cc120xSpiReadReg(CC120X_IOCFG2,        &reg_read_vals[column][1],  1);
    cc120xSpiReadReg(CC120X_IOCFG1,        &reg_read_vals[column][2],  1);
    cc120xSpiReadReg(CC120X_IOCFG0,        &reg_read_vals[column][3],  1);
    cc120xSpiReadReg(CC120X_MODCFG_DEV_E,  &reg_read_vals[column][4],  1);
    cc120xSpiReadReg(CC120X_PREAMBLE_CFG1, &reg_read_vals[column][5],  1);
    cc120xSpiReadReg(CC120X_PREAMBLE_CFG0, &reg_read_vals[column][6],  1);
    cc120xSpiReadReg(CC120X_MDMCFG0,       &reg_read_vals[column][7],  1);
    cc120xSpiReadReg(CC120X_SYMBOL_RATE2,  &reg_read_vals[column][8],  1);
    cc120xSpiReadReg(CC120X_SYMBOL_RATE1,  &reg_read_vals[column][9],  1);
    cc120xSpiReadReg(CC120X_SYMBOL_RATE0,  &reg_read_vals[column][10], 1);
    cc120xSpiReadReg(CC120X_FIFO_CFG,      &reg_read_vals[column][11], 1);
    cc120xSpiReadReg(CC120X_FS_CFG,        &reg_read_vals[column][12], 1);
    cc120xSpiReadReg(CC120X_PKT_CFG0,      &reg_read_vals[column][13], 1);
    cc120xSpiReadReg(CC120X_PA_CFG1,       &reg_read_vals[column][14], 1);
    cc120xSpiReadReg(CC120X_PA_CFG0,       &reg_read_vals[column][15], 1);
    cc120xSpiReadReg(CC120X_PKT_LEN,       &reg_read_vals[column][16], 1);
    cc120xSpiReadReg(CC120X_FREQ2,         &reg_read_vals[column][17], 1);
    cc120xSpiReadReg(CC120X_FREQ1,         &reg_read_vals[column][18], 1);
    cc120xSpiReadReg(CC120X_FREQ0,         &reg_read_vals[column][19], 1);
    cc120xSpiReadReg(CC120X_FS_SPARE,      &reg_read_vals[column][20], 1);

    return;
}

