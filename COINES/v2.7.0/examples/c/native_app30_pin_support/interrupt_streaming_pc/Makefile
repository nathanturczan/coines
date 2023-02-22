#CCE_Board_Definitions:BMI085;BMI088

COINES_INSTALL_PATH ?= ../../../..

EXAMPLE_FILE ?= bmi08x_interrupt_streaming_pc.c

API_LOCATION ?= ../../../bmi08x

C_SRCS += \
$(API_LOCATION)/bmi08a.c \
$(API_LOCATION)/bmi08g.c \

INCLUDEPATHS += \
$(API_LOCATION) \

override TARGET=PC

include $(COINES_INSTALL_PATH)/coines.mk
