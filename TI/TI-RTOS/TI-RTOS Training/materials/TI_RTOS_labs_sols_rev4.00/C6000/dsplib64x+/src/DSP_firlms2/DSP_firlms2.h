/* ======================================================================= */
/* DSP_firlms2.h -- Least Mean Square Adaptive Filter                      */
/*                  Intrinsic C source                                     */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FIRLMS2_H_
#define DSP_FIRLMS2_H_ 1

long DSP_firlms2 (
    short *restrict h,  /* Filter Coefficients */
    short *restrict x,  /* Input Data          */
    short           b,  /* Error from previous FIR */
    int             nh  /* Number of Coefficients  */
);

#endif /* DSP_FIRLMS2_H_ */

/* ======================================================================== */
/*  End of file:  DSP_firlms2.h                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

