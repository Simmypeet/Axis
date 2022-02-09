/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANTEXTURE_HPP
#define AXIS_VULKANTEXTURE_HPP
#pragma once

#include "../../../../System/Include/Axis/Event.hpp"
#include "../../../Include/Axis/Texture.hpp"
#include "VkPtr.hpp"
#include <VulkanMemoryAllocator/vk_mem_alloc.hpp>

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// An implementation of ITexture interface in Vulkan backend.
class VulkanTexture final : public ITexture
{
private:
    struct VulkanImageAllocation
    {
        VkImage       VulkanImage            = VK_NULL_HANDLE;
        VmaAllocation VulkanMemoryAllocation = VK_NULL_HANDLE;

        inline constexpr Bool operator==(const VulkanImageAllocation& other) const noexcept { return VulkanImage == other.VulkanImage && VulkanMemoryAllocation == other.VulkanMemoryAllocation; }
        inline constexpr Bool operator!=(const VulkanImageAllocation& other) const noexcept { return VulkanImage != other.VulkanImage || VulkanMemoryAllocation != other.VulkanMemoryAllocation; }
    };

public:
    // Forward declarations
    struct Internal;

    // Constructs a new VulkanTexture with no memory backed to it. The VkImage won't be destroy in the destructor.
    VulkanTexture(const TextureDescription& description,
                  VkImage                   vulkanImage,
                  VulkanGraphicsDevice&     vulkanGraphicsDevice);

    // Construct VulkanTexture with memory backed to it.
    VulkanTexture(const TextureDescription& description,
                  VulkanGraphicsDevice&     vulkanGraphicsDevice);

    inline VkImage GetVkImageHandle() const noexcept { return ((VulkanImageAllocation)_vulkanImage).VulkanImage; }


private:
    VmaAllocationInfo            _vmaAllocationInfo = {};
    VkPtr<VulkanImageAllocation> _vulkanImage       = {};
};

// An implementation of ITextureView interface in Vulkan backend.
class VulkanTextureView final : public ITextureView
{
public:
    // Constructor
    VulkanTextureView(const TextureViewDescription& description,
                      VulkanGraphicsDevice&         vulkanGraphicsDevice);

    // Gets the internal VulkanTextureView object.
    inline const VkImageView GetVkImageViewHandle() const noexcept { return _vulkanImageView; }

private:
    VkPtr<VkImageView> _vulkanImageView = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANTEXTURE_HPP