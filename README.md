# Using and Testing Nvidia Jetson Orin Hardware Jpeg Block/Engine

The Nvidia Jetson Orin has two hardware Jpeg Compression/Decompression blocks that in theory can speed up encoding and decoding JPEG images, this code investigates encoding RGB images in host memory to JPEGs written back to host memory, it also uses [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) to encode the same images for comparison.

An important thing to note about the Nvidia Hardware JPEG block is that it only accepts YUV420 as an imput image for compression, if your image is in some other format you will have to convert it to 420 first, this code uses [libyuv](https://chromium.googlesource.com/libyuv/libyuv/) for that.

On your Jetson you should find an Nvidia sample of encoding a JPEG image at /usr/src/jetson_multimedia_api/samples/05_jpeg_encode, this reads a YUV image from disk and converts it which is not what I needed (installed as part of ther jetpack I think).

<img width="1040" height="432" alt="image" src="https://github.com/user-attachments/assets/0e5a303d-d460-48fa-b3dd-512d7b8eaa34" />

## Aims:
- Encode a packed rgb24 (R8, G8, B8) image to JPEG from Host Memory
- Encode the image back to Host Memory as JPEG
- Measure average time to encode JPEG image using the hardware block
- Compare this with libjpeg-turbo timings.

## Process

To get the Hardware JPEG engine to compress an rgb24 image to/from memory we have to:
- Convert the image in memory from rgb24 to YUV420 (YCbCr 4:2:0)
- Allocate an NvBuffer and copy the YUV image data into it.
- Create a NvJPEGEncoder and call encodeFromBuffer() to encode into our output buffer.
- Deallocate the NvBuffer

## The Tests
Each test creates an rbg24 image in memory, the code then encodes this image to jpeg multiple times and measures how long it takes to do this, it also saves the last image to disk.  When running the hardware encoder test you can double check that the hardware encoders are beng used by running the Jetson Power GUI and checking the values for **nvjpg0** and **nvjpg1**.

<img width="711" height="871" alt="image" src="https://github.com/user-attachments/assets/3aafa648-2a9c-4253-834c-2a109b5688f4" />

## Converting from RGB to YUV
The Jetson Orin has the means to convert image formats in hardware including converting from RGBA to YU420, however it seems that the JPEG Encoder can't directly access the output in its hardware/DMA memory and the YUV image must be copied back to host memory before passing it to the JPEG encoder block, the overhead of copying the data back and forth is high - I tested this and the overall encoding speeds were very slow, slower than using the CPU for JPEG encoding.  I also tried using CUDA to convert the image from rgb24 to YUC420 using the GPU, but this seemed to have the same problem using the low-levem multimedia API, it seemed that the output of the CUDA GPU processing was not directly accessable to the JPEG encoder and had to be copied back to host memory first before being passed to the encoder - I need to research this further, I may have missed something!

The fastest and simplest mechanism I could find was to use the SIMD optimised library libyuv to convert from RGB24 to YU420 using the CPU.

I will continue look into other more efficient methods of doing the the colour conversion and JEPG encoding all in hardware without repated copies in the future.

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
./nvjpeg_test 5328 3040 100
```

To run libjpeg-turbo test:

```bash
cd build
./jpeg_turbo_test 5328 3040 100
```

In both cases you can specify the width and height of the image and the number of iterations as arguments, if you leave these args out it defaults to 5328x3040 and 20 iterations.

Each will attempt to encode a test image 20 times and output the average time to encode an image, the tests also save the final image to disk for inspection.

## Initial results on Orin AGX

Ecoding the 5328x3040 JPEG images from RGB24 using the hardware encoder is about 30% faster than using libjpeg-turbo, which isn't mind blowing.... However it does use about 50% less CPU time when encoding which might be useful if your system is already CPU pressured. For smaller images like HD images the times are about the same.

<img width="607" height="157" alt="image" src="https://github.com/user-attachments/assets/df9e7c83-b75f-4172-9b86-de5b5619bbaf" />

I will continuie to investiage if all of the colour conversion and JPEG encoding can be achieved in hardare without having to copy the image data back and forth.



