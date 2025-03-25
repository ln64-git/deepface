cmake_minimum_required(VERSION 3.14)
project(face_morph_filter)

set(CMAKE_CXX_STANDARD 17)

find_package(obs REQUIRED)
find_package(OpenCV REQUIRED)

add_library(face_morph_filter MODULE
    plugin.cpp
    morph_filter.cpp
    morph_filter.h
)

target_include_directories(face_morph_filter PRIVATE
    ${OBS_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(face_morph_filter
    ${OBS_LIBRARIES}
    ${OpenCV_LIBS}
)
