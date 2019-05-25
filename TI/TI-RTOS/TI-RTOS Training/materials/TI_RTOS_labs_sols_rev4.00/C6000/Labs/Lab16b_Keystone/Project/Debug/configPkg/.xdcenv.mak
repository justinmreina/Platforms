#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/CCStudio_v5.1.0/bios_6_32_05_54/packages;C:/CCStudio_v5.1.0/ipc_1_23_05_40/packages;C:/SYSBIOSv4/Labs/SYSBIOS_Platforms
override XDCROOT = C:/CCStudio_v5.1.0/xdctools_3_22_04_46
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/CCStudio_v5.1.0/bios_6_32_05_54/packages;C:/CCStudio_v5.1.0/ipc_1_23_05_40/packages;C:/SYSBIOSv4/Labs/SYSBIOS_Platforms;C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages;..
HOSTOS = Windows
endif
