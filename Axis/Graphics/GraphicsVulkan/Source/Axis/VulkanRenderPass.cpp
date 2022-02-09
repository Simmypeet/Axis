/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanRenderPass.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

namespace Graphics
{

VulkanRenderPass::VulkanRenderPass(const RenderPassDescription& description,
                                   VulkanGraphicsDevice&        vulkanGraphicsDevice) :
    IRenderPass(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkRenderPass = [&]() -> VkRenderPass {
        VkRenderPass vkRenderPass = {};

        // Parse the Axis's AttachmentReference to Vulkan's VkAttachmentReference
        constexpr auto ConvertAttachmentsReferences = [](System::List<VkAttachmentReference>& output, const System::List<AttachmentReference>& input) {
            auto i = 0;

            for (auto& attachment : output)
            {
                if (input[i].Index != AttachmentReference::Unused)
                {
                    attachment.attachment = input[i].Index == AttachmentReference::Unused ? VK_ATTACHMENT_UNUSED : input[i].Index;
                    attachment.layout     = VulkanUtility::GetVkImageLayoutFromResourceState(input[i].SubpassState);
                }
                else
                {
                    attachment = {};
                }
                i++;
            }
        };

        System::List<VkAttachmentDescription> attachmentDescriptions          = {};
        System::List<VkSubpassDescription>    subpassDescriptions             = {};
        System::List<VkSubpassDependency>     subpassDependenciesDescriptions = {};

        attachmentDescriptions.ReserveFor(description.Attachments.GetLength());
        subpassDescriptions.ReserveFor(description.Subpasses.GetLength());
        subpassDependenciesDescriptions.ReserveFor(description.Dependencies.GetLength());

        System::List<System::List<VkAttachmentReference>> allColorAttachmentReferences(description.Subpasses.GetLength());
        System::List<System::List<VkAttachmentReference>> allInputAttachmentReferences(description.Subpasses.GetLength());
        System::List<VkAttachmentReference>               allDepthStencilAttachmentReferences(description.Subpasses.GetLength());

        for (Size i = 0; i < description.Subpasses.GetLength(); i++)
        {
            if (description.Subpasses[i].RenderTargetReferences)
            {
                allColorAttachmentReferences[i] = System::List<VkAttachmentReference>(description.Subpasses[i].RenderTargetReferences.GetLength());

                ConvertAttachmentsReferences(allColorAttachmentReferences[i], description.Subpasses[i].RenderTargetReferences);
            }

            if (description.Subpasses[i].InputReferences)
            {
                allInputAttachmentReferences[i] = System::List<VkAttachmentReference>(description.Subpasses[i].InputReferences.GetLength());

                ConvertAttachmentsReferences(allInputAttachmentReferences[i], description.Subpasses[i].InputReferences);
            }

            allDepthStencilAttachmentReferences[i].attachment = description.Subpasses[i].DepthStencilReference.Index == AttachmentReference::Unused ? VK_ATTACHMENT_UNUSED : description.Subpasses[i].DepthStencilReference.Index;
            allDepthStencilAttachmentReferences[i].layout     = VulkanUtility::GetVkImageLayoutFromResourceState(description.Subpasses[i].DepthStencilReference.SubpassState);
        }

        for (auto& attachment : description.Attachments)
        {
            VkAttachmentDescription vkAttachment = {};
            vkAttachment.format                  = VulkanUtility::GetVkFormatFromTextureFormat(attachment.Format);
            vkAttachment.samples                 = static_cast<VkSampleCountFlagBits>(attachment.Samples);
            vkAttachment.loadOp                  = VulkanUtility::GetVkAttachmentLoadOpFromLoadOperation(attachment.ColorDepthLoadOperation);
            vkAttachment.storeOp                 = VulkanUtility::GetVkAttachmentStoreOpFromStoreOperation(attachment.ColorDepthStoreOperation);
            vkAttachment.stencilLoadOp           = VulkanUtility::GetVkAttachmentLoadOpFromLoadOperation(attachment.StencilLoadOperation);
            vkAttachment.stencilStoreOp          = VulkanUtility::GetVkAttachmentStoreOpFromStoreOperation(attachment.StencilStoreOperation);
            vkAttachment.initialLayout           = VulkanUtility::GetVkImageLayoutFromResourceState(attachment.InitialState);
            vkAttachment.finalLayout             = VulkanUtility::GetVkImageLayoutFromResourceState(attachment.FinalState);

            attachmentDescriptions.Append(std::move(vkAttachment));
        }

        Size i = 0;

        for (auto& subpass : description.Subpasses)
        {
            Uint32 colorAttachmentCount = (Uint32)allColorAttachmentReferences[i].GetLength();
            Uint32 inputAttachmentCount = (Uint32)allInputAttachmentReferences[i].GetLength();

            VkAttachmentReference* pColorAttachments       = colorAttachmentCount == 0 ? nullptr : allColorAttachmentReferences[i].GetData();
            VkAttachmentReference* pInputAttachments       = inputAttachmentCount == 0 ? nullptr : allInputAttachmentReferences[i].GetData();
            VkAttachmentReference* pDepthStencilAttachment = subpass.DepthStencilReference.Index != AttachmentReference::Unused ? &allDepthStencilAttachmentReferences[i] : nullptr;

            VkSubpassDescription vkSubpass    = {};
            vkSubpass.flags                   = 0,
            vkSubpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkSubpass.inputAttachmentCount    = inputAttachmentCount,
            vkSubpass.pInputAttachments       = pInputAttachments,
            vkSubpass.colorAttachmentCount    = colorAttachmentCount,
            vkSubpass.pColorAttachments       = pColorAttachments,
            vkSubpass.pDepthStencilAttachment = pDepthStencilAttachment;

            subpassDescriptions.Append(std::move(vkSubpass));

            i++;
        }

        i = 0;

        for (auto& dependency : description.Dependencies)
        {
            VkSubpassDependency vkSubpassDependency = {};
            vkSubpassDependency.srcSubpass          = dependency.SourceSubpassIndex == SubpassDependency::SubpassExternal ? VK_SUBPASS_EXTERNAL : dependency.SourceSubpassIndex;
            vkSubpassDependency.dstSubpass          = dependency.DestSubpassIndex == SubpassDependency::SubpassExternal ? VK_SUBPASS_EXTERNAL : dependency.DestSubpassIndex;
            vkSubpassDependency.srcStageMask        = (VkPipelineStageFlags)VulkanUtility::GetVkPipelineStageFlagBitsFromPipelineStage(dependency.SourceStages);
            vkSubpassDependency.dstStageMask        = (VkPipelineStageFlags)VulkanUtility::GetVkPipelineStageFlagBitsFromPipelineStage(dependency.DestStages);
            vkSubpassDependency.srcAccessMask       = (VkAccessFlags)VulkanUtility::GetVkAccessFlagBitsFromAccessMode(dependency.SourceAccessMode);
            vkSubpassDependency.dstAccessMask       = (VkAccessFlags)VulkanUtility::GetVkAccessFlagBitsFromAccessMode(dependency.DestAccessMode);

            subpassDependenciesDescriptions.Append(std::move(vkSubpassDependency));

            i++;
        }

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        renderPassCreateInfo.attachmentCount        = (Uint32)description.Attachments.GetLength();
        renderPassCreateInfo.pAttachments           = attachmentDescriptions.GetData();
        renderPassCreateInfo.subpassCount           = (Uint32)description.Subpasses.GetLength();
        renderPassCreateInfo.pSubpasses             = subpassDescriptions.GetData();
        renderPassCreateInfo.dependencyCount        = (Uint32)description.Dependencies.GetLength();
        renderPassCreateInfo.pDependencies          = description.Dependencies.GetLength() == 0 ? nullptr : subpassDependenciesDescriptions.GetData();

        auto vkResult = vkCreateRenderPass(vulkanGraphicsDevice.GetVkDeviceHandle(), &renderPassCreateInfo, nullptr, &vkRenderPass);

        if (vkResult != VK_SUCCESS)
            throw System::ExternalException("Failed to create vkRenderPass!");
        else
            return vkRenderPass;
    };

    auto DestroyVkRenderPass = [this](VkRenderPass vkRenderPass) {
        vkDestroyRenderPass(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkRenderPass, nullptr);
    };

    _vulkanRenderPass = VkPtr<VkRenderPass>(CreateVkRenderPass, std::move(DestroyVkRenderPass));
}

} // namespace Graphics

} // namespace Axis