#include <string.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>

#include <jpeglib.h>

#include "jpeg_encode_decode.h"


/*
 * Decode an jpeg file from a file path
 *
 * If return value not zero, error will be an
 * malloced string, must be free'd by caller.
 */
int jpeg_decode_from_path(char *path, char **error) {
	struct stat file_info;

	int rc = stat(path, &file_info);
	if (rc) {
        *error = strdup("Failed to stat file");
        return 1;
	}

	size_t jpeg_size = file_info.st_size;
	uint8_t *jpeg_buffer = malloc(jpeg_size);

	int fd = open(path, O_RDONLY);
	int i = 0;
    // TODO: Risk for loop-for-ever if some error...
	while (i < jpeg_size) {
		rc = read(fd, jpeg_buffer + i, jpeg_size - i);
		i += rc;
	}
	close(fd);

    rc = jpeg_decode_from_buffer(jpeg_buffer, jpeg_size, error);
	free(jpeg_buffer);

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
int jpeg_decode_from_buffer(uint8_t *jpeg_buffer, size_t jpeg_size, char **error) {
    // Decompressor struct and jpeg error struct used to decompress image
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

    // Data about the raw image
	uint8_t *buffer;  // raw byte buffer
	size_t size;  // size of buffer in bytes
    size_t width;  // image width
    size_t height;  // image height
    size_t channels;  // Number of color channels per pixel
	size_t stride;  // Stride of image might not be same as width, i.e. if channels > 1

    // TODO: Replace error handler with something that won't exit..
	cinfo.err = jpeg_std_error(&jerr);	
	jpeg_create_decompress(&cinfo);

	// If other data reader is needed, see jpeg-8d/jdatasrc.c
	jpeg_mem_src(&cinfo, jpeg_buffer, jpeg_size);

    // This verifies that the image is OK and initializes the cinfo
    // with default parameters
	int rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != JPEG_HEADER_OK) {
        // TODO: Seems like the return value should not be needed to check unless
        // a suspending data source is used, which a mem_src is not?
        switch (rc) {
            case JPEG_HEADER_TABLES_ONLY:
                *error = strdup("Can't read jpeg header: JPEG_HEADER_TABLES_ONLY");
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
	
	width = cinfo.output_width;
	height = cinfo.output_height;
	channels = cinfo.output_components;

	stride = width * channels;
	size = stride * height;
	buffer = malloc(size);

    // TODO: Use rec_outbuf_height lines... Avoid using too many at end
    // since this will cause a non-fatal error reported
	while (cinfo.output_scanline < height) {
		unsigned char *buffer_array[1];
		buffer_array[0] = buffer + cinfo.output_scanline * stride;

		jpeg_read_scanlines(&cinfo, buffer_array, 1);

	}


	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);


    // TODO Remove this and return image data instead...
    {
        printf("First pixel values: %x %x %x\n", (uint32_t)buffer[0], (uint32_t)buffer[3], (uint32_t)buffer[6]);

        int fd = open("output.ppm", O_CREAT | O_WRONLY, 0666);
        char buf[1024];

        rc = sprintf(buf, "P6 %d %d 255\n", (int)width, (int)height);
        write(fd, buf, rc);
        write(fd, buffer, size);

        close(fd);
        free(buffer);
    }

	return 0;
}
