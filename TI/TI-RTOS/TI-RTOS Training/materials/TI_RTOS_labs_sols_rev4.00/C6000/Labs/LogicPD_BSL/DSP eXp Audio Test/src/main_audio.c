//-----------------------------------------------------------------------------
// \file    main.c
// \brief   implementation of main() to test bsl drivers.
//
//-----------------------------------------------------------------------------
#include "stdio.h"
#include "types.h"
#include "evmomapl138.h"
#include "evmomapl138_timer.h"
#include "evmomapl138_gpio.h"
#include "evmomapl138_i2c.h"

#include "test_audio.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
// uncomment this define if running without gel initialization.
// #define NO_GEL    (1)

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   entry point for bsl test code.
//
// \param   none.
//
// \return  none.
//-----------------------------------------------------------------------------
int main(void)
{
   uint32_t results = 0;

#if NO_GEL
   EVMOMAPL138_init();
   EVMOMAPL138_initRAM();
#endif

   // init the us timer and i2c for all to use.
   USTIMER_init();
   I2C_init(I2C0, I2C_CLK_400K);

   // TEST AUDIO
   //-----------
   results += TEST_audio();
   printf("\r\n\r\n");
   
   if (results != ERR_NO_ERROR)
      printf("\r\n\r\n********** OMAP-L138 TEST FAILED **********\r\n\r\n");
   else
      printf("\r\n\r\n********** OMAP-L138 TEST PASSED **********\r\n\r\n");
}
