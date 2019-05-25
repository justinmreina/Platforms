//-----------------------------------------------------------------------------
// \file    evmomapl138.h
// \brief   OMAP-L138 internal registers & bit definitions.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_H
#define EVMOMAPL138_H

#include "types.h"
#include "evmomapl138_sysconfig.h"
#include "evmomapl138_psc.h"
#include "evmomapl138_pll.h"

//-----------------------------------------------------------------------------
// bit manipulation macros and bitval lookup table declarations.
//-----------------------------------------------------------------------------
#define SETBIT(dest,mask)     (dest |= mask)
#define CLRBIT(dest,mask)     (dest &= ~mask)
#define TGLBIT(dest,mask)     (dest ^= mask)
#define CHKBIT(dest,mask)     (dest & mask)
extern const uint32_t bitval_u32[32];

//-----------------------------------------------------------------------------
// bitmask defines for EMUMGT.
//-----------------------------------------------------------------------------
#define SOFT                  (0x00000002)   // bit 1
#define FREE                  (0x00000001)   // bit 0

//-----------------------------------------------------------------------------
// Return Error Defines
//-----------------------------------------------------------------------------
#define ERR_NO_ERROR             (0)
#define ERR_FAIL                 (1)
#define ERR_INIT_FAIL            (2)
#define ERR_NO_UI_BOARD          (3)
#define ERR_INVALID_PARAMETER    (4)
#define ERR_TIMEOUT              (5)
#define ERR_UART_RX_FIFO_EMPTY   (6)
#define ERR_SPI_BUS_ERROR        (7)
#define ERR_ENET_LINK_INACTIVE   (8)
#define ERR_ENET_PKT_TOO_LARGE   (9)
#define ERR_ENET_Q_EMPTY         (10)
#define ERR_ENET_RX_ERROR        (11)
#define ERR_FLASH_VERIFY         (12)
#define ERR_MMCSD_TIMEOUT        (13)
#define ERR_NAND_BAD_BLOCK       (14)
#define ERR_UIDB_NUM_DEVICES     (15)    //for UI databus muxing control 
#define ERR_UIDB_INCOMPAT_DEV    (16)
#define ERR_UIDB_INVALID_DEVICE  (17)
//-----------------------------------------------------------------------------
// Utility Print Defines
//-----------------------------------------------------------------------------
#define PRINT_NONE                  (0)
#define PRINT_DURING_VERIFY         (1)
#define PRINT_HEADER                (0)
#define PRINT_CONTINUE              (1)

//-----------------------------------------------------------------------------
// power and sleep config registers
//-----------------------------------------------------------------------------
#define PSC0_REG_BASE         (0x01C10000)
#define PSC1_REG_BASE         (0x01E27000)

//-----------------------------------------------------------------------------
// system config registers
//-----------------------------------------------------------------------------
#define SYSCONFIG_REG_BASE    (0x01C14000)

//-----------------------------------------------------------------------------
// PLL registers
//-----------------------------------------------------------------------------
#define PLL0_REG_BASE         (0x01C11000)
#define PLL1_REG_BASE         (0x01E1A000)

//-----------------------------------------------------------------------------
// DDR registers
//-----------------------------------------------------------------------------
#define DDR_REG_BASE          (0xB0000000)
#define DDR_MEM_BASE          (0xC0000000)
#define DDR_MEM_SIZE          (0x08000000)	// 128MB

//-----------------------------------------------------------------------------
// EMIFA registers
//-----------------------------------------------------------------------------
#define EMIFA_REG_BASE        (0x68000000)

//-----------------------------------------------------------------------------
// GPIO registers
//-----------------------------------------------------------------------------
#define GPIO_REG_BASE         (0x01E26000)
#define GPIO_BANK_OFFSET      (0x28)

#define GPIO_REV              (GPIO_REG_BASE)
#define GPIO_BINTEN           (GPIO_REG_BASE + 0x08)

#define GPIO_BANK01_BASE      (GPIO_REG_BASE + 0x10)
#define GPIO_BANK23_BASE      (GPIO_BANK01_BASE + GPIO_BANK_OFFSET)
#define GPIO_BANK45_BASE      (GPIO_BANK23_BASE + GPIO_BANK_OFFSET)
#define GPIO_BANK67_BASE      (GPIO_BANK45_BASE + GPIO_BANK_OFFSET)
#define GPIO_BANK8_BASE       (GPIO_BANK67_BASE + GPIO_BANK_OFFSET)

// buffer enable pin defines.
#define GPIO_BUFF_OE_BANK        (2)
#define GPIO_BUFF_OE_PIN         (6)
#define PINMUX_GPIO_BUFF_OE_REG  (6)
#define PINMUX_GPIO_BUFF_OE_MASK (0x000000F0)
#define PINMUX_GPIO_BUFF_OE_VAL  (0x00000080)

// sd-nor select pin defines.
#define GPIO_SD_NOR_SELECT_BANK         (0)
#define GPIO_SD_NOR_SELECT_PIN          (11)
#define PINMUX_GPIO_SD_NOR_SELECT_REG   (0)
#define PINMUX_GPIO_SD_NOR_SELECT_MASK  (0x000F0000)
#define PINMUX_GPIO_SD_NOR_SELECT_VAL   (0x00080000)

//-----------------------------------------------------------------------------
// Timer registers
//-----------------------------------------------------------------------------
#define TIMER0_REG_BASE       (0x01C20000)
#define TIMER1_REG_BASE       (0x01C21000)

//-----------------------------------------------------------------------------
// UART registers
//-----------------------------------------------------------------------------
#define UART0_REG_BASE        (0x01C42000)
#define UART1_REG_BASE        (0x01D0C000)
#define UART2_REG_BASE        (0x01D0D000)

//-----------------------------------------------------------------------------
// SPI registers
//-----------------------------------------------------------------------------
#define SPI0_REG_BASE         (0x01C41000)
#define SPI1_REG_BASE         (0x01F0E000)

//-----------------------------------------------------------------------------
// I2C registers
//-----------------------------------------------------------------------------
#define I2C0_REG_BASE         (0x01C22000)
#define I2C1_REG_BASE         (0x01E28000)

//-----------------------------------------------------------------------------
// EMAC registers
//-----------------------------------------------------------------------------
#define EMAC_RAM_BASE         (0x01E20000)
#define EMAC_CTRL_REG_BASE    (0x01E22000)
#define EMAC_REG_BASE         (0x01E23000)
#define MDIO_REG_BASE         (0x01E24000)

//-----------------------------------------------------------------------------
// MMCSD registers
//-----------------------------------------------------------------------------
#define MMCSD0_REG_BASE       (0x01C40000)
#define MMCSD1_REG_BASE       (0x01E1B000)

//-----------------------------------------------------------------------------
// MCASP registers
//-----------------------------------------------------------------------------
#define MCASP_REG_BASE        (0x01D00000)

//-----------------------------------------------------------------------------
// USB registers
//-----------------------------------------------------------------------------
#define USB_OTG_REG_BASE      (0x01E00000)
#define USB_HOST_REG_BASE     (0x01E25000)

//-----------------------------------------------------------------------------
// VPIF registers
//-----------------------------------------------------------------------------
#define VPIF_REG_BASE            (0x01E17000)
#define VPIF_CAP_CH0_REG_BASE    (0x01E17040)
#define VPIF_CAP_CH1_REG_BASE    (0x01E17080)
#define VPIF_DISP_CH2_REG_BASE   (0x01E170C0)
#define VPIF_DISP_CH3_REG_BASE   (0x01E17140)

//-----------------------------------------------------------------------------
// LCDC registers
//-----------------------------------------------------------------------------
#define LCDC_REG_BASE         (0x01E13000)

//-----------------------------------------------------------------------------
// uPP registers
//-----------------------------------------------------------------------------
#define UPP_REG_BASE          (0x01E16000)

//-----------------------------------------------------------------------------
// RTC registers
//-----------------------------------------------------------------------------
#define RTC_REG_BASE          (0x01C23000)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t EVMOMAPL138_init(void);
uint32_t EVMOMAPL138_initRAM(void);
void EVMOMAPL138_enableDsp(void);
void EVMOMAPL138_pinmuxConfig(uint32_t in_reg, uint32_t in_mask, uint32_t in_val);
void EVMOMAPL138_lpscTransition(psc_regs_t *psc_reg, uint32_t in_domain, uint8_t in_module, uint8_t in_next_state);
void UTIL_printMem(uint32_t begin_addr, uint8_t *buffer, uint32_t length, uint8_t continuation);
uint8_t UTIL_isUIBoardAttached(void);
uint32_t config_pll0(uint32_t clkmode, uint32_t pllm, uint32_t postdiv, uint32_t plldiv1, uint32_t plldiv2, uint32_t plldiv3, uint32_t plldiv7);
uint32_t config_pll1(uint32_t pllm, uint32_t postdiv, uint32_t plldiv1, uint32_t plldiv2, uint32_t plldiv3);

#endif
