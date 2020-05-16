// Include required definitions first.
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

// Include other needed headers
#include <string.h>
#include <stdlib.h>

// Include the logic we are wrapping
#include "jpeg_encode_decode.h"
#include "utils.h"

// micropython exception can't be raised (?) with heap
// allocated message strings. We use this to copy the
// strings to a static buffer to avoid lelaking memory.
// Note that this implementation is not thread safe
// (and probably don't have to be...)
static char* get_static_string(const char* string) {
    static char* string_buffer = NULL;
    static size_t string_buffer_len = 0;

    // If static buffer is not large enought, make it larger..
    size_t len = strlen(string);
    if (len >= string_buffer_len) {
        string_buffer_len = len + 1;
        char* new = realloc(string_buffer, string_buffer_len);
        if (!new) {
            free(string_buffer);
            string_buffer_len = 0;
            mp_raise_ValueError("Memory allocation failed!");
        }
        string_buffer = new;
    }

    // Copy the string to the static buffer
    return strncpy(string_buffer, string, string_buffer_len - 1);
}

// Wrapper to decode a jpeg file and save to a ppm (raw) file
STATIC mp_obj_t ujpeg_decode_file(mp_obj_t input_path_obj,
                                  mp_obj_t output_path_obj) {
    const char* input_path = mp_obj_str_get_str(input_path_obj);
    const char* output_path = mp_obj_str_get_str(output_path_obj);

    int rc;

    // Read the image file and decode it to raw data
    struct raw_image image;
    char* error_str = NULL;
    rc = jpeg_decode_from_path(input_path, &image, &error_str);

    if (rc) {
        char* static_error_str =
            error_str ? get_static_string(error_str) : "Unknown error";
        free(error_str);
        mp_raise_msg_varg(&mp_type_ValueError, "Failed to decode file: %s",
                          static_error_str);
    }

    // Write the raw image to a pgm file
    rc = write_image_ppm(image, output_path);

    if (rc) {
        mp_raise_msg_varg(&mp_type_ValueError, "Failed to write file: %s",
                          strerror(rc));
    }

    jpeg_free_raw_image(image);

    return mp_const_none;
}

// This creates micropython objects of the c-functions
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ujpeg_decode_file_obj, ujpeg_decode_file);

// This mapping defines all the properties of the module,
// The __name__ property is set to a string "ujpeg",
// The micropython function objects are added as properties, etc..
STATIC const mp_rom_map_elem_t ujpeg_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),
     MP_ROM_QSTR(MP_QSTR_ujpeg)},  // __name__ for the module
    {MP_ROM_QSTR(MP_QSTR_decode_file),
     MP_ROM_PTR(&ujpeg_decode_file_obj)},  // A function..
};

// Create a python dict of the c styled property mapping
STATIC MP_DEFINE_CONST_DICT(ujpeg_module_globals, ujpeg_module_globals_table);

// Define the module micropython object
const mp_obj_module_t ujpeg_user_cmodule = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t*)&ujpeg_module_globals,
};

// Register the module to make it available in Python
MP_REGISTER_MODULE(MP_QSTR_ujpeg, ujpeg_user_cmodule, MODULE_UJPEG_ENABLED);
