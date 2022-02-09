/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_TEXTURE_HPP
#define AXIS_GRAPHICS_TEXTURE_HPP
#pragma once

#include "../../../System/Include/Axis/Exception.hpp"
#include "../../../System/Include/Axis/Span.hpp"
#include "../../../System/Include/Axis/Vector3.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "StatedGraphicsResource.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
struct TextureViewDescription;
class ITextureView;
class IDeviceContext;

/// \brief Defines the Dimension of the Texture.
enum class TextureDimension : Uint8
{
    /// \brief 1-Dimensional texture.
    Texture1D,

    /// \brief 2-Dimensional texture.
    Texture2D,

    /// \brief 3-Dimensional texture.
    Texture3D,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Texture3D,
};

/// \brief Defines the available texture binding purposes.
enum class TextureBinding : Uint8
{
    /// \brief The texture can be used as the destination of data transferation.
    TransferDestination = AXIS_BIT(1),

    /// \brief The texture can be used as the source of data transferation.
    TransferSource = AXIS_BIT(2),

    /// \brief The texture can be sampled in the shaders.
    Sampled = AXIS_BIT(3),

    /// \brief The texture is created for render target attachments.
    RenderTarget = AXIS_BIT(4),

    /// \brief The texture is created for depth / stencil attachments.
    DepthStencilAttachment = AXIS_BIT(5),

    /// \brief Required for enum reflection.
    MaximumEnumValue = DepthStencilAttachment,
};

/// \brief Defines the available texture binding purposes. (Bit mask)
typedef TextureBinding TextureBindingFlags;

/// \brief Description of ITexture resource.
struct TextureDescription final
{
    /// \brief Defines the Dimension of the Texture.
    TextureDimension Dimension = {};

    /// \brief Defines the number of size in each dimension X, Y, Z.
    System::Vector3UI Size = {};

    /// \brief Specifies what can texture be bound into.
    TextureBindingFlags TextureBinding = {};

    /// \brief The format of the Texture.
    TextureFormat Format = {};

    /// \brief The number of mip-levels.
    Uint32 MipLevels = {};

    /// \brief Number of sample.
    Uint32 Sample = {};

    /// \brief Size of the texture array. leaves 1 for non-array texture.
    Uint32 ArraySize = {};

    /// \brief Specifies the usage of the buffer.
    ResourceUsage Usage = {};

    /// \brief Specifies which device queue family index (at specifies bit position) can use this resource.
    ///
    /// \note Only specifies the device queue family which will use this resource.
    ///       Do not set any device queue family index which won't use this resource unnecessary, this will cause extra overhead.
    ///
    /// \see Axis::Math::AssignBitToPosition
    Uint64 DeviceQueueFamilyMask = {};
};

/// \brief Represents multidimensional (up to 3) arrays of data which can be used for various purposes.
class AXIS_GRAPHICS_API ITexture : public StatedGraphicsResource
{
public:
    /// \brief The description of ITexture resource.
    const TextureDescription Description;

    /// \brief Gets the description of default texture view for this texture.
    ///
    /// \note Default texture view contains all subresource ranges of this texture.
    TextureViewDescription GetDefaultTextureViewDescription() noexcept;

    /// \brief Creates default texture view out of this texture.
    ///        Internally the texture uses the graphics device which created
    ///        itself to create the texture view.
    ///
    /// \note Default texture view contains all subresource ranges of this texture.
    System::SharedPointer<ITextureView> CreateDefaultTextureView();

protected:
    /// \brief Constructor
    ITexture(const TextureDescription& textureDescription);
};

/// \brief Dimension interpretation to the original Texture.
enum class TextureViewDimension : Uint8
{
    /// \brief View to 1-Dimension Texture.
    Texture1D,

    /// \brief View to 1-Dimension Texture Array.
    Texture1DArray,

    /// \brief View to 2-Dimension Texture.
    Texture2D,

    /// \brief View to 2-Dimension Texture Array.
    Texture2DArray,

    /// \brief View to 3-Dimension Texture.
    Texture3D,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Texture3D,
};

/// \brief Allowed usage of the TextureView.
enum class TextureViewUsage : Uint8
{
    /// \brief Can be used as color or resolve attachment.
    RenderTarget = AXIS_BIT(1),

    /// \brief Can be used as Depth resolve attachment.
    Depth = AXIS_BIT(2),

    /// \brief Can be used as Stencil resolve attachment.
    Stencil = AXIS_BIT(3),

    /// \brief Required for enum reflection.
    MaximumEnumValue = Stencil,
};

/// \brief Allowed usage of the TextureView. (Bit mask)
typedef TextureViewUsage TextureViewUsageFlags;

/// \brief Description of ITextureView resource.
struct TextureViewDescription final
{
    /// \brief The Texture which is viewing to.
    System::SharedPointer<ITexture> ViewTexture = {};

    /// \brief Describes the View to the Texture.
    TextureViewDimension ViewDimension = {};

    /// \brief Allowed usage of the TextureView.
    TextureViewUsageFlags ViewUsage = {};

    /// \brief Format interpretation to the original Texture.
    TextureFormat ViewFormat = {};

    /// \brief The start of MipLevel to view.
    Uint32 BaseMipLevel = {};

    /// \brief The count of MipLevel to view starting from BaseMipLevel.
    Uint32 MipLevelCount = {};

    /// \brief The start of ArrayIndex of view.
    Uint32 BaseArrayIndex = {};

    /// \brief The count of ArrayLevel to view starting form BaseArrayIndex.
    Uint32 ArrayLevelCount = {};
};

/// \brief Represents the interpretation to the Texture resource.
class AXIS_GRAPHICS_API ITextureView : public DeviceChild
{
public:
    /// \brief The description of ITexture resource.
    const TextureViewDescription Description;

protected:
    /// Constructor
    ITextureView(const TextureViewDescription& textureDescription);
};

/// \brief Gets \a `TextureViewUsageFlags` from \a `TextureFormat`.
constexpr TextureViewUsageFlags GetTextureViewUsageFlagsFromTextureFormat(TextureFormat textureFormat)
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

#endif // AXIS_GRAPHICS_TEXTURE_HPP