# Minimal version we could use
cmake_minimum_required(VERSION 2.8)

# Project name
project(opencv-cpp)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} opencv_core)
