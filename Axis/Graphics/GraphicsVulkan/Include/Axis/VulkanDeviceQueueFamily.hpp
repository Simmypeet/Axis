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

/// Forward declarations
struct VulkanDeviceQueueFamily;
struct VulkanCommandBuffer;
struct VulkanCommandPool;
class VulkanGraphicsDevice;

/// \brief Wrapper over VkDeviceQueue
struct VulkanDeviceQueue final
{
public:
    /// \brief Constructor
    VulkanDeviceQueue(VulkanDeviceQueueFamily& deviceQueueFamily,
                      VulkanGraphicsDevice&    vulkanGraphicsDevice,
                      Uint32                   deviceQueueIndex);

    /// \brief Default constructor
    VulkanDeviceQueue() noexcept = default;

    /// \brief Blocks the CPU thread until this queue is idle.
    void WaitQueueIdle() const noexcept;

    /// \brief Submits command buffer to this queue.
    void QueueSubmit(VulkanCommandBuffer& commandBuffer,
                     VkDevice             device);

    /// \brief Adds semaphore to wait upon next queue submission.
    ///
    /// \param[in] semaphore Semaphore to be wait.
    /// \param[in] waitStages Pipeline stages which waiting will occur.
    /// \param[in] waitValue Value to wait for (if semaphore is binary
    ///            semaphore the value will be ignored).
    void AppendWaitSemaphore(VkSemaphore          semaphore,
                             VkPipelineStageFlags waitStages,
                             Uint64               waitValue);

    /// \brief Adds the semaphore to signal upon next queue submission.
    ///
    /// \param[in] semaphore Semaphore to signal.
    /// \param[in] signalValue Value to signal
    ///            (if semaphore is binary semaphore the value will be ignored).
    void AppendSignalSeamphore(VkSemaphore semaphore,
                               Uint64      signalValue);

    /// \brief Gets the index of this device queue from DeviceQueueFamily.
    inline Uint32 GetDeviceQueueIndex() const noexcept { return _deviceQueueIndex; }

    /// \brief Gets internal VkDeviceQueue handle.
    inline VkQueue GetVkQueueHandle() const noexcept { return _deviceQueue; }

private:
    Uint32                     _deviceQueueIndex = 0;
    VkQueue                    _deviceQueue      = {};
    List<VkSemaphore>          _waitSemaphores   = {};
    List<Uint64>               _waitValues       = {};
    List<VkPipelineStageFlags> _waitStages       = {};
    List<VkSemaphore>          _signalSemaphores = {};
    List<Uint64>               _signalValues     = {};
};

/// \brief Wrapper over vulkan device queue
struct VulkanDeviceQueueFamily final
{
    /// Constructor
    VulkanDeviceQueueFamily(Uint32                deviceQueueFamilyIndex,
                            Uint32                deviceQueueCount,
                            VulkanGraphicsDevice& vulkanGraphicsDevice) noexcept;

    /// Default constructor
    VulkanDeviceQueueFamily() noexcept = default;

    /// \brief Queue family index.
    inline Uint32 GetDeviceQueueFamilyIndex() const noexcept { return _deviceQueueFamilyIndex; }

    /// \brief Gets the device queues contained in this family.
    VulkanDeviceQueue& GetDeviceQueue(Uint32 deviceQueueIndex);

    // Gets the number of device queues in this device queue family.
    inline Size GetDeviceQueueCount() const noexcept { return _deviceQueues.GetLength(); }

private:
    Uint32                  _deviceQueueFamilyIndex = 0;
    List<VulkanDeviceQueue> _deviceQueues           = {};
};

} // namespace Axis

#endif // AXIS_VULKANDEVICEQUEUEFAMILY_HPP
