# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all header files
set(AXIS_CORE_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/CoreExport.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/CorePch.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Application.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/ComponentCollection.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Core"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/ApplicationComponent.hpp")

# Collects all source files
set(AXIS_CORE_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Application.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/ComponentCollection.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/ApplicationComponent.cpp")

# All source files in the project
set(AXIS_CORE_ALL_SOURCE_FILES ${AXIS_CORE_HEADER_FILES} ${AXIS_CORE_SOURCE_FILES})

# All targets to link / combine
set(AXIS_CORE_TARGET_TO_LINK
    Axis-System
    Axis-Window
    Axis-Graphics)

# Adds System module to the project 
axis_add_library(Axis-Core
                FOLDER "Axis/Axis/Core"
                RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/../"
                INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                SOURCES ${AXIS_CORE_ALL_SOURCE_FILES}
                TARGETS_TO_LINK ${AXIS_CORE_TARGET_TO_LINK})

# Adds precompiled header
target_precompile_headers(Axis-Core PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/CorePch.hpp")

