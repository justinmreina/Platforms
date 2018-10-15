# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.
#
# Description:
# ------------
# This file, config.mk contains rules/functions for:
#
# 	defconfig handling

################# Build Configuration

b-output-dir-y  = bin
BIN_DIR ?= $(b-output-dir-y)
b-autoconf-file := sdk/src/incl/autoconf.h
global-preinclude-y := $(b-autoconf-file)
global-cflags-y += -include $(b-autoconf-file)
