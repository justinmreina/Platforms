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
/* DSP_bexp_i.c -- Block exponent of a vector                              */
/*                 Intrinsic C Implementation                              */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     int DSP_bexp                                                        */
/*     (                                                                   */
/*         const int *x,     // Input Data                                 */
/*         short      nx     // Number of elements                         */
/*     );                                                                  */
/*                                                                         */
/*  Description                                                            */
/*     The bexp function, performs a determination of the block            */
/*     exponent of the vector of elements and returns the maximum          */
/*     exponent.  This information can then be used to re-normalize        */
/*     the vector.  This operation is useful when auto-scaling is          */
/*     required as in a FFT.  The bexp operation returns the minimum       */
/*     norm of the vector.  The minimum norm corresponds to the            */
/*     maximum exponent.                                                   */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays x and r do not overlap                                       */
/*     nx % 8 == 0.                                                        */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_bexp, ".text:intrinsic");

int DSP_bexp (
    const int *x,   /* Input data           */
    short nx        /* Number of elements   */
)
{
    int i;
    double mask_10;
    int unsigned mask;

    /* -------------------------------------------------------------------- */
    /*  Merge the absolute values of all the elements together.  The        */
    /*  leftmost bit-change on this mask gives us the magnitude we desire.  */
    /* -------------------------------------------------------------------- */
    _nassert(nx % 8 == 0);
    #pragma MUST_ITERATE(4,,4)
    for (i = mask = 0; i <= nx; i += 2) {
        mask_10 = _amemd8_const(&x[i]);
        mask |= _lo(mask_10) ^ _sshvr(_lo(mask_10), 31);
        mask |= _hi(mask_10) ^ _sshvr(_hi(mask_10), 31);
    }

    /* -------------------------------------------------------------------- */
    /*  Return the magnitude of the mask.                                   */
    /* -------------------------------------------------------------------- */
    return _norm(mask);
}

/* ======================================================================= */
/*  End of file:  DSP_bexp_i.c                                             */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

