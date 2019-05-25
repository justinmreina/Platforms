/* ======================================================================= */
/* lnk.cmd -- Linker command file                                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

-c
-heap  0x1000
-stack 0x1000

/* MODULE MEM */
MEMORY {
   CACHE_L2    : origin = 0x11808000,  len = 0x8000
   CACHE_L1P   : origin = 0x11e08000,  len = 0x8000
   CACHE_L1D   : origin = 0x11f10000,  len = 0x8000
   DDR2        : origin = 0x80000000,  len = 0x8000000
   L1DSRAM     : origin = 0x11f04000,  len = 0xc000
   IRAM        : origin = 0x11800000,  len = 0x8000
}

SECTIONS
{
    vectors            :> IRAM
    .cinit             :> DDR2
    .text              :> DDR2
    .stack             :> DDR2
    .bss               :> IRAM
    .const             :> IRAM
    .data              :> IRAM
    .far               :> DDR2
    .switch            :> IRAM
    .sysmem            :> IRAM
    .tables            :> IRAM
    .cio               :> IRAM
    .fardata           :> DDR2
    
    GROUP (NEAR_DP_RELATIVE)
    {
       .neardata
       .rodata
       .bss
      }>DDR2
}

