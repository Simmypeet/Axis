include("../../CMake/Utility.cmake")

# Source file
set(AXIS_ROTATINGCUBE_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/RotatingCube/RotatingCube.cpp")

# Targets to link
set(AXIS_ROTATINGCUBE_TARGETS_TO_LNK
    Axis-System
    Axis-Window
    Axis-Graphics
    Axis-Core)

if(${AXIS_VULKAN_SUPPORT})
    list(APPEND AXIS_ROTATINGCUBE_TARGETS_TO_LNK Axis-GraphicsVulkan)
endif()

axis_add_example(Axis-RotatingCube
                 SOURCES ${AXIS_ROTATINGCUBE_SOURCE_FILES}
                 TARGETS_TO_LINK "${AXIS_ROTATINGCUBE_TARGETS_TO_LNK}"
                 FOLDER "Axis/Example/RotatingCube"
                 RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}")