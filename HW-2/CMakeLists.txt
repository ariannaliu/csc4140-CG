cmake_minimum_required(VERSION 3.15)
project(HW2)

set(CMAKE_CXX_STANDARD 17)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(HW2 main.cpp rasterizer.hpp rasterizer.cpp Triangle.cpp Triangle.hpp)
target_link_libraries(HW2 ${OpenCV_LIBS})