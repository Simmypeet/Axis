/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/DeviceContext.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/Texture.hpp>

namespace Axis
{

namespace Graphics
{

ITexture::ITexture(const TextureDescription& description) :
    Description(description) {}

TextureViewDescription ITexture::GetDefaultTextureViewDescription() noexcept
{
    TextureViewDescription textureViewDescription = {};
    textureViewDescription.ViewTexture            = ISharedFromThis::CreateSharedPointerFromThis(*this);

    if (Description.ArraySize == 1)
    {
        switch (Description.Dimension)
        {
            case TextureDimension::Texture1D:
                textureViewDescription.ViewDimension = TextureViewDimension::Texture1D;
                break;

            case TextureDimension::Texture2D:
                textureViewDescription.ViewDimension = TextureViewDimension::Texture2D;
                break;

            case TextureDimension::Texture3D:
                textureViewDescription.ViewDimension = TextureViewDimension::Texture3D;
                break;
        }
    }
    else
    {
        switch (Description.Dimension)
        {
            case TextureDimension::Texture1D:
                textureViewDescription.ViewDimension = TextureViewDimension::Texture1DArray;
                break;

            case TextureDimension::Texture2D:
                textureViewDescription.ViewDimension = TextureViewDimension::Texture2DArray;
                break;

            // TODO: Default view for the 3D texture array.
            case TextureDimension::Texture3D:
                break;
        }
    }

    textureViewDescription.ViewUsage       = GetTextureViewUsageFlagsFromTextureFormat(Description.Format);
    textureViewDescription.ViewFormat      = Description.Format;
    textureViewDescription.BaseMipLevel    = 0;
    textureViewDescription.MipLevelCount   = Description.MipLevels;
    textureViewDescription.BaseArrayIndex  = 0;
    textureViewDescription.ArrayLevelCount = Description.ArraySize;

    return textureViewDescription;
}

System::SharedPointer<ITextureView> ITexture::CreateDefaultTextureView()
{
    return GetCreatorDevice()->CreateTextureView(GetDefaultTextureViewDescription());
}

ITextureView::ITextureView(const TextureViewDescription& description) :
    Description(description) {}

} // namespace Graphics

} // namespace Axis