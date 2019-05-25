/* ======================================================================= */
/* DSP_mat_trans_cn.c -- Matrix Transpose                                  */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_mat_trans_cn (                                             */
/*         const short *restrict x, // Input matrix                        */
/*         short rows,              // Height of input matrix              */
/*         short columns,           // Width of input matrix               */
/*         short *restrict r        // Output matrix                       */
/*     );                                                                  */
/*                                                                         */
/*  Description                                                            */
/*     The program transposes a matrix of 16-bit values and user-          */
/*     determined dimensions. The result of a matrix transpose is a        */
/*     matrix with the number of rows = number of columns of input matrix  */
/*     and number of columns = number of rows of input matrix The value    */
/*     of an elements of the output matrix is equal to the value of the    */
/*     element from the input matrix with switched coordinates (rows,      */
/*     columns).                                                           */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x and r do not overlap                                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_mat_trans_cn, ".text:ansi");

#include "DSP_mat_trans_cn.h"

void DSP_mat_trans_cn (
    const short *restrict   x,          /* Input matrix             */
    short                   rows,       /* Height of input matrix   */
    short                   columns,    /* Width of input matrix    */
    short *restrict         r           /* Output matrix            */
)
{
    int i, j;

#ifdef NOASSUME
    _nassert(columns % 4 == 0);
    _nassert(rows % 4 == 0);
    _nassert((int)(x) % 8 == 0);
    _nassert((int)(r) % 8 == 0);
    _nassert(columns >= 8);
    _nassert(rows >= 8);
#endif

    /* ------------------------------------------------------------------- */
    /*  Write each column of 'x' to a row of 'r'.                          */
    /* ------------------------------------------------------------------- */
    for (i = 0; i < columns; i++) {
        for (j = 0; j < rows; j++) {
            r[(i * rows) + j] = x[i + (columns * j)];
        }
    }
}

/* ======================================================================= */
/*  End of file:  DSP_mat_trans_cn.c                                       */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

