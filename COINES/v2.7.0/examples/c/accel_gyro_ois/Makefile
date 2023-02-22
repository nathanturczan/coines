COINES_INSTALL_PATH ?= ../../..

EXAMPLE_FILE ?= accel_gyro_ois.c

API_LOCATION ?= $(COINES_INSTALL_PATH)/examples/bmi270

COMMON_LOCATION ?= $(API_LOCATION)/examples/bmi270

C_SRCS += \
$(API_LOCATION)/bmi2.c \
$(API_LOCATION)/bmi270.c \
$(API_LOCATION)/bmi2_ois.c \
$(COMMON_LOCATION)/common/common.c

INCLUDEPATHS += \
$(API_LOCATION) \
$(COMMON_LOCATION)/common

include $(COINES_INSTALL_PATH)/coines.mk