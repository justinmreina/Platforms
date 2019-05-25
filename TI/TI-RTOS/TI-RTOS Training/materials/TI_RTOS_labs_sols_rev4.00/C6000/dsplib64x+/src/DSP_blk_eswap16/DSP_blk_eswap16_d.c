/* ======================================================================= */
/* DSP_blk_eswap16_d.c -- Endian-swap a block of 16-bit values             */
/*                        Driver code implementation                       */
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

#include "DSP_blk_eswap16_cn.h"
#include "DSP_blk_eswap16.h"

/* ======================================================================== */
/*  Parameters of fixed dataset.                                            */
/* ======================================================================== */
#define NX (200)
#define PAD (16)


/* ======================================================================== */
/*  Initialized arrays with fixed test data.                                */
/* ======================================================================== */
short x[NX + 2*PAD] =
{
     0x032C, -0x544D, -0x6A9D,  0x09C4,  0x5226,  0x6A85,  0x6EDE,  0x1463,
    -0x158A,  0x75BC,  0x7407,  0x160B, -0x0E16, -0x3F82,  0x5F6D, -0x567B,

     0x453E, -0x321D,  0x2DBB,  0x77C7, -0x57B9, -0x4ADD,  0x10B7, -0x113F,
    -0x1A92,  0x7CA7,  0x4A14,  0x719D,  0x3E61,  0x1858, -0x0D4C,  0x3D94,
    -0x32B8, -0x14B2,  0x1B6A,  0x7A1D,  0x0D5B,  0x1D2C,  0x55D8, -0x5459,
     0x5030, -0x5CBD,  0x0180,  0x059B,  0x4D97,  0x49E3,  0x68A4, -0x3629,
    -0x7AA5, -0x582E,  0x1D05,  0x3A65, -0x4734,  0x3D6B, -0x6CE3,  0x4A99,
     0x1D54, -0x50B4,  0x62F5, -0x75A7, -0x6AFA,  0x5D26,  0x3C1A, -0x793D,
    -0x6493,  0x4282, -0x0020,  0x1BE0,  0x1A7F,  0x621B, -0x1C86, -0x254E,
    -0x12FB, -0x4E5B,  0x3409,  0x223E,  0x3856, -0x4179, -0x60C9, -0x102C,
     0x5EB6, -0x097B,  0x7A6E,  0x1450, -0x41CD, -0x133A,  0x0358, -0x2677,
    -0x0BA0,  0x6732,  0x19A5, -0x4961, -0x1E47, -0x6657,  0x57C6,  0x1E70,
     0x1F12,  0x59F9,  0x13A4,  0x16D9,  0x26E6, -0x4FE0,  0x39FA, -0x6AF4,
    -0x2829, -0x673F,  0x7460,  0x57A6, -0x5BB4,  0x424C, -0x6068,  0x2CC9,
     0x0F61, -0x4030, -0x00AD,  0x6AE1, -0x4FD3,  0x56ED,  0x2FDA, -0x759C,
    -0x726F,  0x78E3,  0x42DF, -0x63D3, -0x1F88, -0x71D0,  0x07E8, -0x6D4A,
     0x21E5, -0x6C4D, -0x0072,  0x0EEE,  0x749D, -0x4622, -0x2AF7, -0x3A2B,
     0x3DC4,  0x05D0, -0x00C7,  0x059F,  0x1C52,  0x36EE,  0x58B6, -0x7864,
    -0x145A, -0x3F1F,  0x4107,  0x1B89,  0x2024,  0x4771,  0x3F8F,  0x4298,
     0x231F,  0x6C44, -0x79AD,  0x58DA, -0x1706,  0x37E1,  0x6B2A,  0x6447,
    -0x7C33,  0x080C,  0x426E,  0x1F1C, -0x2095,  0x64D2, -0x7634,  0x4BC4,
     0x6720,  0x6260, -0x12BD,  0x040C, -0x3F04,  0x2FFF,  0x14F4,  0x00CC,
     0x6007,  0x6E3D, -0x532B,  0x7FD1,  0x3538, -0x3B46, -0x4D28, -0x21D9,
     0x087B, -0x7872,  0x68AC,  0x7E95, -0x1851, -0x3B44, -0x0754,  0x126C,
     0x3DE7, -0x754D,  0x360C, -0x2589, -0x2234, -0x2DAF, -0x7E1F,  0x022A,
    -0x4C8A, -0x34A6,  0x2A23, -0x5A1A,  0x4B95, -0x0317,  0x7430, -0x7091,
    -0x0CC1, -0x0A3E, -0x7AFE,  0x645A, -0x50BD,  0x0909,  0x6090,  0x34AE,

    -0x56B3, -0x3AA9,  0x438B,  0x1C64,  0x62BE, -0x5E6A,  0x7A07,  0x0D08,
     0x6089, -0x5D4D,  0x0B27,  0x4A2C,  0x79F5,  0x5A86,  0x4061, -0x4411
};

short x_cn[NX + 2*PAD];
short x_i[NX + 2*PAD];

short r_cn[NX + 2*PAD];
short r_i[NX + 2*PAD];

/* ======================================================================== */
/*  Generate pointers to skip beyond array padding                          */
/* ======================================================================== */
short *const ptr_x_cn = x_cn + PAD;
short *const ptr_r_cn = r_cn + PAD;

short *const ptr_x_i = x_i + PAD;
short *const ptr_r_i = r_i + PAD;

/* ======================================================================== */
/*  Prototypes for timing functions.                                        */
/* ======================================================================== */
clock_t time_cn(int nx);
clock_t time_i(int nx);

/* ======================================================================== */
/*  MAIN -- Top level driver for the test.                                  */
/* ======================================================================== */
int main()
{   
    clock_t t_overhead, t_start, t_stop;
    clock_t t_cn, t_i;
    int i, nx, fail = 0;

    /* -------------------------------------------------------------------- */
    /*  Compute the overhead of calling clock() twice to get timing info.   */
    /* -------------------------------------------------------------------- */
    t_start = clock();
    t_stop = clock();
    t_overhead = t_stop - t_start;

    for(nx = 8, i = 1; nx <= NX; i++, nx += 8) {
        /* -------------------------------------------------------------------- */
        /*  Copy vector to all inputs.                                          */
        /* -------------------------------------------------------------------- */
        memcpy(x_cn, x, sizeof(x_cn));
        memcpy(x_i, x, sizeof(x_i));

        /* -------------------------------------------------------------------- */
        /*  Force uninitialized arrays to fixed values.                         */
        /* -------------------------------------------------------------------- */
        memset(r_cn, 0xA5, sizeof(r_cn));
        memset(r_i, 0xA5, sizeof(r_i));

        /* -------------------------------------------------------------------- */
        /*  Call the individual timing routines, and subtract off overhead.     */
        /* -------------------------------------------------------------------- */
        t_cn = time_cn(nx) - t_overhead;
        t_i = time_i(nx) - t_overhead;

        /* -------------------------------------------------------------------- */
        /*  Print timing results.                                               */
        /* -------------------------------------------------------------------- */
        printf("DSP_blk_eswap16\tIter#: %d\t", i);

        /* -------------------------------------------------------------------- */
        /*  Check the results arrays, and report any failures.                  */
        /* -------------------------------------------------------------------- */
        if (memcmp(x_i, x_cn, sizeof(x_cn))) {
            fail++;
            printf("Result Failure (x_i)\t");
        }
        else
            printf("Result Successful (x_i)\t");

        if (memcmp(r_i, r_cn, sizeof(r_cn))) {
            fail++;
            printf("Result Failure (r_i)");
        }
        else
            printf("Result Successful (r_i)");

        printf("\tNX = %d\tnatC: %d\tintC: %d\n", nx, t_cn, t_i);
    }

    return (fail);
}

/* ======================================================================== */
/*  TIME_CN  -- Measure elapsed time for natural C version.                 */
/* ======================================================================== */
clock_t time_cn(int nx)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_blk_eswap16_cn(ptr_x_cn, ptr_r_cn, nx);
    DSP_blk_eswap16_cn(ptr_x_cn, NULL, nx);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================== */
/*  TIME_I   -- Measure elapsed time for intrinsic C version.               */
/* ======================================================================== */
clock_t time_i(int nx)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_blk_eswap16(ptr_x_i, ptr_r_i, nx);
    DSP_blk_eswap16(ptr_x_i, NULL, nx);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap16_d.c                                       */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2007 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */

