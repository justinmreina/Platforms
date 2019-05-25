/* ======================================================================= */
/* DSP_fir_r8_hM16_rM8A8X8_d.c -- FIR Filter (Radix 8)                     */
/*                                Driver code implementation               */
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
#include "DSP_fir_r8_hM16_rM8A8X8_cn.h"
#include "DSP_fir_r8_hM16_rM8A8X8.h"

/* ======================================================================= */
/* Array data is double word alligned to 8 bits                            */
/* ======================================================================= */
#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(h, 8);
#pragma DATA_ALIGN(r_c, 8);
#pragma DATA_ALIGN(r_i, 8);

/* ======================================================================= */
/* Parameters of fixed dataset                                             */
/* ======================================================================= */
#define NH (32)
#define NR (256)
#define NX (288)    // NX = NH + NR
#define PAD (16)

/* ======================================================================= */
/* Initilized arrays with fixed test data                                  */
/* ======================================================================= */
const short x[NX + 2*PAD] =
{
     0x032C, -0x544D, -0x6A9D,  0x09C4,  0x5226,  0x6A85,  0x6EDE,  0x1463,     // Beginning
    -0x158A,  0x75BC,  0x7407,  0x160B, -0x0E16, -0x3F82,  0x5F6D, -0x567B,     // PAD

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
     0x6089, -0x5D4D,  0x0B27,  0x4A2C,  0x79F5,  0x5A86,  0x4061, -0x4411,
    -0x6674,  0x19FC,  0x7F77, -0x4EBE,  0x0E21, -0x5D11,  0x72BE,  0x2232,
     0x4040,  0x3DA5, -0x1736,  0x10DD, -0x192A, -0x768A,  0x770A,  0x1261,
    -0x788B, -0x2778,  0x4989, -0x4999,  0x7905,  0x6F1E,  0x184B, -0x3C6D,
     0x1543, -0x6F94, -0x3588, -0x0D80,  0x57D4, -0x728A, -0x5E27,  0x4092,
    -0x59A4, -0x2322, -0x762C,  0x684B, -0x0AA5,  0x5F2B,  0x5F6B, -0x5CA5,
    -0x01CC, -0x1F8D, -0x35A1,  0x432A, -0x35C6, -0x0E0F, -0x7FEF,  0x3D0B,
     0x3D0B,  0x2B67, -0x7BC7,  0x48D5, -0x1FE3,  0x0ADA,  0x4F02, -0x69DF,
    -0x01CC, -0x1F8D, -0x35A1,  0x432A, -0x35C6, -0x0E0F, -0x7FEF,  0x3D0B,
     0x3D0B,  0x2B67, -0x7BC7,  0x48D5, -0x1FE3,  0x0ADA,  0x4F02, -0x69DF,

    -0x01CC, -0x1F8D, -0x35A1,  0x432A, -0x35C6, -0x0E0F, -0x7FEF,  0x3D0B,     // Ending
     0x3D0B,  0x2B67, -0x7BC7,  0x48D5, -0x1FE3,  0x0ADA,  0x4F02, -0x69DF      // PAD
};

const short  h[NH + 2*PAD] =
{
    -0x6226,  0x4E99,  0x52AC, -0x322F, -0x4CF2,  0x34FE, -0x26B0,  0x512C,     // Beginning
    -0x7CE4,  0x59F0, -0x4137, -0x7CBE, -0x7F4B, -0x5471, -0x1A85, -0x0F38,     // PAD

    -0x0007,  0x002A, -0x0006,  0x0027, -0x0016,  0x001E,  0x0047,  0x005B,
     0x0064, -0x0017, -0x001F,  0x0039, -0x0015, -0x0063, -0x0009, -0x0016,
    -0x0007,  0x002A, -0x0006,  0x0027, -0x0016,  0x001E,  0x0047,  0x005B,
     0x0064, -0x0017, -0x001F,  0x0039, -0x0015, -0x0063, -0x0009, -0x0016,

     0x524B, -0x71B5,  0x5C6E,  0x4C92,  0x6100,  0x4ED3,  0x7244, -0x5594,     // Ending
    -0x12C8, -0x6F88,  0x6EDB,  0x3284, -0x36A5,  0x33E6,  0x05AC, -0x1E10      // PAD
};

short r_c[NR + 2*PAD];
short r_i[NR + 2*PAD];

/* ======================================================================= */
/* Generate pointers to skip beyond array padding                          */
/* ======================================================================= */
const short *const ptr_x = x + PAD;
const short *const ptr_h = h + PAD;
short *const ptr_r_c = r_c + PAD;
short *const ptr_r_i = r_i + PAD;

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

    for(nr = 8, i = 1; nr <= NR; nr += 8) {
        for(nh = 16; nh <= NH; i++, nh += 8) {
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
            printf("DSP_fir_r8_hM16_rM8A8X8\tIter#: %d\t", i);

            /* =================================================================== */
            /* Check the results arrays, and report any failures                   */
            /* =================================================================== */
            if (memcmp(r_i, r_c, sizeof(r_c))) {
                fail++;
                printf("Result Failure (r_i)");
            }
            else
                printf("Result Successful (r_i)");

            printf("\tNR = %d\tNH = %d\tnatC: %d\tintC: %d\n", nr, nh, t_c, t_i);
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
    DSP_fir_r8_hM16_rM8A8X8_cn(ptr_x, ptr_h, ptr_r_c, nh, nr);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/* Prototypes for timing functions                                         */
/* ======================================================================= */
clock_t time_i(int nh, int nr)
{
    clock_t t_start, t_stop;

    t_start = clock();
    DSP_fir_r8_hM16_rM8A8X8(ptr_x, ptr_h, ptr_r_i, nh, nr);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_fir_r8_hM16_rM8A8X8_d.c                              */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

