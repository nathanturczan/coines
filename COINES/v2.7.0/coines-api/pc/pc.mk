ifeq ($(OS),Windows_NT)
    PLATFORM = PLATFORM_WINDOWS
    DRIVER ?= LEGACY_USB_DRIVER
else
    PLATFORM = PLATFORM_LINUX
    DRIVER = LIBUSB_DRIVER
endif

CC     = gcc

CFLAGS += -std=c99 -D$(PLATFORM) -c -O0 -g -Wall -D$(DRIVER)

ifeq ($(COINES_BACKEND),COINES_BRIDGE)
    C_SRCS_COINES += \
    coines_bridge.c \
    serial_com/serial_com.c

    INCLUDEPATHS_COINES += \
    . \
    serial_com

else
    C_SRCS_COINES += \
    coines.c \
    comm_intf/comm_intf.c \
    comm_intf/comm_ringbuffer.c \
    comm_driver/usb.c \

    INCLUDEPATHS_COINES += \
    . \
    coines_api \
    comm_intf \
    comm_driver \

endif


ifeq ($(DRIVER),LEGACY_USB_DRIVER)
C_SRCS_COINES += comm_driver/legacy_usb/legacy_usb_support.c
INCLUDEPATHS_COINES += comm_driver/legacy_usb
endif

ifeq ($(DRIVER),LIBUSB_DRIVER)
INCLUDEPATHS_COINES += comm_driver/libusb-1.0
endif
