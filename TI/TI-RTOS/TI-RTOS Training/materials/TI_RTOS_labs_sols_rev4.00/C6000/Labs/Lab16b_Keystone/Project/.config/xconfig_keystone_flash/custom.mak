## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,674 linker.cmd \
  package/cfg/keystone_flash_p674.o674 \

linker.cmd: package/cfg/keystone_flash_p674.xdl
	$(SED) 's"^\"\(package/cfg/keystone_flash_p674cfg.cmd\)\"$""\"C:/SYSBIOSv4/Labs/Lab12b_Keystone/Project/.config/xconfig_keystone_flash/\1\""' package/cfg/keystone_flash_p674.xdl > $@
