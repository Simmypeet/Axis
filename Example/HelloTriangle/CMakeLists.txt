include("../../CMake/Utility.cmake")

# Source file
set(AXIS_HELLOTRIANGLE_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/HelloTriangle/HelloTriangle.cpp")

# Targets to link
set(AXIS_HELLOTRIANGLE_TARGETS_TO_LNK
    Axis-System
    Axis-Window
    Axis-Graphics
    Axis-Core)

if(${AXIS_VULKAN_SUPPORT})
    list(APPEND AXIS_HELLOTRIANGLE_TARGETS_TO_LNK Axis-GraphicsVulkan)
endif()

axis_add_example(Axis-HelloTriangle
                 SOURCES ${AXIS_HELLOTRIANGLE_SOURCE_FILES}
                 TARGETS_TO_LINK "${AXIS_HELLOTRIANGLE_TARGETS_TO_LNK}"
                 FOLDER "Axis/Example"
                 RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}")
