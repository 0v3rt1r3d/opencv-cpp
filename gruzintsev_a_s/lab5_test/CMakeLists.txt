cmake_minimum_required(VERSION 3.0)

project(lab5_test)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab5_test.bin lab5_test.cpp)
target_link_libraries(lab5_test.bin ${OpenCV_LIBS})

