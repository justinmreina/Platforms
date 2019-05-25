/* ======================================================================= */
/* DSP_fltoq15_cn.c -- Float to Q15 conversion                             */
/*                     Natural C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  Usage                                                                  */
/*     This routine is C-callable and can be called as:                    */
/*                                                                         */
/*     void DSP_fltoq15_cn (                                               */
/*         const float *restrict flt, // Input float array                 */
/*         short *restrict q15,       // Output Q15 array                  */
/*         short nx                   // Number of elements                */
/*     );                                                                  */
/*                                                                         */
/*  Description                                                            */
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
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_fltoq15_cn, ".text:ansi");

#include "DSP_fltoq15_cn.h"

void DSP_fltoq15_cn (
    const float *restrict flt,  /* Input float array    */
    short *restrict q15,        /* Output Q15 array     */
    short nx                    /* Number of elements   */
)
{
    int i, a;

#ifndef NOASSUME
    _nassert((int)flt % 4 == 0);
    _nassert((int)flt >= 4);

    _nassert((int)q15 % 4 == 0);
    _nassert((int)q15 >= 4);

    _nassert(nx % 4 == 0);
    _nassert(nx >= 4);

    #pragma MUST_ITERATE(4,,4);
#endif
    for(i = 0; i < nx; i++) {
        /* results in saturated integer if overflow */
        a = 32768 * flt[i];

        /* saturate to 16-bit */
        if (a >  32767) a =  32767;
        if (a < -32768) a = -32768;

        q15[i] = (short)a;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_fltoq15_cn.c                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

