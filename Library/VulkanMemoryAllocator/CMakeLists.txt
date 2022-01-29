# Collects all header files
set(AXIS_VULKAN_MEMORY_ALLOCATOR_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/VulkanMemoryAllocator/vk_mem_alloc.hpp")
# Collects all source files
set(AXIS_VULKAN_MEMORY_ALLOCATOR_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/VulkanMemoryAllocator/vk_mem_alloc.cpp")

# All source files in the project
set(AXIS_VULKAN_MEMORY_ALLOCATOR_ALL_SOURCE_FILES ${AXIS_VULKAN_MEMORY_ALLOCATOR_HEADER_FILES} ${AXIS_VULKAN_MEMORY_ALLOCATOR_SOURCE_FILES})

add_library(VulkanMemoryAllocator STATIC ${AXIS_VULKAN_MEMORY_ALLOCATOR_ALL_SOURCE_FILES})

target_include_directories(VulkanMemoryAllocator PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Include/")

# Includes Vulkan include directory
target_include_directories(VulkanMemoryAllocator PRIVATE "${Vulkan_INCLUDE_DIRS}")

# Links to Vulkan lib
target_link_libraries(VulkanMemoryAllocator PRIVATE Vulkan::Vulkan)

# Uses C++20 standard
set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD 20)

# C++20 standard is required
set_target_properties(VulkanMemoryAllocator PROPERTIES CXX_STANDARD_REQUIRED TRUE)