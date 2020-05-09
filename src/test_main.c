#include "jpeg_encode_decode.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    assert(argc == 2 && "Missing file path as argument");
    printf("Opening file '%s'\n", argv[1]);

    char *error_str = NULL;
    int rc = jpeg_decode_from_path(argv[1], &error_str);

    printf("Ret code: %d\n", rc);
    printf("Error string: %s\n", error_str ? error_str : "(null)");

    return rc;
}
