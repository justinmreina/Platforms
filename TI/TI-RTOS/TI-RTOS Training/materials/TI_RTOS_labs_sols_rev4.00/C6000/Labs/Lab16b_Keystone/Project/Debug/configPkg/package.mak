#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = configPkg
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifneq (clean,$(MAKECMDGOALS))
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/include/utils.tci:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/include/utils.tci
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xdc.tci:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xdc.tci
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/template.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/template.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/om2.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/om2.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xmlgen.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xmlgen.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xmlgen2.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/xmlgen2.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/IPackage.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/IPackage.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/package.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/package.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/global/Clock.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/global/Clock.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/global/Trace.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/global/Trace.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/bld.js:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/bld.js
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/BuildEnvironment.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/BuildEnvironment.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/PackageContents.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/PackageContents.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/_gen.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/_gen.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Library.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Library.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Executable.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Executable.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Repository.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Repository.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Configuration.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Configuration.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Script.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Script.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Manifest.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Manifest.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Utils.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/Utils.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITarget.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITarget.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITarget2.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITarget2.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITargetFilter.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/ITargetFilter.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/package.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/bld/package.xs
package.mak: config.bld
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/ITarget.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/ITarget.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/C28_large.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/C28_large.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/C28_float.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/C28_float.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/package.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/package.xs
package.mak: package.bld
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/compiler.opt.xdt:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/compiler.opt.xdt
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/io/File.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/io/File.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/io/package.xs:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/services/io/package.xs
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/custom.mak.exe.xdt:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/custom.mak.exe.xdt
C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/package.xs.xdt:
package.mak: C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/xdc/tools/configuro/template/package.xs.xdt
endif

ti.targets.C674.rootDir ?= C:/CCStudio_v5.1.0/ccsv5/tools/compiler/c6000
ti.targets.packageBase ?= C:/CCStudio_v5.1.0/xdctools_3_22_04_46/packages/ti/targets/
.PRECIOUS: $(XDCCFGDIR)/%.o674
.PHONY: all,674 .dlls,674 .executables,674 test,674
all,674: .executables,674
.executables,674: .libraries,674
.executables,674: .dlls,674
.dlls,674: .libraries,674
.libraries,674: .interfaces
	@$(RM) $@
	@$(TOUCH) "$@"

.help::
	@$(ECHO) xdc test,674
	@$(ECHO) xdc .executables,674
	@$(ECHO) xdc .libraries,674
	@$(ECHO) xdc .dlls,674


all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_configPkg.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package configPkg" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

.dlls,674 .dlls: keystone_flash.p674

-include package/cfg/keystone_flash_p674.mak
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/keystone_flash_p674.dep
endif
keystone_flash.p674: package/cfg/keystone_flash_p674.xdl
	@


ifeq (,$(wildcard .libraries,674))
keystone_flash.p674 package/cfg/keystone_flash_p674.c: .libraries,674
endif

package/cfg/keystone_flash_p674.c package/cfg/keystone_flash_p674.h package/cfg/keystone_flash_p674.xdl: override _PROG_NAME := keystone_flash.x674
package/cfg/keystone_flash_p674.c: package/cfg/keystone_flash_p674.cfg

clean:: clean,674
	-$(RM) package/cfg/keystone_flash_p674.cfg
	-$(RM) package/cfg/keystone_flash_p674.dep
	-$(RM) package/cfg/keystone_flash_p674.c
	-$(RM) package/cfg/keystone_flash_p674.xdc.inc

clean,674::
	-$(RM) keystone_flash.p674
.executables,674 .executables: keystone_flash.x674

-include package/cfg/keystone_flash.x674.mak
keystone_flash.x674: package/cfg/keystone_flash_p674.o674 
	$(RM) $@
	@$(MSG) lnk674 $@ ...
	$(RM) $(XDCCFGDIR)/$@.map
	$(ti.targets.C674.rootDir)/bin/lnk6x -w -q -u _c_int00 -fs $(XDCCFGDIR)$(dir $@).  -q -o $@ package/cfg/keystone_flash_p674.o674   package/cfg/keystone_flash_p674.xdl  -c -m $(XDCCFGDIR)/$@.map -l $(ti.targets.C674.rootDir)/lib/rts6740.lib
	
keystone_flash.x674:C_DIR=
keystone_flash.x674: PATH:=$(ti.targets.C674.rootDir)/bin/;$(PATH)
keystone_flash.x674: Path:=$(ti.targets.C674.rootDir)/bin/;$(PATH)

keystone_flash.test test,674 test: keystone_flash.x674.test

keystone_flash.x674.test:: keystone_flash.x674
ifeq (,$(_TESTLEVEL))
	@$(MAKE) -R -r --no-print-directory -f $(XDCROOT)/packages/xdc/bld/xdc.mak _TESTLEVEL=1 keystone_flash.x674.test
else
	@$(MSG) running $<  ...
	$(call EXEC.keystone_flash.x674, ) 
endif

clean,674::
	-$(RM) .tmp,keystone_flash.x674,0,*


clean:: clean,674

clean,674::
	-$(RM) keystone_flash.x674
clean:: 
	-$(RM) package/cfg/keystone_flash_p674.pjt
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@
keystone_flash_p674.o674,copy : package/cfg/keystone_flash_p674.o674
keystone_flash_p674.s674,copy : package/cfg/keystone_flash_p674.s674

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,configPkg
configPkg.tar: package/package.bld.xml
configPkg.tar: package/package.ext.xml
configPkg.tar: package/package.rel.dot
configPkg.tar: package/build.cfg
configPkg.tar: package/package.xdc.inc
ifeq (,$(MK_NOGENDEPS))
-include package/rel/configPkg.tar.dep
endif
package/rel/configPkg/configPkg/package/package.rel.xml:

configPkg.tar: package/rel/configPkg.xdc.inc package/rel/configPkg/configPkg/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/configPkg.xdc.inc,package/rel/configPkg.tar.dep)


release release,configPkg: all configPkg.tar
clean:: .clean
	-$(RM) configPkg.tar
	-$(RM) package/rel/configPkg.xdc.inc
	-$(RM) package/rel/configPkg.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
ifeq (,$(wildcard package/external))
    $(shell $(MKDIR) package/external)
endif
endif
clean::
	-$(RMDIR) package

include custom.mak
clean:: 
	-$(RM) package/configPkg.pjt
