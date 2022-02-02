/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanFence.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>

namespace Axis
{

VulkanFence::VulkanFence(Uint64                initialValue,
                         VulkanGraphicsDevice& vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkSemaphore = [&]() -> VkSemaphore {
        VkSemaphore vkSemaphore = VK_NULL_HANDLE;

        // Creates timeline semaphore
        VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo = {};
        semaphoreTypeCreateInfo.sType                     = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        semaphoreTypeCreateInfo.pNext                     = nullptr;
        semaphoreTypeCreateInfo.semaphoreType             = VK_SEMAPHORE_TYPE_TIMELINE;
        semaphoreTypeCreateInfo.initialValue              = initialValue;

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext                 = &semaphoreTypeCreateInfo;

        VkResult vkResult = vkCreateSemaphore(vulkanGraphicsDevice.GetVkDeviceHandle(), &semaphoreCreateInfo, nullptr, &vkSemaphore);

        if (vkResult)
            throw ExternalException("Failed to create VkSemaphore!");
        return vkSemaphore;
    };

    auto DestroyVkSemaphore = [this](VkSemaphore vkSemaphore) {
        vkDestroySemaphore(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkSemaphore, nullptr);
    };

    _timelineSemaphore = VkPtr<VkSemaphore>(CreateVkSemaphore, std::move(DestroyVkSemaphore));
}

Uint64 VulkanFence::GetCurrentValue() const
{
    Uint64 value;

    VkResult result = vkGetSemaphoreCounterValue(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), _timelineSemaphore, &value);

    if (result != VK_SUCCESS)
        throw ExternalException("Failed to retrieve semaphore counter value!");

    return value;
}

void VulkanFence::SetValue(Uint64 value)
{
    VkSemaphoreSignalInfo signalInfo;
    signalInfo.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.pNext     = nullptr;
    signalInfo.semaphore = _timelineSemaphore;
    signalInfo.value     = value;

    auto result = vkSignalSemaphore(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), &signalInfo);

    if (result != VK_SUCCESS)
        throw ExternalException("Failed to set semaphore value!");
}

void VulkanFence::WaitForValue(Uint64 value) const
{
    VkSemaphore semaphore = _timelineSemaphore;

    VkSemaphoreWaitInfo waitInfo = {};
    waitInfo.sType               = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext               = NULL;
    waitInfo.flags               = 0;
    waitInfo.semaphoreCount      = 1;
    waitInfo.pSemaphores         = &semaphore;
    waitInfo.pValues             = &value;

    auto result = vkWaitSemaphores(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), &waitInfo, UINT64_MAX);

    if (result != VK_SUCCESS)
        throw ExternalException("Failed to wait semaphore!");
}

} // namespace Axis