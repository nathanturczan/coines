#CCE_Board_Definitions:BME680

COINES_INSTALL_PATH ?= ../../../..

EXAMPLE_FILE ?= bsec_iot_example.c

SHUTTLE_BOARD ?= BME680

TARGET ?= PC

BSEC_DIR = $(COINES_INSTALL_PATH)/examples/bme680/examples/_BSEC

API_LOCATION ?= ../..

C_SRCS += \
$(API_LOCATION)/bme680.c \
$(BSEC_DIR)/bsec_integration.c \
$(BSEC_DIR)/config/generic_33v_3s_4d/bsec_serialized_configurations_iaq.c

INCLUDEPATHS += \
$(COINES_INSTALL_PATH)/examples/bme680 \
$(BSEC_DIR) \
$(BSEC_DIR)/normal/inc \
$(BSEC_DIR)/config/generic_33v_3s_4d

include $(BSEC_DIR)/bsec.mk

include $(COINES_INSTALL_PATH)/coines.mk
