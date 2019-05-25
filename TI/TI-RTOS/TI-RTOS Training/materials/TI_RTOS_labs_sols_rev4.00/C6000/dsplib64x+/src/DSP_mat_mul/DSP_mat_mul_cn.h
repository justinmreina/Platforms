/* ======================================================================= */
/* DSP_mat_mul_cn.h -- Perform Matrix Multiplication                       */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_MAT_MUL_CN_H_
#define DSP_MAT_MUL_CN_H_ 1

void mat_mul_cn (
    const short *restrict x, int r1, int c1,
    const short *restrict y,         int c2,
    short       *restrict r,
    int                   qs
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_mat_mul_cn.h                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

