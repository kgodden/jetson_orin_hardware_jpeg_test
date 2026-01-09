#include <iostream>
#include <chrono>
#include <tuple>
#include <fstream>

extern std::tuple<uint8_t*, size_t> encode_rgb24_to_jpeg_yuv(uint8_t* rgb_data, int width, int height, int quality = 90);

//
// Call like: nvjpeg_test <width> <height> <iterations>
//
// Defaults to 5328x3040, 20 iterations
//
int main(int argc, char **argv) {

    int width = 5328;
    int height = 3040;
    int iterations = 20;

    if (argc > 2) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }

    if (argc > 3) {
        iterations  = std::stoi(argv[3]);
    }

    std::cout << "Image size: " << width << "x" << height << ", Iterations: " << iterations << std::endl;

    // Allocate rgb24 image (8 + 8 + 8)
    // stride is the same as the width
    uint8_t* rgb24_image = new uint8_t[width * height * 3];

    // Create an input image with a lot of detail
    for (int j = 0 ; j != height; j++) {
     for (int i = 0; i != width; i++) {
            *(rgb24_image + (j * width + i) * 3) = i * j;
            *(rgb24_image + (j * width + i) * 3 + 1) = i;
            *(rgb24_image + (j * width + i) * 3 + 2) = i * j;
     }
    }

    std::cout << "Start" << std::endl;

    auto start = std::chrono::steady_clock::now();

    uint8_t* jpeg_image;
    size_t out_size;

    for (int i = 0; i != iterations; ++i) {
        std::tie(jpeg_image, out_size) = encode_rgb24_to_jpeg_yuv(rgb24_image, width, height, 95);

        if (jpeg_image == nullptr) {
            std::cout << "Error converting image, exiting" << std::endl;
            delete [] rgb24_image;
            return 1;
        }

        // Free the o/p image unless its the last oe
        // as we want to save it to disk.
        if (i < iterations - 1) {
            delete [] jpeg_image;
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "Average encode time " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / iterations << "ms" << std::endl;

    delete [] rgb24_image;

    std::ofstream f("./nvjpeg.jpg", std::ios::binary);

    if (f) {
        f.write((const char*)jpeg_image, out_size);
    }

    delete [] jpeg_image;

    return 0;
}
