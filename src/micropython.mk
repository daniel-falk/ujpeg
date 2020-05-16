UJPEG_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_FILES := ujpeg.c jpeg_encode_decode.c utils.c
SRC_USERMOD += $(addprefix $(UJPEG_MOD_DIR)/, ${SRC_FILES})

# Link to the libraries we need
LDFLAGS_USERMOD += -ljpeg
