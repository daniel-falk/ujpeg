import ujpeg

ujpeg.decode_file("test.jpg", "test.pgm")

try:
    ujpeg.decode_file("/does-not-exist.jpg", "test.pgm")
except Exception as e:
    print(type(e), e)

try:
    ujpeg.decode_file("test.jpg", "/")
except Exception as e:
    print(type(e), e)

# This will currently abort the program!
# We should create a new error handler for the jpeg decoder,
# now we use the default one that will exit() on pretty much
# any error in the decoding..
try:
    ujpeg.decode_file("test.pgm", "test.pgm")
except Exception as e:
    print(type(e), e)

# We never get here..
