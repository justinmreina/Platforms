/*
 * cc1101_cc1190_915MHz_reg_settings.h
 *
 *  Created on: Jun 11, 2017
 *      Author: Justin
 */

#ifndef APPS_PER_TEST_RADIOS_CC1101_CC1190_CC1101_CC1190_915MHZ_REG_SETTINGS_H_
#define APPS_PER_TEST_RADIOS_CC1101_CC1190_CC1101_CC1190_915MHZ_REG_SETTINGS_H_

#include "trx_rf_spi.h"
#include "cc1101_spi.h"

extern const registerSetting_t veryHighDataRate915MHzCC1190RfSettings[];
extern const registerSetting_t highDataRate915MHzCC1190RfSettings[];
extern const registerSetting_t mediumDataRate915MHzCC1190RfSettings[];
extern const registerSetting_t lowDataRate915MHzCC1190RfSettings[];

#endif /* APPS_PER_TEST_RADIOS_CC1101_CC1190_CC1101_CC1190_915MHZ_REG_SETTINGS_H_ */
