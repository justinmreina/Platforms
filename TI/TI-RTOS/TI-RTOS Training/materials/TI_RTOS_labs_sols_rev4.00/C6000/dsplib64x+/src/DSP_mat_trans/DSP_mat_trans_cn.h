/* ======================================================================= */
/* DSP_mat_trans_cn.h -- Matrix Transpose                                  */
/*                       Natural C Implementation                          */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_MAT_TRANS_CN_H_
#define _DSP_MAT_TRANS_CN_H_

void DSP_mat_trans_cn (
    const short *restrict   x,          /* Input matrix             */
    short                   rows,       /* Height of input matrix   */
    short                   columns,    /* Width of input matrix    */
    short *restrict         r           /* Output matrix            */
);

#endif /* _DSP_MAT_TRANS_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_mat_trans_cn.h                                       */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

