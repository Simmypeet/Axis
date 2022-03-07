# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all header files
set(AXIS_WINDOW_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/WindowPch.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/WindowExport.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Mouse.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Keyboard.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Window"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/DisplayWindow.hpp")

# Collects all source files
set(AXIS_WINDOW_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Keyboard.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Mouse.cpp")

# Win32 platform specific header and source files
if(${AXIS_PLATFORM_WIN32})
    list(APPEND AXIS_WINDOW_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Platform/Win32/Win32DisplayWindow.hpp")
    list(APPEND AXIS_WINDOW_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Platform/Win32/Win32Mouse.hpp")

    list(APPEND AXIS_WINDOW_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32DisplayWindow.cpp")
    list(APPEND AXIS_WINDOW_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32Mouse.cpp")
    list(APPEND AXIS_WINDOW_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Platform/Win32/Win32Keyboard.cpp")
endif()

# All source files in the project
set(AXIS_WINDOW_ALL_SOURCE_FILES ${AXIS_WINDOW_SOURCE_FILES} ${AXIS_WINDOW_HEADER_FILES})

# All targets to link / combine
set(AXIS_WINDOW_TARGET_TO_LINK
    Axis-System)

# Adds Window module to the project
axis_add_library(Axis-Window
                    FOLDER "Axis/Axis"
                    RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}"
                    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                    SOURCES ${AXIS_WINDOW_ALL_SOURCE_FILES}
                    TARGETS_TO_LINK ${AXIS_WINDOW_TARGET_TO_LINK})

# Win32 platform specific lib
if(${AXIS_PLATFORM_WIN32})
    target_link_libraries(Axis-Window PRIVATE "winmm.lib")
    target_link_libraries(Axis-Window PRIVATE "shlwapi.lib")
endif()

# Adds precompiled header
target_precompile_headers(Axis-Window PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/WindowPch.hpp")
