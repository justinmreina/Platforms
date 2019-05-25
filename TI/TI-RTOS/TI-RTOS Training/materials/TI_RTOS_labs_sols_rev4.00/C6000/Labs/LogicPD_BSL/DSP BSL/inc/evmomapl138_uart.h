//-----------------------------------------------------------------------------
// \file    evmomapl138_uart.h
// \brief   OMAP-L138 uart registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_UART_H
#define EVMOMAPL138_UART_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
    volatile uint32_t RBR;             // 0x0000
    volatile uint32_t IER;             // 0x0004
    volatile uint32_t IIR;             // 0x0008
    volatile uint32_t LCR;             // 0x000C
    volatile uint32_t MCR;             // 0x0010
    volatile uint32_t LSR;             // 0x0014
    volatile uint32_t MSR;             // 0x0018
    volatile uint32_t SCR;             // 0x001C
    volatile uint32_t DLL;             // 0x0020
    volatile uint32_t DLH;             // 0x0024
    volatile uint32_t REV_ID1;         // 0x0028
    volatile uint32_t REV_ID2;         // 0x002C
    volatile uint32_t PWREMU_MGMT;     // 0x0030
    volatile uint32_t MDR;             // 0x0034
} uart_regs_t;

// these registers use the same address as above, read-only vs. write-only.
#define THR RBR
#define FCR IIR

// define all the available uart peripherals for the processor.
#define UART0        ((uart_regs_t *)UART0_REG_BASE)
#define UART1        ((uart_regs_t *)UART1_REG_BASE)
#define UART2        ((uart_regs_t *)UART2_REG_BASE)
// define the uart port used for debug messages.
#define DEBUG_PORT   (UART2)

// bitmask defines for FCR.
#define RXFIFTL      (0x000000C0)   // bit 6,7
#define DMAMODE1     (0x00000008)   // bit 3
#define TXCLR        (0x00000004)   // bit 2
#define RXCLR        (0x00000002)   // bit 1
#define FIFOEN       (0x00000001)   // bit 0

// bitmask defines for LCR.
#define DLAB         (0x00000080)   // bit 7
#define BC           (0x00000040)   // bit 6
#define SP           (0x00000020)   // bit 5
#define EPS          (0x00000010)   // bit 4
#define PEN          (0x00000008)   // bit 3
#define LCR_STB      (0x00000004)   // bit 2
#define WLS          (0x00000003)   // bit 0,1
#define WLS_6        (0x00000001)   // bit 0
#define WLS_7        (0x00000002)   // bit 1
#define WLS_8        (0x00000003)   // bit 0,1

// bitmask defines for LSR.
#define RXFIFOE      (0x00000080)   // bit 7
#define TEMT         (0x00000040)   // bit 6
#define THRE         (0x00000020)   // bit 5
#define BI           (0x00000010)   // bit 4
#define FE           (0x00000008)   // bit 3
#define PE           (0x00000004)   // bit 2
#define OE           (0x00000002)   // bit 1
#define DR           (0x00000001)   // bit 0

// bitmask defines for PWREMU_MGMT.
#define UTRST        (0x00004000)   // bit 14
#define URRST        (0x00002000)   // bit 13

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t UART_init(uart_regs_t *uart, uint32_t baud_rate);
uint32_t UART_txByte(uart_regs_t *uart, uint8_t in_data);
uint32_t UART_txArray(uart_regs_t *uart, uint8_t *in_data, uint32_t in_length);
uint32_t UART_txString(uart_regs_t *uart, char *in_data);
uint32_t UART_txUint8(uart_regs_t *uart, uint8_t in_data);
uint32_t UART_txUint32(uart_regs_t *uart, uint32_t in_data);
uint32_t UART_rxByte(uart_regs_t *uart, uint8_t *data);

#endif
