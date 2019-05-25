/* ======================================================================= */
/* DSP_fltoq15_d.c -- Float to Q15 conversion                              */
/*                    Driver code implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

/* ======================================================================= */
/* Interface header files for the natural C and optimized C code           */
/* ======================================================================= */
#include "DSP_fltoq15_cn.h"
#include "DSP_fltoq15.h"

/* ======================================================================= */
/* Inform the compiler the arrays are double word alligned                 */
/* ======================================================================= */
#pragma DATA_ALIGN(flt, 4);
#pragma DATA_ALIGN(q15_c, 4);
#pragma DATA_ALIGN(q15_i, 4);

/* ======================================================================= */
/* Parameters of fixed dataset                                             */
/* ======================================================================= */
#define N (64)

/* ======================================================================= */
/* Initilized arrays with fixed test data                                  */
/* ======================================================================= */
const float flt[N] =
{
     3.40282346e+38F,       /* max float                            */
     2.0F,                  /* saturate (exceeds max Q.15 number)   */
     1.000030517578F,       /* saturate (exceeds max Q.15 number)   */
     1.0F,                  /* saturate (exceeds max Q.15 number)   */
     0.999969482F,          /* max pos Q.15 number                  */
     0.5F,
     0.000030617578125F,    /* 1, round towards - inf               */
     0.000030517578125F,    /* 1, pos Q.15 number closest to 0      */
     0.0F,                  /* zero                                 */
    -0.000030517578125F,    /* -1, neg Q.15 number closest to 0     */
    -0.000030617578125F,    /* -2, round towards -inf               */
    -0.5F,
    -1.0F,                  /* most negative Q.15 number            */
    -1.000030517578F,       /* saturate                             */
    -2.0F,                  /* saturate                             */
    -512.0F,                /* saturate                             */
     3.40282346e+38F,       /* max float                            */
     2.0F,                  /* saturate (exceeds max Q.15 number)   */
     1.000030517578F,       /* saturate (exceeds max Q.15 number)   */
     1.0F,                  /* saturate (exceeds max Q.15 number)   */
     0.999969482F,          /* max pos Q.15 number                  */
     0.5F,
     0.000030617578125F,    /* 1, round towards - inf               */
     0.000030517578125F,    /* 1, pos Q.15 number closest to 0      */
     0.0F,                  /* zero                                 */
    -0.000030517578125F,    /* -1, neg Q.15 number closest to 0     */
    -0.000030617578125F,    /* -2, round towards -inf               */
    -0.5F,
    -1.0F,                  /* most negative Q.15 number            */
    -1.000030517578F,       /* saturate                             */
    -2.0F,                  /* saturate                             */
    -512.0F,                /* saturate                             */
     3.40282346e+38F,       /* max float                            */
     2.0F,                  /* saturate (exceeds max Q.15 number)   */
     1.000030517578F,       /* saturate (exceeds max Q.15 number)   */
     1.0F,                  /* saturate (exceeds max Q.15 number)   */
     0.999969482F,          /* max pos Q.15 number                  */
     0.5F,
     0.000030617578125F,    /* 1, round towards - inf               */
     0.000030517578125F,    /* 1, pos Q.15 number closest to 0      */
     0.0F,                  /* zero                                 */
    -0.000030517578125F,    /* -1, neg Q.15 number closest to 0     */
    -0.000030617578125F,    /* -2, round towards -inf               */
    -0.5F,
    -1.0F,                  /* most negative Q.15 number            */
    -1.000030517578F,       /* saturate                             */
    -2.0F,                  /* saturate                             */
    -512.0F,                /* saturate                             */
     3.40282346e+38F,       /* max float                            */
     2.0F,                  /* saturate (exceeds max Q.15 number)   */
     1.000030517578F,       /* saturate (exceeds max Q.15 number)   */
     1.0F,                  /* saturate (exceeds max Q.15 number)   */
     0.999969482F,          /* max pos Q.15 number                  */
     0.5F,
     0.000030617578125F,    /* 1, round towards - inf               */
     0.000030517578125F,    /* 1, pos Q.15 number closest to 0      */
     0.0F,                  /* zero                                 */
    -0.000030517578125F,    /* -1, neg Q.15 number closest to 0     */
    -0.000030617578125F,    /* -2, round towards -inf               */
    -0.5F,
    -1.0F,                  /* most negative Q.15 number            */
    -1.000030517578F,       /* saturate                             */
    -2.0F,                  /* saturate                             */
    -512.0F,                /* saturate                             */
};

/* ======================================================================== */
/*  Result arrays, with padding to catch output overrun.                    */
/* ======================================================================== */
short q15_c[N];
short q15_i[N];

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int n);
clock_t time_i(int n);

/* ======================================================================= */
/* MAIN -- Top level driver for testing the algorithm                      */
/* ======================================================================= */
int main()
{
    clock_t t_overhead, t_start, t_stop;
    clock_t t_c, t_i;
    int i, n, fail = 0;

    /* =================================================================== */
    /* Compute overhead of calling clock() twice to get timing info        */
    /* =================================================================== */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    for (n = 2, i = 1; n <= N; i++, n += 2) {
        /* -------------------------------------------------------------------- */
        /*  Fill output buffers with fixed values.                              */
        /* -------------------------------------------------------------------- */
        memset(q15_c, 0x5A, sizeof(q15_c));
        memset(q15_i, 0x5A, sizeof(q15_i));

        /* =================================================================== */
        /* Call the individual timing routines, and subtract off overhead      */
        /* =================================================================== */
        t_c = time_c(n) - t_overhead;    // Calculate time to run Natural C Algorithm
        t_i = time_i(n) - t_overhead;    // Calculate time to run Optimized C Algorithm

        /* =================================================================== */
        /* Print timing results                                                */
        /* =================================================================== */
        printf("DSP_fltoq15\tIter#: %d\t", i);

        /* =================================================================== */
        /* Check the results arrays, and report any failures                   */
        /* =================================================================== */
        if (memcmp(q15_i, q15_c, sizeof(q15_c))) {
            fail++;
            printf("Result failure (q15_i)");
        }
        else
            printf("Result successful (q15_i)");

        printf("\tN = %d\tnatC: %d, optC: %d\n", n, t_c, t_i);
    }

    return(fail);
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int n)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_fltoq15_cn(flt, q15_c, n);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_i(int n)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_fltoq15(flt, q15_i, n);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_flttoq15_d.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

