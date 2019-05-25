/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  DSPLIB  DSP Signal Processing Library                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* DSP_vecsumsq.c -- Sum of Squares                                         */
/*                   Intrinsic C Implementation                             */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C callable, and has the following C prototype:      */
/*                                                                          */
/*          int DSP_vecsum_sq (                                             */
/*              const short *x,    // Pointer to vector  //                 */
/*              int          nx    // Length of vector.  //                 */
/*          );                                                              */
/*                                                                          */
/*      This routine returns the sum of squares as its return value.        */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The "vecsum_sq" function returns the sum of squares of the          */
/*      elements contained in vector "x".                                   */
/*                                                                          */
/*  C CODE                                                                  */
/*      The following C code is a general implementation without            */
/*      restrictions.  This implementation may have restrictions, as        */
/*      noted under "ASSUMPTIONS" below.                                    */
/*                                                                          */
/*          int DSP_vecsumsq (                                              */
/*              const short *x,    // Pointer to vector  //                 */
/*              int          nx    // Length of vector.  //                 */
/*          )                                                               */
/*          {                                                               */
/*              int i, sum = 0;                                             */
/*                                                                          */
/*              for (i = 0; i < nx; i++)                                    */
/*                  sum += x[i] * x[i];                                     */
/*                                                                          */
/*              return sum;                                                 */
/*          }                                                               */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The code is unrolled 4 times to enable full memory and multiplier   */
/*      bandwidth to be utilized.                                           */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The input length is a multiple of 4 and at >= 4                     */
/*                                                                          */
/*  NOTES                                                                   */
/*      This code is ENDIAN NEUTRAL                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_vecsumsq, ".text:intrinsic");

#include "DSP_vecsumsq.h"

int DSP_vecsumsq (
    const short * x,
    int  count
)
{
    int i;
    int sum = 0;

    #pragma MUST_ITERATE(4,,4);
    for (i = 0; i < count; i++)
        sum += x[i] * x[i];

    return sum;
}

/* ======================================================================== */
/*  End of file:  DSP_vecsumsq.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

