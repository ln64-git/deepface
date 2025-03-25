cmake_minimum_required(VERSION 3.14)
project(face_morph_filter)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/../obs-studio/libobs
    ${CMAKE_SOURCE_DIR}/../obs-studio/UI
    ${CMAKE_SOURCE_DIR}/../obs-studio/deps
)

add_library(deepface_filter MODULE
    plugin.cpp
    morph_filter.cpp
    morph_filter.h
)

set_target_properties(deepface_filter PROPERTIES PREFIX "")
