/* ======================================================================= */
/* DSP_mul32.h -- Optimized C source interface header file                 */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_MUL32_H_
#define DSP_MUL32_H_ 1

void DSP_mul32 (
    const int * restrict x,    /* Input array of length nx  */
    const int * restrict y,    /* Input array of length nx  */
    int       * restrict r,    /* Output array of length nx */
    int                  nx    /* Number of elements.       */
);

#endif /* DSP_MUL32_H_ */

/* ======================================================================== */
/*  End of file:  DSP_mul32.h                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

