cmake_minimum_required(VERSION 3.0)

project(lab3_contrast_correction)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab3.bin lab3_contrast_correction)
target_link_libraries(lab3.bin ${OpenCV_LIBS})

