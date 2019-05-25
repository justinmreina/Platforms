/* ======================================================================= */
/* DSP_minerror_cn.c -- Minimum Energy Error Search                        */
/*                      Natural C Implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_minerror_cn (                                              */
/*         const short *restrict GSP0_TABLE,                               */
/*         const short *restrict errCoefs,                                 */
/*         int         *restrict max_index                                 */
/*     )                                                                   */
/*                                                                         */
/*     GSP0_TABLE[256*9] :  Pointer to GSP0 terms array.                   */
/*                          Must be double-word aligned.                   */
/*     errCoefs[9]       :  Array of error coefficients.                   */
/*     max_index         :  Index to GSP0_TABLE[max_index], the first      */
/*                          element of the 9-element vector that resulted  */
/*                          in the maximum dot product.                    */
/*     return int        :  Maximum dot product result.                    */
/*                                                                         */
/*  Description                                                            */
/*      Performs a dot product on 256 pairs of 9 element vectors and       */
/*      searches for the pair of vectors which produces the maximum dot    */
/*      product result and returns the value of the highest dot product.   */
/*      This is a large part of the VSELP vocoder codebook search.         */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays GSP0_TABLE, errCoefs, and max_index do not overlap           */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_minerror_cn, ".text:ansi");

#include "DSP_minerror_cn.h"

#define GSP0_TERMS 9
#define GSP0_NUM 256

int DSP_minerror_cn (
    const short *restrict GSP0_TABLE, /* Pointer to GSP0 terms array          */
    const short *restrict errCoefs,   /* Array of error coefficients          */
    int *restrict max_index           /* Index to the 9-element vector that   */
                                      /*  resulted in the maximum dot product */
)
{
    int i, j;
    int val;
    int maxVal = -32767;    /* Min val for ints */

#ifndef NOASSUME
    _nassert((int) GSP0_TABLE % 8 == 0);
#endif

    for (i = 0; i < GSP0_NUM; i++) {
        for (val = 0, j = 0; j < GSP0_TERMS; j++)
            val += GSP0_TABLE[i*GSP0_TERMS + j] * errCoefs[j];

        if (val > maxVal) {
            maxVal = val;
            *max_index = i * GSP0_TERMS;
        }
    }

    return(maxVal);
}

/* ======================================================================= */
/*  End of file:  DSP_minerror_cn.c                                        */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

