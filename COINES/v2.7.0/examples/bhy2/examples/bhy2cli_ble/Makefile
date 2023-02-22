COINES_INSTALL_PATH ?= ../../../../

EXAMPLE_FILE ?= bhy2cli.c

BHY2_INTF ?= 

C_SRCS += parse.c
C_SRCS += logbin.c
C_SRCS += cli.c
C_SRCS += bhy2cli_callbacks.c
C_SRCS += common_callbacks.c
C_SRCS += verbose.c
C_SRCS += ../common/common.c
C_SRCS += ../../source/bhy2.c
C_SRCS += ../../source/bhy2_hif.c
C_SRCS += ../../source/bhy2_parse.c
C_SRCS += ../../source/bhy2_klio.c
C_SRCS += ../../source/bhy2_pdr.c
C_SRCS += ../../source/bhy2_swim.c
C_SRCS += ../../source/bhy2_bsec.c
C_SRCS += ../../source/bhy2_head_tracker.c

INCLUDEPATHS += .
INCLUDEPATHS += ../common
INCLUDEPATHS += ../../source
INCLUDEPATHS += ../../firmware

# By default, the sensor is connected over SPI. Define this to change to I2C
ifeq ($(BHY2_INTF), I2C)
CFLAGS += -DBHY2_USE_I2C
endif

# Override other target definitions
#TARGET = MCU_APP30
#LOCATION = FLASH

# By default, the interface is configured for PC mode. For operating in COINES_BRIDGE mode,
# uncomment the below macro and make sure coines_bridge firmware is running in the board
#COINES_BACKEND=COINES_BRIDGE

include $(COINES_INSTALL_PATH)/coines.mk
