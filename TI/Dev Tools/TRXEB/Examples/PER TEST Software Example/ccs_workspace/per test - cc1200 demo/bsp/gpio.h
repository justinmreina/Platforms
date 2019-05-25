#ifndef BSP_GPIO_H_
#define BSP_GPIO_H_

//Libraries
#include <stdint.h>

//INCLUDES
#include <msp430.h>

// LED defines
#define BSP_LED_1               BIT0            //!< P4.0
#define BSP_LED_2               BIT1            //!< P4.1
#define BSP_LED_3               BIT2            //!< P4.2
#define BSP_LED_4               BIT3            //!< P4.3
#define BSP_LED_ALL             (BSP_LED_1 | BSP_LED_2 | BSP_LED_3 | BSP_LED_4)     //!< Bitmask of all LEDs


//Globals
extern void gpio_init(void);


#endif /* BSP_GPIO_H_ */
