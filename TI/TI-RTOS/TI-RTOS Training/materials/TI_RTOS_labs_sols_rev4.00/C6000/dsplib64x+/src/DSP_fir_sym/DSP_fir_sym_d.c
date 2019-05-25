/* ======================================================================= */
/* DSP_fir_sym_d.c -- FIR Filter (Symmetric)                               */
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
#include "DSP_fir_sym_cn.h"
#include "DSP_fir_sym.h"

/* ======================================================================= */
/* Tell compiler arrays are double word alligned                           */
/* ======================================================================= */
#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(h, 8);
#pragma DATA_ALIGN(r_c, 8);
#pragma DATA_ALIGN(r_i, 8);

/* ======================================================================= */
/* Parameters of fixed dataset                                             */
/* ======================================================================= */
#define NH (16)
#define NR (16)
#define NX (50)     // NX = 2*NH + NR + 2
#define PAD (16)

/* ======================================================================= */
/* Initilized arrays with fixed test data                                  */
/* ======================================================================= */
const short x[NX] =
{
    0xFF94,   0x000A,  0x236F,  0xFE45,  0xF9B1,  0x00E3,  0x001F,   0xD5E4,
    0x03EE,   0xF9F2,  0x001A,  0xFFFC,  0xFFD6,  0xFFFD,  0x0000,   0x0000,
    0xFFFF,   0xE82B,  0xABCD,  0xBCDE,  0xCDEF,  0xDEFA,  0xEFAB,   0xFAB1,
    0xAB12,   0xB123,  0x1234,  0x2345,  0x3456,  0x4567,  0x5678,   0x6789,
    0x789A,   0x89AB,  0x9ABC,  0xABCD,  0xADDA,  0xEFFE,  0xBCCA,   0xDEAD,
    0x7FF7,   0x7FFF,  0xFDCA,  0x4823,  0x2567,  0x4EEA,  0x8000,   0x7ACE,
    0x2A3E,   0x4096
};

const short h[NH + 2*PAD] =
{
    0x02AC,   0xFFFF,  0x3F6B,  0x3D15,  0xFFDE,  0xFFFB,  0x0003,  0xFFE5,
    0xFFEA,   0x0032,  0x2411,  0xFC54,  0x0072,  0xFF6A,  0xFFE5,  0xFFFA,
    0xDBC7,   0xFFF8,  0x0002,  0x0000,  0x3FCC,  0xE8AA,  0x019E,  0x0009,
    0xFF26,   0x0005,  0x0023,  0x0009,  0xEA26,  0x07F0,  0xF3CD,  0x001B,
    0xF07B,   0x007B,  0x0001,  0x0372,  0xFFFE,  0xFFF6,  0x0016,  0xFFD2,
    0x185F,   0x00FF,  0xFAAF,  0xDADE,  0xDEAD,  0xCEAD,  0xABBA,  0x7FFF
};

short r_c[NR + 2*PAD];
short r_i[NR + 2*PAD];

/* ======================================================================= */
/* Generate pointers to skip beyond array padding                          */
/* ======================================================================= */
const short *const x_ptr = x;
const short *const h_ptr = h + PAD;
short *const r_ptr_c = r_c + PAD;
short *const r_ptr_i = r_i + PAD;

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int nh, int nr);
clock_t time_i(int nh, int nr);

/* ======================================================================= */
/* MAIN -- Top level driver for testing the algorithm                      */
/* ======================================================================= */
int main()
{
    clock_t t_overhead, t_start, t_stop;
    clock_t t_c, t_i;
    int i, nh, nr, fail = 0;

    /* =================================================================== */
    /* Compute overhead of calling clock() twice to get timing info        */
    /* =================================================================== */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    for (nr = 8, i = 1; nr <= NR; nr += 4) {
        for (nh = 8; nh <= NH; i++, nh += 4) {
            /* =================================================================== */
            /* Force uninitialized arrays to fixed values                          */
            /* =================================================================== */
            memset(r_c, 0xA5A5, sizeof(r_c));
            memset(r_i, 0xA5A5, sizeof(r_i));

            /* =================================================================== */
            /* Call the individual timing toutines, and subtract off overhead      */
            /* =================================================================== */
            t_c = time_c(nh, nr) - t_overhead;  // Calculate time to run Natural C Algorithm
            t_i = time_i(nh, nr) - t_overhead;  // Calculate time to run Optimized C Algorithm

            /* =================================================================== */
            /* Print timing results                                                */
            /* =================================================================== */
            printf("DSP_fir_r8\tIter#: %d\t", i);

            /* =================================================================== */
            /* Check the results arrays, and report any failures                   */
            /* =================================================================== */
            if (memcmp(r_i, r_c, sizeof(r_c))) {
                fail++;
                printf("Result Failure (r_i)");
            }
            else
                printf("Result Successful (r_i)");

            printf("\tNR = %d\tNH = %d\tnatC: %d\toptC: %d\n", nr, nh, t_c, t_i);
        }
    }

    return(fail);
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int nh, int nr)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_fir_sym_cn(x_ptr, h_ptr, r_ptr_c, nh, nr, 0);
    t_stop = clock();
    return t_stop - t_start;
}

clock_t time_i(int nh, int nr)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_fir_sym(x_ptr, h_ptr, r_ptr_i, nh, nr, 0);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_fir_sym_d.c                                          */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

