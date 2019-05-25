#############################################################################
# Makefile                                                                  #
#                                                                           #
#  Make file to build dsplib_c64p library                                   #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
#     http://www.ti.com/                                                    #
#                                                                           #
#############################################################################
# Use the main DSPLIBC674x Rules.make for building apps in this directory
ifndef DSPLIBC64P_INSTALL_DIR
    DSPLIBC64P_INSTALL_DIR = ../
endif

include $(DSPLIBC64P_INSTALL_DIR)/Rules.make

TARGET= dsplib
LTARGET = $(addprefix $(BUILD_TYPE),$(TARGET))

CUR_DIR = .
# Comment this out if you want to see full compiler and linker output.
VERBOSE = @

export CODEGEN_INSTALL_DIR	

INCLUDES = $(CODEGEN_INSTALL_DIR)/include 
MAP_FILE = $(BUILD_TYPE)/$(LTARGET).map

#Set Compiler flags based on Configuration specified
ifeq ($(BUILD_TYPE), Release)
    C_FLAGS	+= -o3 -fr$(BUILD_TYPE) -fs$(BUILD_TYPE) -ft$(BUILD_TYPE) -d__TI_EABI__=0 -mw -mv6400+
    ATARGET	 = $(BUILD_TYPE)/$(TARGET)_rebuild.lib
endif
ifeq ($(BUILD_TYPE), Release_elf)
    C_FLAGS	+= --abi=eabi -o3 -fr$(BUILD_TYPE) -fs$(BUILD_TYPE) -ft$(BUILD_TYPE) -mw -mv6400+
    ATARGET	 = $(BUILD_TYPE)/$(TARGET)_elf_rebuild.lib
endif

#Set Compiler and Archiver paths and flags for the build
COMPILE.c = $(VERBOSE) $(CODEGEN_INSTALL_DIR)/bin/cl6x $(C_FLAGS)
AR.c	  = $(VERBOSE) $(CODEGEN_INSTALL_DIR)/bin/ar6x $(AR_FLAGS)

# C _OBJs to be built
BEXP_CSRC = ../src/DSP_bexp/DSP_bexp.c
ESWAP16_CSRC = ../src/DSP_blk_eswap16/DSP_blk_eswap16.c
ESWAP32_CSRC = ../src/DSP_blk_eswap32/DSP_blk_eswap32.c
ESWAP64_CSRC = ../src/DSP_blk_eswap64/DSP_blk_eswap64.c
FLTOQ15_CSRC = ../src/DSP_fltoq15/DSP_fltoq15.c
Q15TOFL_CSRC = ../src/DSP_q15tofl/DSP_q15tofl.c
AUTOCOR_CSRC = ../src/DSP_autocor/DSP_autocor.c
MOVE_CSRC    = ../src/DSP_blk_move/DSP_blk_move.c
NEG32_CSRC = ../src/DSP_neg32/DSP_neg32.c
MUL32_CSRC = ../src/DSP_mul32/DSP_mul32.c
DOTP_SQR_CSRC = ../src/DSP_dotp_sqr/DSP_dotp_sqr.c
DOTPROD_CSRC = ../src/DSP_dotprod/DSP_dotprod.c
IIR_CSRC = ../src/DSP_iir/DSP_iir.c
IIRLAT_CSRC = ../src/DSP_iir_lat/DSP_iir_lat.c
FIR_GEN_CSRC = ../src/DSP_fir_gen/DSP_fir_gen.c
FIR_SYM_CSRC = ../src/DSP_fir_sym/DSP_fir_sym.c
FIR_R4_CSRC = ../src/DSP_fir_r4/DSP_fir_r4.c
FIR_R8_CSRC = ../src/DSP_fir_r8/DSP_fir_r8.c
FIRLMS2_CSRC = ../src/DSP_firlms2/DSP_firlms2.c
FIR_GEN_HM17_RA8X8_CSRC = ../src/DSP_fir_gen_hM17_rA8X8/DSP_fir_gen_hM17_rA8X8.c
FIR_HM16_RM8A8X8_CSRC = ../src/DSP_fir_r8_hM16_rM8A8X8/DSP_fir_r8_hM16_rM8A8X8.c
FIR_CPLX_CSRC = ../src/DSP_fir_cplx/DSP_fir_cplx.c
FIR_CPLX_HM4X4_CSRC = ../src/DSP_fir_cplx_hM4X4/DSP_fir_cplx_hM4X4.c
MAT_MUL_CSRC = ../src/DSP_mat_mul/DSP_mat_mul.c
MAT_TRANS_CSRC = ../src/DSP_mat_trans/DSP_mat_trans.c
MAXIDX_CSRC = ../src/DSP_maxidx/DSP_maxidx.c
MAXVAL_CSRC = ../src/DSP_maxval/DSP_maxval.c
MINVAL_CSRC = ../src/DSP_minval/DSP_minval.c
MINERR_CSRC = ../src/DSP_minerror/DSP_minerror.c
VECMUL_CSRC = ../src/DSP_vecmul/DSP_vecmul.c
RECIP16_CSRC = ../src/DSP_recip16/DSP_recip16.c
VECSUM_SQ_CSRC = ../src/DSP_vecsumsq/DSP_vecsumsq.c
W_VEC_CSRC = ../src/DSP_w_vec/DSP_w_vec.c          

#Assembly source files to be built
FFT16X16_ASMSRC=../src/DSP_fft16x16/DSP_fft16x16_sa.sa
FFT16X16IMR_ASMSRC=../src/DSP_fft16x16_imre/DSP_fft16x16_imre_sa.sa
FFT16X16R_ASMSRC=../src/DSP_fft16x16r/DSP_fft16x16r_sa.sa
FFT16X32_ASMSRC=../src/DSP_fft16x32/DSP_fft16x32_sa.sa
FFT32X32_ASMSRC=../src/DSP_fft32x32/DSP_fft32x32_sa.sa
FFT32X32S_ASMSRC=../src/DSP_fft32x32s/DSP_fft32x32s_sa.sa
IFFT16X16_ASMSRC=../src/DSP_ifft16x16/DSP_ifft16x16_sa.sa
IFFT16X16IMR_ASMSRC=../src/DSP_ifft16x16_imre/DSP_ifft16x16_imre_sa.sa
IFFT16X32_ASMSRC=../src/DSP_ifft16x32/DSP_ifft16x32_sa.sa
IFFT32X32_ASMSRC=../src/DSP_ifft32x32/DSP_ifft32x32_sa.sa


#Object files to be built        
BEXP_OBJ  = $(BEXP_CSRC:../src/DSP_bexp/%.c=$(BUILD_TYPE)/%.obj)
ESWAP16_OBJ  = $(ESWAP16_CSRC:../src/DSP_blk_eswap16/%.c=$(BUILD_TYPE)/%.obj)
ESWAP32_OBJ  = $(ESWAP32_CSRC:../src/DSP_blk_eswap32/%.c=$(BUILD_TYPE)/%.obj)
ESWAP64_OBJ  = $(ESWAP64_CSRC:../src/DSP_blk_eswap64/%.c=$(BUILD_TYPE)/%.obj) 
FLTOQ15_OBJ  = $(FLTOQ15_CSRC:../src/DSP_fltoq15/%.c=$(BUILD_TYPE)/%.obj)
Q15TOFL_OBJ = $(Q15TOFL_CSRC:../src/DSP_q15tofl/%.c=$(BUILD_TYPE)/%.obj)
AUTOCOR_OBJ = $(AUTOCOR_CSRC:../src/DSP_autocor/%.c=$(BUILD_TYPE)/%.obj)
MOVE_OBJ     = $(MOVE_CSRC:../src/DSP_blk_move/%.c=$(BUILD_TYPE)/%.obj) 
DOTP_SQR_OBJ = $(DOTP_SQR_CSRC:../src/DSP_dotp_sqr/%.c=$(BUILD_TYPE)/%.obj)
DOTPROD_OBJ = $(DOTPROD_CSRC:../src/DSP_dotprod/%.c=$(BUILD_TYPE)/%.obj)
FIR_GEN_OBJ = $(FIR_GEN_CSRC:../src/DSP_fir_gen/%.c=$(BUILD_TYPE)/%.obj)
NEG32_OBJ = $(NEG32_CSRC:../src/DSP_neg32/%.c=$(BUILD_TYPE)/%.obj)
MUL32_OBJ = $(MUL32_CSRC:../src/DSP_mul32/%.c=$(BUILD_TYPE)/%.obj)
FIR_SYM_OBJ = $(FIR_SYM_CSRC:../src/DSP_fir_sym/%.c=$(BUILD_TYPE)/%.obj)
FIR_SYM_OBJ = $(FIR_SYM_CSRC:../src/DSP_fir_sym/%.c=$(BUILD_TYPE)/%.obj)
FIR_R4_OBJ = $(FIR_R4_CSRC:../src/DSP_fir_r4/%.c=$(BUILD_TYPE)/%.obj)
FIR_R8_OBJ = $(FIR_R8_CSRC:../src/DSP_fir_r8/%.c=$(BUILD_TYPE)/%.obj)
FIRLMS2_OBJ = $(FIRLMS2_CSRC:../src/DSP_firlms2/%.c=$(BUILD_TYPE)/%.obj)
FIR_GEN_HM17_RA8X8_OBJ = $(FIR_GEN_HM17_RA8X8_CSRC:../src/DSP_fir_gen_hM17_rA8X8/%.c=$(BUILD_TYPE)/%.obj)
FIR_HM16_RM8A8X8_OBJ = $(FIR_HM16_RM8A8X8_CSRC:../src/DSP_fir_r8_hM16_rM8A8X8/%.c=$(BUILD_TYPE)/%.obj)
FIR_CPLX_OBJ = $(FIR_CPLX_CSRC:../src/DSP_fir_cplx/%.c=$(BUILD_TYPE)/%.obj)
FIR_CPLX_HM4X4_OBJ = $(FIR_CPLX_HM4X4_CSRC:../src/DSP_fir_cplx_hM4X4/%.c=$(BUILD_TYPE)/%.obj)
IIR_OBJ = $(IIR_CSRC:../src/DSP_iir/%.c=$(BUILD_TYPE)/%.obj)
IIRLAT_OBJ = $(IIRLAT_CSRC:../src/DSP_iir_lat/%.c=$(BUILD_TYPE)/%.obj)
MAT_TRANS_OBJ = $(MAT_TRANS_CSRC:../src/DSP_mat_trans/%.c=$(BUILD_TYPE)/%.obj)
MAT_MUL_OBJ = $(MAT_MUL_CSRC:../src/DSP_mat_mul/%.c=$(BUILD_TYPE)/%.obj)
MAXIDX_OBJ = $(MAXIDX_CSRC:../src/DSP_maxidx/%.c=$(BUILD_TYPE)/%.obj)
MAXVAL_OBJ = $(MAXVAL_CSRC:../src/DSP_maxval/%.c=$(BUILD_TYPE)/%.obj)
MINVAL_OBJ = $(MINVAL_CSRC:../src/DSP_minval/%.c=$(BUILD_TYPE)/%.obj)
MINERR_OBJ = $(MINERR_CSRC:../src/DSP_minerror/%.c=$(BUILD_TYPE)/%.obj)
VECMUL_OBJ = $(VECMUL_CSRC:../src/DSP_vecmul/%.c=$(BUILD_TYPE)/%.obj)
RECIP16_OBJ = $(RECIP16_CSRC:../src/DSP_recip16/%.c=$(BUILD_TYPE)/%.obj)
VECSUM_SQ_OBJ = $(VECSUM_SQ_CSRC:../src/DSP_vecsumsq/%.c=$(BUILD_TYPE)/%.obj)
W_VEC_OBJ = $(W_VEC_CSRC:../src/DSP_w_vec/%.c=$(BUILD_TYPE)/%.obj)    
FFT16X16_OBJ=$(FFT16X16_ASMSRC:../src/DSP_fft16x16/%.sa=$(BUILD_TYPE)/%.obj)
FFT16X16IMR_OBJ=$(FFT16X16IMR_ASMSRC:../src/DSP_fft16x16_imre/%.sa=$(BUILD_TYPE)/%.obj)
FFT16X16R_OBJ=$(FFT16X16R_ASMSRC:../src/DSP_fft16x16r/%.sa=$(BUILD_TYPE)/%.obj)
FFT16X32_OBJ=$(FFT16X32_ASMSRC:../src/DSP_fft16x32/%.sa=$(BUILD_TYPE)/%.obj)
FFT32X32_OBJ=$(FFT32X32_ASMSRC:../src/DSP_fft32x32/%.sa=$(BUILD_TYPE)/%.obj)
FFT32X32S_OBJ=$(FFT32X32S_ASMSRC:../src/DSP_fft32x32s/%.sa=$(BUILD_TYPE)/%.obj)
IFFT16X16_OBJ=$(IFFT16X16_ASMSRC:../src/DSP_ifft16x16/%.sa=$(BUILD_TYPE)/%.obj)
IFFT16X16IMR_OBJ=$(IFFT16X16IMR_ASMSRC:../src/DSP_ifft16x16_imre/%.sa=$(BUILD_TYPE)/%.obj)
IFFT16X32_OBJ=$(IFFT16X32_ASMSRC:../src/DSP_ifft16x32/%.sa=$(BUILD_TYPE)/%.obj)
IFFT32X32_OBJ=$(IFFT32X32_ASMSRC:../src/DSP_ifft32x32/%.sa=$(BUILD_TYPE)/%.obj) 

           


.PHONY: clean install 

all:	$(ATARGET)

$(ATARGET):  $(BEXP_OBJ) $(ESWAP16_OBJ) $(ESWAP64_OBJ) $(ESWAP32_OBJ) $(FLTOQ15_OBJ) $(Q15TOFL_OBJ) $(AUTOCOR_OBJ) $(MOVE_OBJ) \
	$(DOTPROD_OBJ) $(DOTP_SQR_OBJ) $(FIR_GEN_OBJ) $(FIR_CPLX_OBJ) $(MAT_TRANS_OBJ) $(FIRLMS2_OBJ) $(FIR_R4_OBJ) $(FIR_R8_OBJ)\
	$(FFT16X16_OBJ) $(FFT16X16IMR_OBJ) $(FFT16X16R_OBJ) $(FFT16X32_OBJ) $(FFT32X32_OBJ) $(FFT32X32S_OBJ) $(FIR_SYM_OBJ) $(FIRLMS2_OBJ)\
	$(IFFT16X16_OBJ) $(IFFT16X16IMR_OBJ) $(IFFT16X32_OBJ) $(IFFT32X32_OBJ) $(NEG32_OBJ) $(MAT_MUL_OBJ) $(MUL32_OBJ) $(FIR_GEN_HM17_RA8X8_OBJ) $(FIR_HM16_RM8A8X8_OBJ)\
	$(FIR_CPLX_HM4X4_OBJ) $(IIR_OBJ) $(IIRLAT_OBJ) $(MAXIDX_OBJ) $(MAXVAL_OBJ) $(MINVAL_OBJ) $(MINERR_OBJ) $(RECIP16_OBJ) $(VECSUM_SQ_OBJ) $(W_VEC_OBJ)
	@echo
	@echo Archiving $@ ..
	@mkdir -p $(dir $@)
	@echo $(AR.c) -r $@ $^
	$(AR.c) -r $@ $^

$(BUILD_TYPE)/%.obj:../src/DSP_fir_cplx/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_cplx_hM4X4/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_gen_hM17_rA8X8/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_r8_hM16_rM8A8X8/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_firlms2/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_neg32/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_bexp/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_mul32/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_mat_mul/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_mat_trans/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_maxidx/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_maxval/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_minval/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_minerror/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_vecmul/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_recip16/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_vecsumsq/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_w_vec/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_blk_eswap16/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_blk_eswap64/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fltoq15/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_dotp_sqr/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_dotprod/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_iir/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_iir_lat/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_gen/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_r4/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_r8/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fir_sym/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_q15tofl/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_autocor/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_blk_eswap32/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_blk_move/%.c $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	@mkdir -p $(BUILD_TYPE)
	@chmod 777 $(BUILD_TYPE)
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft16x16/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft32x32/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft16x32/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft16x16_imre/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_ifft16x16/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_ifft32x32/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_ifft16x32/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_ifft16x16_imre/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft32x32s/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

$(BUILD_TYPE)/%.obj:../src/DSP_fft16x16r/%.sa $(HEADERS) 
	@echo Compiling $@ from $<..
	@echo $(COMPILE.c) -I$(INCLUDES)$<
	$(COMPILE.c) -I$(INCLUDES) $<

clean:
	@echo Removing generated files..
	$(VERBOSE) -$(RM) -r $(BUILD_TYPE) *~ *.d .dep
