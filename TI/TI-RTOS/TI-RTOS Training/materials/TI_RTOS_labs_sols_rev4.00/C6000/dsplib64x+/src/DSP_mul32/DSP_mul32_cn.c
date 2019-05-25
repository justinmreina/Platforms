/* ======================================================================= */
/* DSP_mul32_cn.c -- Natural C Implementation                              */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_mul32_cn, ".text:ansi");

#include "DSP_mul32_cn.h"

void DSP_mul32_cn (
    const int * x,   /* Input array of length nx  */
    const int * y,   /* Input array of length nx  */
    int       * r,   /* Output array of length nx */
    int         nx   /* Number of elements.       */
)
{
    int  i;
    long long llres;

    for (i = 0; i < nx; i++) {
        llres = (long long)((long long)x[i] * (long long)y[i]);
        r[i] = llres >> 32;
    }
}

/* ======================================================================== */
/*  End of file:  DSP_mul32_cn.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

