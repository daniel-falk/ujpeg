// Include required definitions first.
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

// Include other needed headers
#include <stdlib.h>
#include <string.h>

// Include the logic we are wrapping
#include "jpeg_encode_decode.h"
#include "utils.h"

// Include ulab headers since we use the ndarrays
#include <ndarray.h>

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
            m_malloc_fail(string_buffer_len);
        }
        string_buffer = new;
    }

    // Copy the string to the static buffer
    return strncpy(string_buffer, string, string_buffer_len - 1);
}

// Helper to decode a file and return the raw image
// Raises a micropython error if it fails.
static struct raw_image decode_image(const char* input_path) {
    struct raw_image image;
    char* error_str = NULL;
    int rc = jpeg_decode_from_path(input_path, &image, &error_str);

    if (rc) {
        char* static_error_str =
            error_str ? get_static_string(error_str) : "Unknown error";
        free(error_str);
        mp_raise_msg_varg(&mp_type_ValueError, "Failed to decode file: %s",
                          static_error_str);
    }

    return image;
}

// Wrapper to decode a jpeg file and save to a ppm (raw) file
STATIC mp_obj_t ujpeg_decode_to_file(mp_obj_t input_path_obj,
                                     mp_obj_t output_path_obj) {
    const char* input_path = mp_obj_str_get_str(input_path_obj);
    const char* output_path = mp_obj_str_get_str(output_path_obj);

    // Read the image file and decode it to raw bytes
    struct raw_image image = decode_image(input_path);

    // Write the raw image to a pgm file
    int rc = write_image_ppm(image, output_path);

    if (rc) {
        mp_raise_msg_varg(&mp_type_ValueError, "Failed to write file: %s",
                          strerror(rc));
    }

    jpeg_free_raw_image(image);

    return mp_const_none;
}

// Wrapper to decode a jpeg file and get the raw image bytes
STATIC mp_obj_t ujpeg_decode(mp_obj_t input_path_obj) {
    const char* input_path = mp_obj_str_get_str(input_path_obj);

    // Read the image file and decode it to raw bytes
    struct raw_image image = decode_image(input_path);

    // Unfortunately ulab does not support 3 dimensional arrays,
    // only 1D or 2D so lets return a tuple with an ndarray for
    // each collor plane.
    assert(image.channels == 3);
    mp_obj_t mp_image[image.channels];
    for (size_t ch = 0; ch < image.channels; ++ch) {
        // We assume no row padding since we now image is produced by libjpeg..
        assert(image.width * image.channels == image.stride);

        // Creates the base ndarray with image color plane shape.
        // TODO: We should not use the ulab create_new_ndarray function since
        // it will initialize it to all zeros, which is inefficient when we
        // will overwrite it..
        size_t* shape = m_new(size_t, 2);
        shape[0] = image.height;
        shape[1] = image.width;
        ndarray_obj_t* ndarray =
            ndarray_new_dense_ndarray(2, shape, NDARRAY_UINT8);

        // TODO: Instead of copying the data we should create the ndarray first
        //       and hand it's buffer to the decoder to put data inplace. This
        //       is however not trivial since we need an array for each color
        //       channel.
        uint8_t* items = (uint8_t*)ndarray->array;
        for (size_t i = 0; i < image.height * image.width; ++i) {
            items[i] = image.buffer.start[i * image.channels + ch];
        }

        mp_image[ch] = MP_OBJ_FROM_PTR(ndarray);
    }

    jpeg_free_raw_image(image);

    // TODO: Not sure if this does a copy of all arrays or not,
    //       might need create tuple as a pointer and use MP_OBJ_FROM_PTR
    return mp_obj_new_tuple(3, mp_image);
}

// This creates micropython objects of the c-functions
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ujpeg_decode_to_file_obj,
                                 ujpeg_decode_to_file);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ujpeg_decode_obj, ujpeg_decode);

// This mapping defines all the properties of the module,
// The __name__ property is set to a string "ujpeg",
// The micropython function objects are added as properties, etc..
STATIC const mp_rom_map_elem_t ujpeg_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),
     MP_ROM_QSTR(MP_QSTR_ujpeg)},  // __name__ for the module
    {MP_ROM_QSTR(MP_QSTR_decode_to_file),
     MP_ROM_PTR(&ujpeg_decode_to_file_obj)},  // A function..
    {MP_ROM_QSTR(MP_QSTR_decode),
     MP_ROM_PTR(&ujpeg_decode_obj)},  // A function..
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
