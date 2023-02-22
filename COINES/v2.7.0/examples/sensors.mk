#BHY2
ifeq ($(SENSOR),$(filter $(SENSOR),BHA260 BHI260))
C_SRCS += \
$(COINES_INSTALL_PATH)/examples/bhy2/bhy2.c \
$(COINES_INSTALL_PATH)/examples/bhy2/bhy2_hif.c \
$(COINES_INSTALL_PATH)/examples/bhy2/bhy2_parse.c \

INCLUDEPATHS += \
$(COINES_INSTALL_PATH)/examples/bhy2 \
$(COINES_INSTALL_PATH)/examples/bhy2/firmware \

endif

###############################################################################
