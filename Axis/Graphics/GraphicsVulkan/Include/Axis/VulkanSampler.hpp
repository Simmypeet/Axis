/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANSAMPLER_HPP
#define AXIS_VULKANSAMPLER_HPP
#pragma once

#include "../../../Include/Axis/Sampler.hpp"
#include "VkPtr.hpp"

namespace Axis
{

/// Forward declarations
class VulkanGraphicsDevice;

/// \brief An implementation of ISampler interface in Vulkan backend.
class VulkanSampler final : public ISampler
{
public:
    /// Constructor
    VulkanSampler(const SamplerDescription& description,
                  VulkanGraphicsDevice&     vulkanGraphicsDevice);

    /// \brief Gets the internal VkSampler handle.
    inline VkSampler GetVkSamplerHandle() const noexcept { return _vulkanSampler; }

private:
    VkPtr<VkSampler> _vulkanSampler = {};
};

} // namespace Axis

#endif // AXIS_VULKANSAMPLER_HPP