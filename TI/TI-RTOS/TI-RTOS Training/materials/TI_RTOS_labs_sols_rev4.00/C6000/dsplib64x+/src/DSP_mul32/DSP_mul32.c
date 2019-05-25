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
/* DSP_mul32.c -- Intrinsic C Implementation                                */
/*                                                                          */
/* REV 0.0.1                                                                */
/*                                                                          */
/* USAGE                                                                    */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*          void DSP_mul32 (                                                */
/*              const int    *x,   // Input array of length nx              */
/*              const int    *y,   // Input array of length nx              */
/*              int *restrict r,   // Output array of length nx             */
/*              int           nx   // Number of elements.                   */
/*          );                                                              */
/*                                                                          */
/*      This routine performs a 32-bit by 32-bit multiply, returning the    */
/*      upper 32 bits of the 64-bit result.  This is equivalent to a        */
/*      Q31 x Q31 multiply, yielding a Q30 result.                          */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*      nx > = 4 and is a multiple of 4                                     */
/*      The Kernel assumes the input and output arrays are aligned to a     */
/*      8 byte booundary.                                                   */
/*                                                                          */
/* CODE SIZE (based on .map file)                                           */
/*      Code size: 64x+  : 128 bytes                                        */
/*      Code size: 64x   : 832 bytes                                        */
/*                                                                          */
/* PERFORMANCE (based on measurement)                                       */
/*      Kernel performance: 64x+ : 0.75clocks/iteration                     */
/*      Kernel performance: 64x : 1.5clocks/iteration                       */
/*                                                                          */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_mul32, ".text:intrinsic");

#include "DSP_mul32.h"

void DSP_mul32 (
    const int * restrict x,   /* Input array of length nx  */
    const int * restrict y,   /* Input array of length nx  */
          int * restrict r,   /* Output array of length nx */
          int            nx   /* Number of elements.       */
)
{
/* *********************** */
/* Code for the C64x+ Core */
/* *********************** */
#ifdef _TMS320C6400_PLUS

    int i;
    long long llres;

    _nassert(((int)x & 7) ==0);
    _nassert(((int)y & 7) ==0);
    _nassert(((int)r & 7) ==0);
    #pragma MUST_ITERATE(4,,4);
    #pragma UNROLL(4);

    for(i=0; i<nx; i++) {
        llres = _mpy32ll(x[i], y[i]);
        r[i] = _hill(llres);
    }

/* ********************** */
/* Code for the C64x Core */
/* ********************** */
#else

    int i;
    int x1, x0, x3, x2;
    int y0, y1, y2, y3;
    double xh0_xl0, xh1_xl1, xh2_xl2, xh3_xl3;
    int xlow0, xlow1, xlow2, xlow3;
    int xhigh0, xhigh1, xhigh2, xhigh3;
    int c0, c1, c2, c3;
    long long p1_p0, p3_p2, p5_p4, p7_p6;
    int xhi0, xhi1, xhi2, xhi3;
    int s0, s1, s2, s3;

    for (i = 0; i < nx >> 2; i++) {
        x0 = *x++; x1 = *x++; x2 = *x++; x3 = *x++;
        y0 = *y++; y1 = *y++; y2 = *y++; y3 = *y++;

        xh0_xl0 = _mpyhi(x0, y0);
        xlow0   = _mpyluhs(x0, y0);
        c0 = xlow0 >> 31;
        p1_p0 = _lo(xh0_xl0) + xlow0;
        xhigh0 = _hi(xh0_xl0) + _hill(p1_p0);
        xhi0 = xhigh0 + c0;
        s0 = _packlh2(xhi0, _loll(p1_p0));

        xh1_xl1 = _mpyhi(x1, y1);
        xlow1   = _mpyluhs(x1, y1);
        c1 = xlow1 >> 31;
        p3_p2 = _lo(xh1_xl1) + xlow1;
        xhigh1 = _hi(xh1_xl1) + _hill(p3_p2);
        xhi1 = xhigh1 + c1;
        s1 = _packlh2(xhi1, _loll(p3_p2));

        xh2_xl2 = _mpyhi(x2, y2);
        xlow2   = _mpyluhs(x2, y2);
        c2 = xlow2 >> 31;
        p5_p4 = _lo(xh2_xl2) + xlow2;
        xhigh2 = _hi(xh2_xl2) + _hill(p5_p4);
        xhi2 = xhigh2 + c2;
        s2 = _packlh2(xhi2, _loll(p5_p4));

        xh3_xl3 = _mpyhi(x3, y3);
        xlow3   = _mpyluhs(x3, y3);
        c3 = xlow3 >> 31;
        p7_p6 = _lo(xh3_xl3) + xlow3;
        xhigh3 = _hi(xh3_xl3) + _hill(p7_p6);
        xhi3 = xhigh3 + c3;
        s3 = _packlh2(xhi3, _loll(p7_p6));

        *((long long *) r) = _itoll(s1, s0);
        r = r + 2;
        *((long long *) r) = _itoll(s3, s2);
        r = r + 2;
    }

#endif
}

/* ======================================================================== */
/*  End of file:  DSP_mul32.c                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

