# Using and Testing Nvidia Jetson Orin Hardware Jpeg Block/Engine

The Nvidia Jetson Orin has two hardware Jpeg Compression/Decompression blocks that in theory can speed up creating and decoding JPEG images, this code investigates encoding RGB images in host memory to JPEGs written to host memory, it also uses [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) to encode the same images for comparison.

An important thing to note about the Nvidia Hardware JPEG block is that it only accepts YUV420 as an imput image for compression, if your image is in some other format you will have to convert it to 420 first, this code uses [libyuv](https://chromium.googlesource.com/libyuv/libyuv/) for that.

On you Jetson you should find an Nvidia sample of encoding a JPEG image at /usr/src/jetson_multimedia_api/samples/05_jpeg_encode, this reads a YUV image from disk and converts it which is not what I needed.

## Requirements:
- Encode an image from Host Memory
- Encode the image to Host Memory
- Encode an image from rgb24 (i.e. R(8), G(8), B(8)) 
- Measure average time to encode JPEG image using the hardware block
- Compare this with libjpeg-turbo

## Process

To get the Hardware JPEG engine to compress an rgb24 image to/from memory we have to:
- Convert the image in memory from rgb24 to YUV420 (YCbCr 4:2:0)
- Allocate an NvBuffer and copy the YUV image data into it.
- Create a NvJPEGEncoder and call encodeFromBuffer() to encode into our output buffer
- Deallocate the NvBuffer


