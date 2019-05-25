/* ======================================================================= */
/* DSP_minerror.h -- Minimum Energy Error Search                           */
/*                   Optimized C Implementation (w/ Intrinsics)            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef _DSP_MINERROR_H_
#define _DSP_MINERROR_H_

int DSP_minerror (
    const short *restrict GSP0_TABLE,  /* Pointer to GSP0 terms array         */
    const short *restrict errCoefs,    /* Array of error coefficients         */
    int *restrict max_index            /* Index to the 9-element vector that  */
                                       /* resulted in the maximum dot product */
);

#endif /* _DSP_MINERROR_H_ */

/* ======================================================================= */
/*  End of file:  DSP_minerror.h                                           */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

