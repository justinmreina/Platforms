/* ======================================================================== */
/*  NAME                                                                    */
/*      DSP_autocor_cn -- Autocorrelation                                   */
/*                                                                          */
/*   USAGE                                                                  */
/*        This routine has the following C prototype:                       */
/*                                                                          */
/*        void DSP_autocor_cn (                                             */
/*            short *restrict r,                                            */
/*            const short *restrict x,                                      */
/*            int          nx,                                              */
/*            int          nr                                               */
/*         );                                                               */
/*                                                                          */
/*        r[nr]   : Output array                                            */
/*        x[nr+nx]: Input array. The first nr elements are assumed to be 0. */
/*        nx      : Length of autocorrelation                               */
/*        nr      : Number of lags                                          */
/*                                                                          */
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
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_autocor_cn, ".text:ansi");

#include "DSP_autocor_cn.h"

void DSP_autocor_cn (
    short *restrict r,
    const short *restrict x,
    int nx,
    int nr
)
{
   int i, k;
   int sum;

   #ifndef NOASSUME
   _nassert (nr % 4 == 0);
   #endif

   /*----------------------------------------------------------------------*/
   /* Outer loop that iterates for every output. There are 'nr' output     */
   /* samples to be computed. Hence this loop iterates 'nr' times. 'nr'    */
   /* needs to be a multiple of 4. This information is conveyed to the     */
   /* compiler.                                                            */
   /*----------------------------------------------------------------------*/ 
   for (i = 0; i < nr; i++) {
      sum = 0;

      #ifndef NOASSUME
      _nassert(nx % 8 == 0);
      _nassert((int)(x) % 8 == 0);
      _nassert((int)(r) % 4 == 0);
      _nassert(nr % 4 == 0);
      #pragma MUST_ITERATE(8,,4);
      #endif

      /*-------------------------------------------------------------------*/
      /*  Compute one autocorrelation. Each autocorrelation has 'nx' terms */
      /*  'nx' is assumed to be a multiple of 8.                           */
      /*-------------------------------------------------------------------*/
      for (k = nr; k < nx + nr; k++)
          sum += x[k] * x[k-i];

      /*-------------------------------------------------------------------*/
      /* Shift out autocorrelation sum taking into account Q15 math.       */
      /*-------------------------------------------------------------------*/
      r[i] = sum  >> 15;
   }
}

/* ======================================================================== */
/*  End of file:  DSP_autocor_cn.c                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

