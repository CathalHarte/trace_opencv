cmake_minimum_required(VERSION 3.12.0)
set(MODULE_NAME "color_matrix")

project(${MODULE_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(REPO_ROOT "..")

find_package(OpenCV REQUIRED)

# The target
add_library(${MODULE_NAME} ${MODULE_NAME}.cpp)
target_link_libraries(${MODULE_NAME} ${OpenCV_LIBS}) 