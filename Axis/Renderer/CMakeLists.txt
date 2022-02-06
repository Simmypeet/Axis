# Contains CMake utilities functions
include("../../CMake/Utility.cmake")

# Collects all header files
set(AXIS_RENDERER_HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/RendererPch.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SpriteFont.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/RendererExport.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/PackSprite.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/Renderer"
    "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/SpriteBatch.hpp")

# Collects all source files
set(AXIS_RENDERER_SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/SpriteFont.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Source/Axis/SpriteBatch.cpp")

# Clloects all private header files
set (AXIS_RENDERER_PRIVATE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Private/Axis/PackSpriteImpl.inl")

# All source files in the project
set(AXIS_RENDERER_ALL_SOURCE_FILES ${AXIS_RENDERER_HEADER_FILES} ${AXIS_RENDERER_SOURCE_FILES} ${AXIS_RENDERER_PRIVATE_FILES})

# All targets to link / combine
set(AXIS_RENDERER_TARGET_TO_LINK 
    Axis-System 
    Axis-Window
    Axis-Graphics)

# Adds System module to the project
axis_add_library(Axis-Renderer
                 FOLDER "Axis/Axis/Renderer"
                 RELATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}/../"
                 INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include"
                 SOURCES ${AXIS_RENDERER_ALL_SOURCE_FILES}
                 TARGETS_TO_LINK ${AXIS_RENDERER_TARGET_TO_LINK})

# Links free type to the target
target_link_libraries(Axis-Renderer PRIVATE freetype)

# Adds target precompiled header
target_precompile_headers(Axis-Renderer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Include/Axis/RendererPch.hpp")
