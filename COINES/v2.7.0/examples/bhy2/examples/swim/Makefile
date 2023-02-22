#Change file name if required
LOG_FILE = "swim_log.csv"

#CCE_Board_Definitions:BHI260, BHA260
COINES_INSTALL_PATH ?= ../../../../

EXAMPLE_FILE ?= swim.c

C_SRCS += ../common/common.c
C_SRCS += ../../source/bhy2.c
C_SRCS += ../../source/bhy2_hif.c
C_SRCS += ../../source/bhy2_parse.c
C_SRCS += ../../source/bhy2_swim.c

INCLUDEPATHS += ../common
INCLUDEPATHS += ../../source
INCLUDEPATHS += ../../firmware

#SHUTTLE_BOARD ?= BHI260

#TARGET = MCU_APP30

#LOCATION = FLASH

include $(COINES_INSTALL_PATH)/coines.mk

