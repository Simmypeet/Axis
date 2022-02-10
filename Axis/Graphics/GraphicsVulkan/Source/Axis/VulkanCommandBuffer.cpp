/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/Exception.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanFramebuffer.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanRenderPass.hpp>
#include <Axis/VulkanTexture.hpp>
#include <vulkan/vulkan_core.h>


namespace Axis
{

namespace Graphics
{

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandPool         commandPool,
                                         VkCommandBufferLevel  commandBufferLevel,
                                         VulkanGraphicsDevice& vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkCommandBuffer = [&]() -> VkCommandBuffer {
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext              = nullptr,
            .commandPool        = commandPool,
            .level              = commandBufferLevel,
            .commandBufferCount = 1};

        if (vkAllocateCommandBuffers(vulkanGraphicsDevice.GetVkDeviceHandle(), &commandBufferAllocateInfo, &vkCommandBuffer) != VK_SUCCESS)
            throw System::ExternalException("Failed to allocate VkCommandBuffer!");
        else
            return vkCommandBuffer;
    };

    auto DestroyVkCommandBuffer = [](VkCommandBuffer vkCommandBuffer) {
        // Do nothing :)
    };

    _commandBuffer = VkPtr<VkCommandBuffer>(CreateVkCommandBuffer, std::move(DestroyVkCommandBuffer));

    auto CreateVkFence = [&]() -> VkFence {
        VkFence vkFence = VK_NULL_HANDLE;

        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT};

        if (vkCreateFence(vulkanGraphicsDevice.GetVkDeviceHandle(), &fenceCreateInfo, nullptr, &vkFence) != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkFence!");
        else
            return vkFence;
    };

    auto DestroyVkFence = [this](VkFence vkFence) {
        vkDestroyFence(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkFence, nullptr);
    };

    _submitFence = VkPtr<VkFence>(CreateVkFence, std::move(DestroyVkFence));

    auto CreateVkSemaphore = [&]() -> VkSemaphore {
        VkSemaphore vkSemaphore = VK_NULL_HANDLE;

        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0};

        if (vkCreateSemaphore(vulkanGraphicsDevice.GetVkDeviceHandle(), &semaphoreCreateInfo, nullptr, &vkSemaphore) != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkSemaphore!");
        else
            return vkSemaphore;
    };

    auto DestroyVkSemaphore = [this](VkSemaphore vkSemaphore) {
        vkDestroySemaphore(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkSemaphore, nullptr);
    };

    _signalSemaphore = VkPtr<VkSemaphore>(CreateVkSemaphore, std::move(DestroyVkSemaphore));
}

// Default destructor
VulkanCommandBuffer::~VulkanCommandBuffer() noexcept
{
    if (_submitFence)
    {
        VkFence fence = _submitFence;

        vkWaitForFences(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
    }
}

void VulkanCommandBuffer::BeginRecording() noexcept
{
    AXIS_ASSERT(IsCommandBufferAvailable(), "The VulkanCommandBuffer has been already recorded or is in used!");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo         = nullptr;
    beginInfo.pNext                    = nullptr;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_commandBuffer, &beginInfo);

    _isRecording = true;
}

void VulkanCommandBuffer::EndRecording() noexcept
{
    AXIS_ASSERT(_isRecording, "The VulkanCommandBuffer hasn't been recorded yet!");

    if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
        AXIS_VALIDATE(false, "Failed to end recording VkCommandBuffer!");

    _isRecording = false;
}

void VulkanCommandBuffer::ResetCommandBuffer() noexcept
{
    if (_isRecording)
        EndRecording();

    AXIS_ASSERT(IsCommandBufferAvailable(), "Can't reset this CommandBuffer because it is in use!");

    _resourceReference.Clear();

    _activatingRenderPass  = nullptr;
    _activatingFramebuffer = nullptr;

    vkResetCommandBuffer(_commandBuffer, 0);
}

void VulkanCommandBuffer::AddResourceStrongReference(const System::SharedPointer<void>& reference)
{
    _resourceReference.Insert(reference);
}

Bool VulkanCommandBuffer::IsCommandBufferAvailable() const noexcept
{
    auto result = vkGetFenceStatus(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), _submitFence);

    return !_isRecording && result == VK_SUCCESS;
}

void VulkanCommandBuffer::BeginRenderPass(const System::SharedPointer<IRenderPass>&  renderPass,
                                          const System::SharedPointer<IFramebuffer>& framebuffer,
                                          VkRenderPassBeginInfo*                     beginInfo) noexcept
{

    AXIS_ASSERT(!IsRenderPassActivating(), "The render pass instance is already begun!");

    _activatingRenderPass  = renderPass;
    _activatingFramebuffer = framebuffer;

    VkRenderPassBeginInfo implicitRenderPassBeginInfo = {};

    if (beginInfo == nullptr)
    {
        implicitRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        implicitRenderPassBeginInfo.pNext             = nullptr;
        implicitRenderPassBeginInfo.renderPass        = ((VulkanRenderPass*)renderPass)->GetVkRenderPassHandle();
        implicitRenderPassBeginInfo.framebuffer       = ((VulkanFramebuffer*)framebuffer)->GetVkFramebufferHandle();
        implicitRenderPassBeginInfo.clearValueCount   = 0;
        implicitRenderPassBeginInfo.pClearValues      = nullptr;
        implicitRenderPassBeginInfo.renderArea.offset = {0, 0};
        implicitRenderPassBeginInfo.renderArea.extent = {
            framebuffer->Description.Attachments[0]->Description.ViewTexture->Description.Size.X,
            framebuffer->Description.Attachments[0]->Description.ViewTexture->Description.Size.Y};

        beginInfo = &implicitRenderPassBeginInfo;
    }

    vkCmdBeginRenderPass(_commandBuffer,
                         beginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::EndRenderPass() noexcept
{
    AXIS_ASSERT(IsRenderPassActivating(), "The render pass instance hasn't begun yet!");

    _activatingRenderPass  = nullptr;
    _activatingFramebuffer = nullptr;

    vkCmdEndRenderPass(_commandBuffer);
}

} // namespace Graphics

} // namespace Axis
