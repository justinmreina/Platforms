# Copyright (C) 2008-2016 Marvell International Ltd.
# All Rights Reserved.
#

exec-y += connected_maraca
connected_maraca-objs-y := src/main.c src/sensor_acc_drv.c
connected_maraca-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1

#ifneq ($(wildcard $(d)/www),)
connected_maraca-ftfs-y := connected_maraca.ftfs
connected_maraca-ftfs-dir-y     := $(d)/www
connected_maraca-ftfs-api-y := 100
#endif

# Applications could also define custom linker files if required using following:
#connected_maraca-ld-y := /path/to/ldscript

# Applications could also define custom board files if required using following:
#connected_maraca-board-y := /path/to/boardfile
