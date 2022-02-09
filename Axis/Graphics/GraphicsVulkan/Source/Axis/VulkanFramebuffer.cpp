/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanFramebuffer.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanRenderPass.hpp>
#include <Axis/VulkanTexture.hpp>

namespace Axis
{

namespace Graphics
{

VulkanFramebuffer::VulkanFramebuffer(FramebufferDescription description,
                                     VulkanGraphicsDevice&  vulkanGraphicsDevice) :
    IFramebuffer(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkFramebuffer = [&]() -> VkFramebuffer {
        VkFramebuffer vkFramebuffer = {};

        System::List<VkImageView> imageViews = {};
        imageViews.ReserveFor(description.Attachments.GetLength());

        Size index = 0;
        for (const auto& attachment : description.Attachments)
        {
            imageViews.Append(((VulkanTextureView*)attachment)->GetVkImageViewHandle());

            index++;
        }

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass              = ((VulkanRenderPass*)description.RenderPass)->GetVkRenderPassHandle();
        frameBufferCreateInfo.attachmentCount         = (Uint32)imageViews.GetLength();
        frameBufferCreateInfo.pAttachments            = imageViews.GetData();
        frameBufferCreateInfo.width                   = description.FramebufferSize.X;
        frameBufferCreateInfo.height                  = description.FramebufferSize.Y;
        frameBufferCreateInfo.layers                  = description.FramebufferSize.Z;

        auto vkResult = vkCreateFramebuffer(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), &frameBufferCreateInfo, nullptr, &vkFramebuffer);

        if (vkResult != VK_SUCCESS)
            throw System::ExternalException("Failed to create vkBuffer!");
        else
            return vkFramebuffer;
    };

    auto DestroyVkFramebuffer = [this](VkFramebuffer framebuffer) {
        vkDestroyFramebuffer(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), framebuffer, nullptr);
    };

    _vulkanFramebuffer = VkPtr<VkFramebuffer>(CreateVkFramebuffer, std::move(DestroyVkFramebuffer));
}

} // namespace Graphics

} // namespace Axis