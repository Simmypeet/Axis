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

    add_subdirectory("SPIRV-Headers")
    axis_set_directory_root_folder("SPIRV-Headers" "Axis/Library/SPRIV-Headers")
    add_subdirectory("SPIRV-Tools")
    axis_set_directory_root_folder("SPIRV-Tools" "Axis/Library/SPRIV-Tools")
    add_subdirectory("glslang")
    axis_set_directory_root_folder("glslang" "Axis/Library/glslang")
    add_subdirectory("SPIRV-Cross")
    axis_set_directory_root_folder("SPIRV-Cross" "Axis/Library/SPRIV-Cross")
endif()

# Adds stb_image to the project
add_subdirectory(stb_image)
axis_set_directory_root_folder("stb_image" "Axis/Library/stb_image")
