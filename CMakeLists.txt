cmake_minimum_required(VERSION 3.0)

project(jetson_orin_hardware_rgb_jpeg)

add_executable(jetson_orin_hardware_rgb_jpeg main.cpp)

install(TARGETS jetson_orin_hardware_rgb_jpeg RUNTIME DESTINATION bin)
