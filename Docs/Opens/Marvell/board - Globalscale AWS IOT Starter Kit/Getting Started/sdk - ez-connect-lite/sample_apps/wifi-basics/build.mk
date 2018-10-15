# Copyright (C) 2008-2016 Marvell International Ltd.
# All Rights Reserved.
#

exec-y += wifi-basics
wifi-basics-objs-y := src/main.c
wifi-basics-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1



# Applications could also define custom linker files if required using following:
#wifi-basics-linkerscript-y := /path/to/linkerscript
# Applications could also define custom board files if required using following:
#wifi-basics-board-y := /path/to/boardfile
