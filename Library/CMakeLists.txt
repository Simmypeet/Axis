# Contains CMake utilities functions
include("../CMake/Utility.cmake")

# Workaround for suppressing external targets' messages
function(message)
    if (NOT MESSAGE_QUIET)
        _message(${ARGN})
    endif()
endfunction()

set(MESSAGE_QUIET ON)

# Adds Axis-GraphicsVulkan's external dependencies to the project
if(${AXIS_VULKAN_SUPPORT})
    # Adds VulkanMemoryAllocator target to the project
    add_subdirectory(VulkanMemoryAllocator)
    axis_set_directory_root_folder("VulkanMemoryAllocator" "Axis/Library/VulkanMemoryAllocator")

    set(SPIRV_HEADERS_SKIP_INSTALL ON CACHE BOOL "Skip SPIRV headers installation")

    add_subdirectory("SPIRV-Headers")
    axis_set_directory_root_folder("SPIRV-Headers" "Axis/Library/SPRIV-Headers")

    set(SPIRV_SKIP_TESTS ON CACHE BOOL "Skip SPIRV tests")
    set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "Skip building SPIRV executable and tests along with the library")
    set(SPIRV_WERROR OFF CACHE BOOL "Do not enable error on warning")
    set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "Skip SPIRV tools installation")
    set(SPIRV_HEADERS_SKIP_INSTALL ON CACHE BOOL "Skip SPIRV headers installation")

    add_subdirectory("SPIRV-Tools")
    axis_set_directory_root_folder("SPIRV-Tools" "Axis/Library/SPRIV-Tools")

    set(SPIRV_TOOLS_EXCLUDE_TARGETS SPIRV-Tools-link SPIRV-Tools-reduce SPIRV-Tools-shared SPIRV-Headers-example)

    foreach(taget ${SPIRV_TOOLS_EXCLUDE_TARGETS})
        set_target_properties(${taget} PROPERTIES EXCLUDE_FROM_ALL TRUE)
        set_target_properties(${taget} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endforeach()

    set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Do not build glslang binaries")
    set(ENABLE_SPVREMAPPER OFF CACHE BOOL "Do not build SPIRV remapper")
    set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "Skip glslang installation")
    set(ENABLE_CTEST OFF CACHE BOOL "Disable testing")

    add_subdirectory("glslang")
    axis_set_directory_root_folder("glslang" "Axis/Library/glslang")

    unset(CMAKE_DEBUG_POSTFIX CACHE)

    set(SPIRV_CROSS_CLI OFF CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC.")
    set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "Enable SPIRV-Cross tests.")
    set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "Enable HLSL target support.")
    set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "Enable C++ target support.")
    set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "Enable JSON reflection target support.")
    set(SPIRV_CROSS_ENABLE_C_API OFF CACHE BOOL "Enable C API wrapper support in static library.")
    set(SPIRV_CROSS_ENABLE_UTIL OFF CACHE BOOL "Enable util module support.")
    set(SPIRV_CROSS_FORCE_PIC ON CACHE BOOL "Force position-independent code.")
    set(SPIRV_CROSS_SKIP_INSTALL ON CACHE BOOL "Skip SPIRV-Cross installation.")

    add_subdirectory("SPIRV-Cross")
    axis_set_directory_root_folder("SPIRV-Cross" "Axis/Library/SPRIV-Cross")
endif()

# Adds stb_image to the project
add_subdirectory(stb_image)
axis_set_directory_root_folder("stb_image" "Axis/Library/stb_image")

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

# Adds freetype to the project
add_subdirectory(freetype EXCLUDE_FROM_ALL)
axis_set_directory_root_folder("freetype" "Axis/Library/freetype" )