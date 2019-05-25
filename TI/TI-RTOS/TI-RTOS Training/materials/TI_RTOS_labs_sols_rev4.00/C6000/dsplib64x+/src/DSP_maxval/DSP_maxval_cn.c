/* ======================================================================= */
/* DSP_maxval_cn.c -- Return maximum value out of a vector                 */
/*                    Natural C Implementation                             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_maxval_cn, ".text:ansi");

#include "DSP_maxval_cn.h"

short DSP_maxval_cn (
    const short *x,     /* x[nx] = input vector    */
    int nx              /* nx = number of elements */
)
{
    int  i, max = x[0];

    for (i = 1; i < nx; i++) {
        if (x[i] > max) max = x[i];
    }
    return max;
}

/* ======================================================================== */
/*  End of file:  DSP_maxval_cn.c                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

