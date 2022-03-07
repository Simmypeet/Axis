include("../../CMake/Utility.cmake")

# Source file
set(AXIS_TEXTURING_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/Texturing/Texturing.cpp")

# Targets to link
set(AXIS_TEXTURING_TARGETS_TO_LNK
    Axis-System
    Axis-Window
    Axis-Graphics
    Axis-Core)

if(${AXIS_VULKAN_SUPPORT})
    list(APPEND AXIS_TEXTURING_TARGETS_TO_LNK Axis-GraphicsVulkan)
endif()

axis_add_example(Axis-Texturing
                 SOURCES ${AXIS_TEXTURING_SOURCE_FILES}
                 TARGETS_TO_LINK "${AXIS_TEXTURING_TARGETS_TO_LNK}"
                 FOLDER "Axis/Example"
                 RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# Copies the Asset folder
add_custom_command(TARGET Axis-Texturing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_LIST_DIR}/Asset"
    "$<TARGET_FILE_DIR:Axis-Texturing>/Asset")
