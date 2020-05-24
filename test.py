"""
We should include the micropython version of pytest to write real
unit tests. For now, lets just try the most important behaviour.
"""

import ujpeg

try:
    import os.path
    this_folder = os.path.dirname(__file__)
except ImportError:
    # If we are missing os.path module, assume test file
    # is in current working dir instead..
    this_folder = "./"


if __name__ == "__main__":
    jpeg_file = this_folder + "/test.jpg"

    print("Decode file to pgm...")
    try:
        ujpeg.decode_to_file(jpeg_file, "test.pgm")
        print("OK!")
    except Exception:
        print("Failed!")
        raise

    print("Decode file to ndarray...")
    try:
        img = ujpeg.decode(jpeg_file)
        assert img[0].shape() == (1080, 1920)
        print("OK!")
    except Exception:
        print("Failed")
        raise

    print("Try decode non existing file...")
    try:
        ujpeg.decode_to_file("/does-not-exist.jpg", "test.pgm")
        print("OK!")
    except Exception as e:
        print(type(e), e)

    print("Try writing output to invalid path...")
    try:
        ujpeg.decode_to_file(jpeg_file, "/")
        print("No exception!")
    except Exception as e:
        print(type(e), e)
        print("OK!")

    # This will currently abort the program!
    # We should create a new error handler for the jpeg decoder,
    # now we use the default one that will exit() on pretty much
    # any error in the decoding..
    print("Try to decode invalid jpeg file...")
    try:
        ujpeg.decode_to_file("test.pgm", "test.pgm")
        print("No exception!")
    except Exception as e:
        print(type(e), e)
        print("OK!")  # We never get here..

    # We never get here..
