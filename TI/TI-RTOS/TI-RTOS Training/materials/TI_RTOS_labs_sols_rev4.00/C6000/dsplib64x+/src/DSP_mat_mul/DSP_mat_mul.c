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
/* DSP_mat_mul.c -- Perform Matrix Multiplication                           */
/*                  Intrinsic C Implementation                              */
/*                                                                          */
/* Rev 0.0.1                                                                */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is C-callable and can be called as:                    */
/*                                                                          */
/*          void DSP_mat_mul (                                              */
/*              const short *restrict x, int r1, int c1r2,                  */
/*              const short *restrict y,         int c2,                    */
/*              short       *restrict r,                                    */
/*              int                   qs                                    */
/*          );                                                              */
/*                                                                          */
/*      x  == Pointer to r1 by c1 input matrix.                             */
/*      y  == Pointer to c1 by c2 input matrix.                             */
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
/*  Requirements:                                                           */
/*  R1   min = 8, Factor 2                                                  */
/*  c1r2 min = 8, Factor 2                                                  */
/*  C2   min = 4, Factor 4                                                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_mat_mul, ".text:intrinsic");

#include "DSP_mat_mul.h"

void DSP_mat_mul (
    const short *restrict x, int r1, int c1r2,
    const short *restrict y,         int c2,
    short       *restrict r,
    int                   qs
)
{
    int i, j, k;
    int x_2x_1, x_2x_2, y_2x_packed_1, y_2x_packed_2, y_2x_packed_3, y_2x_packed_4;
    int sum_1, sum_2, sum_3, sum_4, sum_5, sum_6, sum_7, sum_8;
    long long y_2x_1, y_2x_2;

    _nassert((int)x % 4 == 0);
    _nassert((int)y % 8 == 0);
    _nassert((int)r % 8 == 0);
    _nassert(c2 % 4 == 0);
    _nassert(c2 >= 4);
    #pragma MUST_ITERATE(1,,1)
    for (j = 0; j < c2; j += 4) {

        _nassert((int)x % 4 == 0);
        _nassert((int)y % 8 == 0);
        _nassert((int)r % 8 == 0);
        _nassert(r1 % 2 == 0);
        _nassert(r1 >= 8);
        #pragma MUST_ITERATE(4,,1)
        for (i = 0; i < r1; i += 2) {
            sum_1 = 0;
            sum_2 = 0;
            sum_3 = 0;
            sum_4 = 0;
            sum_5 = 0;
            sum_6 = 0;
            sum_7 = 0;
            sum_8 = 0;

            _nassert((int)x % 4 == 0);
            _nassert((int)y % 8 == 0);
            _nassert((int)r % 8 == 0);
            _nassert(c1r2 % 2 == 0);
            _nassert(c1r2 >= 8);
            #pragma MUST_ITERATE(4,,1)
            for (k = 0; k < c1r2; k += 2) {
                x_2x_1 =  _amem4((void *)&x[k + i*c1r2]);
                x_2x_2 =  _amem4((void *)&x[k + (i + 1)*c1r2]);

                y_2x_1 = _amem8((void *)&y[j + k*c2]);
                y_2x_2 = _amem8((void *)&y[j + (k + 1)*c2]);

                y_2x_packed_1 = _pack2 (_loll(y_2x_2), _loll(y_2x_1));
                y_2x_packed_2 = _packh2(_loll(y_2x_2), _loll(y_2x_1));
                y_2x_packed_3 = _pack2 (_hill(y_2x_2), _hill(y_2x_1));
                y_2x_packed_4 = _packh2(_hill(y_2x_2), _hill(y_2x_1));

                sum_1 += _dotp2(x_2x_1, y_2x_packed_1);
                sum_2 += _dotp2(x_2x_1, y_2x_packed_2);
                sum_3 += _dotp2(x_2x_1, y_2x_packed_3);
                sum_4 += _dotp2(x_2x_1, y_2x_packed_4);

                sum_5 += _dotp2(x_2x_2, y_2x_packed_1);
                sum_6 += _dotp2(x_2x_2, y_2x_packed_2);
                sum_7 += _dotp2(x_2x_2, y_2x_packed_3);
                sum_8 += _dotp2(x_2x_2, y_2x_packed_4);
            }
            sum_1 >>= qs;
            sum_2 >>= qs;
            sum_3 >>= qs;
            sum_4 >>= qs;
            sum_5 >>= qs;
            sum_6 >>= qs;
            sum_7 >>= qs;
            sum_8 >>= qs;

            _amem8((void *)&r[j + i*c2]) = _itoll(_pack2(sum_4, sum_3), _pack2(sum_2, sum_1));
            _amem8((void *)&r[j + (i + 1)*c2]) = _itoll(_pack2(sum_8, sum_7), _pack2(sum_6, sum_5));
        }
    }
}

/* ======================================================================== */
/*  End of file:  DSP_mat_mul.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

