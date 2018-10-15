# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

tc-src-dir-$(tc-iar-env-y) := IAR
tc-src-dir-$(tc-arm_gcc-env-y) := GCC

global-cflags-y += \
		-Isdk/src/incl/platform/os/freertos     \
		-I$(d)/Source/include/ -I$(d)/

global-cflags-$(CONFIG_ENABLE_FREERTOS_RUNTIME_STATS_SUPPORT) += \
		-DCONFIG_ENABLE_RUNTIME_STATS

global-cflags-$(tc-cortex-m3-y) += \
		-I$(d)/Source/portable/$(tc-src-dir-y)/ARM_CM3

global-cflags-$(tc-cortex-m4-y) += \
		-I$(d)/Source/portable/$(tc-src-dir-y)/ARM_CM4F

-include $(d)/build.freertos.mk

