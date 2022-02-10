/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_TEXTUREIMPL_INL
#define AXIS_GRAPHICS_TEXTUREIMPL_INL
#pragma once

#include "../../Include/Axis/Texture.hpp"

namespace Axis
{

namespace Graphics
{

inline constexpr TextureViewUsageFlags GetTextureViewUsageFlagsFromTextureFormat(TextureFormat textureFormat)
{
    // clang-format off

    switch (textureFormat)
    {
    case TextureFormat::UnormR8G8B8A8:
    case TextureFormat::UnormB5G6R5Pack16:
    case TextureFormat::UnormB5G5R5A1Pack16:
    case TextureFormat::UnormA2B10G10R10Pack32:
    case TextureFormat::UnormR4G4B4A4Pack16:
    case TextureFormat::UnormA2R10G10B10Pack32:
    case TextureFormat::UnormR16G16:
    case TextureFormat::UnormR16G16B16A16:
    case TextureFormat::FloatR16:
    case TextureFormat::FloatR16G16:
    case TextureFormat::FloatR16G16B16A16:
    case TextureFormat::FloatR32:
    case TextureFormat::FloatR32G32:
    case TextureFormat::FloatR32G32B32A32:
    case TextureFormat::UnormR8G8B8A8sRGB:
    case TextureFormat::UnormB8G8R8A8:
        return TextureViewUsage::RenderTarget;
    case TextureFormat::X8UnormDepth24Pack32:
    case TextureFormat::FloatDepth32:
        return TextureViewUsage::Depth;
    case TextureFormat::UnormStencil8:
        return TextureViewUsage::Stencil;
    case TextureFormat::UnormDepth16Stencil8:
    case TextureFormat::UnormDepth24Stencil8:
        return TextureViewUsage::Depth | TextureViewUsage::Stencil;
    default:
        throw System::InvalidArgumentException("`textureFormat` was invalid!");
    }

    // clang-format on
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_TEXTUREIMPL_INL