/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanBuffer.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanDescriptorPool.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanResourceHeap.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanSampler.hpp>
#include <Axis/VulkanTexture.hpp>
#include <Axis/VulkanUtility.hpp>


namespace Axis
{

namespace Graphics
{

VulkanResourceHeap::VulkanResourceHeap(const ResourceHeapDescription& description,
                                       VulkanGraphicsDevice&          vulkanGraphicsDevice) :
    IResourceHeap(description),
    _descriptorPool(description, vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    _currentDescriptorSetGroup = _descriptorPool.GetDescriptorSetGroup();
}

void VulkanResourceHeap::BindBuffers(Uint32                                              bindingIndex,
                                     const System::Span<System::SharedPointer<IBuffer>>& buffers,
                                     const System::Span<Size>&                           offsets,
                                     const System::Span<Size>&                           sizes,
                                     Uint32                                              startingArrayIndex)
{
    // Validates the arguments.
    IResourceHeap::BindBuffers(bindingIndex,
                               buffers,
                               offsets,
                               sizes,
                               startingArrayIndex);

    Size resourceCount = buffers.GetSize();

    // Creates a copy of current binding (in case of exceptions)
    auto bufferBindingBackup = _vulkanBufferBindings;

    for (Size i = 0; i < resourceCount; i++)
    {
        VulkanBufferBinding vulkanBufferBinding = {
            .VulkanBuffer = (System::SharedPointer<VulkanBuffer>)buffers[i],
            .Offset       = offsets == nullptr ? 0 : offsets[i],
            .BufferSize   = sizes == nullptr ? buffers[i]->Description.BufferSize - (offsets == nullptr ? 0 : offsets[i]) : 0};

        ResourceLocation resourceLocation = {bindingIndex,
                                             (Uint32)(startingArrayIndex + i)};

        auto result = bufferBindingBackup.Insert({resourceLocation, vulkanBufferBinding});

        // Elements at this location is already existed!
        if (!result.First)
            result.Second->Second = std::move(vulkanBufferBinding);
    }

    _vulkanBufferBindings = std::move(bufferBindingBackup);

    _currentDescriptorSetGroup->UpToDate = false;

    _descriptorPool.MarkAllAsNotUpToDate();
}

void VulkanResourceHeap::BindSamplers(Uint32                                                   bindingIndex,
                                      const System::Span<System::SharedPointer<ISampler>>&     samplers,
                                      const System::Span<System::SharedPointer<ITextureView>>& textureViews,
                                      Uint32                                                   startingArrayIndex)
{
    // Validates the arguments
    IResourceHeap::BindSamplers(bindingIndex,
                                samplers,
                                textureViews,
                                startingArrayIndex);

    Size resourceCount = samplers.GetSize();

    // Creates a copy of current binding (in case of exceptions)
    auto vulkanSamplerBindingBackup = _vulkanSamplerBindings;

    for (Size i = 0; i < resourceCount; i++)
    {
        VulkanSamplerBinding vulkanSamplerBinding = {
            .VulkanTextureView = (System::SharedPointer<VulkanTextureView>)textureViews[i],
            .VulkanSampler     = (System::SharedPointer<VulkanSampler>)samplers[i],
        };

        ResourceLocation resourceLocation = {.BindingIndex = bindingIndex,
                                             .ArrayIndex   = (Uint32)(startingArrayIndex + i)};

        auto result = vulkanSamplerBindingBackup.Insert({resourceLocation, vulkanSamplerBinding});

        // Elements at this location is already existed!
        if (!result.First)
            result.Second->Second = std::move(vulkanSamplerBinding);
    }


    _vulkanSamplerBindings = std::move(vulkanSamplerBindingBackup);

    _currentDescriptorSetGroup->UpToDate = false;

    _descriptorPool.MarkAllAsNotUpToDate();
}

void VulkanResourceHeap::PrepareResourceHeapBinding(VulkanDeviceContext& deviceContext,
                                                    StateTransition      stateTransition)
{

    // If the current descriptor set group is available, directly write into the descriptor set.
    if (_currentDescriptorSetGroup->IsAvailable())
    {
        InternalBindResources(deviceContext,
                              stateTransition,
                              *_currentDescriptorSetGroup);
    }
    // The current descriptor set group is not available, allocates new one and
    // returns the current descriptor set group back to the pool for future uses.
    else
    {
        auto newDescriptorSetGroup = _descriptorPool.GetDescriptorSetGroup();

        newDescriptorSetGroup->PrepareBindDescriptorSetGroup(deviceContext);

        // Binds the whole new resource binding.
        InternalBindResources(deviceContext,
                              stateTransition,
                              *newDescriptorSetGroup);

        _descriptorPool.ReturnDescriptorSetGroup(std::move(_currentDescriptorSetGroup));

        _currentDescriptorSetGroup = std::move(newDescriptorSetGroup);
    }

    // Sets the fence and stores strong references.
    _currentDescriptorSetGroup->PrepareBindDescriptorSetGroup(deviceContext);
}

void VulkanResourceHeap::InternalBindResources(VulkanDeviceContext&      vulkanDeviceContext,
                                               StateTransition           stateTransition,
                                               VulkanDescriptorSetGroup& descriptorSetGroup)
{
    System::List<VkWriteDescriptorSet>   writeDescriptorSets   = {};
    System::List<VkDescriptorBufferInfo> descriptorBufferInfos = {};
    System::List<VkDescriptorImageInfo>  descriptorImageInfos  = {};

    if (!descriptorSetGroup.UpToDate)
    {
        writeDescriptorSets.Reserve(_vulkanBufferBindings.GetSize() + _vulkanSamplerBindings.GetSize());
        descriptorBufferInfos.Reserve(_vulkanBufferBindings.GetSize());
        descriptorImageInfos.Reserve(_vulkanSamplerBindings.GetSize());
    }

    for (const auto& resourceBindingInfo : _vulkanBufferBindings)
    {
        vulkanDeviceContext.GetVulkanCommandBuffer()->AddResourceStrongReference(resourceBindingInfo.Second.VulkanBuffer);

        if (stateTransition == StateTransition::Transit)
        {
            vulkanDeviceContext.TransitBufferState(resourceBindingInfo.Second.VulkanBuffer,
                                                   resourceBindingInfo.Second.VulkanBuffer->GetCurrentResourceState(),
                                                   ResourceState::Uniform,
                                                   false,
                                                   true);
        }

        if (!descriptorSetGroup.UpToDate)
        {
            VkDescriptorBufferInfo descriptorBufferInfo = {};
            descriptorBufferInfo.buffer                 = resourceBindingInfo.Second.VulkanBuffer->GetVkBufferHandle();
            descriptorBufferInfo.offset                 = resourceBindingInfo.Second.Offset;
            descriptorBufferInfo.range                  = resourceBindingInfo.Second.BufferSize;

            descriptorBufferInfos.Append(descriptorBufferInfo);

            VkDescriptorType descriptorType = {};

            for (const auto& resourceBinding : Description.ResourceHeapLayout->Description.ResourceBindings)
            {
                if (resourceBinding.BindingIndex == resourceBindingInfo.First.BindingIndex)
                {
                    descriptorType = VulkanUtility::GetVkDescriptorTypeFromResourceBinding(resourceBinding.Binding);
                    break;
                }
            }

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.pNext                = nullptr;
            descriptorWrite.dstSet               = descriptorSetGroup.DescriptorSet;
            descriptorWrite.dstBinding           = resourceBindingInfo.First.BindingIndex;
            descriptorWrite.dstArrayElement      = resourceBindingInfo.First.ArrayIndex;
            descriptorWrite.descriptorCount      = 1;
            descriptorWrite.descriptorType       = descriptorType;
            descriptorWrite.pImageInfo           = nullptr;
            descriptorWrite.pBufferInfo          = &descriptorBufferInfos[descriptorBufferInfos.GetSize() - 1];
            descriptorWrite.pTexelBufferView     = nullptr;

            writeDescriptorSets.Append(descriptorWrite);
        }
    }

    for (const auto& resourceBindingInfo : _vulkanSamplerBindings)
    {
        vulkanDeviceContext.GetVulkanCommandBuffer()->AddResourceStrongReference(resourceBindingInfo.Second.VulkanSampler);
        vulkanDeviceContext.GetVulkanCommandBuffer()->AddResourceStrongReference(resourceBindingInfo.Second.VulkanTextureView);

        if (stateTransition == StateTransition::Transit)
        {
            vulkanDeviceContext.TransitTextureState(resourceBindingInfo.Second.VulkanTextureView->Description.ViewTexture,
                                                    resourceBindingInfo.Second.VulkanTextureView->Description.ViewTexture->GetCurrentResourceState(),
                                                    ResourceState::ShaderReadOnly,
                                                    0,
                                                    resourceBindingInfo.Second.VulkanTextureView->Description.ViewTexture->Description.ArraySize,
                                                    0,
                                                    resourceBindingInfo.Second.VulkanTextureView->Description.ViewTexture->Description.MipLevels,
                                                    false,
                                                    true);
        }

        if (!descriptorSetGroup.UpToDate)
        {
            VkDescriptorImageInfo descriptorImageInfo = {};
            descriptorImageInfo.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo.imageView             = resourceBindingInfo.Second.VulkanTextureView->GetVkImageViewHandle();
            descriptorImageInfo.sampler               = resourceBindingInfo.Second.VulkanSampler->GetVkSamplerHandle();

            descriptorImageInfos.Append(descriptorImageInfo);

            VkDescriptorType descriptorType = {};

            for (const auto& resourceBinding : Description.ResourceHeapLayout->Description.ResourceBindings)
            {
                if (resourceBinding.BindingIndex == resourceBindingInfo.First.BindingIndex)
                {
                    descriptorType = VulkanUtility::GetVkDescriptorTypeFromResourceBinding(resourceBinding.Binding);
                    break;
                }
            }

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.pNext                = nullptr;
            descriptorWrite.dstSet               = descriptorSetGroup.DescriptorSet;
            descriptorWrite.dstBinding           = resourceBindingInfo.First.BindingIndex;
            descriptorWrite.dstArrayElement      = resourceBindingInfo.First.ArrayIndex;
            descriptorWrite.descriptorCount      = 1;
            descriptorWrite.descriptorType       = descriptorType;
            descriptorWrite.pImageInfo           = &descriptorImageInfos[descriptorImageInfos.GetSize() - 1];
            descriptorWrite.pBufferInfo          = nullptr;
            descriptorWrite.pTexelBufferView     = nullptr;

            writeDescriptorSets.Append(descriptorWrite);
        }
    }

    if (!descriptorSetGroup.UpToDate)
    {
        vkUpdateDescriptorSets(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(),
                               (Uint32)writeDescriptorSets.GetSize(),
                               writeDescriptorSets.GetData(),
                               0,
                               nullptr);

        descriptorSetGroup.UpToDate = true;
    }
}

} // namespace Graphics

} // namespace Axis
