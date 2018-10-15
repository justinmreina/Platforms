# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

libs-y += libaws_iot

aws_iot_src_dir := $(d)/src

libaws_iot-objs-y := $(wildcard $(aws_iot_src_dir)/*.c) $(d)/platform/wmsdk/timer/timer.c $(d)/platform/wmsdk/network/network_interface.c

libaws_iot-objs-y := $(libaws_iot-objs-y:$(d)/%=%)

libaws_iot-supported-toolchain-y := arm_gcc