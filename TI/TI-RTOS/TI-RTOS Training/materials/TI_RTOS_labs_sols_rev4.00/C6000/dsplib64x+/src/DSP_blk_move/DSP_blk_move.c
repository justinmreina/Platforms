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
/* DSP_blk_move.c -- Move a block of memory                                */
/*                   Intrinsic C Implementation                            */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine has the following C prototype:                        */
/*                                                                         */
/*          void DSP_blk_move                                              */
/*          (                                                              */
/*              short *restrict x,   // Block of data to move              */
/*              short *restrict r,   // Destination of block of data       */
/*              int   nx             // Number of elements in block        */
/*          );                                                             */
/*                                                                         */
/*  DESCRIPTION                                                            */
/*      Move nx 16-bit elements from one memory location                   */
/*      to another.                                                        */
/*                                                                         */
/*      void blk_move(short *x,short *r, int nx)                           */
/*      {                                                                  */
/*          int i;                                                         */
/*          for (i = 0 ; i < nx; i++){                                     */
/*              r[i]=x[i];                                                 */
/*          }                                                              */
/*      }                                                                  */
/*                                                                         */
/*  ASSUMPTIONS                                                            */
/*      nx >= 8;    nx % 8 == 0                                            */
/*      Assumes little ENDIAN format of bytes                              */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_blk_move, ".text:intrinsic");

void DSP_blk_move (
    short * restrict x,
    short * restrict r,
    int nx
)
{
    int i;
    double x3210, x7654;

	nx = nx >> 3;
    #pragma MUST_ITERATE(1,,1);
    for (i = 0; i < nx; i++) {
        x3210 = _amemd8_const(&x[i*8+0]);
        x7654 = _amemd8_const(&x[i*8+4]);
        _amemd8(&r[i*8+0]) = x3210;
        _amemd8(&r[i*8+4]) = x7654;
    }
}

/* ======================================================================= */
/*  End of file:  DSP_blk_move.c                                           */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

