/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANCOMMANDPOOL_HPP
#define AXIS_VULKANCOMMANDPOOL_HPP
#pragma once

#include "../../../../System/Include/Axis/List.hpp"
#include "../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../Include/Axis/DeviceChild.hpp"
#include "VkPtr.hpp"
#include <mutex>

namespace Axis
{

namespace Graphics
{

// Forward declarations
struct VulkanDeviceQueueFamily;
struct VulkanCommandBuffer;
class VulkanGraphicsDevice;

// Encapsulation of VkCommandPool.
struct VulkanCommandPool final : public DeviceChild
{
public:
    // Constructor
    VulkanCommandPool(VulkanDeviceQueueFamily& deviceQueueFamily,
                      VulkanGraphicsDevice&    vulkanGraphicsDevice,
                      VkCommandPoolCreateFlags commandPoolCreateFlag);

    // Default constructor
    VulkanCommandPool() noexcept = default;

    // Gets the command buffer in the pool. also reset the VulkanCommandBuffer.
    System::UniquePointer<VulkanCommandBuffer> GetCommandBuffer();

    // Returns back VulkanCommandBuffer to the pool.
    void ReturnCommandBuffer(System::UniquePointer<VulkanCommandBuffer>&& commandBuffer);

    // Gets internal VkCommandPool handle.
    inline VkCommandPool GetVkCommandPool() const noexcept { return _commandPool; }

private:
    /// Private members
    VkPtr<VkCommandPool>                                     _commandPool           = {};
    System::List<System::UniquePointer<VulkanCommandBuffer>> _returnedCommandBuffer = {};
    std::mutex                                               _mutex                 = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANCOMMANDPOOL_HPP