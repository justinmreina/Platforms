# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

exec-y += i2c_simple_master_demo
i2c_simple_master_demo-objs-y := src/main.c

#Application specific entities can be specified as follows
#<app-name>-board-y := /path/to/boardfile
#<app-name>-linkerscript-y := /path/to/linkerscript

i2c_simple_master_demo-supported-toolchain-y := arm_gcc iar
