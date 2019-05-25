/*
 * jmr_temp.h
 *
 *  Created on: Jun 11, 2017
 *      Author: Justin
 *
 *  @brief      location to store temporary items to help compile (e.g. #include <stdlib.h>, etc.
 *  @details    not intended for final release
 */

#ifndef JMR_TEMP_H_
#define JMR_TEMP_H_

#include "trx_rf_spi.h"


#define CC11xL_FSCTRL0                      (0)
#define CC11xL_MDMCFG4                      (0)
#define CC11xL_MDMCFG3                      (0)
#define CC11xL_MDMCFG2                      (0)
#define CC11xL_DEVIATN                      (0)
#define CC11xL_TEST2                        (0)
#define CC11xL_TEST1                        (0)
#define CC11xL_TEST0                        (0)
#define CC11xL_IOCFG2                       (0)
#define CC11xL_IOCFG1                       (0)
#define CC11xL_IOCFG0                       (0)
#define CC11xL_FIFOTHR                      (0)
#define CC11xL_SYNC1                        (0)
#define CC11xL_SYNC0                        (0)
#define CC11xL_PKTLEN                       (0)
#define CC11xL_PKTCTRL0                     (0)
#define CC11xL_FREQ2                        (0)
#define CC11xL_FREQ1                        (0)
#define CC11xL_FREQ0                        (0)
#define CC11xL_MCSM1CC11xL_MCSM0            (0)
#define CC11xL_MCSM0                        (0)
#define CC11xL_MCSM1                        (0)
#define CC11xL_FSCAL3                       (0)
#define CC11xL_FSCAL2                       (0)
#define CC11xL_FSCAL1                       (0)
#define CC11xL_FSCAL0                       (0)
#define CC11xL_RESERVED_0X29                (0)
#define CC11xL_RESERVED_0X2A                (0)
#define CC11xL_RESERVED_0X2B                (0)
#define CC11xL_PKTCTRL1                     (0)
#define CC11xL_ADDR                         (0)
#define CC11xL_LQI_CRC_OK_BM                (0)
#define CC11xL_LQI_EST_BM                   (0)
#define CC11xL_RSSI                         (0)
//<oops>#define CC11xL_MCSMs                        (0)
#define CC11xL_RXBYTES                      (0)
#define CC11xL_SFRX                         (0)
#define CC11xL_SRX                          (0)
#define CC11xL_STATE_IDLE                   (0)
#define CC11xL_STATE_RX                     (0)
#define CC11xL_MCSM2                        (0)
#define CC11xL_SIDLE                        (0)
#define CC11xL_SPWD                         (0)
#define CC11xL_STATE_TX                     (0)
#define CC11xL_STX                          (0)

extern const registerSetting_t highDataRateRfSettings[];
extern const registerSetting_t lowDataRateRfSettings[];


#endif /* JMR_TEMP_H_ */

