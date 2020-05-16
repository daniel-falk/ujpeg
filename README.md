# ujpeg
Jpeg encoder/decoder library for micropython

Currently only decoding is implemented. 

```python
import ujpeg

ujpeg.decode_file("test.jpg", "test.pgm")
```

Idea is to have a simple micropython library to
- encode jpeg images from raw bytes
- decode jpeg images from raw bytes
- decode jpeg image from file

# How to use
The file structure follows the regular micropython c module workflow, see official instructions for details.

You need to link against libjpeg. If you are using Ubuntu or debian, do
```bash
apt-get install libjpeg-dev -y
```
or if cross-compiling, e.g.
```bash
apt-get install libjpeg-dev:armhf -y
```

A project using this specific library can be found here:
https://github.com/daniel-falk/camera-analytics

# Contribute
All contributions are welcomed, such as:
- patchsets, bug fixes
- documentation
- ideas of API or refactorization
- feature requests

The c-code test file can be compiled with:
```bash
cd src
gcc test_main.c jpeg_encode_decode.c -Wall -Werror -ljpeg
./a.out <path_to_file.jpg>
```

Python test file can be run after building micropython with this module:
```bash
./micropython test.py
```

## Coding style
C code should follow the Chromium style but with 4 spaces indent. This can be automatically done using e.g. `clang-format-9`:
```bash
clang-format-9 -style="{BasedOnStyle: chromium,IndentWidth: 4}" -i *.[ch]
```

## Finding areas of improvement
Start by reading the TODO labels in the code, e.g. using `grep`:
```bash
$ grep -nr "TODO"
jpeg_encode_decode.c:32:    // TODO: Risk for loop-for-ever if some error...
jpeg_encode_decode.c:49: * TODO: Add option to specify color space
jpeg_encode_decode.c:50: * TODO: Add option to scale image (libjpeg scale_num, scale_denom)
jpeg_encode_decode.c:51: * TODO: Add ability to reuse the same decompress object for e.g. video
jpeg_encode_decode.c:72:    // TODO: Replace error handler with something that won't exit..
jpeg_encode_decode.c:84:        // TODO: Seems like the return value should not be needed to check
jpeg_encode_decode.c:111:    // TODO: Use rec_outbuf_height lines... Avoid using too many at end
jpeg_encode_decode.c:123:    // TODO Remove this and return image data instead...
```
