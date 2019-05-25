/* ======================================================================= */
/* DSP_firlms2_cn.h -- Least Mean Square Adaptive Filter                   */
/*                     Natural C source                                    */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_FIRLMS2_CN_H_
#define DSP_FIRLMS2_CN_H_ 1

long DSP_firlms2_cn (
    short       * h,  /* Filter Coefficients */
    const short * x,  /* Input Data          */
    short         b,  /* Error from previous FIR */
    int           nh  /* Number of Coefficients  */
);

#endif /* DSP_FIRLMS2_CN_H_ */

/* ======================================================================== */
/*  End of file:  DSP_firlms2_cn.h                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

