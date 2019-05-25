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
/* DSP_firlms2.c -- Least Mean Square Adaptive Filter                       */
/*                  Intrinsic C Implementation                              */
/*                                                                          */
/* REV 0.0.1                                                                */
/*                                                                          */
/* USAGE                                                                    */
/*                                                                          */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*          long DSP_firlms2 (                                              */
/*              short       *restrict h,  // Filter Coefficients            */
/*              short       *restrict x,  // Input Data                     */
/*              short                 b,  // Error from previous FIR        */
/*              int                   nh, // Number of Coefficients         */
/*          )                                                               */
/*                                                                          */
/*      The DSP_firlms2 routine accepts a list of 'nh' input data and 'nh'  */
/*      adaptive filter coefficients and updates the coefficients by        */
/*      adding weighted error times the inputs to the original              */
/*      coefficients. This assumes single sample input followed by the      */
/*      last nh-1 inputs and nh coefficients.                               */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*                                                                          */
/*      - Assumes 16-bit input data, error and filter coefficients.         */
/*      - nh > = 4 and is a multiple of 4                                   */
/*      - The Kernel assumes the filter coefficients are aligned to 8byte   */
/*        booundary                                                         */
/*      - The Kernel doesn't assumes any alignment for input data           */
/*      - The Kernel doesn't use any extra memory padding                   */
/*                                                                          */
/* PERFORMANCE/CODE SIZE                                                    */
/*      Code size: 64x+  : 192 bytes                                        */
/*      Code size: 64x   : 704 bytes                                        */
/*      Code size: 64x with -mh56 : 416 bytes                               */
/*                                                                          */
/*      Kernel ii: 64x+ and 64x  : 3 clks/4 nh                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_firlms2, ".text:intrinsic");

#include "DSP_firlms2.h"

long DSP_firlms2 (
    short * restrict hadd,       /* h[nh] = Coefficient Array   */
    short * restrict inadd_1,    /* x[nh] = Input Array         */
    short err,                   /* b = Error from Previous FIR */
    int nh                       /* nh = Number of Coefficients */
)
{
    short * restrict inadd_2;
    int err_1;
    int   i;
    long long r;
    int r1_, r2_;
    long long h_3210;
    int h3h2, h1h0;
    long long x_3210, x_4321;
    double xb32d, xb10d;
    int xb32h, xb32l, xb10h, xb10l, xb32, xb10;
    short * restrict hadd2;

    /*
        Make a copy of the coefficient array. hadd ptr is used for reading
        data from the coefficient array. The hadd2 pointer is used to
        update the data in the hadd2 coefficint array. This way compiler
        doesn't generate a longer dependency bound
    */
    hadd2 = hadd;

    /*
        Copy the error value to upper 16bits as well. err_1 has err value in
        both lower 16 and upper 16 bits. This is required so that mpy2
        instruction can be used to do two 16bit multiplies. One multiply uses
        the lower 16bit err and the other uses upper 16bit err value.
    */
    err_1 = _pack2 (err, err);

    /*
        Create new data pointer for input array. The new pointer is shifted by
        1 byte in memory.
    */
    inadd_2 = inadd_1 + 1;

    /* Clear the result to start with */
    r = 0;

    /* The kernel assumes nh is multiple of 4 */
    for (i = 0; i < nh >> 2; i++) {
        /*
            Kernel assumes coefficient array is aligned to 8 byte boundary.
            Do an aligned load for 64 bit data
        */
        h_3210 = _amem8(hadd);
        hadd = hadd + 4;

        /*
            Kernel *doesn't* assumes any memory alignemnt for input data.
            Load non aligned 64 bit data
        */
        x_3210 = _mem8(inadd_1);
        inadd_1 = inadd_1 + 4;

        /*
            inadd_2 is one byte shifted from inadd_1. Therefore the bytes
            loaded are one shifted from the bytes loaded from inadd_1
        */
        x_4321 = _mem8(inadd_2);
        inadd_2 = inadd_2 + 4;

        /*
            Perform smpy2. This intrinsic performs two 16bit signed multiplies
            in a single instruction. The result is generated as a 64bit value.
            The upper 32bits have result for first multiply and the lower 32
            bits have the result for the second multiply
        */
        xb32d = _smpy2 (err_1, _hill(x_3210));
        xb10d = _smpy2 (err_1, _loll(x_3210));

        /* Seperate the 32bit values */
        xb32h = _hi(xb32d);
        xb32l = _lo(xb32d);

        xb10h = _hi(xb10d);
        xb10l = _lo(xb10d);

        /*
            For each 32bit result, extract the upper 16bits to get the
            final result. Pack two 16bit results in 32bit variable.
        */
        xb32 = _packh2(xb32h, xb32l);
        xb10 = _packh2(xb10h, xb10l);

        /* Add the resulting values to h[i] to get the updated h[i] */
        h3h2 = _add2(xb32, _hill(h_3210));
        h1h0 = _add2(xb10, _loll(h_3210));

        /* Store the updated h[i]. Use 64bit store instruction */
        h_3210 = _itoll(h3h2, h1h0);
        *((long long *) hadd2) = h_3210;
        hadd2 = hadd2 + 4;

        /*
            Calculate the result r:
            r += x[i+1] * h[i];
            The dotp2 instruction performs 2 16bit multiplies. The multiplies
            are performed b/w the upper 16bits and the lower 16bits of the two
            integers. The result of both the multiplies is added together to
            provide the final result.
        */
        r1_ = _dotp2(_loll(x_4321), h1h0);
        r2_ = _dotp2(_hill(x_4321), h3h2);

        r = r1_ + r2_ + r;
    }

    /* Return the result */
    return r;
}

/* ======================================================================== */
/*  End of file:  DSP_firlms2.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

