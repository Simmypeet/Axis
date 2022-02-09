/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanSampler.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

namespace Graphics
{

VulkanSampler::VulkanSampler(const SamplerDescription& description,
                             VulkanGraphicsDevice&     vulkanGraphicsDevice) :
    ISampler(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkSampler = [&]() -> VkSampler {
        VkSampler vkSampler = {};

        VkSamplerCustomBorderColorCreateInfoEXT samplerCustomColorBorder = {};
        samplerCustomColorBorder.sType                                   = VK_STRUCTURE_TYPE_SAMPLER_CUSTOM_BORDER_COLOR_CREATE_INFO_EXT;
        samplerCustomColorBorder.pNext                                   = nullptr;
        samplerCustomColorBorder.format                                  = VK_FORMAT_UNDEFINED;
        samplerCustomColorBorder.customBorderColor.float32[0]            = description.BorderColor.R;
        samplerCustomColorBorder.customBorderColor.float32[1]            = description.BorderColor.G;
        samplerCustomColorBorder.customBorderColor.float32[2]            = description.BorderColor.B;
        samplerCustomColorBorder.customBorderColor.float32[3]            = description.BorderColor.A;

        VkSamplerCreateInfo samplerCreateInfo     = {};
        samplerCreateInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.pNext                   = &samplerCustomColorBorder;
        samplerCreateInfo.minFilter               = VulkanUtility::GetVkFilterFromSamplerFilter(description.MinFilter);
        samplerCreateInfo.magFilter               = VulkanUtility::GetVkFilterFromSamplerFilter(description.MagFilter);
        samplerCreateInfo.mipmapMode              = VulkanUtility::GetVkSamplerMipmapModeFromSamplerFilter(description.MipFilter);
        samplerCreateInfo.addressModeU            = VulkanUtility::GetVkSamplerAddressModeFromSamplerAddressMode(description.AddressModeU);
        samplerCreateInfo.addressModeV            = VulkanUtility::GetVkSamplerAddressModeFromSamplerAddressMode(description.AddressModeV);
        samplerCreateInfo.addressModeW            = VulkanUtility::GetVkSamplerAddressModeFromSamplerAddressMode(description.AddressModeW);
        samplerCreateInfo.anisotropyEnable        = description.AnisotropyEnable ? VK_TRUE : VK_FALSE;
        samplerCreateInfo.maxAnisotropy           = (float)description.MaxAnisotropyLevel;
        samplerCreateInfo.mipLodBias              = description.MipLODBias;
        samplerCreateInfo.minLod                  = description.MinLOD;
        samplerCreateInfo.maxLod                  = description.MaxLOD == SamplerDescription::NoLODClamp ? VK_LOD_CLAMP_NONE : description.MaxLOD;
        samplerCreateInfo.compareEnable           = VK_FALSE;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.borderColor             = VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;

        auto vkResult = vkCreateSampler(vulkanGraphicsDevice.GetVkDeviceHandle(),
                                        &samplerCreateInfo,
                                        nullptr,
                                        &vkSampler);

        if (vkResult != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkSampler!");
        return vkSampler;
    };

    auto DestroyVkSampler = [this](VkSampler vkSampler) {
        vkDestroySampler(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkSampler, nullptr);
    };

    _vulkanSampler = VkPtr<VkSampler>(CreateVkSampler, std::move(DestroyVkSampler));
}

} // namespace Graphics

} // namespace Axis