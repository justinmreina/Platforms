# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

# -*-makefile-*-


-include .config
include build/aws_starter_sdk.mk

subdir-y += sdk/external/aws_iot
subdir-y += sdk/external/freertos
subdir-y += sdk/external/lwip

# pre-built libraries
subdir-y += sdk/libs
ifeq ($(APP),)
-include sample_apps/sample_apps.mk
else
APP_PATH ?= $(realpath $(APP))
subdir-y += $(APP_PATH)
endif

include build/aws_starter_sdk_rules.mk
#####################
##################### Only the Default Targets

all: post-build-1
post-build-1: do_the_build
do_the_build: pre-build-1
	$(AT)$(MAKE) build

build: $(b-exec-apps-y)
	@echo $(b-exec-paths-y)
.PHONY: all build do_the_build post-build-1 pre-build-1
