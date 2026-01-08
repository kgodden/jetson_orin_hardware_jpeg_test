
// Lib JpegTurbo
#include <cstdio>
#include <cstring>
#include <jpeglib.h>

// This one is libJPEGTurbo baseline

unsigned long turbo_encode_jpeg_to_memory(unsigned char* image, int width, int height, int quality,
                            const char* comment, unsigned char** jpegBuf) {

#ifdef LIBJPEG_TURBO_VERSION

    printf("Linked to libjpeg-turbo!\n");

#else

    std::cout << "Error, LIBJPEG_TURBO_VERSION is not defined, we have not linked to libJPEGTurbo, probably libJpeg instead, exiting..." << std::endl;
    jpegBuf = nullptr;
    return 0;

#endif

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    cinfo.image_width = width;
    cinfo.image_height = height;

    // RGB
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    //
    //
    // Tell libJpeg to encode to memory, this is the bit that's different!
    // Lib will alloc buffer.
    //
    unsigned long jpegSize;

    jpeg_mem_dest(&cinfo, jpegBuf, &jpegSize);
    jpeg_start_compress(&cinfo, TRUE);

    // Add comment section if any..
    if (comment) {
        jpeg_write_marker(&cinfo, JPEG_COM, (const JOCTET*)comment, strlen(comment));
    }

    row_stride = width * cinfo.input_components;

    // Encode
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &image[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return jpegSize;
}


