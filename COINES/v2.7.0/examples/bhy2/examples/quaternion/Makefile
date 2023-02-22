ifeq ($(OS),Windows_NT)
HOME=C:
endif


COINES_INSTALL_PATH ?= ../../../../


EXAMPLE_FILE ?= quaternion.c

C_SRCS += ../common/common.c
C_SRCS += ../../source/bhy2.c
C_SRCS += ../../source/bhy2_hif.c
C_SRCS += ../../source/bhy2_parse.c

INCLUDEPATHS += .
INCLUDEPATHS += ../common
INCLUDEPATHS += ../../source
INCLUDEPATHS += ../../firmware

include $(COINES_INSTALL_PATH)/coines.mk