UJPEG_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(UJPEG_MOD_DIR)/ujpeg.c

# We can add our module folder to include paths if needed
# This is not actually needed in this ujpeg.
CFLAGS_USERMOD += -I$(UJPEG_MOD_DIR)

CFLAGS_EXTRA = -DMODULE_UJPEG_ENABLED=1
