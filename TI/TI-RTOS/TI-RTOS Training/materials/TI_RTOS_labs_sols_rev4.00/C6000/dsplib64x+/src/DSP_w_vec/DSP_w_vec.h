/* ======================================================================= */
/* DSP_w_vec.h -- Weighted Vector Sum                                      */
/*                Intrinsic C Implementation                               */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_W_VEC_H_
#define DSP_W_VEC_H_ 1

void DSP_w_vec (
    const short * restrict x,   /* Vector being weighted  */
    const short * restrict y,   /* Non-weighted vector    */
    short m,                    /* weighting factor       */
    short * restrict r,         /* Result vector          */
    int nr                      /* Number of samples.     */
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_w_vec.h                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

