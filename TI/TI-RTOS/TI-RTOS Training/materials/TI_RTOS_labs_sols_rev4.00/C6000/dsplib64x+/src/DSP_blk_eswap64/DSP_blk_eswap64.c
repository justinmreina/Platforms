/* ======================================================================= */
/*  TEXAS INSTRUMENTS, INC.                                                */
/*                                                                         */
/*  DSPLIB  DSP Signal Processing Library                                  */
/*                                                                         */
/*  This library contains proprietary intellectual property of Texas       */
/*  Instruments, Inc.  The library and its source code are protected by    */
/*  various copyrights, and portions may also be protected by patents or   */
/*  other legal protections.                                               */
/*                                                                         */
/*  This software is licensed for use with Texas Instruments TMS320        */
/*  family DSPs.  This license was provided to you prior to installing     */
/*  the software.  You may review this license by consulting the file      */
/*  TI_license.PDF which accompanies the files in this library.            */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* DSP_blk_eswap64.c -- Endian-swap a block of 64-bit values               */
/*                      Intrinsic C Implementation                         */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine has the following C prototype:                        */
/*                                                                         */
/*          void DSP_blk_eswap64                                           */
/*          (                                                              */
/*              double *restrict src, // Source data                       */
/*              double *restrict dst, // Destination array                 */
/*              int    n_dbls         // Number of double-words to swap    */
/*          );                                                             */
/*                                                                         */
/*      This function performs an endian-swap on the data in the "src"     */
/*      array, writing the results to "dst".  If NULL is passed in for     */
/*      the destination, then the endian-swap is performed in-place.       */
/*      The "n_dbls" argument gives the total length of the array.         */
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
/*      The input array and output array are expected to be double-word    */
/*      aligned, and a multiple of 2 double-words must be processed.       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_blk_eswap64, ".text:intrinsic");

void DSP_blk_eswap64 (
    void *restrict src,
    void *restrict dst,
    int n_dbls
)
{
    int i;
    char *_src, *_dst;

    _nassert(n_dbls >= 2);
    _nassert(n_dbls % 2 == 0);

    _src = (char *)src;
    _dst = (char *)dst;
    
    if (dst) {
        _nassert((int)_src % 8 == 0);
        _nassert((int)_dst % 8 == 0);
        #pragma UNROLL(2);
        for (i = 0; i < n_dbls; i++)
            _amemd8(&_dst[i * 8]) =
                _itod(_rotl(_swap4(_lo(_amemd8(&_src[i * 8]))),16),
                      _rotl(_swap4(_hi(_amemd8(&_src[i * 8]))),16));
    }
    else {
        _nassert((int)_src % 8 == 0);
        #pragma UNROLL(2);
        for (i = 0; i < n_dbls; i++)
            _amemd8(&_src[i * 8]) =
                _itod(_rotl(_swap4(_lo(_amemd8(&_src[i * 8]))),16),
                      _rotl(_swap4(_hi(_amemd8(&_src[i * 8]))),16));
    }
}

/* ======================================================================== */
/*  End of file:  DSP_blk_eswap64.c                                         */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

