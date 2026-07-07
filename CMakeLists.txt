cmake_minimum_required(VERSION 3.10)

project(TransformBasedCompression)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(compress 
    src/main.cpp
    src/core/Metrics.cpp
    src/processing/SpatialTransformation.cpp
)

target_link_libraries(compress ${OpenCV_LIBS})