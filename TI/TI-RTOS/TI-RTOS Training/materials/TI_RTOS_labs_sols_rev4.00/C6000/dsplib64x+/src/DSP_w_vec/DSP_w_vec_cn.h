/* ======================================================================= */
/* DSP_w_vec_cn.h -- Weighted Vector Sum                                   */
/*                   Natural C Implementation                              */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_W_VEC_CN_H_
#define DSP_W_VEC_CN_H_ 1

void DSP_w_vec_cn (
    const short *x,     /* Vector being weighted  */
    const short *y,     /* Non-weighted vector    */
    short m,            /* weighting factor       */
    short *r,           /* Result vector          */
    int nr              /* Number of samples.     */
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_w_vec_cn.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

