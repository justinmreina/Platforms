/* ======================================================================= */
/* DSP_fir_cplx_hM4X4_d.c -- Complex FIR Filter                            */
/*                           Driver code implementation                    */
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
#include "DSP_fir_cplx_hM4X4_cn.h"
#include "DSP_fir_cplx_hM4X4.h"

/* ======================================================================= */
/* Inform the compiler the arrays are double word alligned                 */
/* ======================================================================= */
#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(h, 8);
#pragma DATA_ALIGN(r_c, 8);
#pragma DATA_ALIGN(r_i, 8);

/* ======================================================================= */
/* Parameters of fixed dataset                                             */
/* ======================================================================= */
#define NX (256)
#define NH (24)
#define NR (100)
#define PAD (16)

/* ======================================================================= */
/* Initilized arrays with fixed test data                                  */
/* ======================================================================= */
const short x[NX + 2*PAD] =
{
    0x76E2, 0x8B0E, 0x4A09, 0xBA35, 0xEB22, 0xE6EB, 0x8CCF, 0xDF71,     // Beginning
    0xAACF, 0x3CE2, 0xADDA, 0x3E77, 0x8CA7, 0x6C43, 0x0F68, 0xCCF6,     // PAD

    0x5A0A, 0xCD7B, 0x8397, 0xA5BC, 0xB196, 0x963C, 0x61C8, 0xE43A,
    0x1C8B, 0x55F2, 0x32B9, 0xB3DD, 0xE468, 0x64CE, 0x3F29, 0x2815,
    0x99CB, 0x813E, 0xCAB7, 0xE031, 0xE954, 0x8AF2, 0x9943, 0x2780,
    0x8DC1, 0x2C18, 0xA0EA, 0x6371, 0x9033, 0x2F1F, 0xF36D, 0xB972,
    0x5B46, 0x9837, 0x6009, 0x7354, 0x7CBB, 0xCB4E, 0x4D5F, 0x90E4,
    0x27B1, 0xCBF3, 0xD4CD, 0xCA53, 0xD3A7, 0xDD56, 0xEF72, 0x866E,
    0x637B, 0x6CE5, 0x084D, 0x3A65, 0xCA8D, 0x45D0, 0x8A5D, 0xB246,
    0x031B, 0xBC84, 0xB401, 0x9DC2, 0xB285, 0xE273, 0x6C99, 0xE626,
    0x3D8A, 0x5E49, 0x0742, 0xDAE3, 0x7488, 0x5DF7, 0x36BD, 0xA466,
    0xBE5F, 0x8DAB, 0x6647, 0xCF7F, 0x8A8D, 0xBFF5, 0x3E81, 0x867D,
    0x1CD2, 0x1103, 0xF7C9, 0x8B0F, 0xAE8D, 0x03A4, 0x22BE, 0x7CC3,
    0x739C, 0x9D08, 0xC9DF, 0xB2AA, 0x155F, 0x2D3D, 0x80AC, 0xD731,
    0x9654, 0x0273, 0x9582, 0x5C69, 0xBC3B, 0xB957, 0x3641, 0x293F,
    0x3533, 0x94DA, 0x1E69, 0xE5A3, 0xD45A, 0xC82B, 0x8758, 0xFAC3,
    0x7ED0, 0x3E98, 0x4E2C, 0x3C10, 0xDED3, 0x0110, 0x3666, 0xD178,
    0x9683, 0xD6E3, 0x3A24, 0x22E9, 0xFC9F, 0xCD7F, 0x3DE5, 0x2133,
    0x6DE5, 0xD5F3, 0x86A9, 0x8805, 0x8695, 0x54AF, 0xE1AD, 0x5BEE,
    0x856E, 0x8261, 0x0052, 0xA09D, 0xCBAE, 0x9EF8, 0x01F5, 0x52E1,
    0xE2D5, 0xC8A4, 0x55F7, 0xB8A8, 0x1121, 0x6813, 0xE576, 0xBCE3,
    0x1112, 0xB2B4, 0x4070, 0x4C3F, 0x5407, 0xF997, 0x9346, 0xD36C,
    0xDFDE, 0x428A, 0xB497, 0x7E59, 0x5157, 0x6DBD, 0x20E0, 0x23F5,
    0x0D70, 0x34BE, 0x8481, 0x744F, 0xD429, 0x4CBC, 0xC51A, 0xBE15,
    0x9F01, 0xD327, 0x8468, 0xF118, 0x3D36, 0x8B2D, 0xF0EC, 0x4CA5,
    0xEDE8, 0xCE3D, 0x3A04, 0xF5AC, 0x5EB5, 0x2187, 0x7BCE, 0x077C,
    0xFCFD, 0xDF80, 0xC98B, 0xE564, 0xE27F, 0xE342, 0xB89A, 0xF7D3,
    0x4F39, 0xBA12, 0xF418, 0x0337, 0x6F4B, 0x13D7, 0x4B05, 0x54A1,
    0x2394, 0x5020, 0xFC80, 0x913E, 0x8252, 0x889D, 0x1FE9, 0xEBDE,
    0x96A4, 0x07DD, 0x959E, 0x80B0, 0x56EB, 0xF72D, 0xA67D, 0xCFC3,
    0xBD24, 0x437E, 0x08C7, 0xE546, 0xFC8E, 0x7F3E, 0x5B7A, 0x85A7,
    0x5609, 0xCF9D, 0xFB15, 0x6ED7, 0x9BF4, 0x3289, 0x5037, 0x0443,
    0x9B2D, 0x1E91, 0x0DE0, 0x60DB, 0xF474, 0xAC45, 0x0A2D, 0xBFCE,
    0x4848, 0x8093, 0x577F, 0x75CB, 0x2727, 0x898B, 0xF1D3, 0x2781,

    0xCC91, 0xC1DA, 0x2F0A, 0x933F, 0xE304, 0x8432, 0x7602, 0x4CB3,     // Ending
    0xE3A0, 0xFA3F, 0x73BA, 0x6F6D, 0xB144, 0xAB32, 0x5C31, 0x7D9D      // PAD
};

const short h[2*NH + 2*PAD] =
{
    0x76E2, 0x8B0E, 0x4A09, 0xBA35, 0xEB22, 0xE6EB, 0x8CCF, 0xDF71,     // Beginning
    0xAACF, 0x3CE2, 0xADDA, 0x3E77, 0x8CA7, 0x6C43, 0x0F68, 0xCCF6,     // PAD

    0x5A0A, 0xCD7B, 0x8397, 0xA5BC, 0xB196, 0x963C, 0x61C8, 0xE43A,
    0x1C8B, 0x55F2, 0x32B9, 0xB3DD, 0xE468, 0x64CE, 0x3F29, 0x2815,
    0x99CB, 0x813E, 0xCAB7, 0xE031, 0xE954, 0x8AF2, 0x9943, 0x2780,
    0x8DC1, 0x2C18, 0xA0EA, 0x6371, 0x9033, 0x2F1F, 0xF36D, 0xB972,
    0x5B46, 0x9837, 0x6009, 0x7354, 0x7CBB, 0xCB4E, 0x4D5F, 0x90E4,
    0x27B1, 0xCBF3, 0xD4CD, 0xCA53, 0xD3A7, 0xDD56, 0xEF72, 0x866E,

    0xCC91, 0xC1DA, 0x2F0A, 0x933F, 0xE304, 0x8432, 0x7602, 0x4CB3,     // Ending
    0xE3A0, 0xFA3F, 0x73BA, 0x6F6D, 0xB144, 0xAB32, 0x5C31, 0x7D9D      // PAD
};

short r_c[2*NR + 2*PAD];
short r_i[2*NR + 2*PAD];

/* ======================================================================= */
/* Generate pointers to skip beyond array padding                          */
/* ======================================================================= */
const short *const ptr_x = x + 2*(NH-1) + PAD;
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

    for (nr = 8, i = 1; nr <= NR; nr += 4) {
        for (nh = 8; nh <= NH; i++, nh += 4) {
            /* =================================================================== */
            /* Force uninitialized arrays to fixed values                          */
            /* =================================================================== */
            memset(r_c, 0x5A, sizeof(r_c));
            memset(r_i, 0x5A, sizeof(r_i));

            /* =================================================================== */
            /* Call the individual timing routines, and subtract off overhead      */
            /* =================================================================== */
            t_c = time_c(nh, nr) - t_overhead;  // Calculate time to run Natural C Algorithm
            t_i = time_i(nh, nr) - t_overhead;  // Calculate time to run Optimized C Algorithm

            /* =================================================================== */
            /* Print timing results                                                */
            /* =================================================================== */
            printf("DSP_fir_cplx_hM4X4\tIter#: %d\t", i);

            /* =================================================================== */
            /* Check the results arrays, and report any failures                   */
            /* =================================================================== */
            if (memcmp(r_i, r_c, sizeof(r_c))) {
                fail++;
                printf("Result Failure (r_c)");
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
    DSP_fir_cplx_hM4X4_cn(ptr_x, ptr_h, ptr_r_c, nh, nr);
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
    DSP_fir_cplx_hM4X4(ptr_x, ptr_h, ptr_r_i, nh, nr);
    t_stop = clock();
    return t_stop - t_start;
}

/* ======================================================================= */
/*  End of file:  DSP_fir_cplx_hM4X4_d.c                                   */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

