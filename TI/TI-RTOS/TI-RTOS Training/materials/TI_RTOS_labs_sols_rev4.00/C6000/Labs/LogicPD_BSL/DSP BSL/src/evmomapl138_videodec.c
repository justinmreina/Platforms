//-----------------------------------------------------------------------------
// \file    evmomapl138_videodec.c
// \brief   implementation of a video decoder driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_i2c_gpio.h"
#include "evmomapl138_emac.h"
#include "evmomapl138_cdce913.h"
#include "evmomapl138_videodec.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// i2c defines.
#define I2C_PORT_VIDEODEC        (I2C0)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t configVideoBus(void);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   configures the composite video decoder.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...composite vdeo decoder ready to use.
//    else - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t CVIDEODEC_init(void)
{
   uint32_t rtn = 0;

   // make sure we have exclusive access to the data bus on the UI board.
   rtn = configVideoBus();
   if (rtn != ERR_NO_ERROR)
      return (rtn);
   
   // put decoder in power down, then normal operating mode to reset device.
   rtn = CVIDEODEC_writeRegister(TVP5147_REG_OPMODE, 0x01);
   USTIMER_delay(100);
   rtn |= CVIDEODEC_writeRegister(TVP5147_REG_OPMODE, 0x00);
   USTIMER_delay(100);
   
   // config decoder for composite, ntsc input and BT.656 output.
   rtn |= CVIDEODEC_writeRegister(0x00, 0x05);
   rtn |= CVIDEODEC_writeRegister(0x08, 0x00);
   rtn |= CVIDEODEC_writeRegister(0x0E, 0x04);
   rtn |= CVIDEODEC_writeRegister(0x34, 0x11);
   rtn |= CVIDEODEC_writeRegister(0x02, 0x01);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   configures the s-video decoder.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...s-video decoder ready to use.
//    else - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t SVIDEODEC_init(void)
{
   uint32_t rtn = 0;
   
   // make sure we have exclusive access to the data bus on the UI board.
   rtn = configVideoBus();
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // put decoder in power down, then normal operating mode to reset device.
   rtn = SVIDEODEC_writeRegister(TVP5147_REG_OPMODE, 0x01);
   USTIMER_delay(100);
   rtn |= SVIDEODEC_writeRegister(TVP5147_REG_OPMODE, 0x00);
   USTIMER_delay(100);
   
   // config decoder for s-video, ntsc input and BT.656 output.
   rtn |= SVIDEODEC_writeRegister(0x00, 0x46);
   rtn |= SVIDEODEC_writeRegister(0x08, 0x00);
   rtn |= SVIDEODEC_writeRegister(0x0E, 0x04);
   rtn |= SVIDEODEC_writeRegister(0x34, 0x11);
   rtn |= SVIDEODEC_writeRegister(0x02, 0x01);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   reads a register from tvp5147 (composite).
//
// \param   uint8_t in_sub_addr - register sub address to be read.
//
// \param   uint8_t *dest_buffer - pointer to memory where data is to be copied.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...register data copied into dest.
//    else - something happened during read.
//-----------------------------------------------------------------------------
uint32_t CVIDEODEC_readRegister(uint8_t in_sub_addr, uint8_t *dest_buffer)
{
   uint32_t rtn;
   
   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEODEC, I2C_ADDR_CVIDEODEC, &in_sub_addr, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the register data.
   rtn = I2C_read(I2C_PORT_VIDEODEC, I2C_ADDR_CVIDEODEC, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   writes a register from tvp5147 (composite).
//
// \param   uint8_t in_sub_addr - register sub address to write.
//
// \param   uint8_t in_data - data to be written.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data written to decoder.
//    else - something happened during read.
//-----------------------------------------------------------------------------
uint32_t CVIDEODEC_writeRegister(uint8_t in_sub_addr, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];
   
   i2c_data[0] = in_sub_addr;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEODEC, I2C_ADDR_CVIDEODEC, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   reads a register from tvp5147 (s-video).
//
// \param   uint8_t in_sub_addr - register sub address to be read.
//
// \param   uint8_t *dest_buffer - pointer to memory where data is to be copied.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...register data copied into dest.
//    else - something happened during read.
//-----------------------------------------------------------------------------
uint32_t SVIDEODEC_readRegister(uint8_t in_sub_addr, uint8_t *dest_buffer)
{
   uint32_t rtn;

   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEODEC, I2C_ADDR_SVIDEODEC, &in_sub_addr, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the register data.
   rtn = I2C_read(I2C_PORT_VIDEODEC, I2C_ADDR_SVIDEODEC, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   writes a register from tvp5147 (s-video).
//
// \param   uint8_t in_sub_addr - register sub address to write.
//
// \param   uint8_t in_data - data to be written.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...data written to decoder.
//    else - something happened during read.
//-----------------------------------------------------------------------------
uint32_t SVIDEODEC_writeRegister(uint8_t in_sub_addr, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];

   i2c_data[0] = in_sub_addr;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEODEC, I2C_ADDR_SVIDEODEC, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// enables the buffers for video data and disables everything else on the bus.
//-----------------------------------------------------------------------------
static uint32_t configVideoBus(void)
{
   uint32_t rtn;

   // enable video (active low) via gpio expander and disable rmii phy to
   // ensure we have exclusive access to the bus.
   rtn = I2CGPIO_init(I2C_ADDR_GPIO_UI);
   if (rtn != ERR_NO_ERROR)
      return (ERR_NO_UI_BOARD);
   
   // make sure we are getting a 27MHz clock.
   rtn = CDCE913_init();

   rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELA, 1);
   rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELB, 1);
   rtn |= I2CGPIO_setOutput(I2C_ADDR_GPIO_UI, I2C_GPIO_UI_SELC, 0);
   
   return (rtn);
}
