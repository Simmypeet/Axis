/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANFRAMEBUFFER_HPP
#define AXIS_VULKANFRAMEBUFFER_HPP
#pragma once

#include "../../../Include/Axis/Framebuffer.hpp"
#include "VkPtr.hpp"

namespace Axis
{

/// Forward declarations
class VulkanGraphicsDevice;

/// \brief An implementation of IFramebuffer interface in Vulkan backend.
class VulkanFramebuffer final : public IFramebuffer
{
public:
    /// Constructor
    VulkanFramebuffer(FramebufferDescription description,
                      VulkanGraphicsDevice&  vulkanGraphicsDevice);

    /// \brief Gets the internal timeline VkSeamphore handle.
    inline VkFramebuffer GetVkFramebufferHandle() const noexcept { return _vulkanFramebuffer; }

private:
    /// Private members
    VkPtr<VkFramebuffer> _vulkanFramebuffer = {}; // VkFramebuffer handle.
};

} // namespace Axis

#endif // AXIS_VULKANFRAMEBUFFER_HPP