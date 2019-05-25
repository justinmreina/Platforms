/* ======================================================================= */
/* DSP_minerror_cn.h -- Minimum Energy Error Search                        */
/*                      Natural C Implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_MINERROR_CN_H_
#define _DSP_MINERROR_CN_H_ 1

int DSP_minerror_cn (
    const short *restrict GSP0_TABLE, /* Pointer to GSP0 terms array          */
    const short *restrict errCoefs,   /* Array of error coefficients          */
    int *restrict max_index           /* Index to the 9-element vector that   */
                                      /*  resulted in the maximum dot product */
);

#endif /* _DSP_MINERROR_CN_H_ */

/* ======================================================================= */
/*  End of file:  DSP_fir_cplx_cn.h                                        */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

