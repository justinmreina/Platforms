/* ======================================================================= */
/* DSP_blk_move_cn.c -- Move a block of memory                             */
/*                      Natural C Implementation                           */
/*                                                                         */
/* Rev 0.0.1                                                               */
/*                                                                         */
/*  USAGE                                                                  */
/*      This routine has the following C prototype:                        */
/*                                                                         */
/*          void DSP_blk_move_cn                                           */
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
/*      None                                                               */
/*                                                                         */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */
#pragma CODE_SECTION(DSP_blk_move_cn, ".text:ansi");

void DSP_blk_move_cn (
    const short *restrict x,
    short *restrict r,
    int nx
)
{
    int i;

#ifndef NOASSUME
    _nassert(nx % 8 == 0);
    _nassert(nx >= 8);
    _nassert((int)x % 8 == 0);
    _nassert((int)r % 8 == 0);
#endif

    for (i = 0 ; i < nx; i++)
        r[i] = x[i];
}

/* ======================================================================= */
/*  End of file:  DSP_blk_move_cn.c                                        */
/* ----------------------------------------------------------------------- */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ======================================================================= */

