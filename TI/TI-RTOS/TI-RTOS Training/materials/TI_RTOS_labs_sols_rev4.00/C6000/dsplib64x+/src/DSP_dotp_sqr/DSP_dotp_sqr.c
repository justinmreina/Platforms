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
/* DSP_dotp_sqr.c --  Vector Dot Product and Square implementation          */
/*                    Intrinsic C Implementation                            */
/*                                                                          */
/* REV 0.0.1                                                                */
/*                                                                          */
/* USAGE                                                                    */
/*                                                                          */
/*      This routine has the following C prototype:                         */
/*                                                                          */
/*    int DSP_dotp_sqr                                                      */
/*    (                                                                     */
/*        int G,            // Sum-of-y-squared initial value               */
/*        const short  *x,  // First input array                            */
/*        const short  *y,  // Second input array.                          */ 
/*        int *restrict d,  // Accumulation of x[] * y[].                   */
/*        int nx            // Length of the input vector.                  */
/*    )                                                                     */
/*                                                                          */
/*      This routine computes the dot product of x[] and y[] arrays,        */
/*      adding it to the value in the location pointed to by 'd'.           */
/*                                                                          */
/*      Additionally, it computes the sum of the squares of the terms       */
/*      in the y[] array, adding it to the argument G.  The final value     */
/*      of G is given as the return value of the function.  This value      */
/*      is used by the VSELP vocoder.                                       */
/*                                                                          */
/* ASSUMPTIONS                                                              */
/*      - The input count is a multiple of 4                                */
/*                                                                          */
/* PERFORMANCE/CODE SIZE                                                    */
/*      Code size: 64x+  :   64                                             */
/*      Code size: 64x   :   256                                            */
/*                                                                          */
/*      Kernel ii: 64x+ and 64x  :  0.5 clock/iterations                    */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_dotp_sqr, ".text:intrinsic");


int DSP_dotp_sqr
(
    int G,
    const short * restrict x,
    const short * restrict y,
    int *restrict d,
    int nx
)
{
    int i;
    int sum = *d;

    /* The kernel assumes that the input count is multiple of 4 */
    for (i = 0; i < nx; i += 4) {
        sum += _dotp2(_lo(_memd8_const(&x[i])),  _lo(_memd8_const(&y[i]))) +
               _dotp2(_hi(_memd8_const(&x[i])),  _hi(_memd8_const(&y[i])));

        G   += _dotp2(_lo(_memd8_const(&y[i])),  _lo(_memd8_const(&y[i]))) +
               _dotp2(_hi(_memd8_const(&y[i])),  _hi(_memd8_const(&y[i])));
    }
    *d = sum;
    return (G);
}

/* ======================================================================== */
/*  End of file:  DSP_dotp_sqr.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

