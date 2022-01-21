# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all system's header files
set(AXIS_SYSTEM_HEADER_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Assert.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Config.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Memory.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemExport.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemPch.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/String.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Math.hpp"
   "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Trait.hpp")

# Collects all system's source files
set(AXIS_SYSTEM_SOURCE_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Memory.cpp")

# Collects all system's private files
set(AXIS_SYSTEM_PRIVATE_FILES
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/MemoryImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/MathImpl.inl"
   "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/StringImpl.inl")

# Collects all target's source files
set(AXIS_SYSTEM_ALL_SOURCE_FILES ${AXIS_SYSTEM_HEADER_FILES} ${AXIS_SYSTEM_SOURCE_FILES} ${AXIS_SYSTEM_PRIVATE_FILES})

# Adds System module to the project
axis_add_library(Axis-System
                    FOLDER "Axis/Axis/System"
                    RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/../"
                    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                    SOURCES ${AXIS_SYSTEM_ALL_SOURCE_FILES})

# Uses precompiled header
target_precompile_headers(Axis-System PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SystemPch.hpp")

# Win32 platform specific lib
if(${AXIS_PLATFORM_WIN32})
    target_link_libraries(Axis-System PRIVATE "winmm.lib")
    target_link_libraries(Axis-System PRIVATE "shlwapi.lib")
endif()