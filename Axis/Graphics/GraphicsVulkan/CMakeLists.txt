# Contains CMake utilities functions
include("../../../CMake/Utility.cmake")

if(${AXIS_VULKAN_SUPPORT})
    # Collects all the header files.
    set(AXIS_GRAPHICSVULKAN_HEADER_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanGraphicsSystem.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanPhysicalDevice.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsVulkanExport.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsVulkanPch.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanGraphicsDevice.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanDeviceQueueFamily.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanCommandPool.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanCommandBuffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanBuffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanUtility.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanDeviceContext.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanSwapChain.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanTexture.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanRenderPass.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanFence.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanFramebuffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanFramebufferCache.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanResourceHeapLayout.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanSampler.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanGraphicsPipeline.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanShaderModule.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanResourceHeap.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanDescriptorPool.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VkPtr.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/VulkanRenderPassCache.hpp")

    # Collects all the source files.
    set(AXIS_GRAPHICSVULKAN_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanPhysicalDevice.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanGraphicsDevice.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanDeviceQueueFamily.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanCommandPool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanCommandBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanDeviceContext.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanSwapCHain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanTexture.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanRenderPass.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanFramebuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanFramebufferCache.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanFence.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanRenderPassCache.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanResourceHeapLayout.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanSampler.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanGraphicsPipeline.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanShaderModule.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanResourceHeap.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanDescriptorPool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/VulkanGraphicsSystem.cpp")

    # Collects all source files.
    set(AXIS_GRAPHICSVULKAN_ALL_SOURCE_FILES ${AXIS_GRAPHICSVULKAN_HEADER_FILES} ${AXIS_GRAPHICSVULKAN_SOURCE_FILES})

    # All targets to link / combine
    set(AXIS_GRAPHICSVULKAN_TARGET_TO_LINK
        Axis-System
        Axis-Window
        Axis-Graphics)

    # Adds System module to the project
    axis_add_library(Axis-GraphicsVulkan
        FOLDER "Axis/Axis"
        RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}"
        INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
        SOURCES ${AXIS_GRAPHICSVULKAN_ALL_SOURCE_FILES}
        TARGETS_TO_LINK ${AXIS_GRAPHICSVULKAN_TARGET_TO_LINK}
        SKIP_INCLUDE_INSTALL TRUE)

    # Links external libraries privately
    target_link_libraries(Axis-GraphicsVulkan PRIVATE Vulkan::Vulkan SPIRV-Tools-opt SPIRV VulkanMemoryAllocator)

    # Links external libraries privately
    target_include_directories(Axis-GraphicsVulkan PRIVATE ${Vulkan_INCLUDE_DIRS})

    # Adds precompiled header
    target_precompile_headers(Axis-GraphicsVulkan PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsVulkanPch.hpp")
endif()
