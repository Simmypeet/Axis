# Collects all header files
set(AXIS_STB_IMAGE_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/stb_image/stb_image.hpp")
# Collects all source files
set(AXIS_STB_IMAGE_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/stb_image/stb_image.cpp")

# All source files in the project
set(AXIS_STB_IMAGE_ALL_SOURCE_FILES ${AXIS_STB_IMAGE_HEADER_FILES} ${AXIS_STB_IMAGE_SOURCE_FILES})

add_library(stb_image STATIC ${AXIS_STB_IMAGE_ALL_SOURCE_FILES})

target_include_directories(stb_image PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Include/")

# Uses C++20 standard
set_target_properties(stb_image PROPERTIES CXX_STANDARD 20)

# C++20 standard is required
set_target_properties(stb_image PROPERTIES CXX_STANDARD_REQUIRED TRUE)