/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file TextureLoader.hpp
///
/// \brief Contains `Axis::TextureLoader` class.

#ifndef AXIS_GRAPHICS_TEXTURELOADER_HPP
#define AXIS_GRAPHICS_TEXTURELOADER_HPP
#pragma once

#include "../../../System/Include/Axis/FileStream.hpp"
#include "Texture.hpp"


/// Forward declarations
typedef unsigned char stbi_uc;

namespace Axis
{

/// Forward declarations
class IGraphicsDevice;
class IDeviceContext;
class IBuffer;

/// \brief Informations used in image loading and texture creation.
struct TextureLoadConfiguration
{
    /// \brief Graphics device used in texture creations.
    SharedPointer<IGraphicsDevice> GraphicsDevice = {};

    /// \brief Immediate device context used in data transferation.
    SharedPointer<IDeviceContext> ImmediateDeviceContext = {};

    /// \brief Specifies whether to create the textures with sRGB format.
    Bool IsSRGB = {};

    /// \brief The level of mip map to use when create the textures.
    Bool GenerateMip = {};

    /// \brief Specifies the resource usage of the created textures.
    ResourceUsage Usage = {};

    /// \brief Specifies the binding of the created textures.
    TextureBinding Binding = {};

    /// \brief Specifies which device queue family index (at specifies bit position) can use this resource.
    ///
    /// \note Only specifies the device queue family which will use this resource.
    ///       Do not set any device queue family index which won't use this resource unnecessary, this will cause extra overhead.
    ///
    /// \see Axis::Math::AssignBitToPosition
    Uint64 DeviceQueueFamilyMask = {};
};

/// \brief Helper class for loading the image files for the disks
///        and creates the textures out of the image.
class AXIS_GRAPHICS_API TextureLoader
{
public:
    /// \brief Constructs the texture loader object.
    ///
    /// \param[in] fileStream The file stream to use for loading the image files.
    ///                       The file stream must be opened in binary mode and should be able to read.
    /// \param[in] loadDescription Specifies the image loading configurations.
    TextureLoader(FileStream&&                    fileStream,
                  const TextureLoadConfiguration& loadDescription);

    /// \brief Destructor
    ~TextureLoader() noexcept;

    /// \brief Creates the texture out of loaded image.
    SharedPointer<ITexture> CreateTexture();

    /// \brief Gets \a `Axis::TextureDescription` that used in texture
    ///        creation.
    TextureDescription GetTextureDescription() const;

private:
    /// Private members
    TextureLoadConfiguration _loadConfiguration = {};
    SharedPointer<IBuffer>   _stagingBuffer     = {}; // Staging buffer for immutable textures.
    FileStream               _fileStream;
    stbi_uc*                 _pixels      = nullptr;
    Int32                    _texWidth    = 0;
    Int32                    _texHeight   = 0;
    Int32                    _texChannels = 0;
};

} // namespace Axis

#endif // AXIS_GRAPHICS_TEXTURELOADER_HPP