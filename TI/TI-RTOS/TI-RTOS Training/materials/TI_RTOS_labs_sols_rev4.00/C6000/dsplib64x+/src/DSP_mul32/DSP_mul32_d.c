/* ======================================================================= */
/* DSP_mul32_d.c -- mul32 driver code implementation                       */
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

/* Interface header files for the natural C and optimized C code */
#include "DSP_mul32_cn.h"
#include "DSP_mul32.h"

#define MAXARRAYSIZE 2048

#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(y, 8);
#pragma DATA_ALIGN(r1, 8);
#pragma DATA_ALIGN(r2, 8);

int x[MAXARRAYSIZE];
int y[MAXARRAYSIZE];
int r1[MAXARRAYSIZE];
int r2[MAXARRAYSIZE];

void main(void)
{
    clock_t t_overhead, time, time_n;
    int i, arraySize;
    int errorMul = 0;

    /*  Compute the overhead of calling clock() twice to get timing info. */
    t_overhead = clock();
    t_overhead = clock() - t_overhead;

    /* Test arrays with size from 8 to 2048 elements. */
    /* Fill each array with random data and do mul32. */
    for(arraySize = 8; arraySize <= MAXARRAYSIZE; arraySize *= 2) {
        for(i = 0; i < arraySize; i++) {
            x[i]= rand();
            y[i]= rand();
        }

        DSP_mul32(x, y, r1, arraySize);
        DSP_mul32_cn(x, y, r2, arraySize);

        for(i = 0; i < arraySize; i++) {
            if (r1[i] != r2[i])
                errorMul++;
        }
    }

    /* And now measure the performance for an array size of 2048 elements. */
    time_n = clock();
    DSP_mul32_cn(x, y, r2, MAXARRAYSIZE);
    time_n = clock() - time_n - t_overhead;

    time = clock();
    DSP_mul32(x, y, r1, MAXARRAYSIZE);
    time = clock() - time - t_overhead;

    /* -------------------------------------------------------------------- */
    /*  Print timing results.                                               */
    /* -------------------------------------------------------------------- */
    if (!errorMul) {
        printf("DSP_mul32\tnatC: %f\tintC: %f\n", time_n/2048.0, time/2048.0);
    }
    else
        printf("DSP_mul32\tResult Failure\n");
}

/* ======================================================================== */
/*  End of file:  DSP_mul32_d.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

