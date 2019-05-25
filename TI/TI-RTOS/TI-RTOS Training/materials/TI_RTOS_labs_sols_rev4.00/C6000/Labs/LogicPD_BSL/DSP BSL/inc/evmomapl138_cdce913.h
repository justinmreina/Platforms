//-----------------------------------------------------------------------------
// \file    evmomapl138_cdc3913.h
// \brief   OMAP-L138 cdc3913 definitions and function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_CDCE913_H
#define EVMOMAPL138_CDCE913_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// cdc3913 register defines.
//-----------------------------------------------------------------------------

#define PDIV_1H	0x03
#define	PDIV_1L 0X02
#define PDIV_2	0x16
#define PDIV_3	0x17

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

typedef enum 
{
	cdce913_output_1 = 1,
	cdce913_output_2,
	cdce913_output_3
} cdce913_output_e;
//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t CDCE913_init(void);
uint32_t CDCE913_readByte(uint8_t in_offset, uint8_t *dest_buffer);
uint32_t CDCE913_writeByte(uint8_t in_offset, uint8_t in_data);
uint32_t CDCE913_setOutput(cdce913_output_e output, uint16_t divide_by);

#endif
