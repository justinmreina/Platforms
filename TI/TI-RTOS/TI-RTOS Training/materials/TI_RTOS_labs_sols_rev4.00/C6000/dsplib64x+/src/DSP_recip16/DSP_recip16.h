/* ======================================================================= */
/* DSP_recip16.h -- Reciprocal Function                                    */
/*                  Optimized C Implementation (w/ Intrinsics)             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_RECIP16_H_
#define _DSP_RECIP16_H_ 1

void DSP_recip16 (
    const short *restrict x,    /* Input array                       */
    short *restrict rfrac,      /* Output array containg Fractions   */
    short *restrict rexp,       /* Output array containing Exponents */
    short nx                    /* Number of elements in arrays      */
);

#endif /* _DSP_RECIP16_H_ */
/* ======================================================================= */
/*  End of file:  DSP_recip16.h                                            */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

