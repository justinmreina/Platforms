//-----------------------------------------------------------------------------
// \file    evmomapl138_spi.c
// \brief   implementation of a spi driver for OMAP-L138.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_spi.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

// pinmux defines.
#define PINMUX_SPI0_REG_0     (3)
#define PINMUX_SPI0_MASK_0    (0x0000FFFF)
#define PINMUX_SPI0_VAL_0     (0x00001111)
#define PINMUX_SPI0_REG_1     (4)
#define PINMUX_SPI0_MASK_1    (0x000000F0)
#define PINMUX_SPI0_VAL_1     (0x00000010)
#define PINMUX_SPI1_REG       (5)
#define PINMUX_SPI1_MASK      (0x00FFFFF0)
#define PINMUX_SPI1_VAL       (0x00111110)

#define DEFAULT_CHAR_LEN      (8)

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the given spi port.
//
// \param   spi_regs_t *spi - pointer to reg struct for the desired spi port.
//
// \param   spi_config_t *in_config - desired spi configuration.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...spi ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t SPI_init(spi_regs_t *spi, spi_config_t *in_config)
{
   uint32_t rtn = ERR_INIT_FAIL;
   
   // enable the psc and config pinmux for the given spi port.
   switch ((uint32_t)spi)
   {
      case SPI0_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC0, DOMAIN0, LPSC_SPI0, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_SPI0_REG_0, PINMUX_SPI0_MASK_0, PINMUX_SPI0_VAL_0);
         EVMOMAPL138_pinmuxConfig(PINMUX_SPI0_REG_1, PINMUX_SPI0_MASK_1, PINMUX_SPI0_VAL_1);
         break;
         
      case SPI1_REG_BASE:
         EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_SPI1, PSC_ENABLE);
         EVMOMAPL138_pinmuxConfig(PINMUX_SPI1_REG, PINMUX_SPI1_MASK, PINMUX_SPI1_VAL);
         break;
      
      default:
         return (ERR_INIT_FAIL);
   }

   if (in_config != NULL)
   {
      uint32_t prescaler;
      
      // reset spi port.
      spi->SPIGCR0 = 0;
      USTIMER_delay(5);
      SETBIT(spi->SPIGCR0, RESET);
      
      // config master/slave mode.
      if (SPI_MODE_MASTER == in_config->mode)
      {
         // set clkmod and master for master mode.
         spi->SPIGCR1 = CLKMOD | MASTER;
      }
      else if (SPI_MODE_SLAVE == in_config->mode)
      {
         // clear spigcr1 for slave mode.
         spi->SPIGCR1 = 0;
      }
      else
      {
         return (ERR_INIT_FAIL);
      }
      
      // config pin options.
      switch (in_config->pin_option)
      {
         case SPI_3PIN:
            // enable spi SOMI, SIMO, and CLK.
            spi->SPIPC0 = SOMI | SIMO | CLK;
            // config SCS0 as gpio output.
            SETBIT(spi->SPIPC1, SCS0);
            break;

         case SPI_4PIN_CS:
            // enable spi SOMI, SIMO, CLK, and SCS0.
            spi->SPIPC0 = SOMI | SIMO | CLK | SCS0;
            break;

         case SPI_4PIN_EN:
            // enable spi SOMI, SIMO, CLK, and ENA.
            spi->SPIPC0 = SOMI | SIMO | CLK | ENA;
            break;

         case SPI_5PIN:
            // enable spi SOMI, SIMO, CLK, SCS0, and ENA.
            spi->SPIPC0 = SOMI | SIMO | CLK | ENA | SCS0;
            break;

         default:
            return (ERR_INIT_FAIL);
      }
      
      // config the cs active...high or low.
      spi->SPIDEF = 0;
      if (SPI_CS_ACTIVE_LOW == in_config->cs_active)
      {
         // clear csnr for active low and set cs default to 1.
         spi->SPIDAT1 = 0;
         SETBIT(spi->SPIDEF, CSDEF0);
      }
      else if (SPI_CS_ACTIVE_HIGH == in_config->cs_active)
      {
         // set csnr for active high and set cs default to 0.
         spi->SPIDAT1 = 0;
         SETBIT(spi->SPIDAT1, CSNR);
      }
      else
      {
         return (ERR_INIT_FAIL);
      }
      
      // config spi direction, polarity, and phase.
      spi->SPIFMT0 = 0;
      
      if (SPI_SHIFT_LSB == in_config->shift_dir)
      {
         SETBIT(spi->SPIFMT0, SHIFTDIR);
      }
      
      if (in_config->polarity)
      {
         SETBIT(spi->SPIFMT0, POLARITY);
      }
      
      if (in_config->phase)
      {
         SETBIT(spi->SPIFMT0, PHASE);
      }
      
      // set the prescaler and character length.
      prescaler = (((SYSCLOCK2_HZ / in_config->freq) - 1) & 0xFF);
      #ifdef DEBUG
      printf("desired spi freq: %u\r\n", in_config->freq);
      printf("spi sysclock:     %u\r\n", SYSCLOCK2_HZ);
      printf("prescaler set to: %u\r\n", prescaler);
      #endif
      SETBIT(spi->SPIFMT0, (prescaler << SHIFT_PRESCALE));
      SETBIT(spi->SPIFMT0, (DEFAULT_CHAR_LEN << SHIFT_CHARLEN));
      
      spi->SPIDELAY = (8 << 24) | (8 << 16);

      // disable interrupts.
      spi->SPIINT = 0x00;
      spi->SPILVL = 0x00;
      
      // enable spi.
      SETBIT(spi->SPIGCR1, ENABLE);

      rtn = ERR_NO_ERROR;
   }

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   transmit data out the spi, at the same time receive data as well.
//
// \param   spi_regs_t *spi - pointer to reg struct for the desired spi port.
//
// \param   uint8_t *src_buffer - pointer to the data to transmit.
//
// \param   uint8_t *dest_buffer - pointer to memory to copy the data being received.
//
// \param   uint32_t in_length - number of bytes to transmit.
//
// \param   spi_cs_hold_e in_cs_hold
//             SPI_HOLD_NONE - do nothing with CSHOLD of SPIDAT1.
//             SPI_HOLD_ACTIVE - keep cs active after completing transfer.
//             SPI_HOLD_CLR - hold cs until the last byte, then clear.
//
// \return  uint32_t
//    ERR_NO_ERROR - input in bounds, byte transmitted.
//    ERR_INVALID_PARAMETER - null pointers.
//-----------------------------------------------------------------------------
uint32_t SPI_xfer(spi_regs_t *spi, uint8_t *src_buffer, uint8_t *dest_buffer, uint32_t in_length, spi_cs_hold_e in_cs_hold)
{
   uint32_t rtn = ERR_INVALID_PARAMETER;

   if (spi != NULL)
   {
      uint32_t i;
      uint32_t spi_data1 = spi->SPIDAT1;
      
      // empty the receive buffer.
      while (!CHKBIT(spi->SPIBUF, RXEMPTY))
      {
         i = spi->SPIBUF;
      }
      
      // set cs hold if desired.
      if ((SPI_HOLD_ACTIVE == in_cs_hold) ||
            (SPI_HOLD_CLR == in_cs_hold))
      {
         SETBIT(spi_data1, CSHOLD);
      }

      // transmit data one byte at a time, copy receive data into input buffer.
      for (i = 0; i < in_length; i++)
      {
         // wait for tx buffer to be empty.
         while (CHKBIT(spi->SPIBUF, TXFULL)) {}

         // clear cs hold if we were told to set it and this is the last byte.
         if ((SPI_HOLD_CLR == in_cs_hold) &&
               (i == (in_length - 1)))
         {
            CLRBIT(spi_data1, CSHOLD);
         }
         
         // clear the tx data buffer in the tmp data reg, then write the data.
         CLRBIT(spi_data1, MASK_TXDATA);
         if (src_buffer != NULL)
         {
            SETBIT(spi_data1, *src_buffer);
            // increment src pointer, if the caller is not using the same
            // memory for src and dest.
            if (src_buffer != dest_buffer)
            {
               src_buffer++;
            }
         }
         
         // copy the tmp reg to the real thing.
         spi->SPIDAT1 = spi_data1;
         
         // wait for data to arrive.
         while (CHKBIT(spi->SPIBUF, RXEMPTY)) {}
         
         // copy the received data.
         if (dest_buffer != NULL)
         {
            *dest_buffer = (uint8_t)spi->SPIBUF;
            // increment src pointer, if the caller is using the same
            // memory as src and dest.
            dest_buffer++;
            if (src_buffer == dest_buffer)
            {
               src_buffer++;
            }
         }
      }
      
      rtn = ERR_NO_ERROR;
   }

   return (rtn);
}

