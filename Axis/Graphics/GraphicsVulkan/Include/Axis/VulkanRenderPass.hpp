/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANRENDERPASS_HPP
#define AXIS_VULKANRENDERPASS_HPP
#pragma once

#include "../../../Include/Axis/RenderPass.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// \brief An implementation of IRenderPass interface in Vulkan backend.
class VulkanRenderPass final : public IRenderPass
{
public:
    // Constructor
    VulkanRenderPass(const RenderPassDescription& description,
                     VulkanGraphicsDevice&        vulkanGraphicsDevice);

    // Gets the internal VkRenderPass handle.
    inline VkRenderPass GetVkRenderPassHandle() const noexcept { return _vulkanRenderPass; }

private:
    VkPtr<VkRenderPass> _vulkanRenderPass = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANRENDERPASS_HPP