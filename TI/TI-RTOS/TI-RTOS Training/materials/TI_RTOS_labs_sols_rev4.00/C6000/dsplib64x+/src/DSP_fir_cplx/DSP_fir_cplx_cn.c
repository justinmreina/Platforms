/* ======================================================================= */
/* DSP_fir_cplx_cn.c -- Complex FIR Filter                                 */
/*                      Natural C Implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_cplx_cn (                                              */
/*         const short *restrict x,                                        */
/*         const short *restrict h,                                        */
/*         short *restrict r,                                              */
/*         int nh,                                                         */
/*         int nr,                                                         */
/*     )                                                                   */
/*                                                                         */
/*     x[2*(nr+nh-1)] : Complex input data. x must point to x[2*(nh-1)].   */
/*     h[2*nh]        : Complex coefficients (in normal order).            */
/*     r[2*nr]        : Complex output data.                               */
/*     nh             : Number of complex coefficients.                    */
/*     nr             : Number of complex output samples.                  */
/*                                                                         */
/*  Description                                                            */
/*      This complex FIR computes nr complex output samples using nh       */
/*      complex coefficients. It operates on 16-bit data with a 32-bit     */
/*      accumulate. Each array consists of an even and odd term with even  */
/*      terms representing the real part of the element and the odd terms  */
/*      the imaginary part. The pointer to input array x must point to the */
/*      (nh)th complex sample, i.e. element 2*(nh-1), upon entry to the    */
/*      function. The coefficients are expected in normal order.           */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x, h, and r do not overlap                                   */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fir_cplx_cn, ".text:ansi");

#include "DSP_fir_cplx_cn.h"

void DSP_fir_cplx_cn (
    const short *restrict x,
    const short *restrict h,
    short       *restrict r,
    int nh,
    int nr
)
{
    int i, j;
    int imag, real;

#ifndef NOASSUME
    _nassert((int) nh % 2 == 0);
    _nassert((int) nh >= 2);
    _nassert((int) nr % 4 == 0);
    _nassert((int) nr >= 4);
#endif

    for (i = 0; i < 2*nr; i += 2) {
        imag = 0;
        real = 0;
        for (j = 0; j < 2*nh; j += 2) {
            real += h[j+0] * x[i-j+0] - h[j+1] * x[i-j+1];
            imag += h[j+1] * x[i-j+0] + h[j+0] * x[i-j+1];
        }
        r[i] = (real >> 15);
        r[i+1] = (imag >> 15);
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fir_cplx_cn.c                                        */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

