/* ======================================================================= */
/* DSP_iir_cn.c -- IIR Filter                                              */
/*                 Natural C Implementation                                */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_iir_cn (                                                   */
/*         short                 Input,                                    */
/*         const short *restrict ptr_Coefs,                                */
/*         int                   nCoefs,                                   */
/*         short       *restrict ptr_State                                 */
/*     )                                                                   */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      This function implements an IIR filter, with a number of biquad    */
/*      stages given by "nCoefs" / 4.  It accepts a single sample of       */
/*      input and returns a single sample of output.  Coefficients are     */
/*      are expected to be in the range [-2.0, 2.0) with Q14 precision.    */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_iir_cn, ".text:ansi");

#include "DSP_iir_cn.h"

short DSP_iir_cn (
    short         Input,
    const short * Coefs,
    int           nCoefs,
    short       * State
)
{
    int x, p0, p1, i, j;

#ifndef NOASSUME
    _nassert((int) Coefs % 8 == 0);
    _nassert((int) State % 8 == 0);
    _nassert((int) nCoefs % 4 == 0);
    _nassert((int) nCoefs >= 4);
#endif

    x = (int) Input;
    for (i = j = 0; i < nCoefs; i += 4, j += 2) {
        p0 = Coefs[i + 2] * State[j] + Coefs[i + 3] * State[j + 1];
        p1 = Coefs[i] * State[j] + Coefs[i + 1] * State[j + 1];

        State[j + 1] = State[j];

        State[j] = x + (p0 >> 14);
        x += (p0 + p1) >> 14;
    }

    return x;
}

/* ======================================================================= */
/*  End of file:  DSP_iir_cn.c                                             */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

