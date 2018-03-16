cmake_minimum_required(VERSION 3.0)

project(lab2_brightness_range)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab2.bin lab2_brightness_range.cpp)
target_link_libraries(lab2.bin ${OpenCV_LIBS})

