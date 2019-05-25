/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  C64xplus DSP Signal Processing Library header file                      */
/*                                                                          */
/*      Release:        Revision 2.1.0.0                                    */
/*      Snapshot date:  21-August-2010                                      */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#include "../src/DSP_autocor/DSP_autocor.h"
#include "../src/DSP_bexp/DSP_bexp.h"
#include "../src/DSP_blk_eswap16/DSP_blk_eswap16.h"
#include "../src/DSP_blk_eswap32/DSP_blk_eswap32.h"
#include "../src/DSP_blk_eswap64/DSP_blk_eswap64.h"
#include "../src/DSP_blk_move/DSP_blk_move.h"
#include "../src/DSP_dotp_sqr/DSP_dotp_sqr.h"
#include "../src/DSP_dotprod/DSP_dotprod.h"
#include "../src/DSP_fft16x16/DSP_fft16x16_sa.h"
#include "../src/DSP_fft16x16_imre/DSP_fft16x16_imre_sa.h"
#include "../src/DSP_fft16x16r/DSP_fft16x16r_sa.h"
#include "../src/DSP_fft16x32/DSP_fft16x32_sa.h"
#include "../src/DSP_fft32x32/DSP_fft32x32_sa.h"
#include "../src/DSP_fft32x32s/DSP_fft32x32s_sa.h"
#include "../src/DSP_fir_cplx/DSP_fir_cplx.h"
#include "../src/DSP_fir_cplx_hM4X4/DSP_fir_cplx_hM4X4.h"
#include "../src/DSP_fir_gen/DSP_fir_gen.h"
#include "../src/DSP_fir_gen_hM17_rA8X8/DSP_fir_gen_hM17_rA8X8.h"
#include "../src/DSP_fir_r4/DSP_fir_r4.h"
#include "../src/DSP_fir_r8/DSP_fir_r8.h"
#include "../src/DSP_fir_r8_hM16_rM8A8X8/DSP_fir_r8_hM16_rM8A8X8.h"
#include "../src/DSP_fir_sym/DSP_fir_sym.h"
#include "../src/DSP_firlms2/DSP_firlms2.h"
#include "../src/DSP_fltoq15/DSP_fltoq15.h"
#include "../src/DSP_ifft16x16/DSP_ifft16x16_sa.h"
#include "../src/DSP_ifft16x16_imre/DSP_ifft16x16_imre_sa.h"
#include "../src/DSP_ifft16x32/DSP_ifft16x32_sa.h"
#include "../src/DSP_ifft32x32/DSP_ifft32x32_sa.h"
#include "../src/DSP_iir/DSP_iir.h"
#include "../src/DSP_iir_lat/DSP_iir_lat.h"
#include "../src/DSP_mat_mul/DSP_mat_mul.h"
#include "../src/DSP_mat_trans/DSP_mat_trans.h"
#include "../src/DSP_maxidx/DSP_maxidx.h"
#include "../src/DSP_maxval/DSP_maxval.h"
#include "../src/DSP_minerror/DSP_minerror.h"
#include "../src/DSP_minval/DSP_minval.h"
#include "../src/DSP_mul32/DSP_mul32.h"
#include "../src/DSP_neg32/DSP_neg32.h"
#include "../src/DSP_q15tofl/DSP_q15tofl.h"
#include "../src/DSP_recip16/DSP_recip16.h"
#include "../src/DSP_vecsumsq/DSP_vecsumsq.h"
#include "../src/DSP_w_vec/DSP_w_vec.h"

#ifdef __cplusplus
}
#endif

