//-----------------------------------------------------------------------------
// \file    evmomapl138_videodec.h
// \brief   OMAP-L138 video decoder definitions and function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_VIDEODEC_H
#define EVMOMAPL138_VIDEODEC_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------
// tvp5147 register defines.
#define TVP5147_REG_INPUT_SELECT    (0x00)
#define TVP5147_REG_AFE_GAIN        (0x01)
#define TVP5147_REG_VIDEO_STAND     (0x02)
#define TVP5147_REG_OPMODE          (0x03)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t CVIDEODEC_init(void);
uint32_t SVIDEODEC_init(void);

uint32_t CVIDEODEC_readRegister(uint8_t in_sub_addr, uint8_t *dest_buffer);
uint32_t CVIDEODEC_writeRegister(uint8_t in_sub_addr, uint8_t in_data);

uint32_t SVIDEODEC_readRegister(uint8_t in_sub_addr, uint8_t *dest_buffer);
uint32_t SVIDEODEC_writeRegister(uint8_t in_sub_addr, uint8_t in_data);

#endif
