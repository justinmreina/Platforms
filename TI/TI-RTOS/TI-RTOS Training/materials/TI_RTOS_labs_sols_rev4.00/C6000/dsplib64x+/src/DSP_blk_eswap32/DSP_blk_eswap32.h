/* ======================================================================= */
/* DSP_blk_eswap32.h -- Endian-swap a block of 32-bit values               */
/*                      Intrinsic C Implementation                         */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

#ifndef DSP_BLK_ESWAP32_H_
#define DSP_BLK_ESWAP32_H_ 1

void DSP_blk_eswap32 (
    void *restrict src,
    void *restrict dst,
    int  n_words
);

#endif

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap32.h                                         */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

