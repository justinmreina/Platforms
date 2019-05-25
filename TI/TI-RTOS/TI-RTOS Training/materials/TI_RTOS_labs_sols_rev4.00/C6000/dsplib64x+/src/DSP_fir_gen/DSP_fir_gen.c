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
/* DSP_fir_gen.c -- FIR Filter (General)                                   */
/*                  Optimized C Implementation (w/ Intrinsics)             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fir_gen (                                                  */
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
/*     nh >= 5;                                                            */
/*     nr >= 4; nr % 4 == 0                                                */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fir_gen, ".text:intrinsic");

#include "DSP_fir_gen.h"

void DSP_fir_gen (
    const short *restrict x,    /* Input array [nr+nh-1 elements] */
    const short *restrict h,    /* Coeff array [nh elements]      */
    short       *restrict r,    /* Output array [nr elements]     */
    int nh,                     /* Number of coefficients         */
    int nr                      /* Number of output samples       */
)
{
/* *********************** */
/* Code for the C64x+ Core */
/* *********************** */
#ifdef _TMS320C6400_PLUS
    int i, j, mask;
    int h_32, h_10;
    int sum0, sum1, sum2, sum3;
    int sum_32, sum_10;

    long long x_3210, x_4321, x_7654;
    long long h_3210, h_3210_mod;
    long long r3, r2, r1, r0;

    /*---------------------------------------------------------------------*/
    /* Pad the end of the filter tap array with zeros in order make the    */
    /* array length a multiple of 4. This allows the loop to be optimized. */
    /*---------------------------------------------------------------------*/
    mask = nh & 3;
    if (!mask) mask = 4;

    h_3210 = _mem8_const(&h[nh - mask]);
    h_10 = _loll(h_3210);
    h_32 = _hill(h_3210);

    if (mask == 4) {    // Already a multiple of 4 (do nothing)
//      h_32 &= 0xFFFFFFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 3) {    // Mask out the last 16 bits (1 short)
        h_32 &= 0x0000FFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 2) {    // Mask out the last 32 bits (2 shorts)
        h_32 &= 0x00000000;
//      h_10 &= 0xFFFFFFFF;
    }
    if (mask == 1) {    // Mask out the last 48 bits (3 shorts)
        h_32 &= 0x00000000;
        h_10 &= 0x0000FFFF;
    }

    /*---------------------------------------------------------------------*/
    /* Modified taps to be used during the the filter tap loop             */
    /*---------------------------------------------------------------------*/
    h_3210_mod = _itoll(h_32, h_10);

    _nassert((int)x % 8 == 0);
    _nassert(nr % 4 == 0);
    _nassert(nr >= 4);
	for (j = 0; j < nr; j += 4) {
        sum0 = 0;
        sum1 = 0;
        sum2 = 0;
        sum3 = 0;

        _nassert((int)x % 8 == 0);
        #pragma MUST_ITERATE(1,,1)
        for (i = 0; i < nh; i += 4) {
            h_3210 = _mem8_const(&h[i]);
            x_3210 = _mem8_const(&x[i + j]);
            x_4321 = _mem8_const(&x[i + j + 1]);
            x_7654 = _mem8_const(&x[i + j + 4]);

            /*-------------------------------------------------------------*/
            /* Use modified taps during the last iteration of the loop.    */
            /*-------------------------------------------------------------*/
            if (i >= nh - 4)
                h_3210 = h_3210_mod;

            h_32 = _hill(h_3210);
            h_10 = _loll(h_3210);

            r3 = _ddotpl2(x_7654, h_32);    // x6h3+x5h2, x5h3+x4h2
            r2 = _ddotph2(x_4321, h_32);    // x4h3+x3h2, x3h3+x2h2
            r1 = _ddotph2(x_4321, h_10);    // x4h1+x3h0, x3h1+x2h0
            r0 = _ddotpl2(x_3210, h_10);    // x2h1+x1h0, x1h1+x0h0

            sum3 += _hill(r3) + _hill(r1);
            sum2 += _loll(r3) + _loll(r1);
            sum1 += _hill(r2) + _hill(r0);
            sum0 += _loll(r2) + _loll(r0);
        }

        sum_10 = _packh2(sum1 << 1, sum0 << 1);
        sum_32 = _packh2(sum3 << 1, sum2 << 1);

        _mem8(&r[j]) = _itoll(sum_32, sum_10);
    }

/* ********************** */
/* Code for the C64x Core */
/* ********************** */
#else
    int i, j, mask;
    int h_32, h_10;
    int sum0, sum1, sum2, sum3;
    int sum_32, sum_10;
    int x_10, x_32, x_54, x_76, x_21, x_43, x_65;

    long long x_3210, x_7654;
    long long h_3210, h_3210_mod;

    /*---------------------------------------------------------------------*/
    /* Pad the end of the filter tap array with zeros in order make the    */
    /* array length a multiple of 4. This allows the loop to be optimized. */
    /*---------------------------------------------------------------------*/
    mask = nh & 3;
    if (!mask) mask = 4;

    h_3210 = _mem8_const(&h[nh - mask]);
    h_10 = _loll(h_3210);
    h_32 = _hill(h_3210);

    if (mask == 4) {    // Already a multiple of 4 (do nothing)
//      h_32 &= 0xFFFFFFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    else if (mask == 3) {    // Mask out the last 16 bits (1 short)
        h_32 &= 0x0000FFFF;
//      h_10 &= 0xFFFFFFFF;
    }
    else if (mask == 2) {    // Mask out the last 32 bits (2 shorts)
        h_32 &= 0x00000000;
//      h_10 &= 0xFFFFFFFF;
    }
    else if (mask == 1) {    // Mask out the last 48 bits (3 shorts)
        h_32 &= 0x00000000;
        h_10 &= 0x0000FFFF;
    }

    /*---------------------------------------------------------------------*/
    /* Modified taps to be used during the the filter tap loop             */
    /*---------------------------------------------------------------------*/
    h_3210_mod = _itoll(h_32, h_10);

    _nassert((int)x % 8 == 0);
    _nassert(nr % 4 == 0);
    _nassert(nr >= 4);
	for (j = 0; j < nr; j += 4) {
        sum0 = 0;
        sum1 = 0;
        sum2 = 0;
        sum3 = 0;

        _nassert((int)x % 8 == 0);
        #pragma MUST_ITERATE(1,,1)
        for (i = 0; i < nh; i += 4) {
            h_3210 = _mem8_const(&h[i]);
            x_3210 = _mem8_const(&x[i + j]);
            x_7654 = _mem8_const(&x[i + j + 4]);

            /*-------------------------------------------------------------*/
            /* Use modified taps during the last iteration of the loop.    */
            /*-------------------------------------------------------------*/
            if (i >= nh - 4)
                h_3210 = h_3210_mod;

            h_32 = _hill(h_3210);
            h_10 = _loll(h_3210);

            x_10 = _loll(x_3210);
            x_32 = _hill(x_3210);
            x_54 = _loll(x_7654);
            x_76 = _hill(x_7654);

            x_21 = _packlh2(x_32,x_10);
            x_43 = _packlh2(x_54,x_32);
            x_65 = _packlh2(x_76,x_54);

            sum0 += _dotp2(x_10, h_10);
            sum0 += _dotp2(x_32, h_32);

            sum1 += _dotp2(x_21, h_10);
            sum1 += _dotp2(x_43, h_32);

            sum2 += _dotp2(x_32, h_10);
            sum2 += _dotp2(x_54, h_32);

            sum3 += _dotp2(x_43, h_10);
            sum3 += _dotp2(x_65, h_32);
        }

        sum_10 = _packh2(sum1 << 1, sum0 << 1);
        sum_32 = _packh2(sum3 << 1, sum2 << 1);

        _mem8(&r[j]) = _itoll(sum_32, sum_10);
    }

#endif
}

/* ======================================================================= */
/*  End of file:  DSP_fir_gen.c                                            */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

