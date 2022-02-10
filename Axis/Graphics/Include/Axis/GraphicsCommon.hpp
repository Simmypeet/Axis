/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_GRAPHICSCOMMON_HPP
#define AXIS_GRAPHICS_GRAPHICSCOMMON_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"
#include "../../../System/Include/Axis/Enum.hpp"
#include "../../../System/Include/Axis/Exception.hpp"
#include "../../../System/Include/Axis/Utility.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
struct RenderPassDescription;
struct FramebufferDescription;
struct TextureViewDescription;
struct PipelineDescription;
struct GraphicsCapability;

/// \brief Defines types of surface formats.
enum class TextureFormat : Uint8
{
    /// \brief Unknown or not supported!
    Unknown,

    /// \brief Unsigned normalized 8-Bits R component.
    UnormR8,

    /// \brief Unsigned normalized 32-Bits RGBA components, each channel contains 8-Bits.
    UnormR8G8B8A8,

    /// \brief Unsigned normalized 32-Bits BGRA components, each channel contains 8-Bits.
    UnormB8G8R8A8,

    /// \brief Unsigned normalized 16-Bits RGB components, 5-Bits for R, 6-Bits for G, 5-Bits for B.
    UnormB5G6R5Pack16,

    /// \brief Unsigned normalized 16-Bits ARGB components, 1-Bit for A and BGR 5-Bits for each.
    UnormB5G5R5A1Pack16,

    /// \brief Unsigned normalized 16-Bits ARGB components, each channel contains 4-Bits.
    UnormR4G4B4A4Pack16,

    /// \brief Unsigned normalized 32-Bits RGBA components, RGB 10-Bits for each and 2-Bits for A.
    UnormA2R10G10B10Pack32,

    /// \brief Unsigned normalized 32 Bits RG components, each channel contains 16-Bits.
    UnormR16G16,

    /// \brief Unsigned normalized 64 Bits RGBA components, each channel contains 16-Bits.
    UnormR16G16B16A16,

    /// \brief Unsigned normalized 32 Bits ABGR components, 2-Bits for A and BGR 10-Bits for each.
    UnormA2B10G10R10Pack32,

    /// \brief Signed Float32 32-Bits R channel.
    FloatR32,

    /// \brief Signed Float32 64-Bits RG components, each channel contains 32-Bits.
    FloatR32G32,

    /// \brief Signed Float32 128-Bits RGBA components, each channel contains 32-Bits.
    FloatR32G32B32A32,

    /// \brief Signed Float32 16-Bits R channel.
    FloatR16,

    /// \brief Signed Float32 32-Bits RG components, each channel contains 16-Bits.
    FloatR16G16,

    /// \brief Signed Float32 64-Bits RGBA components, each channel contains 16-Bits.
    FloatR16G16B16A16,

    /// \brief Unsigned normalized 32-Bits BGRA components, BGR 8-Bits for each stored in non-linear encoding, 8-Bits for A.
    UnormB8G8R8A8sRGB,

    /// \brief Unsigned normalized 32-Bits RGBA components, RGB 8-Bits for each stored in sRGB non-linear encoding, 8-Bits for A.
    UnormR8G8B8A8sRGB,

    /// \brief Unsigned normalized 16-Bits Depth component.
    UnormDepth16,

    /// \brief Signed Float32 32-Bits Depth component.
    FloatDepth32,

    /// \brief Unsigned normalized 8-Bits Stencil component.
    UnormStencil8,

    /// \brief Unsigned normalized 24-Bits Depth component, 8-Bits are optionally discarded.
    X8UnormDepth24Pack32,

    /// \brief Unsigned normalized 32-Bits Depth and Stencil components, 24-Bits for Depth, 8-Bits for Stencil.
    UnormDepth24Stencil8,

    /// \brief Unsigned normalized 24-Bits Depth and Stencil components, 16-Bits for Depth, 8-Bits for Stencil.
    UnormDepth16Stencil8,
};

/// \brief The type of component contained in Axis::TextureFormat.
enum class TextureFormatComponentType : Uint8
{
    /// \brief Unkown component.
    Unknown,

    /// \brief The texture format is made up of signed Float32
    SignedFloat,

    /// \brief The texture format is made up of unsigned integer.
    UnsignedInt,

    /// \brief The texture format is made up of signed integer.
    SignedInt,

    /// \brief The texture format is made up of signed normalized integer.
    SignedNormalized,

    /// \brief The texture format is made up of unsigned normalized integer.
    UnsignedNormalized,

    /// \brief The texture format is made up of unsigned normalized integer in sRGB linear encoding.
    UnsignedNormalizedSRGB,
};

/// \brief Supported Shader stages.
enum class ShaderStage : Uint8
{
    /// \brief Handles the processing of individual vertices fed into the shader.
    ///        Vertex shader stage typically performs the transformation on the vertices.
    Vertex = AXIS_BIT(1),

    /// \brief (AKA: PixelShader) Handles the color fragments output from the rasterization.
    Fragment = AXIS_BIT(2),
};

/// \brief Supported Shader stages. (Bit mask)
typedef ShaderStage ShaderStageFlags;

/// \brief The usage of the \a `TextureFormat`.
enum class TextureFormatUsageType : Uint8
{
    /// \brief Can be used in color attachment.
    ColorAttachment = AXIS_BIT(1),

    /// \brief Can be used in depth attachment.
    Depth = AXIS_BIT(2),

    /// \brief Can be used in stencil attachment.
    Stencil = AXIS_BIT(3),
};

/// \brief The usage of the \a `TextureFormat`. (Bit mask)
typedef TextureFormatUsageType TextureFormatUsageTypeFlags;

/// \brief The type or layout of the attachments.
enum class ViewLayout : Uint8
{
    /// \brief Undefined resource state.
    Undefined,

    /// \brief Color attachment.
    RenderTarget,

    /// \brief Depth and Stencil attachment writeable.
    DepthStencilWrite,

    /// \brief Depth and stencil attachment read only.
    DepthStencilRead,

    /// \brief Layout used for presentation.
    Present,
};

/// \brief Specifies memory access types that will participate in a memory dependency.
enum class AccessMode : Uint8
{
    /// \brief No access.
    None,

    /// \brief Read access to a Color RenderTarget, such as via blending,
    ///        Logic operations, or via certain subpass Load operations.
    RenderTargetWrite,

    /// \brief Write access to a Color, Resolve, or Depth / Stencil attachments via Depth / Stencil operations.
    ///        or via Load or Store operations.
    RenderTargetRead,

    /// \brief Specifies all memory access.
    MemoryReadWrite,
};

/// \brief Specifies memory access types that will participate in a memory dependency. (Bit mask)
typedef AccessMode AccessModeFlags;

/// \brief Enumeration contains the whole rendering pipeline stages.
enum class PipelineStage : Uint32
{
    /// \brief The top of the pipeline.
    TopOfPipeline = AXIS_BIT(1),

    /// \brief The stage where DrawIndirect / DrawDispatchIndirect data structures are consumed.
    DrawIndirect = AXIS_BIT(2),

    /// \brief The stage where Vertex/Index buffer are consumed.
    VertexInput = AXIS_BIT(3),

    /// \brief The stage where VertexShader are performed.
    VertexShader = AXIS_BIT(4),

    /// \brief The stage where FragmentShader are performed.
    FragmentShader = AXIS_BIT(5),

    /// \brief The stage where early Depth / Stencil test are performed. also includes Subpass Load and Store operations
    ///        and Multiple resolve operations for Framebuffer attachments with a Color or DepthStencil format.
    EarlyFragmentTest = AXIS_BIT(6),

    /// \brief The stage where late Depth / Stencil test are performed. also includes Subpass Load and Store operations
    ///        and Multiple resolve operations for Framebuffer attachments with a Color or DepthStencil format.
    LateFragmentTest = AXIS_BIT(7),

    /// \brief The stage after Color blending and he Color are output from the pipeline, also includes
    ///        Subpass Load and Store operations and Multisample resolve operations for Framebuffer attachments
    ///        with a Color or DepthStencil format.
    RenderTarget = AXIS_BIT(8),

    /// \brief The bottom of the pipeline.
    BottomOfPipeline = AXIS_BIT(9),
};

/// \brief Enumeration contains the whole rendering pipeline stages. (Bit mask)
typedef PipelineStage PipelineStageFlags;

/// \brief The data types used in the Shader Code.
enum class ShaderDataType : Uint8
{
    /// \brief 32-Bit floating point number.
    ///
    /// \note counterpart: Float32.
    Float,

    /// \brief 2 32-Bit floating point numbers.
    ///
    /// \note GLSL counterpart      : vec2.
    ///       Axis counterpart   : Axis::Vector2<Float32>
    Float2,

    /// \brief 3 32-Bit floating point numbers.
    ///
    /// \note GLSL counterpart: vec3.
    Float3,

    /// \brief 4 32-Bit floating point numbers.
    ///
    /// \note GLSL counterpart      : vec4.
    ///       Axis counterpart   : Axis::Color<Float32>
    Float4,
};

/// \brief Specifies the access mode to specific memory.
enum class MemoryAccess : Uint8
{
    /// \brief No access at all
    None = 0,

    /// \brief Read access
    Read = AXIS_BIT(1),

    /// \brief Write access
    Write = AXIS_BIT(2),
};

/// \brief The usage of the Axis::TextureFormat. (Bit mask)
typedef MemoryAccess MemoryAccessFlags;

/// \brief The usage of the resource's memory
enum class ResourceUsage : Uint8
{
    /// \brief The GPU memory won't be accessible to the CPU at all. GPU access to
    ///        this type of memory will typically the fastest.
    Immutable,

    /// \brief The GPU merry will be mapped and written by the CPU, as well as read directly by the GPU.
    ///        GPU access to this type of memory is still preferably fast.
    Dynamic,

    /// \brief The memory will be on the system and mappable to the CPU, GPU access to this type of memory is slow.
    StagingSource,
};

/// \brief Supported operations in a device queue.
enum class QueueOperation : Uint8
{
    /// \brief Specifies that queues supports transfer operations.
    Transfer = AXIS_BIT(1),

    /// \brief Specifies that queues supports compute operations.
    Compute = AXIS_BIT(2),

    /// \brief Specifies that queues supports graphics operations.
    Graphics = AXIS_BIT(3),
};

/// \brief Supported operations in a device queue. (Bit mask)
typedef QueueOperation QueueOperationFlags;

/// \brief The state of the graphics resource.
enum class ResourceState : Uint32
{
    /// \brief The resource is in undefined state, typically is the state after resource was created. (Buffer, Texture)
    Undefined = AXIS_BIT(1),

    /// \brief The resource is used as the source of transfer. (Buffer, Texture)
    TransferSource = AXIS_BIT(2),

    /// \brief The resource is used as the destination of transfer. (Buffer, Texture)
    TransferDestination = AXIS_BIT(3),

    /// \brief The resource is used as the uniform buffer. (Buffer)
    Uniform = AXIS_BIT(4),

    /// \brief Vertex buffer resource state. (Buffer)
    Vertex = AXIS_BIT(5),

    /// \brief Index buffer resource state. (Buffer)
    Index = AXIS_BIT(6),

    /// \brief Render target resource state. (Texture: color read/write attachment)
    RenderTarget = AXIS_BIT(7),

    /// \brief Depth stencil read resource state. (Texture)
    DepthStencilRead = AXIS_BIT(8),

    /// \brief Depth stencil write resource state. (Texture)
    DepthStencilWrite = AXIS_BIT(9),

    /// \brief The resource is being read by shaders. (Texture)
    ShaderReadOnly = AXIS_BIT(10),

    /// \brief Presentation (in SwapChain). (Texture)
    Present = AXIS_BIT(11),
};

/// \brief Available value comparison functions.
enum class CompareFunction : Uint8
{
    /// \brief Always returns true value.
    AlwaysTrue,

    /// \brief Always returns false value.
    AlwaysFalse,

    /// \brief Returns true value if the value is greater than another.
    Greater,

    /// \brief Returns true value if the value is greater than or equal to another.
    GreaterEqual,

    /// \brief Returns true value if the value is lesser than another.
    Lesser,

    /// \brief Returns true value if the value is lesser than or equal to another.
    LesserEqual,

    /// \brief Returns true if the value is equal to another.
    Equal,

    /// \brief Returns true if the value is not equal to another.
    NotEqual,
};

/// \brief Specifies the state transition behaviour when resource is
///        used by the device context.
enum class StateTransition : Uint8
{
    /// \brief Device context will transit the resource state automatically if needed.
    ///        The problem is raised when working in the multithreaded environment, data races
    ///        might occurs and automatic resource state transition might working in undefined behaviour.
    ///        `Explicit` resource state transition is needed.
    Transit,

    /// \brief This flag indicates that the client is already explicitly transited the resource state
    ///        to the required one.
    Explicit,
};

/// \brief Specifies the buffer mapping behaviour.
enum class ResourceMapType : Uint8
{
    /// \brief Maps the buffer without synchronization.
    Default,

    /// \brief Discards the old memory of the buffer and allocates the new one.
    ///        This way you can avoid the memory races.
    Discard,
};

/// \brief The state of the graphics resource. (Bit mask)
typedef ResourceState ResourceStateFlags;

/// \brief States that can only be found in texture
constexpr ResourceStateFlags TextureStatesExclusive =
    ResourceState::DepthStencilRead |
    ResourceState::DepthStencilWrite |
    ResourceState::RenderTarget |
    ResourceState::ShaderReadOnly |
    ResourceState::Present;

/// \brief States that can only be found in buffer
constexpr ResourceStateFlags BufferStatesExclusive =
    ResourceState::Index |
    ResourceState::Vertex |
    ResourceState::Uniform;

/// \brief States that can be found in both texture and buffer
constexpr ResourceStateFlags CommonStates =
    ResourceState::TransferDestination |
    ResourceState::TransferSource |
    ResourceState::Undefined;

/// \brief Checks if the specified resource state is belong to the texture.
constexpr Bool IsTextureState(ResourceState state) noexcept;

/// \brief Checks if the specified resource state is belong to the buffer.
constexpr Bool IsBufferState(ResourceState state) noexcept;

/// \brief Gets the \a `TextureFormatComponentType` from the specified \a `TextureFormat`
constexpr TextureFormatComponentType GetTextureFormatComponentTypeFormTextureFormat(TextureFormat textureFormat);

/// \brief Gets the size (in bytes) of \a `ShaderDataType`.
constexpr Size GetShaderDataTypeSize(ShaderDataType shaderDataType);

/// \brief Checks whether if the specified resource usage is mappable or not.
constexpr Bool IsResourceUsageMappable(ResourceUsage resourceUsage);

/// \brief Gets \a `MemoryAccess` from \a `ResourceState`.
constexpr MemoryAccessFlags GetMemoryAccessFromResourceState(ResourceState state);

} // namespace Graphics

} // namespace Axis

#include "../../Private/Axis/GraphicsCommonImpl.inl"

#endif // AXIS_GRAPHICS_GRAPHICSCOMMON_HPP