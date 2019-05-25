//-----------------------------------------------------------------------------
// \file    evmomapl138_led.h
// \brief   OMAP-L138 led function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_LED_H
#define EVMOMAPL138_LED_H

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

#define LED_1           (0)
#define LED_2           (1)
#define MAX_LED_NUM     (2)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t LED_init(void);
uint32_t LED_isOn(uint8_t in_led_num, uint8_t *led_status);
uint32_t LED_turnOn(uint8_t in_led_num);
uint32_t LED_turnOff(uint8_t in_led_num);
uint32_t LED_toggle(uint8_t in_led_num);

#endif
