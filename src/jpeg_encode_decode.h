#ifndef __UJPEG_ENCODE_DECODE
#define __UJPEG_ENCODE_DECODE

#include <inttypes.h>
#include <stddef.h>

int jpeg_decode_from_path(char* path, char** error);
int jpeg_decode_from_buffer(uint8_t* jpeg_buffer,
                            size_t jpeg_size,
                            char** error);

#endif
