/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanTexture.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

inline Tuple<List<Uint32>, VkImageCreateInfo> GetTextureCreationInfosFromTextureDescription(const TextureDescription& description)
{
    auto indices = VulkanUtility::ExtractDeviceQueueFamilyIndices(description.DeviceQueueFamilyMask);

    // Vulkan texture
    VkImageCreateInfo imageCreateInfo     = {};
    imageCreateInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext                 = 0;
    imageCreateInfo.imageType             = VulkanUtility::GetVkImageTypeFromTextureDimension(description.Dimension);
    imageCreateInfo.format                = VulkanUtility::GetVkFormatFromTextureFormat(description.Format);
    imageCreateInfo.extent                = {description.Size.X, description.Size.Y, description.Size.Z};
    imageCreateInfo.mipLevels             = description.MipLevels;
    imageCreateInfo.arrayLayers           = description.ArraySize;
    imageCreateInfo.samples               = (VkSampleCountFlagBits)description.Sample;
    imageCreateInfo.tiling                = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage                 = VulkanUtility::GetVkImageUsageFlagsFromTextureBindingFlags(description.TextureBinding);
    imageCreateInfo.sharingMode           = indices ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = (Uint32)indices.GetLength();
    imageCreateInfo.pQueueFamilyIndices   = indices.GetData();
    imageCreateInfo.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

    return {{std::move(indices)}, {{imageCreateInfo}}};
}

VulkanTexture::VulkanTexture(const TextureDescription& description,
                             VkImage                   vulkanImage,
                             VulkanGraphicsDevice&     vulkanGraphicsDevice) :
    ITexture(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkImage = [&]() -> VulkanImageAllocation {
        return {vulkanImage, VK_NULL_HANDLE};
    };

    auto DestroyVkImage = [](VulkanImageAllocation vkImage) {
        // Do nothing ...
    };

    _vulkanImage = VkPtr<VulkanImageAllocation>(CreateVkImage, std::move(DestroyVkImage));
}

VulkanTexture::VulkanTexture(const TextureDescription& description,
                             VulkanGraphicsDevice&     vulkanGraphicsDevice) :
    ITexture(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkImage = [&]() {
        VulkanImageAllocation vkImage;

        auto createInfos             = GetTextureCreationInfosFromTextureDescription(description);
        auto vmaAllocationCreateInfo = VulkanUtility::GetVmaAllocationCreateInfoFromResourceUsage(description.Usage);

        auto vkResult = vmaCreateImage(vulkanGraphicsDevice.GetVmaAllocatorHandle(),
                                       &GetTuple<1>(createInfos),
                                       &vmaAllocationCreateInfo,
                                       &vkImage.VulkanImage,
                                       &vkImage.VulkanMemoryAllocation,
                                       &_vmaAllocationInfo);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to create VkImage!");

        return vkImage;
    };

    auto DestroyVkImage = [this](VulkanImageAllocation vkImage) {
        vmaDestroyImage(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(), vkImage.VulkanImage, vkImage.VulkanMemoryAllocation);
    };

    _vulkanImage = VkPtr<VulkanImageAllocation>(CreateVkImage, std::move(DestroyVkImage));
}

VulkanTextureView::VulkanTextureView(const TextureViewDescription& description,
                                     VulkanGraphicsDevice&         vulkanGraphicsDevice) :
    ITextureView(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkImageView = [&]() -> VkImageView {
        auto vulkanImage = ((VulkanTexture*)description.ViewTexture)->GetVkImageHandle();

        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.flags                 = 0;
        imageViewCreateInfo.pNext                 = nullptr;

        imageViewCreateInfo.image    = vulkanImage;
        imageViewCreateInfo.format   = VulkanUtility::GetVkFormatFromTextureFormat(description.ViewFormat);
        imageViewCreateInfo.viewType = VulkanUtility::GetVkImageViewTypeFromTextureViewDimension(description.ViewDimension);

        // Assign swizzle value to default.
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageViewCreateInfo.subresourceRange.aspectMask = VulkanUtility::GetVkImageAspectFlagsFromTextureViewUsageFlags(description.ViewUsage);

        // Check if Texture dimension is Array type!
        if (description.ViewDimension == TextureViewDimension::Texture1DArray || description.ViewDimension == TextureViewDimension::Texture2DArray)
        {
            imageViewCreateInfo.subresourceRange.baseArrayLayer = description.BaseArrayIndex;
            imageViewCreateInfo.subresourceRange.layerCount     = description.ArrayLevelCount;
        }
        else
        {
            /// Assign default value to non Array dimension Texture.
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount     = 1;
        }

        imageViewCreateInfo.subresourceRange.baseMipLevel = description.BaseMipLevel;
        imageViewCreateInfo.subresourceRange.levelCount   = description.MipLevelCount;

        VkImageView vkImageView = {};

        auto vkResult = vkCreateImageView(vulkanGraphicsDevice.GetVkDeviceHandle(), &imageViewCreateInfo, nullptr, &vkImageView);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to create vkImageView!");
        return vkImageView;
    };

    auto DestroyVkImageView = [&](VkImageView imageView) {
        vkDestroyImageView(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), imageView, nullptr);
    };

    _vulkanImageView = VkPtr<VkImageView>(CreateVkImageView, std::move(DestroyVkImageView));
}

} // namespace Axis
