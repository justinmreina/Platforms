# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.
#
# Description:
# ------------
# This file, virtual_targets_toolchain.mk contains:
#
# 	Virtual targets for building libraries and apps
# 	specific to supported toolchains.
#
#==============================================================#
# Toolchain specific libs and exec handling

# This function initialises variables to null
define init-tc-vars
  b-$(1)-libs-y :=
  b-$(1)-exec-y :=
endef
$(foreach t,$(b-supported-toolchain-y),$(eval $(call init-tc-vars,$(t))))
#--------------------------------------------------------------#

# This function performs,
# 	1. Check for <app/lib>-supported-toolchain-y variable.
# 	2. Populating toolchain specific <app/lib> list.

define handle-tc-list
  ifndef $(2)-supported-toolchain-y
    b-arm_gcc-$(3)-y += $(2).$(4)
  else
    ifneq ($(filter $(1),$($(2)-supported-toolchain-y)),)
      b-$(1)-$(3)-y += $(2).$(4)
    endif
  endif
endef

define get-tc-vars
  $(foreach l,$(b-libs-y),$(eval $(call handle-tc-list,$(1),$(l),libs,a)))
  $(foreach e,$(b-exec-y),$(eval $(call handle-tc-list,$(1),$(e),exec,app)))
endef

$(foreach t,$(b-supported-toolchain-y),$(eval $(call get-tc-vars,$(t))))
#--------------------------------------------------------------#

# This function will filter-out common things from gcc libs and apps
define prune-arm_gcc-vars
  b-arm_gcc-libs-y := $(filter-out $(b-$(1)-libs-y),$(b-arm_gcc-libs-y))
  b-arm_gcc-exec-y := $(filter-out $(b-$(1)-exec-y),$(b-arm_gcc-exec-y))
endef

$(foreach t,$(filter-out arm_gcc,$(b-supported-toolchain-y)),$(eval $(call prune-arm_gcc-vars,$(t))))
#--------------------------------------------------------------#

# This function creates toolchain specific target for libs and apps
define create-tc-target

$(1)-compat-libs: $(b-$(1)-libs-y)
$(1)-compat-apps: $(b-$(1)-exec-y)

# Application build depend on library build
$(b-$(1)-exec-y): $(b-$(1)-libs-y)

$(1)-compat-libs.clean: $(foreach l,$(b-$(1)-libs-y),$(l).clean)
$(1)-compat-apps.clean: $(foreach e,$(b-$(1)-exec-y),$(e).clean)

endef

$(foreach t,$(b-supported-toolchain-y),$(eval $(call create-tc-target,$(t))))
#==============================================================#
