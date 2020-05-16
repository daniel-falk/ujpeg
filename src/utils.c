#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int write_image_ppm(struct raw_image image, const char* path) {
    char header_buf[1024];
    int header_len =
        sprintf(header_buf, "P6 %zu %zu 255\n", image.width, image.height);

    if (header_len < 0 || (size_t)header_len > sizeof(header_buf)) {
        return ENOMEM;
    }

    errno = 0;
    int fd = open(path, O_CREAT | O_WRONLY, 0666);

    if (fd < 0) {
        return errno;
    }

    write(fd, header_buf, header_len);
    write(fd, image.buffer.start, image.buffer.len);

    close(fd);

    return 0;
}
