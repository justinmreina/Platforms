/* ======================================================================== */
/* DSP_w_vec_cn.c -- Weighted Vector Sum                                    */
/*                   Natural C Implementation                               */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          void DSP_w_vec_cn (                                             */
/*              const short *x,     // Vector being weighted  //            */
/*              const short *y,     // Non-weighted vector    //            */
/*              short m,            // weighting factor       //            */
/*              short *restrict r,  // Result vector          //            */
/*              int nr              // Number of samples.     //            */
/*          );                                                              */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The benchmark performs a weighted vector sum.  The elements         */
/*      of the x[] vector are weighted according to the Q15 value 'm',      */
/*      and then added to the corresponding elements of the y[] vector.     */
/*      The results are written to the r[] vector.                          */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      m is not -32768.                                                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_w_vec_cn, ".text:ansi");

#include "DSP_w_vec_cn.h"

void DSP_w_vec_cn (
    const short *x,     /* Vector being weighted  */
    const short *y,     /* Non-weighted vector    */
    short m,            /* weighting factor       */
    short *r,           /* Result vector          */
    int nr              /* Number of samples.     */
)
{
    int i;

#ifndef NOASSUME
    _nassert(m != -32768);
    _nassert((int)x % 8 == 0);
    _nassert((int)y % 8 == 0);
    _nassert((int)r % 8 == 0);
    #pragma MUST_ITERATE(16, ,8);
#endif

    for (i = 0; i < nr; i++) 
        r[i] = ((m * x[i]) >> 15) + y[i];
}

/* ======================================================================== */
/*  End of file:  DSP_w_vec_cn.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

