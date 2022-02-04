/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

#include <Axis/GraphicsPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/Exception.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/TextureLoader.hpp>
#include <stb_image/stb_image.hpp>

namespace Axis
{

static void ValidateTextureLoadConfiguration(const TextureLoadConfiguration& loadConfiguration)
{
    if (!loadConfiguration.GraphicsDevice)
        throw InvalidArgumentException("loadConfiguration.GraphicsDevice was nullptr!");

    if (!loadConfiguration.ImmediateDeviceContext)
        throw InvalidArgumentException("loadConfiguration.ImmediateDeviceContext was nullptr!");

    if (loadConfiguration.GenerateMip && !(Bool)(loadConfiguration.ImmediateDeviceContext->SupportedQueueOperations & QueueOperation::Graphics))
        throw InvalidArgumentException("if loadConfiguration.GenerateMip was used, loadConfiguration.ImmediateDeviceContext must support graphics operations!");
}

TextureLoader::TextureLoader(FileStream&&                    fileStream,
                             const TextureLoadConfiguration& loadConfiguration) :
    _loadConfiguration(loadConfiguration),
    _fileStream(std::move(fileStream))
{
    if (!_fileStream.IsOpen())
        throw InvalidArgumentException("fileStream was not opened!");

    if (!_fileStream.CanRead())
        throw InvalidArgumentException("fileStream was not readable!");

    if (!(Bool)(_fileStream.GetFileModes() & FileMode::Binary))
        throw InvalidArgumentException("fileStream was not in binary mode!");

    _pixels = stbi_load_from_file(_fileStream.GetFileHandle(), &_texWidth, &_texHeight, &_texChannels, STBI_rgb_alpha);

    ValidateTextureLoadConfiguration(loadConfiguration);

    if (!_pixels)
        throw ExternalException("Failed to load image!");
}

TextureLoader::~TextureLoader() noexcept
{
    if (_pixels)
        stbi_image_free(_pixels);
}

SharedPointer<ITexture> TextureLoader::CreateTexture()
{
    auto textureDescription = GetTextureDescription();
    auto texture            = _loadConfiguration.GraphicsDevice->CreateTexture(textureDescription);

    // Creates new staging buffer
    if (!_stagingBuffer)
    {
        BufferDescription bufferDescription     = {};
        bufferDescription.BufferSize            = _texHeight * _texWidth * 4;
        bufferDescription.BufferBinding         = BufferBinding::TransferSource;
        bufferDescription.Usage                 = ResourceUsage::StagingSource;
        bufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(bufferDescription.DeviceQueueFamilyMask, _loadConfiguration.ImmediateDeviceContext->DeviceQueueFamilyIndex, true);

        _stagingBuffer = _loadConfiguration.GraphicsDevice->CreateBuffer(bufferDescription, nullptr);
    }

    PVoid mappedMemory = _loadConfiguration.ImmediateDeviceContext->MapBuffer(_stagingBuffer, MapAccess::Write, MapType::Discard);

    std::memcpy(mappedMemory, _pixels, _texHeight * _texWidth * 4);

    _loadConfiguration.ImmediateDeviceContext->UnmapBuffer(_stagingBuffer);

    _loadConfiguration.ImmediateDeviceContext->CopyBufferToTexture(_stagingBuffer,
                                                                   0,
                                                                   texture,
                                                                   0,
                                                                   1,
                                                                   0,
                                                                   {(Uint32)0, (Uint32)0, (Uint32)0},
                                                                   {(Uint32)_texWidth, (Uint32)_texHeight, (Uint32)1});


    if (_loadConfiguration.GenerateMip)
    {
        auto defaultTextureView = texture->CreateDefaultTextureView();

        _loadConfiguration.ImmediateDeviceContext->GenerateMips(defaultTextureView);
    }

    _loadConfiguration.ImmediateDeviceContext->Flush();

    return texture;
}

TextureDescription TextureLoader::GetTextureDescription() const
{
    TextureDescription description = {};
    description.Dimension          = TextureDimension::Texture2D;
    description.Format             = _loadConfiguration.IsSRGB ? TextureFormat::UnormR8G8B8A8sRGB : TextureFormat::UnormR8G8B8A8;
    description.Size               = {(Uint32)_texWidth, (Uint32)_texHeight, (Uint32)1};
    description.TextureBinding     = _loadConfiguration.Binding | TextureBinding::TransferDestination;

    if (_loadConfiguration.GenerateMip)
        description.TextureBinding |= TextureBinding::TransferSource;

    description.MipLevels             = _loadConfiguration.GenerateMip ? (Uint32)(std::floor(std::log2(std::max(_texWidth, _texHeight)))) + (Uint32)1 : 1;
    description.Sample                = 1;
    description.ArraySize             = 1;
    description.Usage                 = _loadConfiguration.Usage;
    description.DeviceQueueFamilyMask = _loadConfiguration.DeviceQueueFamilyMask;

    description.DeviceQueueFamilyMask = Math::AssignBitToPosition(description.DeviceQueueFamilyMask, _loadConfiguration.ImmediateDeviceContext->DeviceQueueFamilyIndex, true);

    return description;
}

} // namespace Axis
