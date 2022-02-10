/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_GRAPHICSCOMMONIMPL_INL
#define AXIS_GRAPHICS_GRAPHICSCOMMONIMPL_INL
#pragma once

#include "../../Include/Axis/GraphicsCommon.hpp"

namespace Axis
{

namespace Graphics
{

inline constexpr Bool IsTextureState(ResourceState state) noexcept
{
    return (Bool)(state & (TextureStatesExclusive | CommonStates)) && !(Bool)(state & BufferStatesExclusive) /* Make sure that doesn't contain buffer states also. */;
}

inline constexpr Bool IsBufferState(ResourceState state) noexcept
{
    return (Bool)(state & (BufferStatesExclusive | CommonStates)) && !(Bool)(state & TextureStatesExclusive) /* Make sure that doesn't contain texture states also. */;
}

inline constexpr TextureFormatComponentType GetTextureFormatComponentTypeFormTextureFormat(TextureFormat textureFormat)
{
    // clang-format off

    switch (textureFormat)
    {
    case TextureFormat::UnormR8:
    case TextureFormat::UnormB8G8R8A8:
    case TextureFormat::UnormR8G8B8A8:
    case TextureFormat::UnormB5G6R5Pack16:
    case TextureFormat::UnormB5G5R5A1Pack16:
    case TextureFormat::UnormR4G4B4A4Pack16:
    case TextureFormat::UnormA2B10G10R10Pack32:
    case TextureFormat::UnormA2R10G10B10Pack32:
    case TextureFormat::UnormR16G16:
    case TextureFormat::UnormR16G16B16A16:
    case TextureFormat::UnormStencil8:
    case TextureFormat::UnormDepth16:
    case TextureFormat::X8UnormDepth24Pack32:
    case TextureFormat::UnormDepth24Stencil8:
    case TextureFormat::UnormDepth16Stencil8:
        return TextureFormatComponentType::UnsignedNormalized;
    case TextureFormat::FloatR32:
    case TextureFormat::FloatR32G32:
    case TextureFormat::FloatR32G32B32A32:
    case TextureFormat::FloatR16:
    case TextureFormat::FloatR16G16:
    case TextureFormat::FloatR16G16B16A16:
    case TextureFormat::FloatDepth32:
        return TextureFormatComponentType::SignedFloat;
    case TextureFormat::UnormB8G8R8A8sRGB:
    case TextureFormat::UnormR8G8B8A8sRGB:
        return TextureFormatComponentType::UnsignedNormalizedSRGB;
    default:
        throw System::InvalidArgumentException("`textureFormat` was invalid!");
    }

    // clang-format on
}

inline constexpr Size GetShaderDataTypeSize(ShaderDataType shaderDataType)
{
    // clang-format off

    switch (shaderDataType)
    {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Float2: return 8;
        case ShaderDataType::Float3: return 12;
        case ShaderDataType::Float4: return 16;
        default: 
            throw System::InvalidArgumentException("`shaderDataType` was invalid!");
    }

    // clang-format on
}

inline constexpr Bool IsResourceUsageMappable(ResourceUsage resourceUsage)
{
    return (Bool)(resourceUsage & (ResourceUsage::Dynamic | ResourceUsage::StagingSource));
}

inline constexpr MemoryAccessFlags GetMemoryAccessFromResourceState(ResourceState state)
{
    switch (state)
    {
        case ResourceState::TransferDestination:
        case ResourceState::DepthStencilWrite:
            return MemoryAccess::Write;
        case ResourceState::DepthStencilRead:
        case ResourceState::Index:
        case ResourceState::TransferSource:
        case ResourceState::Vertex:
        case ResourceState::Present:
        case ResourceState::Uniform:
            return MemoryAccess::Read;
        case ResourceState::RenderTarget:
            return MemoryAccess::Read | MemoryAccess::Write;
        case ResourceState::Undefined:
            return MemoryAccess::None;
        default:
            throw System::InvalidArgumentException("`state` was invalid!");
    }
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_GRAPHICSCOMMONIMPL_INL