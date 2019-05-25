/* ======================================================================= */
/* DSP_firlms2_cn.c -- Least Mean Square Adaptive Filter                   */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_firlms2_cn, ".text:ansi");

#include "DSP_firlms2_cn.h"

long DSP_firlms2_cn (
    short * h,         /* h[nh] = Coefficient Array   */
    const short * x,   /* x[nh] = Input Array         */
    short b,           /* b = Error from Previous FIR */
    int nh             /* nh = Number of Coefficients */
)
{
    int       i;
    long  r = 0; /* r is output of the filter */

    for (i = 0; i < nh; i++) {
        h[i] += (x[i] * b) >> 15;
        r    += x[i+1] * h[i];
    }

    return r;
}

/* ======================================================================== */
/*  End of file:  DSP_firlms2_cn.c                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

