COMPILER_PREFIX ?=
CC = $(COMPILER_PREFIX)gcc
LD = $(COMPILER_PREFIX)gcc
TARGET = PC
# On using Software reset API(coines_soft_reset()) from COINES, After reset device jumps to the address specified in APP_START_ADDRESS.
APP_START_ADDRESS ?= 0x00030000

ARCH = $(shell $(CC) -dumpmachine)

ifneq (,$(findstring x86_64,$(ARCH)))
     SUFFIX = 64
else
     SUFFIX = 32
endif

ifeq ($(OS),Windows_NT)
    CFLAGS += -D PLATFORM_WINDOWS -D LEGACY_USB_DRIVER -std=c99 -DAPP_START_ADDRESS=$(APP_START_ADDRESS)
    VARIANT = win$(SUFFIX)
    EXT = dll
    USB_LIB += -lsetupapi
    ifeq ($(notdir $(MAKE)),mingw32-make)
        SHELL = cmd
        RM  = del /s /q
        MKDIR = mkdir
        syspath = $(subst /,\,$(1))
    else
        RM = rm -rf
        MKDIR = mkdir -p
        syspath = $(subst /,/,$(1))
    endif
else
    INCLUDEPATHS_COINES += comm_driver/libusb-1.0
    MKDIR = mkdir -p
    RM  = rm -rf
    syspath = $(subst /,/,$(1))
    CFLAGS += -D PLATFORM_LINUX -D LIBUSB_DRIVER -fPIC
    EXT = so
    VARIANT = linux$(SUFFIX)
    USB_LIB += -lusb-1.0
endif

OBJ_DIR = build/$(VARIANT)

SRC_DIR = ..

ifeq ($(COINES_BACKEND),COINES_BRIDGE)
    C_SRCS_COINES += \
    coines_bridge.c \
    serial_com/serial_com.c

    INCLUDEPATHS_COINES += \
    . \
    .. \
    serial_com

else
    C_SRCS_COINES += \
    coines.c \
    comm_intf/comm_intf.c \
    comm_intf/comm_ringbuffer.c \
    comm_driver/usb.c \
    comm_driver/legacy_usb/legacy_usb_support.c

    INCLUDEPATHS_COINES += \
    . \
    .. \
    coines_api \
    comm_intf \
    comm_driver \
    comm_driver/legacy_usb

endif

C_SRCS = $(addprefix $(SRC_DIR)/,$(C_SRCS_COINES))
INCLUDEPATHS = $(addprefix $(SRC_DIR)/,$(INCLUDEPATHS_COINES))

C_FILES = $(notdir $(C_SRCS))
C_OBJS = $(addprefix $(OBJ_DIR)/, $(C_FILES:.c=.c.o))
C_PATHS = $(sort $(dir $(C_SRCS)))
DEP = $(C_OBJS:%.o=%.d)

vpath %.c $(C_PATHS)

CFLAGS += -c -D $(TARGET)

# Define COINES_VERSION as Git revision string, if
# - Git is installed 
# - it was run inside a Git repository
ifneq ($(shell $(WHERE) git) ,)
GIT_REVISION_STRING = $(shell git describe  --always --dirty --tags)
ifneq ($(GIT_REVISION_STRING),)
CFLAGS += -D COINES_VERSION=\"$(GIT_REVISION_STRING)\"
endif
endif

LDFLAGS += -shared

SHARED_LIB += coinespy/libcoines_$(SUFFIX).$(EXT)

all: $(SHARED_LIB)
	@echo CFLAGS_SAVE = $(CFLAGS) > $(OBJ_DIR)/cflags.save

$(OBJ_DIR):
	@echo [ MKDIR ] $@
	@$(MKDIR) $(call syspath,$@)
	
$(SHARED_LIB): $(OBJ_DIR) $(C_OBJS)
	@echo [ LD ] $@
	@$(CC) $(LDFLAGS) -o "$@" $(C_OBJS) $(USB_LIB)

-include $(DEP)

$(OBJ_DIR)/%.c.o: %.c
	@echo [ CC ] $< 
	@$(CC) $(CFLAGS) -MMD -I . $(addprefix -I,$(INCLUDEPATHS)) -o "$@" "$<"

clean:
	@echo "Cleaning..."
	@$(RM) $(call syspath,$(SHARED_LIB)) build