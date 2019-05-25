#------------------------------------------------------------------------------------------------
# Global Path Setup for Dependent components
#------------------------------------------------------------------------------------------------

# PATH Along which the dsplipc674x is installed
DSPLIBC64P_INSTALL_DIR = /home/user/c64p_dsplib

# Where the Code Gen is installed.
CODEGEN_INSTALL_DIR=/home/user/cgt_7_2_0

# BUILD TYPE Selects the configuration for which the library needs to be built
# OPTION 1: Release : This builds the COFF version of the Library
# OPTION 2: Release_elf: This builds the ELF verison of the Library
BUILD_TYPE= Release_elf

#PATH on the OS where the binaries need to be installed
#Any user defined path can be used to install these binaries
EXEC_DIR = /home/user/dsplib_install



