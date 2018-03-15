cmake_minimum_required(VERSION 3.0)

project(lab1_sinusoidal_circles)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab1 lab1_sinusoidal_circles.cpp)
target_link_libraries(lab1 ${OpenCV_LIBS})



