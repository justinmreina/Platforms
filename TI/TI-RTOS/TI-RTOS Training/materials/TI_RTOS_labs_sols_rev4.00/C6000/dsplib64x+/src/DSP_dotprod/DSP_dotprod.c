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
/* DSP_dotprod.c -- Dot Product implementation                              */
/*                  Intrinsic C Implementation                              */
/*                                                                          */
/* REV 0.0.1                                                                */
/*                                                                          */
/* USAGE                                                                    */
/*                                                                          */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*      int DSP_dotprod                                                     */
/*      (                                                                   */
/*          const short *m,    // Pointer to first vector                   */
/*          const short *n,    // Pointer to second vector                  */
/*          int          count // Length of vectors.                        */
/*      )                                                                   */
/*                                                                          */
/*      The "DSP_dotprod" function implements a dot product of two input    */
/*      vectors, returning the scalar result.  Each element of the          */
/*      first array is multiplied with the corresponding element of the     */
/*      second array, and the products are summed.  The sum is returned.    */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*                                                                          */
/*      - The input count is a multiple of 4                                */
/*      - Both the input pointers are double word aligned                   */
/*                                                                          */
/* PERFORMANCE/CODE SIZE                                                    */
/*      Code size: 64x+  : 64bytes                                          */
/*      Code size: 64x   : 256bytes                                         */
/*                                                                          */
/*      Kernel ii: 64x+ and 64x  :  1 clock/4 iterations                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_dotprod, ".text:intrinsic");

int DSP_dotprod (
    short * restrict m,
    short * restrict n,
    int count
)
{
    int i;
    int sum1 = 0;
    int sum2 = 0;

    /* The kernel assumes that the data pointers are double word aligned */
    _nassert((int)m % 8 == 0);
    _nassert((int)n % 8 == 0);

    /* The kernel assumes that the input count is multiple of 4 */
    for (i = 0; i < count; i+=4) {
        sum1 += _dotp2(_lo(_amemd8_const(&m[i])),  _lo(_amemd8_const(&n[i])));
        sum2 += _dotp2(_hi(_amemd8_const(&m[i])),  _hi(_amemd8_const(&n[i])));
    }

    return (sum1+sum2);
}

/* ======================================================================== */
/*  End of file:  DSP_dotprod.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

