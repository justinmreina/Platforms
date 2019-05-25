/* ======================================================================= */
/* DSP_iir_lat_cn.c -- IIR Lattice Filter                                  */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*      This routine is C-callable and can be called as:                   */
/*                                                                         */
/*      void DSP_iirlat_cn (                                               */
/*          short       *x,                                                */
/*          int          nx,                                               */
/*          const short *restrict k,                                       */
/*          int          nk,                                               */
/*          int         *restrict b,                                       */
/*          short       *r                                                 */
/*      );                                                                 */
/*                                                                         */
/*      x[nx]   : Input vector (16-bit)                                    */
/*      nx      : Length of input vector.                                  */
/*      k[nk]   : Reflection coefficients in Q.15 format                   */
/*      nk      : Number of reflection coefficients/lattice stages         */
/*      b[nk+1] : Delay line elements from previous call. Should be        */
/*                initialized to all zeros prior to the first call.        */
/*      r[nx]   : Output vector (16-bit)                                   */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      This routine implements a real all-pole IIR filter in lattice      */
/*      structure (AR lattice). The filter consists of nk lattice stages.  */
/*      Each stage requires one reflection coefficient k and one delay     */
/*      element b. The routine takes an input vector x[] and returns the   */
/*      filter output in r[]. Prior to the first call of the routine the   */
/*      delay elements in b[] should be set to zero. The input data may    */
/*      have to be pre-scaled to avoid overflow or achieve better SNR. The */
/*      reflections coefficients lie in the range -1.0 < k < 1.0. The      */
/*      order of the coefficients is such that k[nk-1] corresponds to the  */
/*      first lattice stage after the input and k[0] corresponds to the    */
/*      last stage.                                                        */
/*                                                                         */
/* NOTE                                                                    */
/*     The all-pass output of the filter could be easily obtained by       */
/*     returning b[nk]. See also Matlab documentation of lattic filters.   */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_iir_lat_cn, ".text:ansi");

#include "DSP_iir_lat_cn.h"

void DSP_iir_lat_cn (
    short       *x,
    int         nx,
    const short *restrict k,
    int         nk,
    int         *restrict b,
    short       *r
)
{
    int rt;
    int i, j;

    for (j = 0; j < nx; j++) {
        rt = x[j] << 15;

        for (i = nk - 1; i >= 0; i--) {
            rt = rt - (short)(b[i] >> 15) * k[i];
            b[i + 1] = b[i] + (short)(rt >> 15) * k[i];
        }

        b[0] = rt;
        r[j] = rt >> 15;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_iir_lat_cn.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

