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
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*          short DSP_minval (                                              */
/*              const short *x, // Input Array                              */
/*              int nx          // Number of elements in input array        */
/*          )                                                               */
/*                                                                          */
/*      The DSP_minval routine accepts an array with 'nx' input data and    */
/*      searches for the minimum value. This minimum value is returned to   */
/*      the calling function.                                               */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*      - Assumes 16-bit input data                                         */
/*      - nx > = 8 and is a multiple of 8                                   */
/*      - The Kernel assumes the input array is aligned to a 8 byte         */
/*        booundary                                                         */
/*                                                                          */
/* CODE SIZE (based on .map file)                                           */
/*      Code size: 64x+  : 128 bytes                                        */
/*      Code size: 64x   : 256 bytes                                        */
/*                                                                          */
/* PERFORMANCE (based on measurement)                                       */
/*      Kernel performance: 64x+ : (nx/8) + 19 cycles                       */
/*                          64x  : (nx/4) + 23 cycles                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_minval, ".text:intrinsic");

#include "DSP_minval.h"

short DSP_minval (
    const short *x,     /* x[nx] = input vector    */
    int nx              /* nx = number of elements */
)
{
    int  i;

    const long long *xll;

    double x0123, x4567;
    int min01, min23, min45, min67;

    /* Set all 8 intermediate min values to most negative */
    /* Each 32bit var contains two shorts                 */
    min01 = 0x7FFF7FFF;
    min23 = 0x7FFF7FFF;
    min45 = 0x7FFF7FFF;
    min67 = 0x7FFF7FFF;

    /* Convert the short pointer to a 64bit long long pointer */
    xll = (const long long *)x;

    /* In each loop iteration we will load 8 short values from the array. */
    /* On the C64x+ we can do 4 min2 operations in one cycle. This will   */
    /* give us 8 results, that we keep seperated. Outside the loop we'll  */
    /* find the min out of these 8 intermediate values.                   */

    for (i = 0; i < nx/8; i++) {
        x0123 = _amemd8((void *)xll++);     /* Use LDDW to load 4 shorts  */
        x4567 = _amemd8((void *)xll++);     /* Use LDDW to load 4 shorts  */

        min01 = _min2( min01, _lo(x0123));
        min23 = _min2( min23, _hi(x0123));
        min45 = _min2( min45, _lo(x4567));
        min67 = _min2( min67, _hi(x4567));
    }

    min01 = _min2(min01, min23);  /* Calculate 2 minimums of min01 and min23 */
    min45 = _min2(min45, min67);  /* Calculate 2 minimums of min45 and min67 */

    min01 = _min2(min01, min45);  /* Get the 2 min values of the remaining 4 */

    min45 = _rotl(min01,16);      /* Swap lower and higher 16 bit */

    /* Find the final minimum value (will be in higher and lower part) */
    min01 = _min2(min01, min45);

    /* min01 is a 32-bit value with the result in the upper and lower 16 bit */
    /* Use an AND operation to only return the lower 16 bit to the caller    */
    return (min01 & 0xFFFF);
}

/* ======================================================================== */
/*  End of file:  DSP_minval.c                                              */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

