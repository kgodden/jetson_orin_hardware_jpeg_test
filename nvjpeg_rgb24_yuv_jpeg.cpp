#include <libyuv.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "NvUtils.h"
#include "NvJpegEncoder.h"

// Ths seems to work uses libyuv to switch from rgb24 to yuv420 and encodeFromBuffer()
// This one is the fastest

std::tuple<uint8_t*, size_t> encode_rgb24_to_jpeg_yuv(uint8_t* rgb_data, int width, int height, int quality = 90)
{
    // Allocate I420 buffer (YUV420P)
    int y_size = width * height;
    int uv_size = (width / 2) * (height / 2);
    std::vector<uint8_t> y_plane(y_size);
    std::vector<uint8_t> u_plane(uv_size);
    std::vector<uint8_t> v_plane(uv_size);

    // First convert RGB24 to I420
    // nvJpeg engine only wrks with YUV420
    auto ret = libyuv::RAWToI420(
        rgb_data, width * 3,
        y_plane.data(), width,
        u_plane.data(), width / 2,
        v_plane.data(), width / 2,
        width, height);

    if (ret != 0) {
        std::cerr << "libyuv::RGB24ToI420 failed" << std::endl;
        return {};
    }

    // Create NvBuffer for YUV420M
    NvBuffer buffer(V4L2_PIX_FMT_YUV420M, width, height, 0);
    buffer.allocateMemory();

    // Copy to NvBuffer planes
    memcpy(buffer.planes[0].data, y_plane.data(), y_size);
    memcpy(buffer.planes[1].data, u_plane.data(), uv_size);
    memcpy(buffer.planes[2].data, v_plane.data(), uv_size);

    // Set pitches for each plane
    buffer.planes[0].bytesused = y_size;
    buffer.planes[1].bytesused = uv_size;
    buffer.planes[2].bytesused = uv_size;

    // Create JPEG encoder
    NvJPEGEncoder* jpegenc = NvJPEGEncoder::createJPEGEncoder("jpegenc");

    if (!jpegenc) {
        std::cerr << "Failed to create JPEG encoder" << std::endl;
        buffer.deallocateMemory();
        return {};
    }

    // Output buffer
    unsigned long out_buf_size = width * height * 3 / 2;
    uint8_t* out_buf = new uint8_t[out_buf_size];

    ret = jpegenc->encodeFromBuffer(buffer, JCS_YCbCr, &out_buf, out_buf_size, quality);
    if (ret < 0) {
        std::cerr << "encodeFromBuffer failed" << std::endl;
        buffer.deallocateMemory();
        delete jpegenc;
        return {};
    }

    // Cleanup
    buffer.deallocateMemory();
    delete jpegenc;

    return {out_buf, out_buf_size};
}
