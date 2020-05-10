#include "jpeg_encode_decode.h"

#include <fcntl.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

/*
 * Decode an jpeg file from a file path
 *
 * If return value not zero, error will be an
 * malloced string, must be free'd by caller.
 *
 * TODO: Use fd_src instead of reading everything into memory
 */
int jpeg_decode_from_path(char* path, struct raw_image* image, char** error) {
    struct stat file_info;

    int rc = stat(path, &file_info);
    if (rc) {
        *error = strdup("Failed to stat file");
        return ENOENT;
    }

    struct mem_buffer jpeg_buffer;
    jpeg_buffer.len = file_info.st_size;
    jpeg_buffer.start = malloc(jpeg_buffer.len);

    if (!jpeg_buffer.start) {
        *error = strdup("Failed to allocate memory for jpeg buffer");
        return ENOMEM;
    }

    int fd = open(path, O_RDONLY);

    if (fd < 0) {
        *error = strdup("Failed to open jpeg file");
        return errno;
    }

    int i = 0;
    // TODO: Risk for loop-for-ever if some error...
    while (i < jpeg_buffer.len) {
        rc = read(fd, jpeg_buffer.start + i, jpeg_buffer.len - i);
        i += rc;
    }

    close(fd);

    rc = jpeg_decode_from_buffer(jpeg_buffer, image, error);
    free(jpeg_buffer.start);

    return rc;
}

/*
 * Decode an jpeg file from a in-memory buffer and return it as
 * planar RGB color space.
 *
 * TODO: Add option to specify color space
 * TODO: Add option to scale image (libjpeg scale_num, scale_denom)
 * TODO: Add ability to reuse the same decompress object for e.g. video
 *
 * If return value not zero, error will be an
 * malloced string, must be free'd by caller.
 */
int jpeg_decode_from_buffer(struct mem_buffer jpeg_buffer,
                            struct raw_image* image,
                            char** error) {
    if (!jpeg_buffer.start || !jpeg_buffer.len) {
        *error = strdup("No jpeg image supplied");
        return EINVAL;
    }

    // Decompressor struct and jpeg error struct used to decompress image
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // TODO: Replace error handler with something that won't exit..
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // If other data reader is needed, see jpeg-8d/jdatasrc.c
    jpeg_mem_src(&cinfo, jpeg_buffer.start, jpeg_buffer.len);

    // This verifies that the image is OK and initializes the cinfo
    // with default parameters
    int rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != JPEG_HEADER_OK) {
        // TODO: Seems like the return value should not be needed to check
        // unless a suspending data source is used, which a mem_src is not?
        switch (rc) {
            case JPEG_HEADER_TABLES_ONLY:
                *error =
                    strdup("Can't read jpeg header: JPEG_HEADER_TABLES_ONLY");
                break;
            case JPEG_SUSPENDED:
                *error = strdup("Can't read jpeg header: JPEG_SUSPENDED");
                break;
            default:
                *error = strdup("Can't read jpeg header:");
        }
        return 255 + rc;
    }

    // This allocates memory and initializes the states
    jpeg_start_decompress(&cinfo);

    image->width = cinfo.output_width;
    image->height = cinfo.output_height;
    image->channels = cinfo.output_components;

    image->stride = image->width * image->channels;
    image->buffer.len = image->stride * image->height;
    image->buffer.start = malloc(image->buffer.len);

    // TODO: Use rec_outbuf_height lines... Avoid using too many at end
    // since this will cause a non-fatal error reported
    while (cinfo.output_scanline < image->height) {
        unsigned char* buffer_array[1];
        buffer_array[0] =
            image->buffer.start + cinfo.output_scanline * image->stride;

        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return 0;
}
