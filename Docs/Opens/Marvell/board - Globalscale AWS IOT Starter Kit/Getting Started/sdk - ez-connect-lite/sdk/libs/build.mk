# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

# makefile to include pre-built libraries present in current directory

global-prebuilt-libs-y += $(wildcard sdk/libs/*.a)
