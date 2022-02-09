/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/Utility.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanCommandPool.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <vulkan/vulkan_core.h>

namespace Axis
{

namespace Graphics
{

VulkanCommandPool::VulkanCommandPool(VulkanDeviceQueueFamily& deviceQueueFamily,
                                     VulkanGraphicsDevice&    vulkanGraphicsDevice,
                                     VkCommandPoolCreateFlags commandPoolCreateFlag)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkCommandPool = [&]() -> VkCommandPool {
        VkCommandPool vkCommandPool = VK_NULL_HANDLE;

        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags                   = commandPoolCreateFlag;
        commandPoolCreateInfo.pNext                   = nullptr;
        commandPoolCreateInfo.queueFamilyIndex        = deviceQueueFamily.GetDeviceQueueFamilyIndex();

        if (vkCreateCommandPool(vulkanGraphicsDevice.GetVkDeviceHandle(), &commandPoolCreateInfo, nullptr, &vkCommandPool) != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkCommandPool!");
        else
            return vkCommandPool;
    };

    auto DestroyVkCommandPool = [this](VkCommandPool vkCommandPool) {
        vkDestroyCommandPool(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkCommandPool, nullptr);
    };

    _commandPool = VkPtr<VkCommandPool>(CreateVkCommandPool, std::move(DestroyVkCommandPool));
}

System::UniquePointer<VulkanCommandBuffer> VulkanCommandPool::GetCommandBuffer()
{
    if (_commandPool == VK_NULL_HANDLE)
        return {};

    // Uses the recycled command buffer.
    if (_returnedCommandBuffer.GetLength())
    {
        System::UniquePointer<VulkanCommandBuffer> commandBufferToReturn = nullptr;

        {
            std::scoped_lock<std::mutex> lockGuard(_mutex);

            for (Size i = 0; i < _returnedCommandBuffer.GetLength(); i++)
            {
                if (_returnedCommandBuffer[i]->IsCommandBufferAvailable())
                {
                    commandBufferToReturn = std::move(_returnedCommandBuffer[i]);

                    _returnedCommandBuffer.RemoveAt(i);

                    commandBufferToReturn->ResetCommandBuffer();

                    break;
                }
            }
        }

        if (commandBufferToReturn)
            return commandBufferToReturn;
    }

    // If there aren't any command buffer available. Creates a new one.
    return System::UniquePointer<VulkanCommandBuffer>(Axis::System::New<VulkanCommandBuffer>(_commandPool,
                                                                                             VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                                             (VulkanGraphicsDevice&)(*GetCreatorDevice())));
}

void VulkanCommandPool::ReturnCommandBuffer(System::UniquePointer<VulkanCommandBuffer>&& commandBuffer)
{
    // Checks if the CommandBuffer is recording?.
    AXIS_ASSERT(!commandBuffer->IsRecording(), "Can't return this CommandBuffer because it is recording!");

    std::scoped_lock<std::mutex> lockGuard(_mutex);

    // Pushes CommandBuffer to the back of the queue.
    _returnedCommandBuffer.Append(std::move(commandBuffer));
}

} // namespace Graphics

} // namespace Axis