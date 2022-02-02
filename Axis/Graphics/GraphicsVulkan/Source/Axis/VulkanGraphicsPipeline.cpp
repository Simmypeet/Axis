/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsPipeline.hpp>
#include <Axis/VulkanRenderPass.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanShaderModule.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineDescription& description,
                                               VulkanGraphicsDevice&              vulkanGraphicsDevice) :
    IGraphicsPipeline(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVkPipelineLayout = [&]() -> VkPipelineLayout {
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;

        List<VkDescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.ReserveFor(description.ResourceHeapLayouts.GetLength());

        for (const auto& resourceHeapLayout : description.ResourceHeapLayouts)
        {
            descriptorSetLayouts.Append(((VulkanResourceHeapLayout*)resourceHeapLayout)->GetVkDescriptorSetLayoutHandle());
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext                      = nullptr;
        pipelineLayoutCreateInfo.pPushConstantRanges        = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount     = 0;
        pipelineLayoutCreateInfo.setLayoutCount             = (Uint32)descriptorSetLayouts.GetLength();
        pipelineLayoutCreateInfo.pSetLayouts                = descriptorSetLayouts.GetLength() == 0 ? nullptr : descriptorSetLayouts.GetData();

        auto vkResult = vkCreatePipelineLayout(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(),
                                               &pipelineLayoutCreateInfo,
                                               nullptr,
                                               &vkPipelineLayout);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to create VkPipelineLayout!");

        return vkPipelineLayout;
    };

    auto DestroyVkPipelineLayout = [this](VkPipelineLayout vkPipelineLayout) {
        vkDestroyPipelineLayout(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkPipelineLayout, nullptr);
    };

    _vulkanPipelineLayout = VkPtr<VkPipelineLayout>(CreateVkPipelineLayout, std::move(DestroyVkPipelineLayout));

    // Creates implicit renderpass
    if (!description.RenderPass)
    {
        RenderPassDescription renderPassDesc = {};
        renderPassDesc.Attachments           = List<RenderPassAttachment>(description.DepthStencilViewFormat == TextureFormat::Unknown ? description.RenderTargetViewFormats.GetLength() : description.RenderTargetViewFormats.GetLength() + 1);
        renderPassDesc.Subpasses             = List<SubpassDescription>(1);

        auto& allAttachments = renderPassDesc.Attachments;

        if (description.DepthStencilViewFormat != TextureFormat::Unknown)
        {
            allAttachments[0].Samples = description.SampleCount;
            allAttachments[0].Format  = description.DepthStencilViewFormat;

            allAttachments[0].ColorDepthLoadOperation  = LoadOperation::Load;
            allAttachments[0].ColorDepthStoreOperation = StoreOperation::Store;

            allAttachments[0].StencilLoadOperation  = LoadOperation::Load;
            allAttachments[0].StencilStoreOperation = StoreOperation::Store;

            allAttachments[0].InitialState = ResourceState::DepthStencilWrite;
            allAttachments[0].FinalState   = ResourceState::DepthStencilWrite;

            renderPassDesc.Subpasses[0].DepthStencilReference.Index        = 0;
            renderPassDesc.Subpasses[0].DepthStencilReference.SubpassState = ResourceState::DepthStencilWrite;
        }

        renderPassDesc.Subpasses[0].RenderTargetReferences = List<AttachmentReference>(description.RenderTargetViewFormats.GetLength());

        for (Size i = 0; i < description.RenderTargetViewFormats.GetLength(); i++)
        {
            Size indexNow = description.DepthStencilViewFormat == TextureFormat::Unknown ? i : i + 1;

            allAttachments[indexNow].Samples = description.SampleCount;
            allAttachments[indexNow].Format  = description.RenderTargetViewFormats[i];

            allAttachments[indexNow].ColorDepthLoadOperation  = LoadOperation::Load;
            allAttachments[indexNow].ColorDepthStoreOperation = StoreOperation::Store;

            allAttachments[indexNow].StencilLoadOperation  = LoadOperation::DontCare;
            allAttachments[indexNow].StencilStoreOperation = StoreOperation::DontCare;

            allAttachments[indexNow].InitialState = ResourceState::RenderTarget;
            allAttachments[indexNow].FinalState   = ResourceState::RenderTarget;

            renderPassDesc.Subpasses[0].RenderTargetReferences[i].Index        = (Uint32)indexNow;
            renderPassDesc.Subpasses[0].RenderTargetReferences[i].SubpassState = ResourceState::RenderTarget;
        }

        _implicitRenderPass = vulkanGraphicsDevice.CreateRenderPass(renderPassDesc);
    }

    auto CreateVkPipeline = [&]() -> VkPipeline {
        VkPipeline vkPipeline = VK_NULL_HANDLE;

        List<VkPipelineShaderStageCreateInfo> shaderStages = {};
        shaderStages.ReserveFor(2);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage                           = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module                          = ((VulkanShaderModule*)description.VertexShader)->GetVkShaderModuleHandle();
        vertShaderStageInfo.pName                           = description.FragmentShader->Description.EntryPoint.GetCString();

        VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
        fragShaderStageInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage                           = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module                          = ((VulkanShaderModule*)description.FragmentShader)->GetVkShaderModuleHandle();
        fragShaderStageInfo.pName                           = description.FragmentShader->Description.EntryPoint.GetCString();

        shaderStages.Append(std::move(vertShaderStageInfo));
        shaderStages.Append(std::move(fragShaderStageInfo));

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        List<VkVertexInputBindingDescription>   inputBindingDescriptions;
        List<VkVertexInputAttributeDescription> inputAttributesDescriptions;
        inputBindingDescriptions.ReserveFor(description.VertexBindingDescriptions.GetLength());
        inputAttributesDescriptions.ReserveFor(description.VertexBindingDescriptions.GetLength());

        if (description.VertexBindingDescriptions)
        {
            for (const auto& vertexBindingDesc : description.VertexBindingDescriptions)
            {
                VkVertexInputBindingDescription vkVertextBindingDesc = {};
                vkVertextBindingDesc.binding                         = vertexBindingDesc.BindingSlot;
                vkVertextBindingDesc.stride                          = (Uint32)vertexBindingDesc.GetStride();
                vkVertextBindingDesc.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

                inputBindingDescriptions.Append(std::move(vkVertextBindingDesc));

                Uint32 offset = 0;
                for (const auto& vertexBindingAttribute : vertexBindingDesc.Attributes)
                {
                    VkVertexInputAttributeDescription vkVertexInputAttribute = {};
                    vkVertexInputAttribute.location                          = vertexBindingAttribute.Location;
                    vkVertexInputAttribute.binding                           = vertexBindingDesc.BindingSlot;
                    vkVertexInputAttribute.format                            = VulkanUtility::GetVkFormatFromShaderDataType(vertexBindingAttribute.Type);
                    vkVertexInputAttribute.offset                            = offset;

                    offset += (Uint32)Graphics::GetShaderDataTypeSize(vertexBindingAttribute.Type);

                    inputAttributesDescriptions.Append(vkVertexInputAttribute);
                }
            }

            vertexInputInfo.vertexBindingDescriptionCount = (Uint32)inputBindingDescriptions.GetLength();
            vertexInputInfo.pVertexBindingDescriptions    = inputBindingDescriptions.GetData();

            vertexInputInfo.vertexAttributeDescriptionCount = (Uint32)inputAttributesDescriptions.GetLength();
            vertexInputInfo.pVertexAttributeDescriptions    = inputAttributesDescriptions.GetData();
        }
        else
        {
            vertexInputInfo.vertexBindingDescriptionCount   = 0;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
        }

        constexpr auto GetVkStencilOpStateFromStencilOperationDescription = [](const StencilOperationDescription& stencilOperationDescription,
                                                                               Uint8                              compareMask,
                                                                               Uint8                              writeMask) -> VkStencilOpState {
            VkStencilOpState stencilOpState = {};
            stencilOpState.failOp           = VulkanUtility::GetVkStencilOpFromStencilOperation(stencilOperationDescription.StencilFailOperation);
            stencilOpState.passOp           = VulkanUtility::GetVkStencilOpFromStencilOperation(stencilOperationDescription.StencilPassDepthPassOperation);
            stencilOpState.depthFailOp      = VulkanUtility::GetVkStencilOpFromStencilOperation(stencilOperationDescription.StencilPassDepthFailOperation);
            stencilOpState.compareOp        = VulkanUtility::GetVkCompareOpFromCompareFunction(stencilOperationDescription.StencilCompareFunction);
            stencilOpState.compareMask      = compareMask;
            stencilOpState.writeMask        = writeMask;
            stencilOpState.reference        = 0; /*dynamic*/

            return stencilOpState;
        };

        // DepthStencilState
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext                                 = nullptr;
        depthStencil.depthTestEnable                       = description.DepthStencil.DepthTestEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthWriteEnable                      = description.DepthStencil.DepthWriteEnable ? VK_TRUE : VK_FALSE;
        depthStencil.depthCompareOp                        = VulkanUtility::GetVkCompareOpFromCompareFunction(description.DepthStencil.DepthCompareFunction);
        depthStencil.depthBoundsTestEnable                 = VK_FALSE; /*What the hell is this*/
        depthStencil.stencilTestEnable                     = description.DepthStencil.StencilEnable ? VK_TRUE : VK_FALSE,
        depthStencil.front                                 = GetVkStencilOpStateFromStencilOperationDescription(description.DepthStencil.FrontFaceStencilOperation, description.DepthStencil.StencilReadMask, description.DepthStencil.StencilWriteMask);
        depthStencil.back                                  = GetVkStencilOpStateFromStencilOperationDescription(description.DepthStencil.BackFaceStencilOperation, description.DepthStencil.StencilReadMask, description.DepthStencil.StencilWriteMask);


        // RasterizerState
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable                 = VK_FALSE;

        // RasterizerState
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable                       = description.Rasterizer.DepthClipEnable ? VK_FALSE : VK_TRUE; // Reversed
        rasterizer.rasterizerDiscardEnable                = VK_FALSE;
        rasterizer.polygonMode                            = VulkanUtility::GetVkPolygonModeFromFillMode(description.Rasterizer.PrimitiveFillMode);
        rasterizer.cullMode                               = VulkanUtility::GetVkCullModeFlagsFromCullModeFlags(description.Rasterizer.FaceCulling);
        rasterizer.frontFace                              = VulkanUtility::GetVkFrontFaceFromFrontFace(description.Rasterizer.FrontFaceWinding);
        rasterizer.depthBiasEnable                        = description.Rasterizer.DepthBias != 0.0f || description.Rasterizer.SlopeScaledDepthBias != 0.0f ? VK_TRUE : VK_FALSE;
        rasterizer.depthBiasConstantFactor                = (Float32)description.Rasterizer.DepthBias;
        rasterizer.depthBiasSlopeFactor                   = description.Rasterizer.SlopeScaledDepthBias;
        rasterizer.depthBiasClamp                         = description.Rasterizer.DepthBiasClamp;
        rasterizer.lineWidth                              = 1.0f;

        // RasterizerState
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
        multisampling.sampleShadingEnable                  = VK_FALSE;


        List<VkPipelineColorBlendAttachmentState> attachmentBlendStates = {};
        attachmentBlendStates.ReserveFor(description.Blend.RenderTargetBlendStates.GetLength());

        for (const auto& attachmentBlendState : description.Blend.RenderTargetBlendStates)
        {
            constexpr auto GetVkPipelineColorBlendAttachmentStateFromAttachmentBlendState = [](const AttachmentBlendState& attachmentBlendState) {
                VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
                colorBlendAttachment.alphaBlendOp                        = VulkanUtility::GetVkBlendOpFromBlendOperation(attachmentBlendState.AlphaOperation);
                colorBlendAttachment.blendEnable                         = attachmentBlendState.BlendEnable ? VK_TRUE : VK_FALSE;
                colorBlendAttachment.colorBlendOp                        = VulkanUtility::GetVkBlendOpFromBlendOperation(attachmentBlendState.ColorOperation);
                colorBlendAttachment.colorWriteMask                      = VulkanUtility::GetVkColorComponentFlagsColorChannelFlags(attachmentBlendState.WriteChannelFlags);
                colorBlendAttachment.dstAlphaBlendFactor                 = VulkanUtility::GetVkBlendFactorFromBlendFactor(attachmentBlendState.DestAlphaBlendFactor);
                colorBlendAttachment.dstColorBlendFactor                 = VulkanUtility::GetVkBlendFactorFromBlendFactor(attachmentBlendState.DestColorBlendFactor);
                colorBlendAttachment.srcAlphaBlendFactor                 = VulkanUtility::GetVkBlendFactorFromBlendFactor(attachmentBlendState.SourceAlphaBlendFactor);
                colorBlendAttachment.srcColorBlendFactor                 = VulkanUtility::GetVkBlendFactorFromBlendFactor(attachmentBlendState.SourceColorBlendFactor);

                return colorBlendAttachment;
            };

            attachmentBlendStates.Append(GetVkPipelineColorBlendAttachmentStateFromAttachmentBlendState(attachmentBlendState));
        }

        // BlendState
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable                       = description.Blend.LogicOperationEnable ? VK_TRUE : VK_FALSE;
        colorBlending.logicOp                             = VulkanUtility::GetVkLogicOpFromLogicOperation(description.Blend.LogicOp);
        colorBlending.attachmentCount                     = (Uint32)attachmentBlendStates.GetLength();
        colorBlending.pAttachments                        = attachmentBlendStates.GetData();
        colorBlending.blendConstants[0]                   = 0.0f;
        colorBlending.blendConstants[1]                   = 0.0f;
        colorBlending.blendConstants[2]                   = 0.0f;
        colorBlending.blendConstants[3]                   = 0.0f;

        VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_BLEND_CONSTANTS};

        VkPipelineDynamicStateCreateInfo dynamicStateCreateinfo = {};
        dynamicStateCreateinfo.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateinfo.dynamicStateCount                = 3;
        dynamicStateCreateinfo.pDynamicStates                   = dynamicStates;

        VkViewport viewport = {};
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = 0.0f;
        viewport.height     = 0.0f;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        VkRect2D scissor = {};
        scissor.offset   = {0, 0};
        scissor.extent   = {0, 0};

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount                     = 1;
        viewportState.pViewports                        = &viewport;
        viewportState.scissorCount                      = 1;
        viewportState.pScissors                         = &scissor;

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount                   = (Uint32)shaderStages.GetLength();
        pipelineInfo.pStages                      = shaderStages.GetData();
        pipelineInfo.pVertexInputState            = &vertexInputInfo;
        pipelineInfo.pViewportState               = &viewportState;
        pipelineInfo.pInputAssemblyState          = &inputAssembly;
        pipelineInfo.pRasterizationState          = &rasterizer;
        pipelineInfo.pMultisampleState            = &multisampling;
        pipelineInfo.pDepthStencilState           = &depthStencil;
        pipelineInfo.pColorBlendState             = &colorBlending;
        pipelineInfo.layout                       = _vulkanPipelineLayout;
        pipelineInfo.pDynamicState                = &dynamicStateCreateinfo;

        if (description.RenderPass)
        {
            pipelineInfo.renderPass = ((VulkanRenderPass*)description.RenderPass)->GetVkRenderPassHandle();
            pipelineInfo.subpass    = description.SubpassIndex;
        }
        else
        {
            pipelineInfo.renderPass = ((VulkanRenderPass*)_implicitRenderPass)->GetVkRenderPassHandle();
            pipelineInfo.subpass    = 0;
        }

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        auto vkResult = vkCreateGraphicsPipelines(vulkanGraphicsDevice.GetVkDeviceHandle(),
                                                  VK_NULL_HANDLE,
                                                  1,
                                                  &pipelineInfo,
                                                  nullptr,
                                                  &vkPipeline);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to create VkPipeline!");

        return vkPipeline;
    };

    auto DestroyVkPipeline = [this](VkPipeline vkPipeline) {
        vkDestroyPipeline(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkPipeline, nullptr);
    };

    _vulkanPipeline = VkPtr<VkPipeline>(CreateVkPipeline, std::move(DestroyVkPipeline));
}

} // namespace Axis