#############################################################################
# Makefile                                                                  #
#                                                                           #
#  Make file to build all test driver projects in src folder                #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
#     http://www.ti.com/                                                    #
#                                                                           #
#############################################################################

SUBDIRS = $(sort $(dir $(wildcard */)))
CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))
INSTALLSUBDIRS = $(addsuffix .install, $(SUBDIRS))

.PHONY: install clean $(SUBDIRS)

all:	$(SUBDIRS)

$(SUBDIRS):
	@echo
	@echo Executing make in subdirectory $(basename $@)...
	@$(MAKE) -C $(basename $@) all

install: $(INSTALLSUBDIRS)

$(INSTALLSUBDIRS):
	@echo
	@echo Executing make install in subdirectory $(basename $@)...
	$(MAKE) -C $(basename $@) install

clean:	$(CLEANSUBDIRS)

$(CLEANSUBDIRS):
	$(MAKE) -C $(basename $@) clean
