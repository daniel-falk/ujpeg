#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpeg_encode_decode.h"

int main(int argc, char* argv[]) {
    assert(argc == 2 && "Missing file path as argument");
    printf("Opening file '%s'\n", argv[1]);

    struct raw_image image;
    char* error_str = NULL;
    int rc = jpeg_decode_from_path(argv[1], &image, &error_str);

    printf("Ret code: %d\n", rc);
    printf("Error string: %s\n", error_str ? error_str : "(null)");
    printf("Image size: %zu x %zu\n", image.width, image.height);
    free(image.buffer.start);

    return rc;
}
