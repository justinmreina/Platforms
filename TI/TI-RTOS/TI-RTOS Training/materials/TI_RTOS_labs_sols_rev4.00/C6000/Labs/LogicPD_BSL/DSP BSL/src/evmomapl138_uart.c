//-----------------------------------------------------------------------------
// \file    evmomapl138_uart.c
// \brief   implementation of a uart driver for OMAP-L138.
//
//-----------------------------------------------------------------------------
#ifndef BOOT
#include "stdio.h"
#endif
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_uart.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// pinmux defines.
#define PINMUX_UART0_REG      (3)
#define PINMUX_UART0_MASK     (0x0000FFFF)
#define PINMUX_UART0_VAL      (0x00002222)
#define PINMUX_UART1_REG_0    (0)
#define PINMUX_UART1_MASK_0   (0x00FF0000)
#define PINMUX_UART1_VAL_0    (0x00440000)
#define PINMUX_UART1_REG_1    (4)
#define PINMUX_UART1_MASK_1   (0xFF000000)
#define PINMUX_UART1_VAL_1    (0x22000000)
#define PINMUX_UART2_REG_0    (0)
#define PINMUX_UART2_MASK_0   (0xFF000000)
#define PINMUX_UART2_VAL_0    (0x44000000)
#define PINMUX_UART2_REG_1    (4)
#define PINMUX_UART2_MASK_1   (0x00FF0000)
#define PINMUX_UART2_VAL_1    (0x00220000)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t getStringLength(char *in_string);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the given serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint32_t baud_rate - desired baud rate for serial port.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...uart ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t UART_init(uart_regs_t *uart, uint32_t baud_rate)
{
   uint32_t divisor;
   
   // enable the psc and config pinmux for the given uart port.
   switch ((uint32_t)uart)
   {
      case UART0_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC0, DOMAIN0, LPSC_UART0, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_UART0_REG, PINMUX_UART0_MASK, PINMUX_UART0_VAL);
         break;
         
      case UART1_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_UART1, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_UART1_REG_0, PINMUX_UART1_MASK_0, PINMUX_UART1_VAL_0);
         EVMOMAPL138_pinmuxConfig(PINMUX_UART1_REG_1, PINMUX_UART1_MASK_1, PINMUX_UART1_VAL_1);
         break;
         
      case UART2_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_UART2, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_UART2_REG_0, PINMUX_UART2_MASK_0, PINMUX_UART2_VAL_0);
         EVMOMAPL138_pinmuxConfig(PINMUX_UART2_REG_1, PINMUX_UART2_MASK_1, PINMUX_UART2_VAL_1);
         break;

      default:
         return (ERR_INIT_FAIL);
   }

   // put xmtr/rcvr in reset.
   uart->PWREMU_MGMT = 0;

   // set baud rate...assumes default 16x oversampling.
   divisor = SYSCLOCK2_HZ / (baud_rate * 16);
   uart->DLH = (divisor & 0x0000FF00) >> 8;
   uart->DLL = divisor & 0x000000FF;

   // enable xmtr/rcvr fifos.
   uart->FCR = 0;
   SETBIT(uart->FCR, FIFOEN);
   SETBIT(uart->FCR, RXCLR | TXCLR | DMAMODE1);

   // disable interrupts, flow control, and loop back.
   uart->IER = 0;
   uart->MCR = 0;
   uart->MDR = 0;

   // config LCR for no parity, one stop bit, 8 data bits, no flow control.
   uart->LCR = 0;
   SETBIT(uart->LCR, WLS_8);

   // take xmtr/rcvr out of reset.
   SETBIT(uart->PWREMU_MGMT, UTRST | URRST | FREE);
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   transmit a byte out the debug serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint8_t in_data - the byte to transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, byte transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t UART_txByte(uart_regs_t *uart, uint8_t in_data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (uart != 0)
   {
      // wait until there is room in the FIFO.
      while (!CHKBIT(uart->LSR, THRE)) {}

      // send the character.
      uart->THR = in_data;
      
      rtn = ERR_NO_ERROR;
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit an array of bytes out a serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint8_t *in_data - pointer to the array to transmit.
//
// \param   uint32_t in_length - number of bytes to transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, array transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t UART_txArray(uart_regs_t *uart, uint8_t *in_data, uint32_t in_length)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   uint32_t i;
   
   if ((uart != 0) && (in_data != 0))
   {
      for (i = 0; i < in_length; i++)
      {
         (void)UART_txByte(uart, in_data[i]);
      }
      
      rtn = ERR_NO_ERROR;
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit a string out a serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint8_t *in_data - pointer to the string to transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, string transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t UART_txString(uart_regs_t *uart, char *in_data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;
   
   if ((uart != 0) && (in_data != 0))
   {
      uint32_t numBytes = getStringLength(in_data);

      rtn = UART_txArray(uart, (uint8_t *)in_data, numBytes);
   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit a hex byte out a serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint8_t in_data - the uint8 to transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, byte transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t UART_txUint8(uart_regs_t *uart, uint8_t in_data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (uart != 0)
   {
      uint8_t tmp_array[5];

      #ifndef BOOT
      sprintf((char *)tmp_array, "0x%02X", in_data);
      #endif

      rtn = UART_txArray(uart, tmp_array, 4);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit a uint32 out a serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint32_t in_data - the uint32 to transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, uint32 transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t UART_txUint32(uart_regs_t *uart, uint32_t in_data)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (uart != 0)
   {
      uint8_t tmp_array[12];

      #ifndef BOOT
      sprintf((char *)tmp_array, "0x%08X", in_data);
      #endif

      rtn = UART_txArray(uart, tmp_array, 10);
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   Receives a byte from the debug serial port.
//
// \param   uart_regs_t *uart - pointer to register struct for the desired uart.
//
// \param   uint8_t *data - pointer to memory where data is to be copied.
//
// \return  uint32_t
//    ERR_RX_FIFO_EMPTY - no data in fifo.
//    else - data copied from fifo.
//-----------------------------------------------------------------------------
uint32_t UART_rxByte(uart_regs_t *uart, uint8_t *data)
{
   if (CHKBIT(uart->LSR, DR))
   {
      // data in the fifo...copy it it.
      *data = (uint8_t)uart->RBR;
      return (ERR_NO_ERROR);
   }
   else
   {
      // fifo empty...let caller know.
      return ERR_UART_RX_FIFO_EMPTY;
   }
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// helper function to compute the number of bytes in a string.
//-----------------------------------------------------------------------------
static uint32_t getStringLength(char *in_string)
{
   uint32_t numBytes = 0;

   while (in_string[numBytes] != 0)
   {
      numBytes++;
   }

   return numBytes;
}
