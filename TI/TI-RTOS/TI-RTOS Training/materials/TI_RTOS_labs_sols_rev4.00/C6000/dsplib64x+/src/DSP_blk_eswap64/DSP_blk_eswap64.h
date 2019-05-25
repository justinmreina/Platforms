/* ======================================================================= */
/* DSP_blk_eswap64.h -- Endian-swap a block of 64-bit values               */
/*                      Intrinsic C Implementation                         */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_BLK_ESWAP64_H_
#define DSP_BLK_ESWAP64_H_ 1

void DSP_blk_eswap64 (
    void *restrict src,
    void *restrict dst,
    int    n_dbls
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap64.h                                         */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

