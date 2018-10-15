# Copyright (C) 2008-2016 Marvell International Ltd.
# All Rights Reserved.
#

exec-y += aws_starter_demo
aws_starter_demo-objs-y := src/main.c
aws_starter_demo-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1

#ifneq ($(wildcard $(d)/www),)
aws_starter_demo-ftfs-y := aws_starter_demo.ftfs
aws_starter_demo-ftfs-dir-y     := $(d)/www
aws_starter_demo-ftfs-api-y := 100
#endif

# Applications could also define custom linker files if required using following:
#aws_starter_demo-linkerscript-y := /path/to/linkerscript
# Applications could also define custom board files if required using following:
#aws_starter_demo-board-y := /path/to/boardfile
