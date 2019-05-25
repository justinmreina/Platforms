/* ======================================================================= */
/* DSP_maxval_d.c -- Maximum value driver code implementation              */
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

/* Interface header files for the natural C and optimized C code */
#include "DSP_maxval_cn.h"
#include "DSP_maxval.h"

#define MAXARRAYSIZE 8192

#pragma DATA_ALIGN(x, 8);

short x[MAXARRAYSIZE];

void main(void)
{
    clock_t t_overhead, time, time_n;
    int j, maxValC, maxValLib, arraySize;
    int errorMax = 0;

    /*  Compute the overhead of calling clock() twice to get timing info. */
    t_overhead = clock();
    t_overhead = clock() - t_overhead;

    /*
        TEST 1: Test arrays with sizes from 8 to 8192 elements.
        Fill each array with random data and find max.
    */
    for(arraySize = 8; arraySize <= MAXARRAYSIZE; arraySize *= 2) {

        for(j = 0; j < arraySize; j++)
            x[j] = rand();

        maxValC   = DSP_maxval_cn(x, arraySize);
        maxValLib = DSP_maxval(x, arraySize);

        if(maxValC != maxValLib)
            errorMax++;
    }

    /*
        TEST 2: Fill the array with the smallest short value
    */
    for(j = 0; j < MAXARRAYSIZE; j++)
        x[j] = SHRT_MIN;

    maxValC   = DSP_maxval_cn(x, MAXARRAYSIZE);
    maxValLib = DSP_maxval(x, MAXARRAYSIZE);
    if(maxValC != maxValLib)
        errorMax++;

    /*
        TEST 3:  Fill the array with the biggest short value
    */
    for(j = 0; j < MAXARRAYSIZE; j++)
        x[j] = SHRT_MAX;

    maxValC   = DSP_maxval_cn(x, MAXARRAYSIZE);
    maxValLib = DSP_maxval(x, MAXARRAYSIZE);
    if(maxValC != maxValLib)
        errorMax++;

    /* 
        TEST 4: Fill the array with zeros
    */
    for(j=0; j<MAXARRAYSIZE; j++)
        x[j]=0;

    maxValC   = DSP_maxval_cn(x, MAXARRAYSIZE);
    maxValLib = DSP_maxval(x, MAXARRAYSIZE);
    if(maxValC != maxValLib)
        errorMax++;

    /* 
        TEST 5: Benchmark performance for an array size of 8192 elements.
    */
    time_n = clock();
    maxValC = DSP_maxval_cn(x, MAXARRAYSIZE);
    time_n = clock() - time_n - t_overhead;

    time = clock();
    maxValLib = DSP_maxval(x, MAXARRAYSIZE);
    time = clock() - time - t_overhead;

    if(maxValC != maxValLib)
        errorMax++;

    /* -------------------------------------------------------------------- */
    /*  Print timing results.                                               */
    /* -------------------------------------------------------------------- */
    if (!errorMax)
        printf("DSP_maxval\tnatC: %f\toptC: %f\n", time_n/8192.0f, time/8192.0f);
    else
        printf("DSP_maxval: Result failure\n");
}

/* ======================================================================== */
/*  End of file:  DSP_maxval_d.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

