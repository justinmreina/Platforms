/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  DSPLIB  DSP Signal Processing Library                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* DSP_minval.c -- Return minimum value out of a vector                     */
/*                 Intrinsic C Implementation                               */
/*                                                                          */
/* REV 0.0.1                                                                */
/*                                                                          */
/* USAGE                                                                    */
/*      This C routine may be called using the following prototype:         */
/*                                                                          */
/*          void DSP_neg32 (                                                */
/*              int * restrict x,    // Input data array                    */
/*              int * restrict r,    // Output data array                   */
/*              int            nx    // Number of elements.                 */
/*          );                                                              */
/*                                                                          */
/*      This routine negates an array of 32-bit integers, and stores        */
/*      the result either in a new array, or back to the same array.        */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*      - The input must be at least 4 elements long                        */
/*      - The input must be a multiple of 4.                                */
/*      - The input and output arrays are double word aligned.              */
/*                                                                          */
/* CODE SIZE (based on .map file)                                           */
/*      Code size: 64x+  :  96                                              */
/*      Code size: 64x   :  256                                             */
/*                                                                          */
/* PERFORMANCE (based on measurement)                                       */
/*      Kernel performance: 64x+ :  0.5clocks/iteration                     */
/*                          64x  :  0.5clocks/iteration                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_neg32, ".text:intrinsic");

#include "DSP_neg32.h"

void DSP_neg32 (
    const int * restrict x,    /* Input data array   */
          int * restrict r,    /* Output data array  */
          int            nx    /* Number of elements */
)
{
    int i;

    _nassert(((int)x & 7) == 0);  /* Tell compiler that x is 64bit aligned */
    _nassert(((int)r & 7) == 0);  /* Tell compiler that x is 64bit aligned */
    #pragma MUST_ITERATE(4,,4);   /* LOop iteration is multiple of 4 */

    for(i = 0; i < nx; i++) {
        r[i] = -x[i];
    }
}

/* ======================================================================== */
/*  End of file:  DSP_neg32.c                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

