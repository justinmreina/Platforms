## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,674 linker.cmd \
  package/cfg/keystone_flash_p674.o674 \

linker.cmd: package/cfg/keystone_flash_p674.xdl
	$(SED) 's"^\"\(package/cfg/keystone_flash_p674cfg.cmd\)\"$""\"C:/SYSBIOSv4/Labs/Lab11b_Keystone/Project/Debug/configPkg/\1\""' package/cfg/keystone_flash_p674.xdl > $@
