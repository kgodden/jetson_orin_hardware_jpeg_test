###############################################################################################
#
# Tests Nvidia Jetson Orin Jpeg Engine compressing 24 bit RGB images in host memory to memory
# also tests libjpeg-turbo doing the same for comparison.
#
# nvJpeg Test:          nvjpeg_test
# libjpeg-turbo Test:   jpeg_turbo
#
# To build you will need t have the Nvidia jetpack installed as well as libyuv
# libjpeg-turbo should already be installed on your jetson
#
# cd build
# cmake ..
# make
#
#
# Kevin Godden 01/26
#
###############################################################################################


cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)


project(jetson_orin_hardware_rgb_jpeg)

###############################################################################################
#
# nvjpeg_test tests jpegging to memory from rgb24 using Jetson's JPEG Hardware enngine.
#
# compiled as: build/nvjpeg_test
#
#
###############################################################################################

add_executable(nvjpeg_test
     nvjpeg_test.cpp               # defines main()
     nvjpeg_rgb24_yuv_jpeg.cpp     # Implements jpeg-ing code

     # Add all of the Nvidia Jpeg code from the samples
     /usr/src/jetson_multimedia_api/samples/common/classes/NvJpegEncoder.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvBuffer.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvElement.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvElementProfiler.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvLogging.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvV4l2Element.cpp
     /usr/src/jetson_multimedia_api/samples/common/classes/NvV4l2ElementPlane.cpp

)

target_include_directories(nvjpeg_test PRIVATE
     /usr/src/jetson_multimedia_api/include
     /usr/src/jetson_multimedia_api/include/libjpeg-8b
)


target_link_directories(nvjpeg_test PRIVATE
     /usr/lib/aarch64-linux-gnu/nvidia
)


target_link_libraries(nvjpeg_test PRIVATE
     nvjpeg
     nvv4l2
     nvbufsurface
     yuv
)


###############################################################################################
#
# libjpeg-turbo tests jpegging to memory from rgb24 using libjpeg-turbo.
#
# compiled as: build/jpeg-turbo_test
#
###############################################################################################

find_package(JPEG REQUIRED)

add_executable(jpeg_turbo_test
     jpeg-turbo_test.cpp           # defines main()
     libjpeg-turbo_jpeg.cpp        # jpegs to memory buffer
)

target_link_libraries(jpeg_turbo_test PRIVATE
     ${JPEG_LIBRARIES}
)

install(TARGETS nvjpeg_test jpeg_turbo_test RUNTIME DESTINATION bin)
