# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all header files
set(AXIS_GRAPHICS_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/DeviceChild.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/DeviceContext.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Framebuffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Fence.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsCommon.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsPch.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsExport.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsDevice.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsSystem.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsPipeline.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Pipeline.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/ResourceHeapLayout.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/RenderPass.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/ShaderModule.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/StatedGraphicsResource.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Sampler.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Buffer.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Color.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/ResourceHeap.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SwapChain.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Texture.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/TextureLoader.hpp")

# Collects all source files
set(AXIS_GRAPHICS_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/DeviceChild.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/DeviceContext.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Framebuffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Fence.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/GraphicsDevice.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/GraphicsSystem.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Pipeline.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/ResourceHeapLayout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/RenderPass.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/ShaderModule.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/StatedGraphicsResource.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/GraphicsPipeline.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Buffer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Sampler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/ResourceHeap.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/SwapChain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/Texture.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/TextureLoader.cpp")

# Collects all private implementation files
set(AXIS_GRAPHICS_PRIVATE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/ColorImpl.hpp")

# All source files in the project
set(AXIS_GRAPHICS_ALL_SOURCE_FILES ${AXIS_GRAPHICS_HEADER_FILES} ${AXIS_GRAPHICS_SOURCE_FILES} ${AXIS_GRAPHICS_PRIVATE_FILES})

# All targets to link / combine
set(AXIS_GRAPHICS_TARGET_TO_LINK
    Axis-System
    Axis-Window)

# Adds System module to the project
axis_add_library(Axis-Graphics
                    FOLDER "Axis/Axis/Graphics"
                    RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/../"
                    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                    SOURCES ${AXIS_GRAPHICS_ALL_SOURCE_FILES}
                    TARGETS_TO_LINK ${AXIS_GRAPHICS_TARGET_TO_LINK})

# Adds precompiled header
target_precompile_headers(Axis-Graphics PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/GraphicsPch.hpp")

# Links to the library
target_link_libraries(Axis-Graphics PUBLIC stb_image)