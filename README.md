# ujpeg
Jpeg decoder library for micropython

! __Under construction__ !

Idea is to have a simple micropython library to
- encode jpeg images from raw bytes
- decode jpeg images from raw bytes
- decode jpeg image from file

Currently the jpeg decode code is working but micropython bindings are missing.

Compile test code with:
```bash
cd src
gcc test_main.c jpeg_encode_decode.c -Wall -Werror -ljpeg
```

# Contribute
All contributions are welcomed, such as:
- patchsets, bug fixes
- documentation
- ideas of API or refactorization
- feature requests

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
