/* ======================================================================= */
/* DSP_iir_d.c -- IIR Filter                                               */
/*                Driver code implementation                               */
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
#include "DSP_iir_cn.h"
#include "DSP_iir.h"

/* ======================================================================= */
/* Tell compiler arrays are double word alligned                           */
/* ======================================================================= */
#pragma DATA_ALIGN(ptr_Coefs, 8);
#pragma DATA_ALIGN(ptr_Input, 8);
#pragma DATA_ALIGN(ptr_State, 8);
#pragma DATA_ALIGN(ptr_r_c, 8);
#pragma DATA_ALIGN(ptr_r_i, 8);

/* ======================================================================= */
/* Parameters of fixed dataset                                             */
/* ======================================================================= */
#define NCOEFS (64)
#define N (128)
#define T (NCOEFS + PAD)
#define PAD (8)
#define SFB (16383.5)

short ptr_State[N + PAD];

/* ======================================================================= */
/* Initilized arrays with fixed test data                                  */
/* ======================================================================= */
short ptr_Coefs[NCOEFS + 2 * PAD] =
{
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000,
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    (short)(-2.0*SFB), (short)(1.0*SFB),
    (short)( 1.4*SFB), (short)(-0.74*SFB),
    (short)( 2.0*SFB), (short)(1.0*SFB),
    (short)(-1.4*SFB), (short)(-0.74*SFB),
    0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000
};

short ptr_Input[N + 2 * PAD] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0CCC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

short ptr_r_c[N + PAD];
short ptr_r_i[N + PAD];

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int nCoefs);
clock_t time_i(int nCoefs);

/* ======================================================================= */
/* Main -- Top level driver for testing the algorithm                      */
/* ======================================================================= */
int main()
{
    clock_t t_overhead, t_start, t_stop, t_c, t_i;
    int i, nCoefs, fail = 0;

    /* ------------------------------------------------------------------- */
    /* Compute the overhead of calling clock twice to get timing info      */
    /* ------------------------------------------------------------------- */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    for(nCoefs = 8, i = 1; nCoefs <= NCOEFS; i++, nCoefs += 8) {
        /* ---------------------------------------------------------------- */
        /*  Fill output buffers with fixed values.                          */
        /* ---------------------------------------------------------------- */
        memset(ptr_r_c, 0x5A, sizeof(ptr_r_c));
        memset(ptr_r_i, 0x5A, sizeof(ptr_r_i));

        /* ---------------------------------------------------------------- */
        /* Call the individual timing routines                              */
        /* ---------------------------------------------------------------- */
        t_c = time_c(nCoefs) - t_overhead;
        t_i = time_i(nCoefs) - t_overhead;

        /* ---------------------------------------------------------------- */
        /* Print timing results                                             */
        /* ---------------------------------------------------------------- */
        printf("DSP_iir\tIter#: %d\t", i);

        /* ---------------------------------------------------------------- */
        /* Check the results array                                          */
        /* ---------------------------------------------------------------- */
        if (memcmp(ptr_r_i, ptr_r_c, sizeof(ptr_r_c))) {
            fail++;
            printf("Result Failure (r_i)");
        }
        else
            printf("Result Successful (r_i)");

        printf("\tnCoefs = %d\tnatC: %d\toptC: %d\n", nCoefs, t_c, t_i);
    }

    return (fail);
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int nCoefs)
{
    clock_t t_start, t_stop;
    int i;

    for (i = 0; i < T; i++)
        ptr_State[i] = 0;

    for (i = 0; i < N-1; i++)
        ptr_r_c[i] = DSP_iir_cn(ptr_Input[i], ptr_Coefs, nCoefs, ptr_State);

    /* -------------------------------------------------------------------- */
    /* Measure the cycle count of the last iteration of the loop            */
    /* Other measurements are redundant - they take the same amount of time */
    /* -------------------------------------------------------------------- */
    t_start = clock();
    ptr_r_c[i] = DSP_iir_cn(ptr_Input[N-1], ptr_Coefs, nCoefs, ptr_State);
    t_stop = clock();

    return t_stop - t_start;
}

clock_t time_i(int nCoefs)
{
    clock_t t_start, t_stop;
    int i;

    for (i = 0; i < T; i++)
        ptr_State[i] = 0;

    for (i = 0; i < N-1; i++)
        ptr_r_i[i] = DSP_iir(ptr_Input[i], ptr_Coefs, nCoefs, ptr_State);

    /* -------------------------------------------------------------------- */
    /* Measure the cycle count of the last iteration of the loop            */
    /* Other measurements are redundant - they take the same amount of time */
    /* -------------------------------------------------------------------- */
    t_start = clock();
    ptr_r_i[i] = DSP_iir(ptr_Input[N-1], ptr_Coefs, nCoefs, ptr_State);
    t_stop = clock();

    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_iir_d.c                                              */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

