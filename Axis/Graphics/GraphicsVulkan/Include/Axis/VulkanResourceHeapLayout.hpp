/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANRESOURCEHEAPLAYOUT_HPP
#define AXIS_VULKANRESOURCEHEAPLAYOUT_HPP
#pragma once

#include "../../../Include/Axis/ResourceHeapLayout.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// An implementation of IRenderPass interface in Vulkan backend.
class VulkanResourceHeapLayout final : public IResourceHeapLayout
{
public:
    // Constructor
    VulkanResourceHeapLayout(const ResourceHeapLayoutDescription& description,
                             VulkanGraphicsDevice&                vulkanGraphicsDevice);

    // Gets the internal VkDescriptorSetLayout handle.
    inline VkDescriptorSetLayout GetVkDescriptorSetLayoutHandle() const noexcept { return _vulkanDescriptorSetLayout; }

private:
    VkPtr<VkDescriptorSetLayout> _vulkanDescriptorSetLayout = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANRESOURCEHEAPLAYOUT_HPP