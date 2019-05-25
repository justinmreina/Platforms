//-----------------------------------------------------------------------------
// \file    evmomapl138_lcd_raster.h
// \brief   OMAP-L138 lcd raster function prototypes.
//
//-----------------------------------------------------------------------------

#ifndef EVMOMAPL138_LCD_RASTER_H
#define EVMOMAPL138_LCD_RASTER_H

//-----------------------------------------------------------------------------
// Public Defines and Macros
//-----------------------------------------------------------------------------

// #define DISPLAY_3_6
#define DISPLAY_4_3
// #define DISPLAY_6_4


#define FRAMEBUF_BASE            (0xC0000000)
#define FRAMEBUF_PALETTE_BASE    (FRAMEBUF_BASE)
#define FRAMEBUF_PALETTE_SIZE    (32)
#define FRAMEBUF_PIXEL_BASE      (FRAMEBUF_BASE + FRAMEBUF_PALETTE_SIZE)

#ifdef DISPLAY_3_6
   #define LCD_WIDTH                320
   #define LCD_HEIGHT               240
#endif

#ifdef DISPLAY_4_3
   #define LCD_WIDTH                480
   #define LCD_HEIGHT               272
#endif

#ifdef DISPLAY_6_4
   #define LCD_WIDTH                640
   #define LCD_HEIGHT               480
#endif


#define LCD_HALF_HEIGHT          (LCD_HEIGHT / 2)

#define COLOR_RED                0xf800
#define COLOR_BLUE               0x001f
#define COLOR_GREEN              0x07e0

#define COLOR_WHITE              0xffff
#define COLOR_BLACK              0x0000
#define COLOR_GRAY               0x8410
#define COLOR_DARK_GRAY          0x4208

//-----------------------------------------------------------------------------
// Public Function Prototypes
//-----------------------------------------------------------------------------

uint32_t RASTER_init(void);
void RASTER_plot(uint16_t x, uint16_t y, uint16_t pixel_data);
void RASTER_clear(uint16_t color);


#endif

