/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANDEVICEQUEUEFAMILY_HPP
#define AXIS_VULKANDEVICEQUEUEFAMILY_HPP
#pragma once

#include "../../../../System/Include/Axis/List.hpp"
#include "../../../Include/Axis/DeviceChild.hpp"
#include <vulkan/vulkan.h>

namespace Axis
{

namespace Graphics
{

// Forward declarations
struct VulkanDeviceQueueFamily;
struct VulkanCommandBuffer;
struct VulkanCommandPool;
class VulkanGraphicsDevice;

// Wrapper over VkDeviceQueue
struct VulkanDeviceQueue final
{
public:
    // Constructor
    VulkanDeviceQueue(VulkanDeviceQueueFamily& deviceQueueFamily,
                      VulkanGraphicsDevice&    vulkanGraphicsDevice,
                      Uint32                   deviceQueueIndex);

    // Default constructor
    VulkanDeviceQueue() noexcept = default;

    // Blocks the CPU thread until this queue is idle.
    void WaitQueueIdle() const noexcept;

    // Submits command buffer to this queue.
    void QueueSubmit(VulkanCommandBuffer& commandBuffer,
                     VkDevice             device);

    // Adds semaphore to wait upon next queue submission.
    void AppendWaitSemaphore(VkSemaphore          semaphore,
                             VkPipelineStageFlags waitStages,
                             Uint64               waitValue);

    // Adds the semaphore to signal upon next queue submission.
    void AppendSignalSeamphore(VkSemaphore semaphore,
                               Uint64      signalValue);

    // Gets the index of this device queue from DeviceQueueFamily.
    inline Uint32 GetDeviceQueueIndex() const noexcept { return _deviceQueueIndex; }

    // Gets internal VkDeviceQueue handle.
    inline VkQueue GetVkQueueHandle() const noexcept { return _deviceQueue; }

private:
    Uint32                             _deviceQueueIndex = 0;
    VkQueue                            _deviceQueue      = {};
    System::List<VkSemaphore>          _waitSemaphores   = {};
    System::List<Uint64>               _waitValues       = {};
    System::List<VkPipelineStageFlags> _waitStages       = {};
    System::List<VkSemaphore>          _signalSemaphores = {};
    System::List<Uint64>               _signalValues     = {};
};

// Wrapper over vulkan device queue
struct VulkanDeviceQueueFamily final
{
    // Constructor
    VulkanDeviceQueueFamily(Uint32                deviceQueueFamilyIndex,
                            Uint32                deviceQueueCount,
                            VulkanGraphicsDevice& vulkanGraphicsDevice) noexcept;

    // Default constructor
    VulkanDeviceQueueFamily() noexcept = default;

    // Queue family index.
    inline Uint32 GetDeviceQueueFamilyIndex() const noexcept { return _deviceQueueFamilyIndex; }

    // Gets the device queues contained in this family.
    VulkanDeviceQueue& GetDeviceQueue(Uint32 deviceQueueIndex);

    // Gets the number of device queues in this device queue family.
    inline Size GetDeviceQueueCount() const noexcept { return _deviceQueues.GetSize(); }

private:
    Uint32                          _deviceQueueFamilyIndex = 0;
    System::List<VulkanDeviceQueue> _deviceQueues           = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANDEVICEQUEUEFAMILY_HPP
