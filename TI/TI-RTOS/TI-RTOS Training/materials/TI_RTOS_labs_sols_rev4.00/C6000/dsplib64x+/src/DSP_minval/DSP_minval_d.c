/* ======================================================================= */
/* DSP_minval_d.c -- Minimum value driver code implementation              */
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
#include "DSP_minval_cn.h"
#include "DSP_minval.h"

#define MAXARRAYSIZE 8192

#pragma DATA_ALIGN(x, 8);

short x[MAXARRAYSIZE];

void main(void)
{
    clock_t t_overhead, time, time_n; 
    int j, minValC, minValLib, arraySize;
    int errorMin = 0;

    /*  Compute the overhead of calling clock() twice to get timing info. */
    t_overhead = clock();
    t_overhead = clock() - t_overhead;

    /*
        TEST 1: Test arrays with size from 8 to 8192 elements.
        Fill each array with random data and find min.
    */
    for(arraySize = 8; arraySize <= MAXARRAYSIZE; arraySize *= 2) {
        for(j = 0; j < arraySize; j++)
            x[j] = rand();

        minValC   = DSP_minval_cn(x, arraySize);
        minValLib = DSP_minval(x, arraySize);
        if(minValC != minValLib)
            errorMin++;
    }

    /* 
        TEST 2: Fill the array with the smallest short value 
    */
    for(j = 0; j < MAXARRAYSIZE; j++)
        x[j] = SHRT_MIN;

    minValC   = DSP_minval_cn(x, MAXARRAYSIZE);
    minValLib = DSP_minval(x, MAXARRAYSIZE);
    if(minValC != minValLib)
        errorMin++;

    /*
        TEST 3:  Fill the array with the biggest short value
    */
    for(j = 0; j < MAXARRAYSIZE; j++)
        x[j] = SHRT_MIN;

    minValC   = DSP_minval_cn(x, MAXARRAYSIZE);
    minValLib = DSP_minval(x, MAXARRAYSIZE);
    if(minValC != minValLib)
        errorMin++;

    /*
        TEST 4: Fill the array with zeros
    */
    for(j = 0; j < MAXARRAYSIZE; j++)
        x[j]=0;

    minValC   = DSP_minval_cn(x, MAXARRAYSIZE);
    minValLib = DSP_minval(x, MAXARRAYSIZE);
    if(minValC != minValLib)
        errorMin++;

    /*
        TEST 5: Benchmark performance for an array size of 8192 elements.
    */
    time_n = clock();
    minValC = DSP_minval_cn(x, MAXARRAYSIZE);
    time_n = clock() - time_n - t_overhead;

    time = clock();
    minValLib = DSP_minval(x, MAXARRAYSIZE);
    time = clock() - time - t_overhead;

    if(minValC != minValLib)
        errorMin++;

    /* -------------------------------------------------------------------- */
    /*  Print timing results.                                               */
    /* -------------------------------------------------------------------- */
    if (!errorMin)
        printf("DSP_minval\tnatC: %f\toptC: %f\n", time_n/8192.0f, time/8192.0f);
    else
        printf("DSP_minval: Result Failure\n");
}

/* ======================================================================== */
/*  End of file:  DSP_minval_d.c                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

