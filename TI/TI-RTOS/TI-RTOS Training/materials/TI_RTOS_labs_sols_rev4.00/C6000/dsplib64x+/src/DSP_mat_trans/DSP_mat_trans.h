/* ======================================================================= */
/* DSP_mat_trans.h -- Matrix Transpose                                     */
/*                    Optimized C Implementation (w/ Intrinsics)           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_MAT_TRANS_H_
#define _DSP_MAT_TRANS_H_ 1

void DSP_mat_trans (
    const short *restrict   x,          /* Input matrix             */
    short                   rows,       /* Height of input matrix   */
    short                   columns,    /* Width of input matrix    */
    short *restrict         r           /* Output matrix            */
);

#endif /* _DSP_MAT_TRANS_H_ */

/* ======================================================================= */
/*  End of file:  DSP_mat_trans.h                                          */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

