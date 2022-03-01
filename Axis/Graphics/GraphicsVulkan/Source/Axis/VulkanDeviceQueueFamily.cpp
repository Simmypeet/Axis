/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanCommandPool.hpp>
#include <Axis/VulkanDeviceQueueFamily.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>

namespace Axis
{

namespace Graphics
{

VulkanDeviceQueue::VulkanDeviceQueue(VulkanDeviceQueueFamily& deviceQueueFamily,
                                     VulkanGraphicsDevice&    vulkanGraphicsDevice,
                                     Uint32                   deviceQueueIndex) :
    _deviceQueueIndex(deviceQueueIndex)
{
    vkGetDeviceQueue(vulkanGraphicsDevice.GetVkDeviceHandle(),
                     deviceQueueFamily.GetDeviceQueueFamilyIndex(),
                     deviceQueueIndex,
                     &_deviceQueue);

    if (_deviceQueue == VK_NULL_HANDLE)
        throw System::ExternalException("Failed to get VkQueue!");
}

void VulkanDeviceQueue::WaitQueueIdle() const noexcept
{
    vkQueueWaitIdle(_deviceQueue);
}

void VulkanDeviceQueue::QueueSubmit(VulkanCommandBuffer& commandBuffer,
                                    VkDevice             device)
{
    VkFence submitFence = commandBuffer._submitFence;

    AppendSignalSeamphore(commandBuffer.GetSignalVkSemaphore(), 0);

    // Un-signals the commandBuffer's fence (marks as in used).
    vkResetFences(device, 1, &submitFence);

    VkCommandBuffer vkCommandBuffer = commandBuffer.GetVkCommandBufferHandle();

    VkSubmitInfo submitInfo       = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &vkCommandBuffer;

    submitInfo.waitSemaphoreCount = (Uint32)_waitSemaphores.GetSize();
    submitInfo.pWaitSemaphores    = _waitSemaphores.GetSize() == 0 ? nullptr : _waitSemaphores.GetData();
    submitInfo.pWaitDstStageMask  = _waitStages.GetSize() == 0 ? nullptr : _waitStages.GetData();

    submitInfo.signalSemaphoreCount = (Uint32)_signalSemaphores.GetSize();
    submitInfo.pSignalSemaphores    = _signalSemaphores.GetSize() == 0 ? nullptr : _signalSemaphores.GetData();

    // Fills the timeline semaphore struct.
    VkTimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo = {};
    timelineSemaphoreSubmitInfo.sType                         = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
    timelineSemaphoreSubmitInfo.pNext                         = nullptr;

    timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = (Uint32)_signalValues.GetSize();
    timelineSemaphoreSubmitInfo.pSignalSemaphoreValues    = _signalValues.GetSize() == 0 ? nullptr : _signalValues.GetData();

    timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = (Uint32)_waitValues.GetSize();
    timelineSemaphoreSubmitInfo.pWaitSemaphoreValues    = _waitValues.GetSize() == 0 ? nullptr : _waitValues.GetData();

    submitInfo.pNext = &timelineSemaphoreSubmitInfo; // adds timeline semaphore submit info structure.

    auto result = vkQueueSubmit(_deviceQueue,
                                1,
                                &submitInfo,
                                commandBuffer._submitFence) /* Passes commandBuffer's fence to check if the commandBuffer is done using and ready */;

    // Clears all synchronization primitives.
    _waitSemaphores.Clear();
    _waitStages.Clear();
    _waitValues.Clear();

    _signalSemaphores.Clear();
    _signalValues.Clear();

    if (result != VK_SUCCESS)
        throw System::ExternalException("Failed to submit VkQueue!");
}

void VulkanDeviceQueue::AppendWaitSemaphore(VkSemaphore          semaphore,
                                            VkPipelineStageFlags waitStages,
                                            Uint64               waitValue)
{
    Size count = _waitSemaphores.GetSize();

    try
    {
        _waitSemaphores.Append(semaphore);
        _waitStages.Append(waitStages);
        _waitValues.Append(waitValue);
    }
    catch (...)
    {
        if (_waitSemaphores.GetSize() > count)
            _waitSemaphores.PopBack();

        if (_waitStages.GetSize() > count)
            _waitStages.PopBack();

        if (_waitValues.GetSize() > count)
            _waitValues.PopBack();

        throw;
    }
}

void VulkanDeviceQueue::AppendSignalSeamphore(VkSemaphore semaphore,
                                              Uint64      signalValue)
{

    Size count = _signalSemaphores.GetSize();

    try
    {
        _signalSemaphores.Append(semaphore);
        _signalValues.Append(signalValue);
    }
    catch (...)
    {
        if (_signalSemaphores.GetSize() > count)
            _signalSemaphores.PopBack();

        if (_signalValues.GetSize() > count)
            _signalValues.PopBack();

        throw;
    }
}

VulkanDeviceQueueFamily::VulkanDeviceQueueFamily(Uint32                deviceQueueFamilyIndex,
                                                 Uint32                deviceQueueCount,
                                                 VulkanGraphicsDevice& vulkanGraphicsDevice) noexcept :
    _deviceQueueFamilyIndex(deviceQueueFamilyIndex)
{
    _deviceQueues.ReserveFor(deviceQueueCount);

    for (Uint32 i = 0; i < deviceQueueCount; i++)
    {
        _deviceQueues.EmplaceBack(*this,
                                  vulkanGraphicsDevice,
                                  i);
    }
}


VulkanDeviceQueue& VulkanDeviceQueueFamily::GetDeviceQueue(Uint32 deviceQueueIndex)
{
    AXIS_ASSERT(deviceQueueIndex < _deviceQueues.GetSize(), "deviceQueueIndex was out of range!");

    return _deviceQueues[deviceQueueIndex];
}

} // namespace Graphics

} // namespace Axis