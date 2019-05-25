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
/* DSP_fltoq15.c -- Float to Q15 conversion                                */
/*                  Optimized C Implementation (w/ Intrinsics)             */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fltoq15 (                                                  */
/*         const float *restrict flt, // Input float array                 */
/*         short *restrict q15,       // Output Q15 array                  */
/*         short nx                   // Number of elements                */
/*     );                                                                  */
/*                                                                         */
/*     Convert the IEEE floating point numbers stored in vector x[] into   */
/*     Q.15 format numbers stored in vector r[]. Results are truncated     */
/*     towards zero. Values that exceed the size limit will be saturated   */
/*     to 0x7fff if value is positive and 0x8000 if value is negative.     */
/*     All values too small to be correctly represented will be truncated  */
/*     to 0.                                                               */
/*                                                                         */
/*     The 16-bit Q.15 format is defined as follows:                       */
/*                                                                         */
/*         1 11111                                                         */
/*         5 432109876543210                                               */
/*         S.XXXXXXXXXXXXXXX                                               */
/*                                                                         */
/*         range: 1.000000000000000 = -1.0               <-> -32768        */
/*                0.111111111111111 =  0.999969482421875 <->  32767        */
/*                                                                         */
/*     IEEE floating point format is defined as follows:                   */
/*                                                                         */
/*         31 | 30....23 | 22.....0                                        */
/*         S  | EXPONENT | MANTISSA                                        */
/*                                                                         */
/*     The value is obtained as: (-1)^S * 1.MANTISSA * 2^(EXPONENT-127)    */
/*                                                                         */
/*  Assumptions                                                            */
/*     Arrays flt and q15 do not overlap                                   */
/*     nx >= 2    nx % 2 == 0                                              */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fltoq15, ".text:intrinsic");

#include "DSP_fltoq15.h"

void DSP_fltoq15 (
    const float *restrict flt,  /* Input float array    */
    short *restrict q15,        /* Output Q15 array     */
    short nx                    /* Number of elements   */
)
{
    unsigned int *flt_ptr = (void *)flt;
    unsigned int x;
    unsigned int mantissa;
    unsigned int exponent;
    unsigned int i;
    int shift;

    _nassert(nx % 2 == 0);
    _nassert(nx >= 2);
    #pragma MUST_ITERATE(2,,2)
    for(i = 0; i < nx; i += 1) {
        x = *(flt_ptr++);

        /* extract the unsigned mantissa (bit 22..0) */
        mantissa = _clr(x, 23, 31);

        /* extract the offset 127 exponent (bit 30..23) */
        exponent = _extu(x, 1, 24);

        /* set the implied 1 bit in the mantissa */
        mantissa |= 0x00800000;

        /* shift to a positive 16-bit fraction */
        shift = 135 - exponent;
        mantissa = _sshvr(mantissa, shift);

        /* negate the fraction if float was negative */
        if(0x80000000 & x) mantissa = -mantissa;

        /* saturate to signed 16-bit */
        mantissa = _spack2(0, mantissa);

        /* store in the output array */
        *(q15++) = (short)mantissa;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fltoq15.c                                            */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

