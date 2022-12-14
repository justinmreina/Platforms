//*****************************************************************************
//! @file       bsp.h
//! @brief      Board support package header file for MSP430F5438A on
//!             SmartRF TrxEB.
//!
//! Revised     $Date: 2013-09-18 13:18:48 +0200 (on, 18 sep 2013) $
//! Revision    $Revision: 10587 $
//
//  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//****************************************************************************/
#ifndef __BSP_H__
#define __BSP_H__


/******************************************************************************
* If building with a C++ compiler, make all of the definitions in this header
* have a C binding.
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
* INCLUDES
*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

extern void bspIoSpiUninit(uint8_t ui8Spi);

/******************************************************************************
* DEFINES
*/
#define BSP_SYS_CLK_1MHZ        1000000UL
#define BSP_SYS_CLK_4MHZ        4000000UL
#define BSP_SYS_CLK_8MHZ        8000000UL
#define BSP_SYS_CLK_12MHZ       12000000UL
#define BSP_SYS_CLK_16MHZ       16000000UL
#define BSP_SYS_CLK_20MHZ       20000000UL
#define BSP_SYS_CLK_25MHZ       25000000UL

//
// Clock speed defines
//


//
//! Default system clock speed
//
#define BSP_SYS_CLK_SPD         BSP_SYS_CLK_8MHZ

//
//! Default IO SPI0 clock speed
//
#define BSP_FLASH_LCD_SPI_SPD   BSP_SYS_CLK_SPD

//
//! Default IO SPI1 clock speed
//
#define BSP_ACC_SPI_SPD         400000

//
// IO SPI defines
//
#define BSP_IO_SPI0             1
#define BSP_IO_SPI1             2
#define BSP_FLASH_LCD_SPI       BSP_IO_SPI0
#define BSP_ACC_SPI             BSP_IO_SPI1

//
// IO SPI0 defines
//
#define BSP_IO_SPI0_BUS_BASE    __MSP430_BASEADDRESS_PORT9_R__
#define BSP_IO_SPI0_PORT        9
#define BSP_IO_SPI0_MOSI        BIT1        //! P9.1
#define BSP_IO_SPI0_MISO        BIT2        //! P9.2
#define BSP_IO_SPI0_SCLK        BIT3        //! P9.3

//
// IO SPI1 defines
//
#define BSP_IO_SPI1_BUS_BASE    __MSP430_BASEADDRESS_PORT9_R__
#define BSP_IO_SPI1_PORT        9
#define BSP_IO_SPI1_MOSI        BIT4                //! P9.4
#define BSP_IO_SPI1_MISO        BIT5                //! P9.5
#define BSP_IO_SPI1_SCLK        BIT0                //! P9.0

//
// SPI flash defines
//
#define BSP_FLASH_SPI           BSP_IO_SPI0
#define BSP_FLASH_SPI_PORT      BSP_IO_SPI0_PORT
#define BSP_FLASH_SPI_BUS_BASE  BSP_IO_SPI0_BUS_BASE
#define BSP_FLASH_MOSI          BSP_IO_SPI0_MOSI
#define BSP_FLASH_MISO          BSP_IO_SPI1_MISO
#define BSP_FLASH_SCLK          BSP_IO_SPI1_SCLK
#define BSP_FLASH_CS_N_BASE     __MSP430_BASEADDRESS_PORT8_R__
#define BSP_FLASH_CS_N_PORT     8
#define BSP_FLASH_CS_N          BIT6                //! P8.6
#define BSP_FLASH_PWR_BASE      __MSP430_BASEADDRESS_PORT7_R__
#define BSP_FLASH_PWR_PORT      7
#define BSP_FLASH_PWR           BIT6                //! P7.6
#define BSP_FLASH_RST_BASE      __MSP430_BASEADDRESS_PORT7_R__
#define BSP_FLASH_RST_PORT      7
#define BSP_FLASH_RST           BIT2                //! P7.2

//
// LCD defines
//
#define BSP_LCD_SPI             BSP_IO_SPI0
#define BSP_LCD_SPI_PORT        BSP_IO_SPI0_PORT
#define BSP_LCD_SPI_BUS_BASE    BSP_IO_SPI0_BUS_BASE
#define BSP_LCD_MOSI            BSP_IO_SPI0_MOSI
#define BSP_LCD_MISO            BSP_IO_SPI1_MISO
#define BSP_LCD_SCLK            BSP_IO_SPI1_SCLK
#define BSP_LCD_CS_N_BASE       __MSP430_BASEADDRESS_PORT9_R__
#define BSP_LCD_CS_N_PORT       9
#define BSP_LCD_CS_N            BIT6                //! P9.6
#define BSP_LCD_PWR_BASE        __MSP430_BASEADDRESS_PORT7_R__
#define BSP_LCD_PWR_PORT        7
#define BSP_LCD_PWR             BIT7                //! P7.7
#define BSP_LCD_RST_BASE        __MSP430_BASEADDRESS_PORT7_R__
#define BSP_LCD_RST_PORT        7
#define BSP_LCD_RST             BIT3                //! P7.3
#define BSP_LCD_MODE_BASE       __MSP430_BASEADDRESS_PORT9_R__
#define BSP_LCD_MODE_PORT       9
#define BSP_LCD_MODE            BIT7                //! P9.7 (aka. LCD A0)

//
// LED defines
//
#define BSP_LED_1               BIT0            //!< P4.0
#define BSP_LED_2               BIT1            //!< P4.1
#define BSP_LED_3               BIT2            //!< P4.2
#define BSP_LED_4               BIT3            //!< P4.3
#define BSP_LED_ALL             (BSP_LED_1 | \
                                 BSP_LED_2 | \
                                 BSP_LED_3 | \
                                 BSP_LED_4)     //!< Bitmask of all LEDs

//
// key defines
//
#define BSP_KEY_1               BIT1            //!< P2.1
#define BSP_KEY_2               BIT2            //!< P2.2
#define BSP_KEY_3               BIT4            //!< P2.4
#define BSP_KEY_4               BIT5            //!< P2.5
#define BSP_KEY_5               BIT3            //!< P2.3
#define BSP_KEY_ALL             (BSP_KEY_1|                                   \
                                 BSP_KEY_2|                                   \
                                 BSP_KEY_3|                                   \
                                 BSP_KEY_4|                                   \
                                 BSP_KEY_5)     //!< Bitmask of all keys
#define BSP_KEY_LEFT            BSP_KEY_1
#define BSP_KEY_RIGHT           BSP_KEY_2
#define BSP_KEY_UP              BSP_KEY_3
#define BSP_KEY_DOWN            BSP_KEY_4
#define BSP_KEY_SELECT          BSP_KEY_5
#define BSP_KEY_DIR_ALL         (BSP_KEY_LEFT |                               \
                                 BSP_KEY_RIGHT|                               \
                                 BSP_KEY_UP   |                               \
                                 BSP_KEY_DOWN)  //!< Bitmask of all dir. keys

//
// USB UART defines
//
#define BSP_UART_PORT           5
#define BSP_UART_TXD            BIT6
#define BSP_UART_RXD            BIT7
#define BSP_UART_RTS_PORT       4
#define BSP_UART_RTS            BIT4
#define BSP_UART_CTS_PORT       2
#define BSP_UART_CTS            BIT7

//
// Ambient light sensor defines
//
#define BSP_ALS_BASE            __MSP430_BASEADDRESS_PORT6_R__
#define BSP_ALS_PORT            6
#define BSP_ALS_PWR             BIT1            //!< P6.1
#define BSP_ALS_OUT             BIT2            //!< P6.2

//
// Accelerometer defines
//
// TBD


/******************************************************************************
* FUNCTION PROTOTYPES
*/
extern void bspInit(uint32_t ui32SysClockSpeed);
extern uint32_t bspSysClockSpeedGet(void);
extern void bspSysClockSpeedSet(uint32_t ui32SystemClockSpeed);
extern uint32_t bspIoSpiInit(uint8_t ui8Spi, uint32_t ui32ClockSpeed);
#ifdef BLOCKA
extern uint32_t bspIoSpiClockSpeedGet(uint8_t ui8Spi);
extern void bspIoSpiUninit(uint8_t ui8Spi);
#endif
extern void bspAssert(void);
extern void bspMcuStartXT1(void);

extern void bspMcuStartXT1(void);
extern void bspMcuSetVCoreUp(uint8_t ui8Level);
extern void bspMcuSetVCoreDown(uint8_t ui8Level);
extern void bspMcuGetSystemClockSettings(uint32_t ui32SystemClockSpeed, uint8_t *pui8SetDcoRange, uint8_t *pui8SetVCore, uint32_t *pui32SetMultiplier);
extern void bspMcuSetVCore(uint8_t ui8Level);


/******************************************************************************
* Mark the end of the C bindings section for C++ compilers.
******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* #ifndef __BSP_H__ */

