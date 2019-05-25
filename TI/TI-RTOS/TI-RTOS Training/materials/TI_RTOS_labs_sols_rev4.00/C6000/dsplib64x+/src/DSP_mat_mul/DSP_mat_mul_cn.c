/* ======================================================================== */
/* DSP_mat_mul_cn.c -- Perform Matrix Multiplication                        */
/*                     Natural C Implementation                             */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          void DSP_mat_mul_cn (                                           */
/*              const short *restrict x, int r1, int c1r2,                  */
/*              const short *restrict y,         int c2,                    */
/*              short       *restrict r,                                    */
/*              int                   qs                                    */
/*          );                                                              */
/*                                                                          */
/*      x  == Pointer to r1 by c1 input matrix.                             */
/*      y  == Pointer to r2 by c2 input matrix.                             */
/*      r  == Pointer to r1 by c2 output matrix.                            */
/*                                                                          */
/*      r1   == Number of rows in x.                                        */
/*      c1r2 == Number of columns in x.  Also number of rows in y.          */
/*      c2   == Number of columns in y.                                     */
/*                                                                          */
/*      qs == Final right-shift to apply to the result.                     */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      This function computes the expression "r = x * y" for the           */
/*      matrices x and y.  The columnar dimension of x must match           */
/*      the row dimension of y.  The resulting matrix has the same          */
/*      number of rows as x and the same number of columns as y.            */
/*                                                                          */
/*      The values stored in the matrices are assumed to be fixed-point     */
/*      or integer values.  All intermediate sums are retained to 32-bit    */
/*      precision, and no overflow checking is performed.  The results      */
/*      are right-shifted by a user-specified amount, and then truncated    */
/*      to 16 bits.                                                         */
/*                                                                          */
/*      This code is suitable for dense matrices.  No optimizations are     */
/*      made for sparse matrices.                                           */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      The arrays 'x', 'y', and 'r' are stored in distinct arrays.  That   */
/*      is, in-place processing is not allowed.                             */
/*                                                                          */
/*      The input matrices have minimum dimensions of at least 1 row and    */
/*      1 column, and no more than 32767 rows or 32767 columns.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_mat_mul_cn, ".text:ansi");

#include "DSP_mat_mul_cn.h"

void DSP_mat_mul_cn (
    const short *restrict x, int r1, int c1r2,
    const short *restrict y,         int c2,
    short       *restrict r,
    int                   qs
)
{
    int i, j, k;
    int sum;

#ifndef NOASSUME
    _nassert(r1 >= 1 && r1 <= 32767);
    _nassert(c1r2 >= 1 && c1r2 <= 32767);
    _nassert(c2 >= 1 && c2 <= 32767);
#endif

    /* -------------------------------------------------------------------- */
    /*  Multiply each row in x by each column in y.  The product of row m   */
    /*  in x and column n in y is placed in position (m,n) in the result.   */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < r1; i++) {
        for (j = 0; j < c2; j++) {
            for (k = 0, sum = 0; k < c1r2; k++)
                sum += x[k + i*c1r2] * y[j + k*c2];
            r[j + i*c2] = sum >> qs;
        }
    }
}

/* ======================================================================== */
/*  End of file:  DSP_mat_mul_cn.c                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2007 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */

