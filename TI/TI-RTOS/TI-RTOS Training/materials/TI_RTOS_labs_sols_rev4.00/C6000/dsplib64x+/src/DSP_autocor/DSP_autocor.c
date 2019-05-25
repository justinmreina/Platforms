/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  DSPLIB  DSP Signal Processing Library                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  NAME                                                                    */
/*      autocor -- Autocorrelation                                          */
/*                                                                          */
/*   USAGE                                                                  */
/*        This routine has the following C prototype:                       */
/*                                                                          */
/*        void DSP_autocor (                                                */
/*            short *restrict r,                                            */
/*            const short *restrict x,                                      */
/*            int          nx,                                              */
/*            int          nr                                               */
/*        );                                                                */
/*                                                                          */
/*        r[nr]   : Output array                                            */
/*        x[nr+nx]: Input array. The first nr elements are assumed to be 0. */
/*        nx      : Length of autocorrelation                               */
/*        nr      : Number of lags                                          */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This routine performs an autocorrelation of an input vector       */
/*        x. The length of the autocorrelation is nx samples. Since nr      */
/*        such autocorrelations are performed, input vector x needs to be   */
/*        of length nx + nr. This produces nr output results which are      */
/*        stored in an output array r.                                      */
/*                                                                          */
/*        The following diagram illustrates how the correlations are        */
/*        obtained.                                                         */
/*                                                                          */
/*        Example for nr=8, nx=24:                                          */
/*        0       nr                  nx+nr-1                               */
/*        |-------|----------------------|  <- x[]                          */
/*        |       |----------------------|  -> r[0]                         */
/*        |      |----------------------|   -> r[1]                         */
/*        |     |----------------------|    -> r[2]                         */
/*        |    |----------------------|     -> r[3]                         */
/*        |   |----------------------|      -> r[4]                         */
/*        |  |----------------------|       -> r[5]                         */
/*        | |----------------------|        -> r[6]                         */
/*                                                                          */
/*        Note that x[0] is never used, but is required for padding to make */
/*        x[nr] double-word aligned.                                        */
/*                                                                          */
/*   ASSUMPTIONS                                                            */
/*        The first nr elements are assumed to be 0.                        */
/*        NX >= 8;    NX % 8 == 0                                           */
/*        NR >= 4;    NR % 4 == 0                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_autocor, ".text:intrinsic");

#include "DSP_autocor.h"

void DSP_autocor (
    short *restrict r,
    const short *restrict x,
    int nx,
    int nr
)
{
    int k, ik;
    int lk;
    int j;

    unsigned sum_0, sum_2;
    unsigned sum_1, sum_3;
    unsigned temp_0, temp_2;
    unsigned temp_1, temp_3;
    unsigned pack_0, pack_1;

    unsigned int * restrict rptr;
    unsigned int x1x0, x3x2, x5x4, x7x6;
    unsigned int y1y0, y5y4, y3y2, z0z1;

    long long y7y6_y5y4, y3y2_y1y0, z0z1_z2z3;
    long long y1y0_z0z1, y5y4_y3y2;
    long long x3x2_x1x0, x7x6_x5x4;
    long long prod1_prod5, prod8_prodC, prodA_prodE, prod3_prod7;
    long long prod0_prod4, prod2_prod6, prod9_prodD, prodB_prodF;

    /*----------------------------------------------------------------*/
    /*  Determine the number of inner and outer loops.                */
    /*----------------------------------------------------------------*/
    ik = nr >> 2;
    lk = nx >> 3;

    /*----------------------------------------------------------------*/
    /*  Point to output array                                         */
    /*----------------------------------------------------------------*/
    rptr = (unsigned int *)r;

    /*----------------------------------------------------------------*/
    /* Inform the compiler the assumptions that can be made. The      */
    /* assumptions on 'nr' and the pragma's are not redundant as      */
    /* alignment of pointers needs to be proven given that the data   */
    /* array 'x' is double word aligned. Inform compiler that 'nr'    */
    /* is a multiple of 4, 'nx' is a multiple of '8'. Also inform     */
    /* the compiler that input data array 'x' is dword aligned. The   */
    /* output array 'r' is word aligned.                              */
    /*----------------------------------------------------------------*/
    _nassert (nr % 4 == 0);
    _nassert (nr >= 4);
    _nassert (nx % 8 == 0);
    _nassert (nx >= 8);
    _nassert ((int)(x) % 8 == 0);
    _nassert ((int)(r) % 4 == 0);

    /*----------------------------------------------------------------*/
    /* Each iteration of the outer loop calculates 4 outputs.         */
    /*----------------------------------------------------------------*/
    #pragma MUST_ITERATE(1,,1);
    for (j = 0; j < ik; j++) {
        sum_0 = sum_1 = sum_2 = sum_3 = 0;

        /*----------------------------------------------------------------*/
        /* Each iteration of the inner loop accumulates the intermediate  */
        /* results for 8 halfwords.                                       */
        /*----------------------------------------------------------------*/
        #pragma MUST_ITERATE(1,,1);
        for (k = 0; k < lk; k++)  {
            /*----------------------------------------------------------*/
            /* Load input data using double word wide accesses using    */
            /* _amem intrinsic. Use the _loll and _hill parts to get to */
            /* individual halves.                                       */
            /*----------------------------------------------------------*/
            x3x2_x1x0 = _amem8_const(&x[nr + 8*k]);
            x7x6_x5x4 = _amem8_const(&x[nr + 8*k + 4]);

            /*----------------------------------------------------------*/
            /* This part loads the data required for performing correl- */
            /* ation. To begin with 'i' the lag is zero. 'i' increments */
            /* by 1 after every 'nx' iterations of the loop to compute  */
            /* 'nr' lags.                                               */
            /*----------------------------------------------------------*/
            y3y2_y1y0 = _amem8_const(&x[nr + 8*k - 4*j]);
            y7y6_y5y4 = _amem8_const(&x[nr + 8*k - 4*j + 4]);
            z0z1_z2z3 = _amem8_const(&x[nr + 8*k - 4*j - 4]);

            /*----------------------------------------------------------*/
            /* Break down the 64 bit input data into two 32-bit values  */
            /*----------------------------------------------------------*/
            x1x0 = _loll(x3x2_x1x0);
            x3x2 = _hill(x3x2_x1x0);
            x5x4 = _loll(x7x6_x5x4);
            x7x6 = _hill(x7x6_x5x4);

            y1y0 = _loll(y3y2_y1y0);
            y3y2 = _hill(y3y2_y1y0);
            y5y4 = _loll(y7y6_y5y4);
            z0z1 = _hill(z0z1_z2z3);

            /*---------------------------------------------------------*/
            /* Prepare the other combinations of data required using   */
            /* _dmv.                                                   */
            /*---------------------------------------------------------*/
            y1y0_z0z1 = _dmv(y1y0, z0z1);
            y5y4_y3y2 = _dmv(y5y4, y3y2);

            /*---------------------------------------------------------*/
            /* sum_0 += x[nr + 8*k]     * x[nr + 8*k - 4*i];           */
            /* sum_0 += x[nr + 8*k + 1] * x[nr + 8*k - 4*i + 1];       */
            /* sum_0 += x[nr + 8*k + 2] * x[nr + 8*k - 4*i + 2];       */
            /* sum_0 += x[nr + 8*k + 3] * x[nr + 8*k - 4*i + 3];       */
            /* sum_0 += x[nr + 8*k + 4] * x[nr + 8*k - 4*i + 4];       */
            /* sum_0 += x[nr + 8*k + 5] * x[nr + 8*k - 4*i + 5];       */
            /* sum_0 += x[nr + 8*k + 6] * x[nr + 8*k - 4*i + 6];       */
            /* sum_0 += x[nr + 8*k + 7] * x[nr + 8*k - 4*i + 7];       */
            /*                                                         */
            /* sum_1 += x[nr + 8*k]     * x[nr + 8*k - 4*i - 1];       */
            /* sum_1 += x[nr + 8*k + 1] * x[nr + 8*k - 4*i];           */
            /* sum_1 += x[nr + 8*k + 2] * x[nr + 8*k - 4*i + 1];       */
            /* sum_1 += x[nr + 8*k + 3] * x[nr + 8*k - 4*i + 2];       */
            /* sum_1 += x[nr + 8*k + 4] * x[nr + 8*k - 4*i + 3];       */
            /* sum_1 += x[nr + 8*k + 5] * x[nr + 8*k - 4*i + 4];       */
            /* sum_1 += x[nr + 8*k + 6] * x[nr + 8*k - 4*i + 5];       */
            /* sum_1 += x[nr + 8*k + 7] * x[nr + 8*k - 4*i + 6];       */
            /*                                                         */
            /* sum_2 += x[nr + 8*k]     * x[nr + 8*k - 4*i - 2];       */
            /* sum_2 += x[nr + 8*k + 1] * x[nr + 8*k - 4*i - 1];       */
            /* sum_2 += x[nr + 8*k + 2] * x[nr + 8*k - 4*i];           */
            /* sum_2 += x[nr + 8*k + 3] * x[nr + 8*k - 4*i + 1];       */
            /* sum_2 += x[nr + 8*k + 4] * x[nr + 8*k - 4*i + 2];       */
            /* sum_2 += x[nr + 8*k + 5] * x[nr + 8*k - 4*i + 3];       */
            /* sum_2 += x[nr + 8*k + 6] * x[nr + 8*k - 4*i + 4];       */
            /* sum_2 += x[nr + 8*k + 7] * x[nr + 8*k - 4*i + 5];       */
            /*                                                         */
            /* sum_3 += x[nr + 8*k]     * x[nr + 8*k - 4*i - 3];       */
            /* sum_3 += x[nr + 8*k + 1] * x[nr + 8*k - 4*i - 2];       */
            /* sum_3 += x[nr + 8*k + 2] * x[nr + 8*k - 4*i - 1];       */
            /* sum_3 += x[nr + 8*k + 3] * x[nr + 8*k - 4*i];           */
            /* sum_3 += x[nr + 8*k + 4] * x[nr + 8*k - 4*i + 1];       */
            /* sum_3 += x[nr + 8*k + 5] * x[nr + 8*k - 4*i + 2];       */
            /* sum_3 += x[nr + 8*k + 6] * x[nr + 8*k - 4*i + 3];       */
            /* sum_3 += x[nr + 8*k + 7] * x[nr + 8*k - 4*i + 4];       */
            /*---------------------------------------------------------*/
            prod1_prod5 = _ddotph2(y3y2_y1y0, x3x2);
            prod8_prodC = _ddotph2(z0z1_z2z3, x1x0);

            prodA_prodE = _ddotph2(y3y2_y1y0, x5x4);
            prod3_prod7 = _ddotph2(y7y6_y5y4, x7x6);

            prod0_prod4 = _ddotph2(y1y0_z0z1, x1x0);
            prod2_prod6 = _ddotph2(y5y4_y3y2, x5x4);

            prod9_prodD = _ddotph2(y1y0_z0z1, x3x2);
            prodB_prodF = _ddotph2(y5y4_y3y2, x7x6);

            temp_0 = _hill(prod0_prod4) + _hill(prod1_prod5) + _hill(prod2_prod6) + _hill(prod3_prod7);
            temp_1 = _loll(prod0_prod4) + _loll(prod1_prod5) + _loll(prod2_prod6) + _loll(prod3_prod7);
            temp_2 = _hill(prod8_prodC) + _hill(prod9_prodD) + _hill(prodA_prodE) + _hill(prodB_prodF);
            temp_3 = _loll(prod8_prodC) + _loll(prod9_prodD) + _loll(prodA_prodE) + _loll(prodB_prodF);

            sum_0 += temp_0;
            sum_1 += temp_1;
            sum_2 += temp_2;
            sum_3 += temp_3;
        }

        /*---------------------------------------------------------*/
        /* Pack the results for storage                            */
        /*---------------------------------------------------------*/
        pack_0 = _packh2(sum_3 << 1, sum_2 << 1);
        pack_1 = _packh2(sum_1 << 1, sum_0 << 1);

        /*---------------------------------------------------------*/
        /* Store results away                                      */
        /*---------------------------------------------------------*/
        rptr[0] = pack_1;
        rptr[1] = pack_0;
        rptr = rptr+2;
    }
}

/* ======================================================================== */
/*  End of file:  DSP_autocor.c                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

