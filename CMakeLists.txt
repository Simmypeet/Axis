# Specifies required CMake's minimum version 
cmake_minimum_required(VERSION 3.8.0)

# Axis library top level project
project(Axis VERSION 0.1.0)

# Define GNU standard installation directories such as CMAKE_INSTALL_INCLUDEDIR, CMAKE_INSTALL_LIBDIR, etc.
include(GNUInstallDirs)

# Default value for AXIS_BUILD_TESTS
set(AXIS_BUILD_TESTS ON CACHE BOOL "Build tests")
# Default value for AXIS_BUILD_EXAMPLES
set(AXIS_BUILD_EXAMPLES ON CACHE BOOL "Build examples")
# Default value for AXIS_BUILD_EXAMPLES
set(AXIS_SKIP_INSTALLS OFF CACHE BOOL "Don't skip installation")

option(AXIS_BUILD_TESTS "Build Axis's test cases" ${AXIS_BUILD_TESTS})
option(AXIS_BUILD_EXAMPLES "Build Axis's example executables" ${AXIS_BUILD_EXAMPLES})
option(AXIS_SKIP_INSTALLS "Includes Axis's installations" ${AXIS_SKIP_INSTALLS})

# Sets default install prefix
set(CMAKE_INSTALL_PREFIX "Install")

# Uses C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Uses folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Finds vulkan
find_package(Vulkan)

# Platform detection
set(AXIS_PLATFORM_WIN32 FALSE CACHE INTERNAL "")
set(AXIS_PLATFORM_UNIVERSAL_WINDOWS FALSE CACHE INTERNAL "")

if (${WIN32})
    if(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
        set(AXIS_PLATFORM_UNIVERSAL_WINDOWS TRUE CACHE INTERNAL "Target platform: Windows Store")
        message(NOTICE "[Axis] Target platform            : Window store")
    else()
        set(AXIS_PLATFORM_WIN32 TRUE CACHE INTERNAL "Target platform: Win32") 
        message(NOTICE "[Axis] Target platform            : Win32")
    endif()
endif()

set(AXIS_GRAPHICS_VULKAN_MESSAGE "[Axis::Graphics] Vulkan Support   : FALSE")

# Axis Vulkan graphics backend support
set(AXIS_VULKAN_SUPPORT FALSE CACHE INTERNAL "Vulkan is not supported!")

# Vulkan SDK is supported
if (Vulkan_FOUND)
    set(AXIS_GRAPHICS_VULKAN_MESSAGE "[Axis::Graphics] Vulkan Support   : TRUE")
    set(AXIS_VULKAN_SUPPORT TRUE CACHE INTERNAL "Vulkan is supported")
endif()

message(NOTICE ${AXIS_GRAPHICS_VULKAN_MESSAGE})

# Adds Axis framework's third party libraries
add_subdirectory(Library)

# Adds Axis framework subdirectory
add_subdirectory(Axis)

if(${AXIS_BUILD_TESTS})
    # Enables testing
    enable_testing()
    # Adds test subdirectory
    add_subdirectory(Test/System)
endif()

if (${AXIS_BUILD_EXAMPLES})
    # Adds examples subdirectory
    add_subdirectory(Example/SimpleApplicationLoop)
    add_subdirectory(Example/HelloTriangle)
    add_subdirectory(Example/RotatingCube)
    add_subdirectory(Example/Texturing)
    add_subdirectory(Example/SpriteBatching)
endif()
