/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANFENCE_HPP
#define AXIS_VULKANFENCE_HPP
#pragma once

#include "../../../Include/Axis/Fence.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// An implementation of IFence interface in Vulkan backend.
class VulkanFence final : public IFence
{
public:
    // Constructor
    VulkanFence(Uint64                initialValue,
                VulkanGraphicsDevice& vulkanGraphicsDevice);

    // An implementation of IFence::GetCurrentValue in Vulkan backend.
    Uint64 GetCurrentValue() const override final;

    // An implementation of IFence::SetValue in Vulkan backend.
    void SetValue(Uint64 value) override final;

    // An implementation of IFence::WaitForValue in Vulkan backend.
    void WaitForValue(Uint64 value) const override final;

    // Gets the internal timeline VkSeamphore handle.
    inline VkSemaphore GetVkSemaphoreHandle() const noexcept { return _timelineSemaphore; }

private:
    VkPtr<VkSemaphore> _timelineSemaphore = {}; // Timeline semaphore handle
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANFENCE_HPP