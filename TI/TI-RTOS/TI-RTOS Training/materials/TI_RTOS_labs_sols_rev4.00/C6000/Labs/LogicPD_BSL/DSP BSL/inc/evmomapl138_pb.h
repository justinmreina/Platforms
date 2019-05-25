//-----------------------------------------------------------------------------
// \file    evmomapl138_pb.h
// \brief   OMAP-L138 pushbutton function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_PB_H
#define EVMOMAPL138_PB_H

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

#define PB_EX_1        (0)
#define PB_UI_1        (1)
#define PB_UI_2        (2)
#define PB_UI_3        (3)
#define PB_UI_4        (4)
#define PB_UI_5        (5)
#define PB_UI_6        (6)
#define PB_UI_7        (7)
#define PB_UI_8        (8)
#define MAX_PB_NUM     (9)	// TODO: update when UI board is up.

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t PB_init(void);
uint32_t PB_get(uint8_t in_pb_num, uint8_t *pb_val);
uint32_t PB_getAll(uint32_t *pb_val);

#endif
