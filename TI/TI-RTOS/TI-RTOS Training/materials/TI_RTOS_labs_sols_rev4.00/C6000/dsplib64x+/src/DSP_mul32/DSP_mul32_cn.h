/* ======================================================================= */
/* DSP_mul32_cn.h -- Natural C Implementation interface header file        */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_MUL32_CN_H_
#define DSP_MUL32_CN_H_ 1

void DSP_mul32_cn (
    const int    *x,   /* Input array of length nx  */
    const int    *y,   /* Input array of length nx  */
    int *restrict r,   /* Output array of length nx */
    int           nx   /* Number of elements.       */
);

#endif /* DSP_MUL32_CN_H_ */

/* ======================================================================== */
/*  End of file:  DSP_mul32_cn.h                                            */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

