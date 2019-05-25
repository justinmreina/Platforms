#
_XDCBUILDCOUNT = 7
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = 
override XDCROOT = C:/CCStudio_v4.2.4/xdctools_3_22_01_21
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/CCStudio_v4.2.4/xdctools_3_22_01_21/packages;..
HOSTOS = Windows
endif
