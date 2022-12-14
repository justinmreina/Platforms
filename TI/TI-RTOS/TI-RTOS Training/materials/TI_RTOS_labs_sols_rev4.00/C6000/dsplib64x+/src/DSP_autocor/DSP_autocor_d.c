/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      DSP_autocor -- Driver file for autocor                              */
/*                                                                          */
/*  USAGE                                                                   */
/*      This code contains a driver program for testing the 'autocor'       */
/*      kernel.  The function prototype appears below.  The driver itself   */
/*      is invoked as a standard C program, and results are reported on     */
/*      stdout.                                                             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2007 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

#include "DSP_autocor_cn.h"
#include "DSP_autocor.h"

/* ======================================================================== */
/*  Kernel-specific alignments                                              */
/* ======================================================================== */
#pragma DATA_ALIGN(r_c, 64);
#pragma DATA_ALIGN(r_i, 64);
#pragma DATA_ALIGN(x, 8);

/* ======================================================================== */
/*  Parameters of fixed dataset.                                            */
/* ======================================================================== */
#define Nr (32)
#define Nr_plus_Nx (192)
#define Nr_plus_Nx_plus_PAD (208)
#define Nx (160)
#define PAD (16)

/* ======================================================================== */
/*  Number of input arguments:                                              */
/* ======================================================================== */
#define NUM_INPUTS (1)

/* ======================================================================== */
/*  Initialized arrays with fixed test data.                                */
/* ======================================================================== */
short r_c[NUM_INPUTS][Nx + 2*PAD];
short r_i[NUM_INPUTS][Nx + 2*PAD];

short x[NUM_INPUTS][Nr_plus_Nx_plus_PAD] =
{
/* Input Number: 0 */

{
    -0x1465, -0x7F47,  0x3EA8, -0x6F4F, -0x04CC, -0x1123,  0x2374, -0x46C4,
     0x159D,  0x16CD,  0x0276, -0x4E02,  0x57D7,  0x1881,  0x4631, -0x2E1C,
    -0x1F71,  0x1F63,  0x65B4, -0x2231, -0x40BA, -0x197E,  0x5824, -0x1749,
    -0x5BA4, -0x5E3C,  0x2861, -0x1FF5, -0x278E, -0x3466, -0x08B1,  0x3EDA,
    -0x024B,  0x55B8,  0x0F3E,  0x5C36,  0x118C, -0x3AB9,  0x75F7, -0x13A3,
     0x301A, -0x54E1, -0x761A,  0x1D1D, -0x56CC,  0x097E,  0x20E7, -0x2700,
    -0x39DA,  0x14C3, -0x56CD, -0x4A4A, -0x1319, -0x195B, -0x2813, -0x09C5,
     0x6D23, -0x778E,  0x4DD5,  0x5049, -0x4D51, -0x113A, -0x296A, -0x4D7F,
     0x17D5,  0x4D50, -0x6BC9, -0x3327,  0x1F0C,  0x5D09, -0x38D1, -0x5036,
     0x5A87, -0x018C, -0x7768,  0x12C3, -0x5ED7, -0x3F9C,  0x1203, -0x3FA9,
     0x0C5B, -0x2A49,  0x0F6D, -0x5553,  0x3144, -0x2166,  0x68D7,  0x1AB8,
     0x28DB,  0x2ECD,  0x1CFC, -0x6B5B, -0x2D9E, -0x2397, -0x6756, -0x30EC,
    -0x1740,  0x519F,  0x1C3D,  0x608C,  0x4D39, -0x58FC, -0x654B, -0x7D51,
     0x2708,  0x7AD9, -0x5A0A, -0x7A48,  0x365F, -0x6634, -0x205C,  0x62AE,
    -0x01C1,  0x1DC9,  0x6A1D, -0x0A21,  0x27BA,  0x6340, -0x1CE5,  0x5D1E,
    -0x57F0,  0x7071,  0x6271, -0x04C5,  0x3E2E,  0x3238, -0x0692, -0x62C1,
    -0x64EE,  0x31DB,  0x3F5B,  0x4BEE, -0x1D77,  0x1859,  0x5BA3,  0x3902,
    -0x152A, -0x504B,  0x180E,  0x5EAD, -0x414D, -0x4E34,  0x7AAF,  0x30D6,
    -0x17C9,  0x0E22,  0x5BE3,  0x07A1,  0x001C,  0x0C35, -0x7209, -0x6055,
    -0x001A, -0x2D2D, -0x020B,  0x4A92, -0x2696, -0x2D17,  0x6007,  0x3A39,
    -0x27C6, -0x5DA4, -0x1AFC,  0x784B,  0x4069, -0x529F,  0x6599, -0x007A,
    -0x4BC3,  0x54CD, -0x5E6D,  0x7D40, -0x16CA, -0x110B, -0x0032,  0x33AE,
     0x0FFC,  0x3184,  0x5040, -0x7E8C, -0x7251,  0x78D2,  0x16AC, -0x6A19,
     0x7C13,  0x7C45, -0x2B95, -0x4560,  0x6422, -0x1CD6, -0x1C1F, -0x293A,
    -0x3884,  0x3D93,  0x430D, -0x7C65, -0x1AC6, -0x1B4D,  0x00BB,  0x52CB,
     0x539E,  0x4341, -0x6E26, -0x4A03, -0x38D3, -0x06AA, -0x4494,  0x0721
},

};

/* ======================================================================== */
/*  Generate pointers to skip beyond array padding                          */
/* ======================================================================== */
short *const ptr_r_c = r_c[0] + PAD;
short *const ptr_r_i = r_i[0] + PAD;

/* ======================================================================== */
/*  Prototypes for timing functions.                                        */
/* ======================================================================== */
clock_t time_c(int cur_input, int nx, int nr);
clock_t time_i(int cur_input, int nx, int nr);

/* ======================================================================== */
/*  MAIN -- Top level driver for the test.                                  */
/* ======================================================================== */
int main()
{
    clock_t t_overhead, t_start, t_stop;
    clock_t t_c, t_i;
    int i, j, nx, nr, fail = 0;
    short  ii;

    /* -------------------------------------------------------------------- */
    /*  Compute the overhead of calling clock() twice to get timing info.   */
    /* -------------------------------------------------------------------- */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    /* -------------------------------------------------------------------- */
    /*  Make sure that input values are small enough that they do not       */
    /*  saturate the calculations                                           */
    /* -------------------------------------------------------------------- */
    for (ii = 0; ii < Nx + Nr + PAD; ii++) {
        if (x[0][ii] < 0) {
            x[0][ii] = -x[0][ii];
            x[0][ii] =  x[0][ii] & 0x1fff;
            x[0][ii] = -x[0][ii];
        }
        else
            x[0][ii] = x[0][ii] & 0x1fff;
    }

    for(nx = 8, j = 1; nx <= Nx; nx += 8) {
        for(nr = 4; nr <= Nr; j++, nr += 4) {
            t_c = 0;
            t_i = 0;

            /* -------------------------------------------------------------------- */
            /*  Force uninitialized arrays to fixed values.                         */
            /* -------------------------------------------------------------------- */
            memset(r_c, 0xA5, sizeof(r_c));
            memset(r_i, 0xA5, sizeof(r_i));

            /* -------------------------------------------------------------------- */
            /*  Check the results arrays, and report any failures.                  */
            /* -------------------------------------------------------------------- */
            for(i = 0; i < NUM_INPUTS; i++) {
                t_c += time_c(i, nx, nr) - t_overhead;
                t_i += time_i(i, nx, nr) - t_overhead;
            }

            printf("DSP_autocor\tIter#: %d\t", j);

            if (memcmp(r_i, r_c, sizeof(r_c))) {
                fail++;
                printf("Result Failure (r_i)");
            }
            else
                printf("Result Successful (r_i)");

            printf("\tNX = %d\tNR = %d\tnatC: %d\toptC: %d\n", nx, nr, t_c, t_i);
        }
    }

    return (fail);
}

/* ======================================================================== */
/*  TIME_C   -- Measure elapsed time for natural C version.                 */
/* ======================================================================== */
clock_t time_c(int cur_input, int nx, int nr)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_autocor_cn(&r_c[cur_input][PAD], &x[cur_input][0], nx, nr);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================== */
/*  TIME_I   -- Measure elapsed time for intrinsic C version.               */
/* ======================================================================== */
clock_t time_i(int cur_input, int nx, int nr)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_autocor(&r_i[cur_input][PAD], &x[cur_input][0], nx, nr);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================== */
/*  End of file:  DSP_autocor_d.c                                           */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2007 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */

