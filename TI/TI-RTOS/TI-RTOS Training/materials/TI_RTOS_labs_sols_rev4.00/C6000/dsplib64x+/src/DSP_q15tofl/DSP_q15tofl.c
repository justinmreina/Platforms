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
/* DSP_q15tofl.c -- Float to Q15 conversion                                */
/*                  Optimized C Implementation (w/ Intrinsics)             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_q15tofl (                                                  */
/*         const short *restrict q15, // Input Q15 array                   */
/*         float *restrict flt,       // Output float array                */
/*         short nx                   // Number of elements                */
/*     );                                                                  */
/*                                                                         */
/*     x[nx]  ---  Pointer to Q15 input vector of size nx                  */
/*     r[nx]  ---  Pointer to floating-point output data vector            */
/*                 of size nx containing the floating-point equivalent     */
/*                 of vector input                                         */
/*     nx     ---  length of input and output data vectors                 */
/*                                                                         */
/*  Description                                                            */
/*     Converts the Q15 stored in vector input to IEEE floating point      */
/*     numbers stored in vector output.                                    */
/*                                                                         */
/*     void q15tofl (short *q15, float *flt, int nx)                       */
/*     {                                                                   */
/*      int i;                                                             */
/*                                                                         */
/*      for (i=0;i<nx;i++)                                                 */
/*           flt[i]=(float)q15[i]/0x8000;                                  */
/*     }                                                                   */
/*                                                                         */
/*     The above C code is a general implementation without                */
/*     restrictions.  The assembly code may have some restrictions, as     */
/*     noted below.                                                        */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays q15 and flt do not overlap                                   */
/*     nx >= 4;   nx % 4 == 0;                                             */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_q15tofl, ".text:intrinsic");

#include "DSP_q15tofl.h"

void DSP_q15tofl (
    const short *restrict q15,  /* Input Q15 array      */
    float *restrict flt,        /* Output float array   */
    short nx                    /* Number of elements   */
)
{
    int *output;
    int input;
    int shift;
    int sign;
    short i;

    output = (int *)flt;

    #pragma MUST_ITERATE(4,,4)
    for (i = 0; i < nx; i++) {
        input = (int)q15[i];                /* read Q15 to 32-bit word    */

        sign = input & 0x80000000;
        input= _abs(input);                 /* if negative, make positive */

        shift = _lmbd(1, input);            /* calculate shift count      */
        input = input << shift;             /* normalize the fraction     */

        input = _extu(input, 1, 9);         /* clear the implied MSB and shift into place */
        if(q15[i])
            shift = (143 - shift) << 23;    /* calculate offset 128 exponent */
        else
            shift = 0;

        output[i] = input | shift | sign;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_q15tofl.c                                            */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

