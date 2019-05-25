//-----------------------------------------------------------------------------
// \file    evmomapl138_raster.c
// \brief   implementation of the raster LCD driver for the OMAP-L138 EVM.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_lcdc.h"
#include "evmomapl138_lcd_raster.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
#define PINMUX_LCD_D_BUS_0_REG      16
#define PINMUX_LCD_D_BUS_1_REG      17
#define PINMUX_LCD_D_BUS_2_REG      18
#define PINMUX_LCD_CONTROL_0_REG    18
#define PINMUX_LCD_CONTROL_1_REG    19

#define PINMUX_LCD_D_BUS_0_MASK     (0xFFFFFF00)
#define PINMUX_LCD_D_BUS_1_MASK     (0xFFFFFFFF)
#define PINMUX_LCD_D_BUS_2_MASK     (0x000000FF)
#define PINMUX_LCD_CONTROL_0_MASK   (0xFF000000)
#define PINMUX_LCD_CONTROL_1_MASK   (0x000000FF)

#define PINMUX_LCD_D_BUS_0_VALUE    (0x22222200)
#define PINMUX_LCD_D_BUS_1_VALUE    (0x22222222)
#define PINMUX_LCD_D_BUS_2_VALUE    (0x00000022)
#define PINMUX_LCD_CONTROL_0_VALUE  (0x22000000)
#define PINMUX_LCD_CONTROL_1_VALUE  (0x02000022)

#define PINMUX_LCD_PWR_REG          (5)
#define PINMUX_LCD_PWR_MASK         (0xF0000000)
#define PINMUX_LCD_PWR_VALUE        (0x80000000)
#define PINMUX_BACKLIGHT_PWR_REG    (5)
#define PINMUX_BACKLIGHT_PWR_MASK   (0x0000000F)
#define PINMUX_BACKLIGHT_PWR_VALUE  (0x00000008)
#define PINMUX_BACKLIGHT_PWM_REG    (1)
#define PINMUX_BACKLIGHT_PWM_MASK   (0x0000000F)
#define PINMUX_BACKLIGHT_PWM_VALUE  (0x00000008)

#define LCD_PWR_GPIO_BANK           (2)
#define LCD_PWR_GPIO_PIN            (8)
#define BACKLIGHT_PWR_GPIO_BANK     (2)
#define BACKLIGHT_PWR_GPIO_PIN      (15)
#define BACKLIGHT_PWM_GPIO_BANK     (0)
#define BACKLIGHT_PWM_GPIO_PIN      (7)

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t raster_config(void);
static uint32_t enable_lcdc(void);
static uint32_t disable_lcdc(void);
static uint32_t disable_lcd_power(void);
static uint32_t disable_lcd_backlight(void);
static uint32_t enable_lcd_power(void);
static uint32_t enable_lcd_backlight(void);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the lcd controller for raster use.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...lcdc ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t RASTER_init(void)
{
   uint32_t rtn = ERR_NO_ERROR;

   /* Set MUX_MODE on UI GPIO expander to LCD */
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_0_REG, PINMUX_LCD_D_BUS_0_MASK, PINMUX_LCD_D_BUS_0_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_1_REG, PINMUX_LCD_D_BUS_1_MASK, PINMUX_LCD_D_BUS_1_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_D_BUS_2_REG, PINMUX_LCD_D_BUS_2_MASK, PINMUX_LCD_D_BUS_2_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_CONTROL_0_REG, PINMUX_LCD_CONTROL_0_MASK, PINMUX_LCD_CONTROL_0_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_CONTROL_1_REG, PINMUX_LCD_CONTROL_1_MASK, PINMUX_LCD_CONTROL_1_VALUE);

   // configure the gpio pins used for lcd power, backlight power and pwm.
   EVMOMAPL138_pinmuxConfig(PINMUX_LCD_PWR_REG, PINMUX_LCD_PWR_MASK, PINMUX_LCD_PWR_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_BACKLIGHT_PWR_REG, PINMUX_BACKLIGHT_PWR_MASK, PINMUX_BACKLIGHT_PWR_VALUE);
   EVMOMAPL138_pinmuxConfig(PINMUX_BACKLIGHT_PWM_REG, PINMUX_BACKLIGHT_PWM_MASK, PINMUX_BACKLIGHT_PWM_VALUE);
   GPIO_setDir(LCD_PWR_GPIO_BANK, LCD_PWR_GPIO_PIN, GPIO_OUTPUT);
   GPIO_setDir(BACKLIGHT_PWR_GPIO_BANK, BACKLIGHT_PWR_GPIO_PIN, GPIO_OUTPUT);
   GPIO_setDir(BACKLIGHT_PWM_GPIO_BANK, BACKLIGHT_PWM_GPIO_PIN, GPIO_OUTPUT);
   
   // initially set the backlight pwm high.
   GPIO_setOutput(BACKLIGHT_PWM_GPIO_BANK, BACKLIGHT_PWM_GPIO_PIN, OUTPUT_HIGH);

   // enable power and setup lcdc.
   disable_lcdc();
   enable_lcdc();
   raster_config();

   // power down the display kit.
   disable_lcd_backlight();
   disable_lcd_power();
   USTIMER_delay(300000);

   // power-up the display kit following the correct sequence.
   enable_lcd_power();
   USTIMER_delay(300000);
   enable_lcd_backlight();

   return (rtn);
}

//-----------------------------------------------------------------------------
// \brief   plot an x, y pixel on the graphics LCD.
//
// \param   uint16_t x - .
//
// \param   uint16_t y - .
//
// \param   uint16_t pixel_data - .
//
// \return  none.
//-----------------------------------------------------------------------------
void RASTER_plot(uint16_t x, uint16_t y, uint16_t pixel_data)
{
   *((uint16_t *)(FRAMEBUF_PIXEL_BASE + y * (LCD_WIDTH * 2) + (x << 1))) = pixel_data;
}

//-----------------------------------------------------------------------------
// \brief   clear the graphics LCD to the input color.
//
// \param   uint16_t color - .
//
// \return  none.
//-----------------------------------------------------------------------------
void RASTER_clear(uint16_t color)
{
   uint32_t x, y;

   // Paint the background
   for (x = 0; x < LCD_WIDTH; x++)
      for (y = 0; y < LCD_HEIGHT; y++)
         RASTER_plot(x, y, color);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t raster_config( void )
{
   uint32_t x;
   uint16_t *pdata;

   /*
   *  LCD Panel
   */
   LCDC->RASTER_CTRL       &= 0xfffffffe;         // Turn raster controller off

   LCDC->LCD_STAT           = 0x000003ff;         // Clear status bits

#ifdef DISPLAY_3_6
    // 18.75 MHz, raster mode pixel clock, 320 x 240 viewable frame
   LCDC->LCD_CTRL           = 0x00001101;    // PCLK = 150MHz / 8 = 8.824 MHz, raster mode
   LCDC->RASTER_TIMING_0    = (0x14 << 24)
                           | (0x14 << 16)
                           | (0x14 << 10)
                           | (0x13 << 4);    // HBP = 0x14; HFP = 0x14; HSW = 0x14; PPL = 0x13
   LCDC->RASTER_TIMING_1    = (0x03 << 24)
                           | (0x03 << 16)
                           | (0x02 << 10)
                           | (0xEF);         // VBP = 0x3; VFP = 0x3; VSW = 0x2: LPP = 0xEF;
   LCDC->RASTER_TIMING_2    = 0x02700000;
#endif



#ifdef DISPLAY_4_3
   // 8.824 MHz, raster mode pixel clock, 480 x 272 viewable frame
   LCDC->LCD_CTRL           = 0x00001101;    // PCLK = 150MHz / 17 = 8.824 MHz, raster mode
   LCDC->RASTER_TIMING_0    = (0x01 << 24)
                           | (0x02 <<16)
                           | (0x29 << 10)
                           | (0x1D << 4);    // HBP = 0x01; HFP = 0x02; HSW = 0x29; PPL = 0x1D
   LCDC->RASTER_TIMING_1    = (0x03 << 24)
                           | (0x02 << 16)
                           | (0x0A << 10)
                           | (0x10F);        // VBP = 0x3; VFP = 0x2; VSW = 0xA: LPP = 0x10F;
   LCDC->RASTER_TIMING_2    = 0x02700000;
#endif

#ifdef DISPLAY_6_4
   // 18.75 MHz, raster mode pixel clock, 640 x 480 viewable frame
   LCDC->LCD_CTRL           = 0x00000801;    // PCLK = 150MHz / 8 = 18.75 MHz, raster mode
   LCDC->RASTER_TIMING_0    = (0x19 << 24)
                           | (0x89 << 16)
                           | (0x31 << 10)
                           | (0x27 << 4);    // HBP = 0x19; HFP = 0x89; HSW = 0x31; PPL = 0x27
   LCDC->RASTER_TIMING_1    = (0x1F << 24)
                           | (0x20 <<16)
                           | (0x02 <<10)
                           | (0x1DF);        // VBP = 0x1F; VFP = 0x20; VSW = 0x2: LPP = 0x1DF;
   LCDC->RASTER_TIMING_2    = 0x02700000;
#endif

   LCDC->LCDDMA_CTRL        = 0x00000020;
   LCDC->LCDDMA_FB0_BASE    = FRAMEBUF_BASE;     // Frame buffer start
   LCDC->LCDDMA_FB0_CEILING = FRAMEBUF_PIXEL_BASE + (LCD_WIDTH * LCD_HEIGHT * 2) - 4; // Frame buffer end
   LCDC->RASTER_CTRL        = 0x01000080;         // 6-5-6 mode,
   LCDC->RASTER_CTRL       |= 0x00000001;         // Enable controller

   /* Palette */
   pdata = (uint16_t *)FRAMEBUF_PALETTE_BASE;
   *pdata++ = 0x4000;
   for (x = 0; x < 15; x++)
   {
      *pdata++ = 0x0000;
   }

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Enable the LCD Hardware.
//-----------------------------------------------------------------------------
uint32_t enable_lcdc( void )
{
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_LCDC, PSC_ENABLE);

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// Disable the LCD Hardware.
//-----------------------------------------------------------------------------
uint32_t disable_lcdc( void )
{
   EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_LCDC, PSC_DISABLE);

   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// clear the gpio pin that controls the lcd power.
//-----------------------------------------------------------------------------
uint32_t disable_lcd_power( void )
{
   return (GPIO_setOutput(LCD_PWR_GPIO_BANK, LCD_PWR_GPIO_PIN, OUTPUT_LOW));
}

//-----------------------------------------------------------------------------
// clear the gpio pin that controls the backlight power.
//-----------------------------------------------------------------------------
uint32_t disable_lcd_backlight( void )
{
   return (GPIO_setOutput(BACKLIGHT_PWR_GPIO_BANK, BACKLIGHT_PWR_GPIO_PIN, OUTPUT_LOW));
}

//-----------------------------------------------------------------------------
// set the gpio pin that controls the lcd power.
//-----------------------------------------------------------------------------
uint32_t enable_lcd_power( void )
{
   uint32_t i;

   GPIO_setOutput(LCD_PWR_GPIO_BANK, LCD_PWR_GPIO_PIN, OUTPUT_HIGH);

   /* For 4.3" display, we need a few ms before we turn on backlight, or */
   /* the display panel logic goes out to lunch.                         */
   for (i = 0; i < 300; i++)
   {
      USTIMER_delay(1000);
   }

   return(ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// set the gpio pin that controls the backlight power.
//-----------------------------------------------------------------------------
uint32_t enable_lcd_backlight( void )
{
   return (GPIO_setOutput(BACKLIGHT_PWR_GPIO_BANK, BACKLIGHT_PWR_GPIO_PIN, OUTPUT_HIGH));
}
