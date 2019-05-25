/* ======================================================================= */
/*  TEXAS INSTRUMENTS, INC.                                                */
/*                                                                         */
/*  DSPLIB  DSP Signal Processing Library                                  */
/*                                                                         */
/*  This library contains proprietary intellectual property of Texas       */
/*  Instruments, Inc.  The library and its source code are protected by    */
/*  various copyrights, and portions may also be protected by patents or   */
/*  other legal protections.                                               */
/*                                                                         */
/*  This software is licensed for use with Texas Instruments TMS320        */
/*  family DSPs.  This license was provided to you prior to installing     */
/*  the software.  You may review this license by consulting the file      */
/*  TI_license.PDF which accompanies the files in this library.            */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* DSP_fir_cplx_hM4X4.c -- Complex FIR Filter                              */
/*                         Optimized C Implementation (w/ Intrinsics)      */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_cplx_hM4X4 (                                           */
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
/*     nr >= 8; nr % 4 == 0                                                */
/*     nh >= 8; nh % 4 == 0                                                */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fir_cplx_hM4X4, ".text:intrinsic");

#include "DSP_fir_cplx_hM4X4.h"

void DSP_fir_cplx_hM4X4 (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
)
{
    int i, j;
    int imag0, real0;
    int imag1, real1;

    int imag_real_0;
    int imag_real_1;

    int h_10, h_01;
    int x_10, x_32;

    _nassert((int)x % 8 == 0);
    _nassert((int)h % 8 == 0);
    _nassert(nr % 4 == 0);
    _nassert(nr >= 8);
    #pragma MUST_ITERATE(4,,2)
    for (i = 0; i < 2*nr; i += 4) {
        /*----------------------------------------------------------------*/
        /* Zero out accumulators for 4 complex output samples             */
        /*----------------------------------------------------------------*/
        imag0 = real0 = imag1 = real1 = 0;

        /*----------------------------------------------------------------*/
        /* Inform compiler that filter taps is at least 8, and a multiple */
        /* of 4.                                                          */
        /*----------------------------------------------------------------*/
        _nassert((int)x % 8 == 0);
        _nassert((int)h % 8 == 0);
        _nassert((int)nh % 4 == 0);
        _nassert((int)nh >= 8);
        #pragma MUST_ITERATE(8,,4)
        for (j = 0; j < 2*nh; j += 2) {
            /*------------------------------------------------------------*/
            /* Perform word-wide loads using intrinsic, swizzle using the */
            /* packlh2 instruction to exchange the lower/upper half words */
            /*------------------------------------------------------------*/
            h_10 = _mem4_const(&h[j]);
            h_01 = _packlh2(h_10, h_10);

            /*------------------------------------------------------------*/
            /* Load input data using aligned word wide loads.             */
            /*------------------------------------------------------------*/
            x_10 = _mem4_const(&x[i - j + 0]);
            x_32 = _mem4_const(&x[i - j + 2]);

            /*------------------------------------------------------------*/
            /* Perform multiplies using complex data, filter taps and     */
            /* accumulate results using either dotp2/dotpn2 instr.        */
            /*------------------------------------------------------------*/
            real0 -= _dotpn2(x_10, h_10);
            real1 -= _dotpn2(x_32, h_10);

            imag0 += _dotp2 (x_10, h_01);
            imag1 += _dotp2 (x_32, h_01);
        }

        /*----------------------------------------------------------------*/
        /*  Shift out accumulated sum, and store as half words            */
        /*----------------------------------------------------------------*/
        imag_real_0 = _packh2(imag0 << 1, real0 << 1);
        imag_real_1 = _packh2(imag1 << 1, real1 << 1);
        _memd8(&r[i]) = _itod(imag_real_1, imag_real_0);
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fir_cplx_hM4X4.c                                     */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

