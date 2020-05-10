#ifndef __UJPEG_ENCODE_DECODE
#define __UJPEG_ENCODE_DECODE

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

struct mem_buffer {
    uint8_t* start;
    size_t len;
};

struct raw_image {
    struct mem_buffer buffer;  // raw image buffer
    size_t width;              // image width
    size_t height;             // image height
    size_t channels;           // Number of color channels per pixel
    size_t stride;  // Stride of image might not be same as width, i.e. if
                    // channels > 1
};

int jpeg_decode_from_path(char* path, struct raw_image* image, char** error);

int jpeg_decode_from_buffer(struct mem_buffer jpeg_buffer,
                            struct raw_image* image,
                            char** error);

#endif
