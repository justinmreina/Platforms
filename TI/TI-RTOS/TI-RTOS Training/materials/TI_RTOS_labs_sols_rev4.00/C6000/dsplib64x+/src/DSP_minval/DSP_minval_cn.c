/* ======================================================================= */
/* DSP_minval_cn.c -- Return minimum value out of a vector                 */
/*                    Natural C Implementation                             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_minval_cn, ".text:ansi");

#include "DSP_minval_cn.h"

short DSP_minval_cn (   
    const short *x,     /* x[nx] = input vector    */ 
    int nx              /* nx = number of elements */
)
{
    int  i, min = x[0];

    for (i = 1; i < nx; i++) {
        if (x[i] < min) min = x[i];
    }
    return min;
}

/* ======================================================================== */
/*  End of file:  DSP_minval_cn.c                                           */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

