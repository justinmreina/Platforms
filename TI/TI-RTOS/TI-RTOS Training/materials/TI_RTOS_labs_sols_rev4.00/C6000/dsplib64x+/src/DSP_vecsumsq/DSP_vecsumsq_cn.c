/* ======================================================================== */
/* DSP_vecsumsq_cn.c -- Sum of Squares                                      */
/*                      Natural C Implementation                            */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*          int DSP_vecsumsq_cn (                                           */
/*              const short *x,    // Pointer to vector  //                 */
/*              int nx             // Length of vector.  //                 */
/*          );                                                              */
/*                                                                          */
/*      This routine returns the sum of squares as its return value.        */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The "vecsum_sq" function returns the sum of squares of the          */
/*      elements contained in vector "x".                                   */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code is ENDIAN NEUTRAL                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_vecsumsq_cn, ".text:ansi");

#include "DSP_vecsumsq_cn.h"

int DSP_vecsumsq_cn (
    const short *x, 
    int  count
)
{
    int i;
    int prod, sum = 0;

    for(i = 0; i < count; i++) {
         prod = x[i] * x[i];
         sum += prod;
    }

    return sum;
}

/* ======================================================================== */
/*  End of file:  DSP_vecsumsq_cn.c                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

