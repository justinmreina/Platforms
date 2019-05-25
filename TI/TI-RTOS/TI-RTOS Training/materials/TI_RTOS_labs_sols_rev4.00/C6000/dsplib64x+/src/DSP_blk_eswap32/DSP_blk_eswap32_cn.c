/* ======================================================================= */
/* DSP_blk_eswap32_cn.c -- Endian-swap a block of 32-bit values            */
/*                         Natural C Implementation                        */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine has the following C prototype:                        */
/*                                                                         */
/*          void DSP_blk_eswap32_cn                                        */
/*          (                                                              */
/*              void *restrict src,     // Source data                     */
/*              void *restrict dst,     // Destination array               */
/*              int  n_words            // Number of words to swap         */
/*          );                                                             */
/*                                                                         */
/*      This function performs an endian-swap on the data in the "src"     */
/*      array, writing the results to "dst".  If NULL is passed in for     */
/*      the destination, then the endian-swap is performed in-place.       */
/*      The "n_words" argument gives the total length of the array.        */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      The data in the "src" array is endian swapped, meaning that the    */
/*      byte-order of the bytes within each word of the src[] array is     */
/*      reversed.  This is meant to facilitate moving big-endian data      */
/*      to a little-endian system or vice-versa.                           */
/*                                                                         */
/*      When the "dst" pointer is non-NULL, the endian-swap occurs         */
/*      out-of-place, similar to a block move.  When the "dst" pointer     */
/*      is NULL, the endian-swap occurs in-place, allowing the swap to     */
/*      occur without using any additional storage.                        */
/*                                                                         */
/*  ASSUMPTIONS                                                            */
/*      Input and output arrays do not overlap, except in the very         */
/*      specific case that "dst == NULL" so that the operation occurs      */
/*      in-place.                                                          */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_blk_eswap32_cn, ".text:ansi");

void DSP_blk_eswap32_cn (
    void *restrict src,
    void *restrict dst,
    int  n_words
)
{
    int i;
    char *_src, *_dst;

#ifndef NOASSUME
    _nassert(n_words >= 4);
    _nassert(n_words % 4 == 0);
    _nassert((int)src % 8 == 0);
    _nassert((int)dst % 8 == 0);
#endif

    if (dst) {
        _src = (char *)src;
        _dst = (char *)dst;
    } else {
        _src = (char *)src;
        _dst = (char *)src;
    }

    for (i = 0; i < n_words; i++) {
        char t0, t1, t2, t3;
        t0 = _src[i*4 + 3];
        t1 = _src[i*4 + 2];
        t2 = _src[i*4 + 1];
        t3 = _src[i*4 + 0];

        _dst[i*4 + 0] = t0;
        _dst[i*4 + 1] = t1;
        _dst[i*4 + 2] = t2;
        _dst[i*4 + 3] = t3;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_blk_eswap32_cn.c                                     */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

