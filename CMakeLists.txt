cmake_minimum_required(VERSION 3.14)
project(deepface_filter)

set(CMAKE_CXX_STANDARD 17)

# Include OBS headers
include_directories(
    ${CMAKE_SOURCE_DIR}/../obs-studio/libobs
    ${CMAKE_SOURCE_DIR}/../obs-studio/UI
    ${CMAKE_SOURCE_DIR}/../obs-studio/deps
)

# Add all source files
add_library(deepface_filter MODULE
    src/plugin.cpp
    src/morph_filter/morph_filter.cpp
    src/morph_filter/morph_filter.h
    src/landmark_detector/landmark_detector.cpp   # ✅ This line was missing!
    src/landmark_detector/landmark_detector.h
)

set_target_properties(deepface_filter PROPERTIES PREFIX "")

# Dependencies
find_package(OpenCV REQUIRED)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  /usr/include/onnxruntime
)

target_link_libraries(deepface_filter
  ${OpenCV_LIBS}
  onnxruntime
)
