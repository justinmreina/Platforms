//-----------------------------------------------------------------------------
// \file    evmomapl138_lcdc.h
// \brief   OMAP-L138 lcdc registers, bit definitions, and
//          function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_LCDC_H
#define EVMOMAPL138_LCDC_H

//-----------------------------------------------------------------------------
// Register Structure & Defines
//-----------------------------------------------------------------------------
typedef struct
{
   volatile uint32_t REVID;               // 0x0000
   volatile uint32_t LCD_CTRL;            // 0x0004
   volatile uint32_t LCD_STAT;            // 0x0008
   volatile uint32_t LIDD_CTRL;           // 0x000C
   volatile uint32_t LIDD_CS0_CONF;       // 0x0010
   volatile uint32_t LIDD_CS0_ADDR;       // 0x0014
   volatile uint32_t LIDD_CS0_DATA;       // 0x0018
   volatile uint32_t LIDD_CS1_CONF;       // 0x001C
   volatile uint32_t LIDD_CS1_ADDR;       // 0x0020
   volatile uint32_t LIDD_CS1_DATA;       // 0x0024
   volatile uint32_t RASTER_CTRL;         // 0x0028
   volatile uint32_t RASTER_TIMING_0;     // 0x002C
   volatile uint32_t RASTER_TIMING_1;     // 0x0030
   volatile uint32_t RASTER_TIMING_2;     // 0x0034
   volatile uint32_t RASTER_SUBPANEL;     // 0x0038
   volatile uint32_t RSVD0;               // 0x003C
   volatile uint32_t LCDDMA_CTRL;         // 0x0040
   volatile uint32_t LCDDMA_FB0_BASE;     // 0x0044
   volatile uint32_t LCDDMA_FB0_CEILING;  // 0x0048
   volatile uint32_t LCDDMA_FB1_BASE;     // 0x004C
   volatile uint32_t LCDDMA_FB1_CEILING;  // 0x0050
} lcdc_regs_t;

#define LCDC        ((lcdc_regs_t *)LCDC_REG_BASE)


#endif

