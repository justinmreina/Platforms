/* ======================================================================= */
/* DSP_fir_sym_cn.c -- Symmetric FIR Filter                                */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_sym_cn (                                               */
/*         const short *restrict x,                                        */
/*         const short *restrict h,                                        */
/*         short *restrict r,                                              */
/*         int nh,                                                         */
/*         int nr,                                                         */
/*         int s,                                                          */
/*     )                                                                   */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*     This function applies a symmetric filter to the input samples.      */
/*     The filter tap array h[] provides 'nh + 1' total filter taps.       */
/*     The filter tap at h[nh] forms the center point of the filter.       */
/*     The taps at h[nh - 1] through h[0] form a symmetric filter          */
/*     about this central tap.  The effective filter length is thus        */
/*     2*nh + 1 taps.                                                      */
/*                                                                         */
/*     The filter is performed on 16-bit data with 16-bit coefficients,    */
/*     accumulating intermediate results to 40-bit precision.  The         */
/*     accumulator is rounded and truncated according to the value         */
/*     provided in 's'.  This allows a variety of Q-points to be used.     */
/*                                                                         */
/*     Note that samples are added together before multiplication, and     */
/*     so overflow *may* result for large-scale values, despite the        */
/*     40-bit accumulation.                                                */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fir_sym_cn, ".text:ansi");

#include "DSP_fir_sym_cn.h"

void DSP_fir_sym_cn (
    const short *restrict x,    /* Input samples                   */
    const short *restrict h,    /* Filter taps                     */
    short       *restrict r,    /* Output samples                  */
    int nh,                     /* Number of symmetric filter taps */
    int nr,                     /* Number of output samples        */
    int s                       /* Final output shift              */
)
{
    int i, j;
    long y0, round;

    round = (long) 1 << (s - 1);

    #ifndef NOASSUME
    _nassert((int)x % 8 == 0);
    _nassert((int)h % 8 == 0);
    _nassert(nh >= 8);
    _nassert((int)nh % 8 == 0);
    #endif

    #pragma UNROLL(2);
    for (j = 0; j < nr; j++) {
        y0 = round;
        #pragma UNROLL(4);
        for (i = 0; i < nh; i++)
            y0 += ((short) (x[j + i] + x[2*nh + j - i])) * h[i];

        y0 += x[j + nh] * h[nh];

        r[j] = (int)(y0 >> s);;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fir_sym_cn.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

