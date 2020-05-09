// Include required definitions first.
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

// This is the function which will be called from Python as ujpeg.add_ints(a, b).
STATIC mp_obj_t ujpeg_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a + b);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_2(ujpeg_add_ints_obj, ujpeg_add_ints);

// Define all properties of the ujpeg module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t ujpeg_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ujpeg) },
    { MP_ROM_QSTR(MP_QSTR_add_ints), MP_ROM_PTR(&ujpeg_add_ints_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ujpeg_module_globals, ujpeg_module_globals_table);

// Define module object.
const mp_obj_module_t ujpeg_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ujpeg_module_globals,
};

// Register the module to make it available in Python
MP_REGISTER_MODULE(MP_QSTR_ujpeg, ujpeg_user_cmodule, MODULE_UJPEG_ENABLED);
