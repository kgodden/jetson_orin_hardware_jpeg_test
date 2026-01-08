# Using and Testing Nvidia Jetson Orin Hardware Jpeg Block/Engine

The Nvidia Jetson Orin has two hardware Jpeg Compression/Decompression blocks that in theory can speed up creating and decoding JPEG images, this code investigates encoding RGB images in host memory to JPEGs written to host memory, it also uses [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) to encode the same images for comparison.

An important thing to note about the Nvidia Hardware JPEG block is that it only accepts YUV420 as an imput image for compression, if your image is in some other format you will have to convert it to 420 first, this code uses [libyuv](https://chromium.googlesource.com/libyuv/libyuv/) for that.

On you Jetson you should find an Nvidia sample of encoding a JPEG image at /usr/src/jetson_multimedia_api/samples/05_jpeg_encode, this reads a YUV image from disk and converts it which is not what I needed.

## Requirements:
- Encode an image to JPEG from Host Memory
- Encode the image back to Host Memory
- Encode an image from rgb24 (i.e. R(8), G(8), B(8)) 
- Measure average time to encode JPEG image using the hardware block
- Compare this with libjpeg-turbo timings.

## Process

To get the Hardware JPEG engine to compress an rgb24 image to/from memory we have to:
- Convert the image in memory from rgb24 to YUV420 (YCbCr 4:2:0)
- Allocate an NvBuffer and copy the YUV image data into it.
- Create a NvJPEGEncoder and call encodeFromBuffer() to encode into our output buffer.
- Deallocate the NvBuffer

## Converting from RGB to YUV
The Jetson Orin has the means to convert image formats in hardware via the VIC including converting from RGBA to YU420, however it seems that the JPEG Encoder can't directly access the VIC's output in it's hardware memory and YUV image must be copied back to host memory before passing it to the JPEG encoder block, the overhead of copying the data back and forth is high - I tested this and the overall encoding speeds were very slow, slower than using the CPU for JPEG encoding.

The fastest and simplest mechanism I could find was to use the SIMD optimised library libyuv to convert from RGB24 to YU420 using the CPU.

## The Code

**nvjpeg_rgb24_yuv_jpeg.cpp** - Code to JPEG the image using the Hardware JPEG Encoder block.
**nvjpeg_test.cpp** - Code to run the Hardware Encoder tests.

**libjpeg-turbo_jpeg.cpp** - Code to encode an image using libjpeg-turbo.
**jpeg-turbo_test.cpp** - Code to run the libjpeg-turbo Encoder tests.

# Building and Running the Tests

If the Nvidia Jetpack isn't already installed then install it:

```bash
sudo apt install nvidia-jetpack
```
You will also need libyuv:
```bash
sudo apt install libyuv-dev
```
To build:

```bash
cd build
cmake ..
make
```

To run NVJpeg test:

```bash
cd build
./nvjpeg_test
```

To run libjpeg-turbo test:

```bash
cd build
./jpeg_turbo_test
```

Each will attempt to encode a test image 20 times and output the average time to encode an image.

## Initial results on Orin AGX

Ecoding the JPEG images from RGB24 using the hardware encoder is about 30% faster than using libjpeg-turbo, which isn't much to write home about.... However it does use about 50% less CPU time when encoding.  More tests and analysis to follow...





