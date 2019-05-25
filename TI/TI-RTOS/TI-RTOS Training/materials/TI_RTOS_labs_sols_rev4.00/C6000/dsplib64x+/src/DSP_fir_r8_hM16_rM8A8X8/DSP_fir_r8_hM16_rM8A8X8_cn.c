/* ======================================================================= */
/* DSP_fir_r8_hM16_rM8A8X8_cn.c -- FIR Filter (Radix 8)                    */
/*                                 Natural C Implementation                */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_r8_hM16_rM8A8X8_cn (                                   */
/*         const short *restrict x,                                        */
/*         const short *restrict h,                                        */
/*         short *restrict r,                                              */
/*         int nh,                                                         */
/*         int nr,                                                         */
/*     )                                                                   */
/*                                                                         */
/*  Description                                                            */
/*     Computes a real FIR filter (direct-form) using coefficients         */
/*     stored in vector h.  The real data input is stored in vector x.     */
/*     The filter output result is stored in vector r.  Input data and     */
/*     filter taps are 16-bit, with intermediate values kept at 32-bit     */
/*     precision.  Filter taps are expected in Q15 format.                 */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x, h, and r do not overlap                                   */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fir_r8_hM16_rM8A8X8_cn, ".text:ansi");

#include "DSP_fir_r8_hM16_rM8A8X8_cn.h"

void DSP_fir_r8_hM16_rM8A8X8_cn (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
)
{
    int i, j, sum;

#ifndef NOASSUME
    _nassert((int)x % 8 == 0);
    _nassert((int)h % 8 == 0);
    _nassert(nr >= 4);
    _nassert(nh >= 8);
#endif

    #pragma UNROLL(4);
    for (j = 0; j < nr; j++) {
        sum = 0;
        #pragma UNROLL(8);
        for (i = 0; i < nh; i++)
            sum += x[i + j] * h[i];
        r[j] = sum >> 15;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fir_r8_hM16_rM8A8X8_cn.c                             */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

