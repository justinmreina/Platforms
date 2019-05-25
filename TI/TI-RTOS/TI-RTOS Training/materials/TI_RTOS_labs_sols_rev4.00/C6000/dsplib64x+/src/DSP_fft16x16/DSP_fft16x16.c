/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  DSPLIB  DSP Signal Processing Library                                   */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      DSP_fft16x16 -- fft16x16                                            */
/*                                                                          */
/*      USAGE                                                               */
/*           This routine is C-callable and can be called as:               */
/*                                                                          */
/*          void DSP_fft16x16_i (                                           */
/*              const short * ptr_w,                                        */
/*              int npoints,                                                */
/*              short * ptr_x,                                              */
/*              short * ptr_y                                               */
/*          );                                                              */
/*                                                                          */
/*            ptr_w   =  input twiddle factors                              */
/*            npoints =  number of points                                   */
/*            ptr_x   =  transformed data reversed                          */
/*            ptr_y   =  linear transformed data                            */
/*                                                                          */
/*                                                                          */
/*  DESCRIPTION                                                             */
/*      The following code performs a mixed radix FFT for "npoints" which   */
/*      is either a multiple of 4 or 2. It uses logN4 - 1 stages of radix4  */
/*      transform and performs either a radix2 or radix4 transform on the   */
/*      last stage depending on "npoints". If "npoints" is a multiple of 4, */
/*      then this last stage is also a radix4 transform, otherwise it is a  */
/*      radix2 transform.                                                   */
/*                                                                          */
/*                                                                          */
/* int gen_twiddle_fft16x16(short *w, int n)                                */
/*                                                                          */
/*    int i, j, k;                                                          */
/*     double M = 32767.5;                                                  */
/*                                                                          */
/*    for (j = 1, k = 0; j < n >> 2; j = j << 2)                            */
/*    {                                                                     */
/*        for (i = 0; i < n >> 2; i += j << 1)                              */
/*        {                                                                 */
/*                                                                          */
/*          w[k + 11] = d2s(M * cos(6.0 * PI * (i + j) / n));               */
/*          w[k + 10] = d2s(M * sin(6.0 * PI * (i + j) / n));               */
/*          w[k +  9] = d2s(M * cos(6.0 * PI * (i    ) / n));               */
/*          w[k +  8] = d2s(M * sin(6.0 * PI * (i    ) / n));               */
/*                                                                          */
/*          w[k +  7] = d2s(M * cos(4.0 * PI * (i + j) / n));               */
/*          w[k +  6] = d2s(M * sin(4.0 * PI * (i + j) / n));               */
/*          w[k +  5] = d2s(M * cos(4.0 * PI * (i    ) / n));               */
/*          w[k +  4] = d2s(M * sin(4.0 * PI * (i    ) / n));               */
/*                                                                          */
/*          w[k +  3] = d2s(M * cos(2.0 * PI * (i + j) / n));               */
/*          w[k +  2] = d2s(M * sin(2.0 * PI * (i + j) / n));               */
/*          w[k +  1] = d2s(M * cos(2.0 * PI * (i    ) / n));               */
/*          w[k +  0] = d2s(M * sin(2.0 * PI * (i    ) / n));               */
/*                                                                          */
/*          k += 12;                                                        */
/*                                                                          */
/*                                                                          */
/*      }                                                                   */
/*    }                                                                     */
/*                                                                          */
/*    return k;                                                             */
/*                                                                          */
/*                                                                          */
/*  ASSUMPTIONS                                                             */
/*      This code works for  both "npoints" a multiple of 2 or 4.           */
/*      The arrays 'x[]', 'y[]', and 'w[]' all must be aligned on a         */
/*      double-word boundary for the "optimized" implementations.           */
/*                                                                          */
/*      The input and output data are complex, with the real/imaginary      */
/*      components stored in adjacent locations in the array.  The real     */
/*      components are stored at even array indices, and the imaginary      */
/*      components are stored at odd array indices.                         */
/*                                                                          */
/*  TECHNIQUES                                                              */
/*      The following C code represents an implementation of the Cooley     */
/*      Tukey radix 4 DIF FFT. It accepts the inputs in normal order and    */
/*      produces the outputs in digit reversed order. The natural C code    */
/*      shown in this file on the other hand, accepts the inputs in nor-    */
/*      mal order and produces the outputs in normal order.                 */
/*                                                                          */
/*      Several transformations have been applied to the original Cooley    */
/*      Tukey code to produce the natural C code description shown here.    */
/*      In order to understand these it would first be educational to       */
/*      understand some of the issues involved in the conventional Cooley   */
/*      Tukey FFT code.                                                     */
/*                                                                          */
/*      void radix4(int n, short x[], short wn[])                           */
/*      {                                                                   */
/*          int    n1,  n2,  ie,   ia1,  ia2, ia3;                          */
/*          int    i0,  i1,  i2,    i3,    i, j,     k;                     */
/*          short  co1, co2, co3,  si1,  si2, si3;                          */
/*          short  xt0, yt0, xt1,  yt1,  xt2, yt2;                          */
/*          short  xh0, xh1, xh20, xh21, xl0, xl1,xl20,xl21;                */
/*                                                                          */
/*          n2 = n;                                                         */
/*          ie = 1;                                                         */
/*          for (k = n; k > 1; k >>= 2)                                     */
/*          {                                                               */
/*              n1 = n2;                                                    */
/*              n2 >>= 2;                                                   */
/*              ia1 = 0;                                                    */
/*                                                                          */
/*              for (j = 0; j < n2; j++)                                    */
/*              {                                                           */
/*                   ia2 = ia1 + ia1;                                       */
/*                   ia3 = ia2 + ia1;                                       */
/*                                                                          */
/*                   co1 = wn[2 * ia1    ];                                 */
/*                   si1 = wn[2 * ia1 + 1];                                 */
/*                   co2 = wn[2 * ia2    ];                                 */
/*                   si2 = wn[2 * ia2 + 1];                                 */
/*                   co3 = wn[2 * ia3    ];                                 */
/*                   si3 = wn[2 * ia3 + 1];                                 */
/*                   ia1 = ia1 + ie;                                        */
/*                                                                          */
/*                   for (i0 = j; i0< n; i0 += n1)                          */
/*                   {                                                      */
/*                       i1 = i0 + n2;                                      */
/*                       i2 = i1 + n2;                                      */
/*                       i3 = i2 + n2;                                      */
/*                                                                          */
/*                                                                          */
/*                       xh0  = x[2 * i0    ] + x[2 * i2    ];              */
/*                       xh1  = x[2 * i0 + 1] + x[2 * i2 + 1];              */
/*                       xl0  = x[2 * i0    ] - x[2 * i2    ];              */
/*                       xl1  = x[2 * i0 + 1] - x[2 * i2 + 1];              */
/*                                                                          */
/*                       xh20 = x[2 * i1    ] + x[2 * i3    ];              */
/*                       xh21 = x[2 * i1 + 1] + x[2 * i3 + 1];              */
/*                       xl20 = x[2 * i1    ] - x[2 * i3    ];              */
/*                       xl21 = x[2 * i1 + 1] - x[2 * i3 + 1];              */
/*                                                                          */
/*                       x[2 * i0    ] = xh0 + xh20;                        */
/*                       x[2 * i0 + 1] = xh1 + xh21;                        */
/*                                                                          */
/*                       xt0  = xh0 - xh20;                                 */
/*                       yt0  = xh1 - xh21;                                 */
/*                       xt1  = xl0 + xl21;                                 */
/*                       yt2  = xl1 + xl20;                                 */
/*                       xt2  = xl0 - xl21;                                 */
/*                       yt1  = xl1 - xl20;                                 */
/*                                                                          */
/*                       x[2 * i1    ] = (xt1 * co1 + yt1 * si1) >> 15;     */
/*                       x[2 * i1 + 1] = (yt1 * co1 - xt1 * si1) >> 15;     */
/*                       x[2 * i2    ] = (xt0 * co2 + yt0 * si2) >> 15;     */
/*                       x[2 * i2 + 1] = (yt0 * co2 - xt0 * si2) >> 15;     */
/*                       x[2 * i3    ] = (xt2 * co3 + yt2 * si3) >> 15;     */
/*                       x[2 * i3 + 1] = (yt2 * co3 - xt2 * si3) >> 15;     */
/*                   }                                                      */
/*             }                                                            */
/*                                                                          */
/*             ie <<= 2;                                                    */
/*         }                                                                */
/*     }                                                                    */
/*                                                                          */
/*      The conventional Cooley Tukey FFT, is written using three loops.    */
/*      The outermost loop "k" cycles through the stages. There are log     */
/*      N to the base 4 stages in all. The loop "j" cycles through the      */
/*      groups of butterflies with different twiddle factors, loop "i"      */
/*      reuses the twiddle factors for the different butterflies within     */
/*      a stage. It is interesting to note the following:                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*      Stage#     #Groups     # Butterflies with common     #Groups*Bflys  */
/*                               twiddle factors                            */
/* ------------------------------------------------------------------------ */
/*       1         N/4          1                            N/4            */
/*       2         N/16         4                            N/4            */
/*       ..                                                                 */
/*       logN      1            N/4                          N/4            */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      The following statements can be made based on above observations:   */
/*                                                                          */
/*      a) Inner loop "i0" iterates a veriable number of times. In          */
/*      particular the number of iterations quadruples every time from      */
/*      1..N/4. Hence software pipelining a loop that iterates a vraiable   */
/*      number of times is not profitable.                                  */
/*                                                                          */
/*      b) Outer loop "j" iterates a variable number of times as well.      */
/*      However the number of iterations is quartered every time from       */
/*      N/4 ..1. Hence the behaviour in (a) and (b) are exactly opposite    */
/*      to each other.                                                      */
/*                                                                          */
/*      c) If the two loops "i" and "j" are colaesced together then they    */
/*      will iterate for a fixed number of times namely N/4. This allows    */
/*      us to combine the "i" and "j" loops into 1 loop. Optimized impl-    */
/*      ementations will make use of this fact.                             */
/*                                                                          */
/*      In addition the Cooley Tukey FFT accesses three twiddle factors     */
/*      per iteration of the inner loop, as the butterflies that re-use     */
/*      twiddle factors are lumped together. This leads to accessing the    */
/*      twiddle factor array at three points each sepearted by "ie". Note   */
/*      that "ie" is initially 1, and is quadrupled with every iteration.   */
/*      Therfore these three twiddle factors are not even contiguous in     */
/*      the array.                                                          */
/*                                                                          */
/*      In order to vectorize the FFT, it is desirable to access twiddle    */
/*      factor array using double word wide loads and fetch the twiddle     */
/*      factors needed. In order to do this a modified twiddle factor       */
/*      array is created, in which the factors WN/4, WN/2, W3N/4 are        */
/*      arranged to be contiguous. This eliminates the seperation between   */
/*      twiddle factors within a butterfly. However this implies that as    */
/*      the loop is traversed from one stage to another, that we maintain   */
/*      a redundant version of the twiddle factor array. Hence the size     */
/*      of the twiddle factor array increases as compared to the normal     */
/*      Cooley Tukey FFT.  The modified twiddle factor array is of size     */
/*      "2 * N" where the conventional Cooley Tukey FFT is of size"3N/4"    */
/*      where N is the number of complex points to be transformed. The      */
/*      routine that generates the modified twiddle factor array was        */
/*      presented earlier. With the above transformation of the FFT,        */
/*      both the input data and the twiddle factor array can be accessed    */
/*      using double-word wide loads to enable packed data processing.      */
/*                                                                          */
/*      The final stage is optimised to remove the multiplication as        */
/*      w0 = 1.  This stage also performs digit reversal on the data,       */
/*      so the final output is in natural order.                            */
/*                                                                          */
/*      The fft() code shown here performs the bulk of the computation      */
/*      in place. However, because digit-reversal cannot be performed       */
/*      in-place, the final result is written to a separate array, y[].     */
/*                                                                          */
/*      The actual twiddle factors for the FFT are cosine, - sine. The      */
/*      twiddle factors stored in the table are csine and sine, hence       */
/*      the sign of the "sine" term is comprehended during multipli-        */
/*      cation as shown above.                                              */
/*                                                                          */
/*  MEMORY NOTE                                                             */
/*      The optimized implementations are written for LITTLE ENDIAN.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2007 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#pragma CODE_SECTION(DSP_fft16x16_i, ".text:intrinsic");

#include "DSP_fft16x16.h"

static inline void radix_2(short *restrict ptr_x, short *restrict ptr_y, int npoints);
static inline void radix_4(short *restrict ptr_x, short *restrict ptr_y, int npoints);

void DSP_fft16x16_i (
    const short * restrict ptr_w,
    int npoints,
    short * restrict ptr_x,
    short * restrict ptr_y
)
{
    const short * restrict w;
    const short * restrict w2;
    short * restrict x, * restrict x2, * restrict x0;

    int j, i, l1, radix;
    int l2, h2, fft_jmp;
    int predj, tw_offset, stride;

    int x_1o_x_0o, x_3o_x_2o;
    int xl2_1_0, xl2_3_2;

    unsigned int myt0_0_mxt0_0, myt0_1_mxt0_1;
    unsigned int xl1_1_0, xl1_3_2, xh2_1_0, xh2_3_2;
    unsigned int yt1_0_xt1_0, yt1_1_xt1_1, yt2_0_xt2_0, yt2_1_xt2_1;

    unsigned int xl0_0_xl1_0, xt1_0_yt2_0;
    unsigned int xt2_0_yt1_0, xl0_1_xl1_1;
    unsigned int xt1_1_yt2_1, xt2_1_yt1_1;

    unsigned int xh1_0_xh0_0, xh1_1_xh0_1;
    unsigned int xl1_0_xl0_0, xl1_1_xl0_1;
    unsigned int xh21_0_xh20_0, xh21_1_xh20_1;
    unsigned int xl21_0_xl20_0, xl21_1_xl20_1;

    double x_3210;
    double co11si11_co10si10, co21si21_co20si20, co31si31_co30si30;
    double x_l1_32_x_l1_10, x_l2_32_x_l2_10, x_h2_32_x_h2_10; 

    long long xt1_0_yt2_0_xt2_0_yt1_0, xt1_1_yt2_1_xt2_1_yt1_1;
    long long yt1_0_xt1_0_yt2_0_xt2_0, yt1_1_xt1_1_yt2_1_xt2_1;
    long long xh1_0_xh0_0_xl1_0_xl0_0, xh1_1_xh0_1_xl1_1_xl0_1;
    long long xh21_0_xh20_0_xl21_0_xl20_0, xh21_1_xh20_1_xl21_1_xl20_1;

    /*---------------------------------------------------------------------*/
    /* Determine the magnitude od the number of points to be transformed.  */
    /* Check whether we can use a radix4 decomposition or a mixed radix    */
    /* transformation, by determining modulo 2.                            */
    /*---------------------------------------------------------------------*/
    radix = _norm(npoints) & 1 ? 2 : 4;

    /*----------------------------------------------------------------------*/
    /* The stride is quartered with every iteration of the outer loop. It   */
    /* denotes the seperation between any two adjacent inputs to the butter */
    /* -fly. This should start out at N/4, hence stride is initially set to */
    /* N. For every stride, 6*stride twiddle factors are accessed. The      */
    /* "tw_offset" is the offset within the current twiddle factor sub-     */
    /* table. This is set to zero, at the start of the code and is used to  */
    /* obtain the appropriate sub-table twiddle pointer by offseting it     */
    /* with the base pointer "ptr_w".                                       */
    /*----------------------------------------------------------------------*/
    stride = npoints;
    tw_offset = 0;
    fft_jmp = 6 * stride;

    _nassert(stride > 4);
    #pragma MUST_ITERATE(1,,1);
    while (stride > radix) {
        /*-----------------------------------------------------------------*/
        /* At the start of every iteration of the outer loop, "j" is set   */
        /* to zero, as "w" is pointing to the correct location within the  */
        /* twiddle factor array. For every iteration of the inner loop     */
        /* 6 * stride twiddle factors are accessed. For eg,                */
        /*                                                                 */
        /* #Iteration of outer loop  # twiddle factors    #times cycled    */
        /*  1                          6 N/4               1               */
        /*  2                          6 N/16              4               */
        /*  ...                                                            */
        /*-----------------------------------------------------------------*/
        j = 0;
        fft_jmp >>= 2;

        /*-----------------------------------------------------------------*/
        /* Set up offsets to access "N/4", "N/2", "3N/4" complex point or  */
        /* "N/2", "N", "3N/2" half word                                    */
        /*-----------------------------------------------------------------*/
        h2 = stride >> 1;
        l1 = stride;
        l2 = stride + (stride >> 1);

        /*-----------------------------------------------------------------*/
        /*  Reset "x" to point to the start of the input data array.       */
        /* "tw_offset" starts off at 0, and increments by "6 * stride"     */
        /*  The stride quarters with every iteration of the outer loop     */
        /*-----------------------------------------------------------------*/
        x = ptr_x;
        w = ptr_w + tw_offset;
        tw_offset += fft_jmp;
        stride >>= 2;

        /*----------------------------------------------------------------*/
        /* The following loop iterates through the different butterflies, */
        /* within a given stage. Recall that there are logN to base 4     */
        /* stages. Certain butterflies share the twiddle factors. These   */
        /* are grouped together. On the very first stage there are no     */
        /* butterflies that share the twiddle factor, all N/4 butter-     */
        /* flies have different factors. On the next stage two sets of    */
        /* N/8 butterflies share the same twiddle factor. Hence after     */
        /* half the butterflies are performed, j the index into the       */
        /* factor array resets to 0, and the twiddle factors are reused.  */
        /* When this happens, the data pointer 'x' is incremented by the  */
        /* fft_jmp amount. In addition the following code is unrolled to  */
        /* perform "2" radix4 butterflies in parallel.                    */
        /*----------------------------------------------------------------*/
        _nassert(npoints >= 16);
        #pragma MUST_ITERATE(2,,2);
        for (i = 0; i < npoints; i += 8) {
            /*------------------------------------------------------------*/
            /* Read the first 12 twiddle factors, six of which are used   */
            /* for one radix 4 butterfly and six of which are used for    */
            /* next one.                                                  */
            /*                                                            */
            /*  co10 = w[j+1];        si10 = w[j+0];                      */
            /*  co11 = w[j+3];        si11 = w[j+2];                      */
            /*  co20 = w[j+5];        si20 = w[j+4];                      */
            /*  co21 = w[j+7];        si21 = w[j+6];                      */
            /*  co30 = w[j+9];        si30 = w[j+8];                      */
            /*  co31 = w[j+11];       si31 = w[j+10];                     */
            /*                                                            */
            /*  The code shown above performs half word wide loads. The   */
            /*  C64x can perform double word wide loads. This is done by  */
            /*  the use of the _amemd8_const intrinsic. The const quali-  */
            /*  fies the fact that this array will only be read from and  */
            /*  not written to.                                           */
            /*------------------------------------------------------------*/
            w2 = w + j;
            co11si11_co10si10 = _amemd8_const(w2);
            co21si21_co20si20 = _amemd8_const(&w2[4]);
            co31si31_co30si30 = _amemd8_const(&w2[8]);

            /*------------------------------------------------------------*/
            /* Read in the first complex input for the butterflies.       */
            /* 1st complex input to 1st butterfly: x[0] + jx[1]           */
            /* 1st complex input to 2nd butterfly: x[2] + jx[3]           */
            /* Read in the complex inputs for the butterflies. Each of the*/
            /* successive complex inputs of the butterfly are seperated   */
            /* by a fixed amount known as stride. The stride starts out   */
            /* at N/4, and quarters with every stage.                     */
            /*                                                            */
            /*  x_0    =   x[0];          x_1    =   x[1];                */
            /*  x_2    =   x[2];          x_3    =   x[3];                */
            /*  x_l1_0 =   x[l1  ];       x_l1_1 =   x[l1 + 1];           */
            /*  x_l1_2 =   x[l1 + 2];     x_l1_3 =   x[l1 + 3];           */
            /*  x_l2_0 =   x[l2  ];       x_l2_1 =   x[l2 + 1];           */
            /*  x_l2_2 =   x[l2 + 2];     x_l2_3 =   x[l2 + 3];           */
            /*  x_h2_0 =   x[h2  ];       x_h2_1 =   x[h2 + 1];           */
            /*  x_h2_2 =   x[h2 + 2];     x_h2_3 =   x[h2 + 3];           */
            /*                                                            */
            /* These loads are performed using _amemd8. Note that there   */
            /* is no const qualifier in this case as this loop writes     */
            /* back results into the data array "x". The "a" in _amemd8   */
            /* qualifies the fact that the loads are aligned.             */
            /*------------------------------------------------------------*/
            x_3210 = _amemd8(&x[0]);
            x_l1_32_x_l1_10 = _amemd8(&x[l1]);
            x_l2_32_x_l2_10 = _amemd8(&x[l2]);
            x_h2_32_x_h2_10 = _amemd8(&x[h2]);

            /*-----------------------------------------------------------*/
            /* Two butterflies are evaluated in parallel. The following  */
            /* results will be shown for one butterfly only, although    */
            /* both are being evaluated in parallel.                     */
            /*                                                           */
            /* Perform radix2 style DIF butterflies.                     */
            /*                                                           */
            /*  xh0_0  = x_0    + x_l1_0;     xh1_0  = x_1    + x_l1_1;  */
            /*  xh0_1  = x_2    + x_l1_2;     xh1_1  = x_3    + x_l1_3;  */
            /*  xl0_0  = x_0    - x_l1_0;     xl1_0  = x_1    - x_l1_1;  */
            /*  xl0_1  = x_2    - x_l1_2;     xl1_1  = x_3    - x_l1_3;  */
            /*  xh20_0 = x_h2_0 + x_l2_0;     xh21_0 = x_h2_1 + x_l2_1;  */
            /*  xh20_1 = x_h2_2 + x_l2_2;     xh21_1 = x_h2_3 + x_l2_3;  */
            /*  xl20_0 = x_h2_0 - x_l2_0;     xl21_0 = x_h2_1 - x_l2_1;  */
            /*  xl20_1 = x_h2_2 - x_l2_2;     xl21_1 = x_h2_3 - x_l2_3;  */
            /*                                                           */
            /*  These are done using packed data processing, and these   */
            /*  are done using _add2, _sub2.                             */
            /*  For example "xh1_0_xh0_0" has a packed value xh1_0,      */
            /*  xh0_0.                                                   */
            /*-----------------------------------------------------------*/
            xh1_0_xh0_0_xl1_0_xl0_0 = _addsub2((_lo(x_3210)), (_lo(x_l1_32_x_l1_10)));
            xh1_1_xh0_1_xl1_1_xl0_1 = _addsub2((_hi(x_3210)), (_hi(x_l1_32_x_l1_10)));

            xh1_0_xh0_0 = (_hill(xh1_0_xh0_0_xl1_0_xl0_0));
            xh1_1_xh0_1 = (_hill(xh1_1_xh0_1_xl1_1_xl0_1));
            xl1_0_xl0_0 = (_loll(xh1_0_xh0_0_xl1_0_xl0_0));
            xl1_1_xl0_1 = (_loll(xh1_1_xh0_1_xl1_1_xl0_1));

            xh21_0_xh20_0_xl21_0_xl20_0 = _addsub2((_lo(x_h2_32_x_h2_10)), (_lo(x_l2_32_x_l2_10)));
            xh21_1_xh20_1_xl21_1_xl20_1 = _addsub2((_hi(x_h2_32_x_h2_10)), (_hi(x_l2_32_x_l2_10)));

            xh21_0_xh20_0 = (_hill(xh21_0_xh20_0_xl21_0_xl20_0));
            xh21_1_xh20_1 = (_hill(xh21_1_xh20_1_xl21_1_xl20_1));
            xl21_0_xl20_0 = (_loll(xh21_0_xh20_0_xl21_0_xl20_0));
            xl21_1_xl20_1 = (_loll(xh21_1_xh20_1_xl21_1_xl20_1));

            /*----------------------------------------------------------*/
            /* Derive output pointers using the input pointer "x"       */
            /*----------------------------------------------------------*/
            x0 = (short *)_mvd((int)x);
            x2 = (short *)_mvd((int)x0);

            /*-----------------------------------------------------------*/
            /* When the twiddle factors are not to be re-used, j is      */
            /* incremented by 12, to reflect the fact that 12 half words */
            /* are consumed in every iteration. The input data pointer   */
            /* increments by 4. Note that within a stage, the stride     */
            /* does not change and hence the offsets for the other three */
            /* legs, 0, h2, l1, l2.                                      */
            /*-----------------------------------------------------------*/
            j += 12;
            x += 4;

            predj = (j - fft_jmp);
            if (!predj) x += fft_jmp;
            if (!predj) j = 0;

            /*----------------------------------------------------------*/
            /* x0[0] = (xh0_0 + xh20_0 + 1) >> 1;                       */
            /* x0[1] = (xh1_0 + xh21_0 + 1) >> 1;                       */
            /* x0[2] = (xh0_1 + xh20_1 + 1) >> 1;                       */
            /* x0[3] = (xh1_1 + xh21_1 + 1) >> 1;                       */
            /*                                                          */
            /* "y0x0" and "y1x1" are the first four outputs of the      */
            /*  radix4 butterfly. These are computed on the C64x        */
            /* using _avg2.                                             */
            /*----------------------------------------------------------*/
            x_1o_x_0o = _avg2(xh21_0_xh20_0, xh1_0_xh0_0);
            x_3o_x_2o = _avg2(xh21_1_xh20_1, xh1_1_xh0_1);

            /*----------------------------------------------------------*/
            /* Perform one more stage of radix2 decompositions as shown */
            /* These are performed using packed data processing.        */
            /*                                                          */
            /* xt0_0 = xh0_0 - xh20_0;  yt0_0 = xh1_0 - xh21_0;         */
            /* xt0_1 = xh0_1 - xh20_1;  yt0_1 = xh1_1 - xh21_1;         */
            /* xt1_0 = xl0_0 + xl21_0;  yt2_0 = xl1_0 + xl20_0;         */
            /* xt2_0 = xl0_0 - xl21_0;  yt1_0 = xl1_0 - xl20_0;         */
            /* xt1_1 = xl0_1 + xl21_1;  yt2_1 = xl1_1 + xl20_1;         */
            /* xt2_1 = xl0_1 - xl21_1;  yt1_1 = xl1_1 - xl20_1;         */
            /*                                                          */
            /*----------------------------------------------------------*/
            myt0_0_mxt0_0 = _sub2(xh21_0_xh20_0, xh1_0_xh0_0);
            myt0_1_mxt0_1 = _sub2(xh21_1_xh20_1, xh1_1_xh0_1);

            /*----------------------------------------------------------*/
            /* Repack coefficients to sustain packed data processing.   */
            /* This requires a swizzle so that intermediate results     */
            /* end up in the right spot. This is needed to compute      */
            /* other intermediate results using add2 and sub2.          */
            /*----------------------------------------------------------*/
            xl0_0_xl1_0 = _rotl(xl1_0_xl0_0, 16);
            xl0_1_xl1_1 = _rotl(xl1_1_xl0_1, 16);

            //;----------------------------------------------------------;
            //; xt1_0 = xl0_0 + xl21_0   yt2_0 = xl1_0 + xl20_0          ;
            //; xt1_1 = xl0_1 + xl21_1   yt2_1 = xl1_1 + xl20_1          ;
            //; xt2_0 = xl0_0 - xl21_0   yt1_0 = xl1_0 - xl20_0          ;
            //; xt2_1 = xl0_1 - xl21_1   yt1_1 = xl1_1 - xl20_1          ;
            //;----------------------------------------------------------;
            xt1_0_yt2_0_xt2_0_yt1_0 = _addsub2(xl0_0_xl1_0, xl21_0_xl20_0);
            xt1_1_yt2_1_xt2_1_yt1_1 = _addsub2(xl0_1_xl1_1, xl21_1_xl20_1);

            xt1_0_yt2_0 = (_hill(xt1_0_yt2_0_xt2_0_yt1_0));
            xt1_1_yt2_1 = (_hill(xt1_1_yt2_1_xt2_1_yt1_1));
            xt2_0_yt1_0 = (_loll(xt1_0_yt2_0_xt2_0_yt1_0));
            xt2_1_yt1_1 = (_loll(xt1_1_yt2_1_xt2_1_yt1_1));

            /*----------------------------------------------------------*/
            /* Repack results so that xt_XXyt_XX are a packed  32 bit   */
            /* quantity. The operations performed above tend to leave   */
            /* the results as xt_XXyt_YY and xt_YYyt_XX. This is where  */
            /* the packed instructions come in handy. For example       */
            /* xt_XXyt_XX = _packlh2(xt_XXyt_YY, xt_YY_yt_XX)          */
            /*----------------------------------------------------------*/
            yt1_0_xt1_0_yt2_0_xt2_0 = _dpackx2(xt1_0_yt2_0, xt2_0_yt1_0);
            yt1_1_xt1_1_yt2_1_xt2_1 = _dpackx2(xt1_1_yt2_1, xt2_1_yt1_1);

            yt1_0_xt1_0 = (_hill(yt1_0_xt1_0_yt2_0_xt2_0));
            yt1_1_xt1_1 = (_hill(yt1_1_xt1_1_yt2_1_xt2_1));
            yt2_0_xt2_0 = (_loll(yt1_0_xt1_0_yt2_0_xt2_0));
            yt2_1_xt2_1 = (_loll(yt1_1_xt1_1_yt2_1_xt2_1));

            /*---------------------------------------------------------*/
            /* Notice that in this version of the code the two middle  */
            /* legs are swapped as indicated by the stores to x[l1]..  */
            /* x[l1 + 3] which preceede the stores to x[h2]..x[h2 + 3] */
            /* This reversal guarantees that a radix4 DIF butterfly    */
            /* produces results in digit reversed order. Note that in  */
            /* addition to the rounding, the shift is performed by 16, */
            /* as opposed to 15, to give scaling.                      */
            /*---------------------------------------------------------*/

            /*---------------------------------------------------------*/
            /*                                                         */
            /* x2[l1  ] = (si10 * yt1_0 + co10 * xt1_0 + 0x8000) >> 16;*/
            /* x2[l1+1] = (co10 * yt1_0 - si10 * xt1_0 + 0x8000) >> 16;*/
            /* x2[l1+2] = (si11 * yt1_1 + co11 * xt1_1 + 0x8000) >> 16;*/
            /* x2[l1+3] = (co11 * yt1_1 - si11 * xt1_1 + 0x8000) >> 16;*/
            /*                                                         */
            /* These four results are retained in registers and a      */
            /* double word is formed so that it can be stored with     */
            /* one STDW.                                               */
            /*---------------------------------------------------------*/
            xh2_1_0 = _cmpyr((_lo(co11si11_co10si10)), yt1_0_xt1_0);
            xh2_3_2 = _cmpyr((_hi(co11si11_co10si10)), yt1_1_xt1_1);

            /*---------------------------------------------------------*/
            /* The following code computes intermediate results for:   */
            /*                                                         */
            /* x2[h2  ] = (si20 * yt0_0 + co20 * xt0_0 + 0x8000) >> 16;*/
            /* x2[h2+1] = (co20 * yt0_0 - si20 * xt0_0 + 0x8000) >> 16;*/
            /* x2[h2+2] = (si21 * yt0_1 + co21 * xt0_1 + 0x8000) >> 16;*/
            /* x2[h2+3] = (co21 * yt0_1 - si21 * xt0_1 + 0x8000) >> 16;*/
            /*---------------------------------------------------------*/
            xl1_1_0 = _cmpyr((_lo(co21si21_co20si20)), myt0_0_mxt0_0);
            xl1_3_2 = _cmpyr((_hi(co21si21_co20si20)), myt0_1_mxt0_1);

            /*---------------------------------------------------------*/
            /* The following code computes intermediate results for:   */
            /*                                                         */
            /* x2[l2  ] = (si30 * yt2_0 + co30 * xt2_0 + 0x8000) >> 16;*/
            /* x2[l2+1] = (co30 * yt2_0 - si30 * xt2_0 + 0x8000) >> 16;*/
            /* x2[l2+2] = (si31 * yt2_1 + co31 * xt2_1 + 0x8000) >> 16;*/
            /* x2[l2+3] = (co31 * yt2_1 - si31 * xt2_1 + 0x8000) >> 16;*/
            /*---------------------------------------------------------*/
            xl2_1_0 = _cmpyr((_lo(co31si31_co30si30)), yt2_0_xt2_0);
            xl2_3_2 = _cmpyr((_hi(co31si31_co30si30)), yt2_1_xt2_1);

            /*---------------------------------------------------------*/
            /* Prepare the doubleword for each of the four outputs, by */
            /* the use of the _itod intrinsic that takes integers, and */
            /* forms a double word wide quantity.                      */
            /* Store out the outputs to the four legs of the butterfly */
            /* using aligned store double words. Notice the use of the */
            /* indices "l1", "l2", "h2" to derive the pointers for the */
            /* legs of the butterfly.                                  */
            /*---------------------------------------------------------*/
            _amemd8(&x2[0]) = _itod(x_3o_x_2o, x_1o_x_0o);
            _amemd8(&x2[l1]) = _itod(xl1_3_2, xl1_1_0);
            _amemd8(&x2[h2]) = _itod(xh2_3_2, xh2_1_0);
            _amemd8(&x2[l2]) = _itod(xl2_3_2, xl2_1_0);
        }
    }

    if (radix == 2)
        radix_2(ptr_x, ptr_y, npoints);
    else if (radix == 4)
        radix_4(ptr_x, ptr_y, npoints);

    return;
}


void radix_2 (
    short *restrict ptr_x,
    short *restrict ptr_y,
    int npoints
)
{
    short *restrict x0, *restrict x2;
    short *restrict y0, *restrict y1, *restrict y2, *restrict y3;
    int i, j, j0, n0, h2, l1;

    int x_10, x_32, x_54, x_76, x_98, x_BA, x_DC, x_FE;
    int y_10, y_32, y_54, y_76, y_98, y_BA, y_DC, y_FE;

    int xl1_0_xl0_0, xl20_0_xl21_0;
    int xl1_1_xl0_1, xl20_1_xl21_1;
    int xt1_0_yt2_0, xt2_0_yt1_0;
    int xt1_1_yt2_1, xt2_1_yt1_1;

    double x_3210, x_7654, x_BA98, x_FEDC;

    /*-----------------------------------------------------------------*/
    /* The following code performs either a standard radix4 pass or a  */
    /* radix2 pass. Two pointers are used to access the input data.    */
    /* The input data is read "N/4" complex samples apart or "N/2"     */
    /* words apart using pointers "x0" and "x2". This produces out-    */
    /* puts that are 0, N/4, N/2, 3N/4 for a radix4 FFT, and 0, N/8    */
    /* N/2, 3N/8 for radix 2.                                          */
    /*-----------------------------------------------------------------*/
    y0 = ptr_y;
    y2 = ptr_y + (int)npoints;
    x0 = ptr_x;
    x2 = ptr_x + (int)(npoints >> 1);

    /*----------------------------------------------------------------*/
    /* The pointers are set at the following locations which are half */
    /* the offsets of a radix4 FFT.                                   */
    /*----------------------------------------------------------------*/
    y1 = y0 + (int)(npoints >> 2);
    y3 = y2 + (int)(npoints >> 2);
    l1 = _norm(npoints) + 2;
    j0 = 8;
    n0 = npoints >> 1;

    /*--------------------------------------------------------------------*/
    /* The following code reads data indentically for either a radix 4    */
    /* or a radix 2 style decomposition. It writes out at different       */
    /* locations though. It checks if either half the points, or a        */
    /* quarter of the complex points have been exhausted to jump to       */
    /* pervent double reversal.                                           */
    /*--------------------------------------------------------------------*/
    j = 0;

    _nassert((int)(n0) % 4  == 0);
    _nassert((int)(ptr_x) % 8 == 0);
    _nassert((int)(ptr_y) % 8 == 0);
    _nassert((int)(x0) % 8 == 0);
    _nassert((int)(x2) % 8 == 0);
    _nassert((int)(y0) % 8 == 0);
    _nassert(npoints >= 16);
    #pragma MUST_ITERATE(2,,2);
    for (i = 0; i < npoints; i += 8) {
        /*---------------------------------------------------------------*/
        /* The following code taks an index "j" and generates the bit    */
        /* reversed index. It does through the "deal", "bitr" and "shfl" */
        /* instructions. The shift by "l1" is used to return the result  */
        /* in the right number of bits depending on the size of the FFT  */
        /*---------------------------------------------------------------*/
        h2 = _deal(j);
        h2 = _bitr(h2);
        h2 = _rotl(h2, 16);
        h2 = _shfl(h2);
        h2 >>= l1;

        /*---------------------------------------------------------------*/
        /* Load 8 data values from the top and middle halves using the   */
        /* _amemd8 intrinsic. These can be accessed using the _lo and    */
        /* _hi intrinsic.                                                */
        /*---------------------------------------------------------------*/
        x_3210 = _amemd8(&x0[i]);
        x_7654 = _amemd8(&x0[i + 4]);
        x_BA98 = _amemd8(&x2[i]);
        x_FEDC = _amemd8(&x2[i + 4]);

        x_10 = _lo(x_3210);
        x_32 = _hi(x_3210);
        x_54 = _lo(x_7654);
        x_76 = _hi(x_7654);
        x_98 = _lo(x_BA98);
        x_BA = _hi(x_BA98);
        x_DC = _lo(x_FEDC);
        x_FE = _hi(x_FEDC);

        /*-------------------------------------------------------------*/
        /* radix = 2  y0 = x0 + x2                                     */
        /*            y1 = x1 + x3                                     */
        /*            y4 = x0 - x2                                     */
        /*            y5 = x1 - x3                                     */
        /*-------------------------------------------------------------*/
        y_10 = _add2(x_10, x_32);
        y_54 = _sub2(x_10, x_32);

        /*--------------------------------------------------------------*/
        /* xl0_0  = x_0 - x_4;       xl1_0  = x_1 - x_5;                */
        /* xl20_0 = x_2 - x_6;       xl21_0 = x_3 - x_7;                */
        /*                                                              */
        /*--------------------------------------------------------------*/
        xl1_0_xl0_0 = _mvd(x_54);
        xl20_0_xl21_0 = _mvd(x_76);


        xt1_0_yt2_0 = _add2(xl1_0_xl0_0, xl20_0_xl21_0);
        xt2_0_yt1_0 = _sub2(xl1_0_xl0_0, xl20_0_xl21_0);

        y_32 = _mvd(xt1_0_yt2_0);
        y_76 = _mvd(xt2_0_yt1_0);

        /*-------------------------------------------------------------*/
        /* xh0_1  = x_8 + x_c;       xh1_1  = x_9 + x_e;               */
        /* xh20_1 = x_a + x_e;       xh21_1 = x_b + x_f;               */
        /*                                                             */
        /* radix = 2  y8 = x8 + xa                                     */
        /*            y9 = x9 + xb                                     */
        /*            yc = x8 - xa                                     */
        /*            ye = x9 - xb                                     */
        /*-------------------------------------------------------------*/
        y_98 = _add2(x_98, x_BA);
        y_DC = _sub2(x_98, x_BA);

        xl1_1_xl0_1 = _mvd(x_DC);
        xl20_1_xl21_1 = _mvd(x_FE);

        xt1_1_yt2_1 = _add2(xl1_1_xl0_1, xl20_1_xl21_1);
        xt2_1_yt1_1 = _sub2(xl1_1_xl0_1, xl20_1_xl21_1);

        y_BA = _mvd(xt1_1_yt2_1);
        y_FE = _mvd(xt2_1_yt1_1);

        /*------------------------------------------------------------*/
        /*  Store out the results of all four butterflies as double   */
        /*  words.                                                    */
        /*------------------------------------------------------------*/
        _amemd8(&y0[4 * h2]) = _itod(y_98, y_10);
        _amemd8(&y1[4 * h2]) = _itod(y_BA, y_32);
        _amemd8(&y2[4 * h2]) = _itod(y_DC, y_54);
        _amemd8(&y3[4 * h2]) = _itod(y_FE, y_76);

        /*-----------------------------------------------------------------*/
        /* Increment "j" by "j0". If j equals n0, then increment both "x0" */
        /* and "x2" so that double inversion is avoided.                   */
        /*-----------------------------------------------------------------*/
        j += j0;

        if (j == n0) {
            j  += n0;
            x0 += (int)npoints >> 1;
            x2 += (int)npoints >> 1;
        }
    }
}


void radix_4 (
    short *restrict ptr_x,
    short *restrict ptr_y,
    int npoints
)
{
    short *restrict x0, *restrict x2;
    short *restrict y0, *restrict y1, *restrict y2, *restrict y3;
    int j0, n0, j, i, h2, l1;

    int x_10, x_32, x_54, x_76, x_98, x_BA, x_DC, x_FE;
    int y_10, y_32, y_54, y_76, y_98, y_BA, y_DC, y_FE;

    int xh1_0_xh0_0, xh21_0_xh20_0;
    int xh1_1_xh0_1, xh21_1_xh20_1;
    int xl1_0_xl0_0, xl21_0_xl20_0, xl20_0_xl21_0;
    int xl1_1_xl0_1, xl21_1_xl20_1, xl20_1_xl21_1;
    int xt1_0_yt2_0, xt2_0_yt1_0;
    int xt1_1_yt2_1, xt2_1_yt1_1;

    double x_3210, x_7654, x_BA98, x_FEDC;

    /*-----------------------------------------------------------------*/
    /* The following code performs either a standard radix4 pass or a  */
    /* radix2 pass. Two pointers are used to access the input data.    */
    /* The input data is read "N/4" complex samples apart or "N/2"     */
    /* words apart using pointers "x0" and "x2". This produces out-    */
    /* puts that are 0, N/4, N/2, 3N/4 for a radix4 FFT, and 0, N/8    */
    /* N/2, 3N/8 for radix 2.                                          */
    /*-----------------------------------------------------------------*/
    y0 = ptr_y;
    y2 = ptr_y + (int)npoints;
    x0 = ptr_x;
    x2 = ptr_x + (int)(npoints >> 1);

    y1 = y0 + (int)(npoints >> 1);
    y3 = y2 + (int)(npoints >> 1);
    l1 = _norm(npoints) + 3;
    j0 = 4;
    n0 = npoints >> 2;

    /*--------------------------------------------------------------------*/
    /* The following code reads data indentically for either a radix 4    */
    /* or a radix 2 style decomposition. It writes out at different       */
    /* locations though. It checks if either half the points, or a        */
    /* quarter of the complex points have been exhausted to jump to       */
    /* pervent double reversal.                                           */
    /*--------------------------------------------------------------------*/
    j = 0;

    _nassert((int)(n0) % 4  == 0);
    _nassert((int)(x0) % 8 == 0);
    _nassert((int)(x2) % 8 == 0);
    _nassert((int)(y0) % 8 == 0);
    _nassert(npoints >= 16);
    #pragma MUST_ITERATE(2,,2);

    for (i = 0; i < npoints; i += 8) {
        /*---------------------------------------------------------------*/
        /* The following code taks an index "j" and generates the bit    */
        /* reversed index. It does through the "deal", "bitr" and "shfl" */
        /* instructions. The shift by "l1" is used to return the result  */
        /* in the right number of bits depending on the size of the FFT  */
        /*---------------------------------------------------------------*/
        h2 = _deal(j);
        h2 = _bitr(h2);
        h2 = _rotl(h2, 16);
        h2 = _shfl(h2);
        h2 >>= l1;

        /*---------------------------------------------------------------*/
        /* Load 8 data values from the top and middle halves using the   */
        /* _amemd8 intrinsic. These can be accessed using the _lo and    */
        /* _hi intrinsic.                                                */
        /*---------------------------------------------------------------*/
        x_3210 = _amemd8(&x0[i]);
        x_7654 = _amemd8(&x0[i + 4]);
        x_BA98 = _amemd8(&x2[i]);
        x_FEDC = _amemd8(&x2[i + 4]);

        x_10 = _lo(x_3210);
        x_32 = _hi(x_3210);
        x_54 = _lo(x_7654);
        x_76 = _hi(x_7654);
        x_98 = _lo(x_BA98);
        x_BA = _hi(x_BA98);
        x_DC = _lo(x_FEDC);
        x_FE = _hi(x_FEDC);

        /*-------------------------------------------------------------*/
        /* xh0_0  = x_0 + x_4;       xh1_0  = x_1 + x_5;               */
        /* xh20_0 = x_2 + x_6;       xh21_0 = x_3 + x_7;               */
        /*                                                             */
        /* xh0_0  = x0                                                 */
        /* xh0_1  = x1                                                 */
        /* xh20_0 = x2                                                 */
        /* xh21_0 = x3                                                 */
        /*-------------------------------------------------------------*/
        xh1_0_xh0_0 = _add2(x_54, x_10);
        xh21_0_xh20_0 = _add2(x_76, x_32);

        /*-------------------------------------------------------------*/
        /* radix = 4, y0 = x_0 + x_4 + x_1 + x_5                       */
        /*            y1 = x_2 + x_6 + x_3 + x_7                       */
        /*            y4 = x_0 + x_4 - x_1 - x_5                       */
        /*            y5 = x_2 + x_6 - x_3 - x_7                       */
        /*-------------------------------------------------------------*/
        y_10 = _add2(xh1_0_xh0_0, xh21_0_xh20_0);
        y_54 = _sub2(xh1_0_xh0_0, xh21_0_xh20_0);

        /*--------------------------------------------------------------*/
        /* xl0_0  = x_0 - x_4;       xl1_0  = x_1 - x_5;                */
        /* xl20_0 = x_2 - x_6;       xl21_0 = x_3 - x_7;                */
        /*                                                              */
        /* xl0_0  = x5                                                  */
        /* xl1_0  = x4                                                  */
        /* xl21_0 = x7                                                  */
        /* xl20_0 = x6                                                  */
        /*--------------------------------------------------------------*/
        xl1_0_xl0_0   = _sub2(x_10, x_54);
        xl21_0_xl20_0 = _sub2(x_32, x_76);
        xl20_0_xl21_0 = _rotl(xl21_0_xl20_0, 16);

        /*-------------------------------------------------------------*/
        /*  radix = 4 xt1_0 = x_0 + x_3 - x_4 - x_7                    */
        /*            yt2_0 = x_1 + x_2 - x_5 - x_6                    */
        /*            xt2_0 = x_0 + x_3 + x_4 + x_7                    */
        /*            yt1_0 = x_1 + x_2 + x_5 + x_6                    */
        /*                                                             */
        /*  xt1_0 = x5 + x7                                            */
        /*  yt2_0 = x4 + x6                                            */
        /*  xt2_0 = x5 - x7                                            */
        /*  yt1_0 = x4 - x6                                            */
        /*-------------------------------------------------------------*/
        xt1_0_yt2_0 = _add2(xl1_0_xl0_0, xl20_0_xl21_0);
        xt2_0_yt1_0 = _sub2(xl1_0_xl0_0, xl20_0_xl21_0);

        y_32 = _packhl2(xt2_0_yt1_0, xt1_0_yt2_0);
        y_76 = _packhl2(xt1_0_yt2_0, xt2_0_yt1_0);

        /*-------------------------------------------------------------*/
        /* xh0_1  = x_8 + x_c;       xh1_1  = x_9 + x_e;               */
        /* xh20_1 = x_a + x_e;       xh21_1 = x_b + x_f;               */
        /*                                                             */
        /* xh0_1  = x8                                                 */
        /* xh1_1  = x9                                                 */
        /* xh20_1 = xa                                                 */
        /* xh21_1 = xb                                                 */
        /*-------------------------------------------------------------*/
        xh1_1_xh0_1 = _add2(x_DC, x_98);
        xh21_1_xh20_1 = _add2(x_FE, x_BA);

        /*-------------------------------------------------------------*/
        /* radix = 4, y8 = x_8 + x_d + x_9 + x_e                       */
        /*            y9 = x_a + x_c + x_b + x_f                       */
        /*            yc = x_8 + x_d - x_9 - x_e                       */
        /*            ye = x_a + x_c - x_b - x_f                       */
        /*-------------------------------------------------------------*/
        y_98 = _add2(xh1_1_xh0_1, xh21_1_xh20_1);
        y_DC = _sub2(xh1_1_xh0_1, xh21_1_xh20_1);

        xl1_1_xl0_1 = _sub2(x_98, x_DC);
        xl21_1_xl20_1 = _sub2(x_BA, x_FE);
        xl20_1_xl21_1 = _rotl(xl21_1_xl20_1, 16);

        xt1_1_yt2_1 = _add2(xl1_1_xl0_1, xl20_1_xl21_1);
        xt2_1_yt1_1 = _sub2(xl1_1_xl0_1, xl20_1_xl21_1);

        y_FE = _packhl2(xt1_1_yt2_1, xt2_1_yt1_1);
        y_BA = _packhl2(xt2_1_yt1_1, xt1_1_yt2_1);

        /*------------------------------------------------------------*/
        /*  Store out the results of all four butterflies as double   */
        /*  words.                                                    */
        /*------------------------------------------------------------*/
        _amemd8(&y0[4 * h2]) = _itod(y_98, y_10);
        _amemd8(&y1[4 * h2]) = _itod(y_BA, y_32);
        _amemd8(&y2[4 * h2]) = _itod(y_DC, y_54);
        _amemd8(&y3[4 * h2]) = _itod(y_FE, y_76);

        /*-----------------------------------------------------------------*/
        /* Increment "j" by "j0". If j equals n0, then increment both "x0" */
        /* and "x2" so that double inversion is avoided.                   */
        /*-----------------------------------------------------------------*/
        j += j0;

        if (j == n0) {
            j  += n0;
            x0 += (int)npoints >> 1;
            x2 += (int)npoints >> 1;
        }
    }
}

/* ======================================================================== */
/*  End of file: DSP_fft16x16.c                                             */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2007 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */

