cmake_minimum_required(VERSION 3.0)

project(lab4_filters)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab4 lab4_filters.cpp)
target_link_libraries(lab4 ${OpenCV_LIBS})

