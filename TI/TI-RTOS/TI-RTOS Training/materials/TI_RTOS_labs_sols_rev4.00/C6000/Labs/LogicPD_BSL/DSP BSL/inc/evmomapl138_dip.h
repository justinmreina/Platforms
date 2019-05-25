//-----------------------------------------------------------------------------
// \file    evmomapl138_dip.h
// \brief   OMAP-L138 dip switch function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_DIP_H
#define EVMOMAPL138_DIP_H

//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

#define DIP_1           (0)
#define DIP_2           (1)
#define DIP_3           (2)
#define DIP_4           (3)
#define DIP_5           (4)
#define DIP_6           (5)
#define DIP_7           (6)
#define DIP_8           (7)
#define MAX_DIP_NUM     (8)

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------
uint32_t DIP_init(void);
uint32_t DIP_get(uint8_t in_dip_num, uint8_t *dip_val);
uint32_t DIP_getAll(uint32_t *dip_val);

#endif
