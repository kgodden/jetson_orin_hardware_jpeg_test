#include <iostream>
#include <fstream>
#include <tuple>
#include <chrono>

extern unsigned long turbo_encode_jpeg_to_memory(unsigned char* image, int width, int height, int quality,
                            const char* comment, unsigned char** jpegBuf);

//
// Call like: libjpeg-turbo_test <width> <height> <iterations>
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
    uint8_t* rgb24_image = (uint8_t*)malloc(width * height * 3);

    // Create an input image with a lot of detail
    for (int j = 0 ; j != height; j++) {
     for (int i = 0; i != width; i++) {
            *(rgb24_image + (j * width + i) * 3) = i * j;
            *(rgb24_image + (j * width + i) * 3 + 1) = i;
            *(rgb24_image + (j * width + i) * 3 + 2) = j;
     }
    }


    auto blstart = std::chrono::steady_clock::now();

    unsigned long jpeg_size;
    unsigned char* jpeg_image = nullptr;

    for (int i = 0; i != iterations; i++) {

        jpeg_size = turbo_encode_jpeg_to_memory(rgb24_image, width, height, 95, "", &jpeg_image);

        if (jpeg_image == nullptr) {
            std::cout <<"Error compressing image, exiting..." << std::endl;
            return 1;
        }

        // Free jpeg image unless its the last one as
        // we want to save that one
        if (i < iterations - 1) {
            free(jpeg_image);
            jpeg_image = nullptr;
            jpeg_size = 0;
        }
    }

    auto blend = std::chrono::steady_clock::now();

    std::cout << "Turbo took (each) " << std::chrono::duration_cast<std::chrono::milliseconds>(blend - blstart).count() / iterations << "ms" << std::endl;

    std::ofstream f("./turbo.jpg", std::ios::binary);

    if (f) {
        f.write((const char*)jpeg_image, jpeg_size);
    }

    free(jpeg_image);
    free(rgb24_image);

    return 0;
}
