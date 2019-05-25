/* ======================================================================= */
/* DSP_neg32_cn.c -- Negate the input vector                               */
/*                   Natural C Implementation                              */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_neg32_cn, ".text:ansi");

#include "DSP_neg32_cn.h"

void DSP_neg32_cn (
    const int *x,    /* Input data array   */
    int *r,          /* Output data array  */
    int nx           /* Number of elements */
)
{
    int i;

    for(i = 0; i < nx; i++) {
        r[i] = -x[i];
    }
}

/* ======================================================================== */
/*  End of file:  DSP_neg32_cn.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

