/* ======================================================================= */
/* DSP_iir_lat_d.c -- IIR lattice Filter                                   */
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
#include "DSP_iir_lat_cn.h"
#include "DSP_iir_lat.h"

/* ======================================================================= */
/* Kernel specific data allignment(s)                                      */
/* ======================================================================= */
#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(k, 8);
#pragma DATA_ALIGN(b_c, 8);
#pragma DATA_ALIGN(b_i, 8);
#pragma DATA_ALIGN(r_c, 8);
#pragma DATA_ALIGN(r_i, 8);

/* ======================================================================= */
/*  Parameters of fixed dataset.                                           */
/* ======================================================================= */
#define NK (32)
#define NK1 (NK+1)
#define NX (20)
#define PAD (16)


/* ======================================================================= */
/*  Initialized arrays with fixed test data.                               */
/* ======================================================================= */
short x[NX + 2*PAD] =
{
    0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A, 0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A,
    0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A, 0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A,

    0xBBCD, 0xABCD,  0x1223, 0x1111, 0x2222, 0x3333, 2191, 738, -242, 308,
    1411, -322, 603, 197, -1972, 2191, 738, -242, 308, 0,

    0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A, 0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A,
    0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A, 0x5A5A,  0x5A5A,  0x5A5A,  0x5A5A,
};

const short k[NK] =
{
    -13839, 22437, -27054, 19741,
    -16792, -30550, -25856, 6973,
    -13839, 22437, -27054, 19741,
    -16792, -30550, -25856, 6973,
    -13839, 22437, -27054, 19741,
    -16792, -30550, -25856, 6973,
    -13839, 22437, -27054, 19741,
    -16792, -30550, -25856, 6973,
};

int  b_c[NK1 + 2*PAD] =
{
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,

    0x000A0000,  0x00050000,  0x00A00000,  0x00110000,
    0x00340000,  0xF6B50000,  0x003C0000,  0xFFE10000,
    0x009D0000,

    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
    0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,  0x5A5A5A5A,
};

int b_i[NK1 + 2*PAD];

short r_c[NX + 2*PAD];
short r_i[NX + 2*PAD];

/* ======================================================================= */
/*  Generate pointers to skip beyond array padding                         */
/* ======================================================================= */
short *const ptr_x = x + PAD;

int *const ptr_b_c = b_c + PAD;
int *const ptr_b_i = b_i + PAD;

short *const ptr_r_c = r_c + PAD;
short *const ptr_r_i = r_i + PAD;

/* ======================================================================= */
/*  Prototypes for timing functions.                                       */
/* ======================================================================= */
clock_t time_c(int nx, int nk);
clock_t time_i(int nx, int nk);

/* ======================================================================= */
/* Main -- Top level driver for testing the algorithm                      */
/* ======================================================================= */
int main()
{
    clock_t t_overhead, t_start, t_stop, t_c, t_i;
    int i, j, nx, nk, fail = 0;

    /* ------------------------------------------------------------------- */
    /* Compute the overhead of calling clock twice to get timing info      */
    /* ------------------------------------------------------------------- */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    for (nx = 2, j = 1; nx <= NX; nx += 2) {
        for (nk = 4; nk <= NK; j++, nk += 4) {
            /* ------------------------------------------------------------------- */
            /* Initialize all arrays                                               */
            /* ------------------------------------------------------------------- */
            memset(r_c, 0xA5, sizeof(r_c));
            memset(r_i, 0xA5, sizeof(r_i));
            memcpy(b_i, b_c, sizeof(b_i));

            /* ------------------------------------------------------------------- */
            /* natC and optC maintain b[] << 15                                    */
            /* ------------------------------------------------------------------- */
            for (i = 0; i < (NK1 + 2*PAD); i++)
                b_c[i] = b_i[i] >> 1;

            /* ------------------------------------------------------------------- */
            /* Call the individual timing routines                                 */
            /* ------------------------------------------------------------------- */
            t_c = time_c(nx, nk) - t_overhead;
            t_i = time_i(nx, nk) - t_overhead;

            /* ------------------------------------------------------------------- */
            /* Print timing results                                                */
            /* ------------------------------------------------------------------- */
            printf("DSP_iir_lat\tIter#: %d\t", j);

            /* ------------------------------------------------------------------- */
            /* Check the results array                                             */
            /* ------------------------------------------------------------------- */
            if (memcmp(r_i, r_c, sizeof(r_c))) {
                fail++;
                printf("Result Failure (r_i)");
            }
            else
                printf("Result Successful (r_i)");

            printf("\tNX = %d\tNK = %d\tnatC: %d\toptC: %d\n", nx, nk, t_c, t_i);
        }
    }

    return (fail);
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_c(int nx, int nk)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_iir_lat_cn(ptr_x, nx, k, nk, ptr_b_c, ptr_r_c);
    t_stop = clock();
    return t_stop - t_start;
}

clock_t time_i(int nx, int nk)
{
    clock_t t_start, t_stop;
    t_start = clock();
    DSP_iir_lat(ptr_x, nx, k, nk, ptr_b_i, ptr_r_i);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_iir_d.c                                              */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

