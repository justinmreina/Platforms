/* ======================================================================= */
/* DSP_q15tofl_cn.c -- Float to Q15 conversion                             */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_q15tofl_cn (                                               */
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
/*     void q15tofl(short *q15, float *flt, int nx)                        */
/*     {                                                                   */
/*      int i;                                                             */
/*                                                                         */
/*      for (i=0;i<nx;i++)                                                 */
/*           flt[i]=(float)q15[i]/0x8000;                                  */
/*     }                                                                   */
/*                                                                         */
/*     The above C code is a general implementation without restrictions.  */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays q15 and flt do not overlap                                   */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_q15tofl_cn, ".text:ansi");

#include "DSP_q15tofl_cn.h"

void DSP_q15tofl_cn (
    const short *restrict q15,  /* Input Q15 array      */
    float *restrict flt,        /* Output float array   */
    short nx                    /* Number of elements   */
)
{
    int i;

    for (i = 0; i < nx; i++)
        flt[i] = (float)q15[i] / 32768;
}

/* ======================================================================= */
/*  End of file:  DSP_q15tofl_cn.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

