/* ======================================================================= */
/* DSP_neg32_d.c -- neg32 driver code implementation                       */
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
#include "DSP_neg32_cn.h"
#include "DSP_neg32.h"

#define MAXARRAYSIZE 2048

#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(y1, 8);
#pragma DATA_ALIGN(y2, 8);

int x[MAXARRAYSIZE];
int y1[MAXARRAYSIZE];
int y2[MAXARRAYSIZE];

void main(void)
{
    clock_t t_overhead, time, time_n;
    int i, arraySize;
    int errorNeg = 0;

    /*  Compute the overhead of calling clock() twice to get timing info. */
    t_overhead = clock();
    t_overhead = clock() - t_overhead;

    /*
        TEST 1: Test arrays with size from 8 to 2048 elements.
        Fill each array with random data and do Neg32.
    */
    for(arraySize = 8; arraySize <= MAXARRAYSIZE; arraySize *= 2) {
        for(i = 0; i < arraySize; i++) {
            x[i] = (rand() << 16) + rand();
        }

        DSP_neg32(x, y1, arraySize);
        DSP_neg32_cn(x, y2, arraySize);

        for(i = 0; i < arraySize; i++) {
            if(y1[i] != y2[i])
                errorNeg++;
        }
    }

    /*
        TEST 2: Ensure that the kernel can be used to do inplace processing.
    */
    DSP_neg32(x, y1, MAXARRAYSIZE);
    DSP_neg32(y1, y1, MAXARRAYSIZE);
    for(i = 0; i < MAXARRAYSIZE; i++) {
        if(x[i] != y1[i])
            errorNeg++;
    }

    /*
        TEST 3: Benchmark performance for an array size of 2048 elements.
    */
    time_n = clock();
    DSP_neg32_cn(x, y2, MAXARRAYSIZE);
    time_n = clock() - time_n - t_overhead;

    time = clock();
    DSP_neg32(x, y1, MAXARRAYSIZE);
    time = clock() - time - t_overhead;

    /* -------------------------------------------------------------------- */
    /*  Print timing results.                                               */
    /* -------------------------------------------------------------------- */
    if (!errorNeg)
        printf("DSP_neg32\tnatC: %f\tintC: %f\n", time_n/2048.0f, time/2048.0f);
    else
        printf("DSP_neg32\tResult Failure\n");
}

/* ======================================================================== */
/*  End of file:  DSP_neg32_d.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

