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
