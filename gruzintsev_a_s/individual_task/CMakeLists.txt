cmake_minimum_required(VERSION 3.0)

project(individual_task)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(individual_task.bin main.cpp NightRainDetector.cpp NightRainDetector.h NightRainDetectorTester.cpp NightRainDetectorTester.h)

target_link_libraries(individual_task.bin ${OpenCV_LIBS} PocoFoundation )

