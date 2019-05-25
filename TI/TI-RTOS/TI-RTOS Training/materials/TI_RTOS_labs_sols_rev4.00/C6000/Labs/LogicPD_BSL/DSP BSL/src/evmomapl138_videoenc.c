//-----------------------------------------------------------------------------
// \file    evmomapl138_videoenc.c
// \brief   implementation of a video encoder driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_i2c.h"
#include "evmomapl138_vpif.h"
#include "evmomapl138_videoenc.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// i2c defines.
#define I2C_PORT_VIDEOENC        (I2C0)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   configures the video encoder for composite or s-video.
//
// \param   video_connector_e in_conn - which connector to use for transmit.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...composite video encoder ready to use.
//    else - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t VIDEOENC_init(video_connector_e in_conn)
{
   uint32_t rtn = 0;
   
   switch (in_conn)
   {
      // composite and s-video use same encoder settings.
      case VIDEO_CONN_COMPOSITE:
      case VIDEO_CONN_SVIDEO:
         rtn = VIDEOENC_writeRegister(0x17, 0x02);
         USTIMER_delay(100);
         
         rtn |= VIDEOENC_writeRegister(0x00, 0xFC);
      	rtn |= VIDEOENC_writeRegister(0x82, 0xCB);
      	rtn |= VIDEOENC_writeRegister(0x84, 0x00);
      	break;

      default:
         return (ERR_INVALID_PARAMETER);

   }
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// reads a register from adv7343.
//-----------------------------------------------------------------------------
uint32_t VIDEOENC_readRegister(uint8_t in_sub_addr, uint8_t *dest_buffer)
{
   uint32_t rtn;
   
   // write the register address that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEOENC, I2C_ADDR_CVIDEOENC, &in_sub_addr, 1, SKIP_STOP_BIT_AFTER_WRITE);
   if (rtn != ERR_NO_ERROR)
      return (rtn);

   // clock out the register data.
   rtn = I2C_read(I2C_PORT_VIDEOENC, I2C_ADDR_CVIDEOENC, dest_buffer, 1, SKIP_BUSY_BIT_CHECK);
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// writes a register to adv7343.
//-----------------------------------------------------------------------------
uint32_t VIDEOENC_writeRegister(uint8_t in_sub_addr, uint8_t in_data)
{
   uint32_t rtn;
   uint8_t i2c_data[2];
   
   i2c_data[0] = in_sub_addr;
   i2c_data[1] = in_data;

   // write the register that we want to read.
   rtn = I2C_write(I2C_PORT_VIDEOENC, I2C_ADDR_CVIDEOENC, i2c_data, 2, SET_STOP_BIT_AFTER_WRITE);

   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------
