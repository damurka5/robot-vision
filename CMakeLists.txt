cmake_minimum_required(VERSION 3.17)
project(robot_vision)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)

include_directories( ${OpenCV_INCLUDE_DIRS})

add_executable(robot_vision main.cpp)

target_link_libraries( robot_vision  ${OpenCV_LIBS})