/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanBuffer.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanDescriptorPool.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanFence.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanSampler.hpp>
#include <Axis/VulkanTexture.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

VulkanDescriptorSetGroup::VulkanDescriptorSetGroup(VkDescriptorSet       descriptorSet,
                                                   VulkanGraphicsDevice& vulkanGraphicsDevice) :
    DescriptorSet(descriptorSet),
    _expectedValue(0)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    _descriptorFinished = vulkanGraphicsDevice.CreateFence(0);
}

void VulkanDescriptorSetGroup::PrepareBindDescriptorSetGroup(VulkanDeviceContext& vulkanDeviceContext)
{
    if (_expectedValue == UINT64_MAX)
    {
        _expectedValue      = 0;
        _descriptorFinished = GetCreatorDevice()->CreateFence(0);
    }

    _expectedValue++;

    vulkanDeviceContext.AppendSignalFence(_descriptorFinished, _expectedValue);
}

Bool VulkanDescriptorSetGroup::IsAvailable() const noexcept
{
    return _descriptorFinished->GetCurrentValue() == _expectedValue;
}

VulkanDescriptorPool::VulkanDescriptorPool(const ResourceHeapDescription& description,
                                           VulkanGraphicsDevice&          vulkanGraphicsDevice) :
    _resourceHeapDesc(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);
}

UniquePointer<VulkanDescriptorSetGroup> VulkanDescriptorPool::GetDescriptorSetGroup()
{
    UniquePointer<VulkanDescriptorSetGroup> descriptorSetGroupToReturn;

    if (_descriptorSetGroups.GetLength())
    {
        {
            std::scoped_lock lockGuard(_vectorMutex);

            for (Size i = 0; i < _descriptorSetGroups.GetLength(); i++)
            {
                if (_descriptorSetGroups[i]->IsAvailable())
                {
                    descriptorSetGroupToReturn = std::move(_descriptorSetGroups[i]);

                    _descriptorSetGroups.RemoveAt(i);

                    break;
                }
            }
        }

        if (descriptorSetGroupToReturn)
            return descriptorSetGroupToReturn;
    }

    if (_currentAllocation == GetCurrentGroupSize() || _descriptorPools.GetLength() == 0)
        AddPool();

    VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;

    const auto descriptorSetLayout = ((VulkanResourceHeapLayout*)_resourceHeapDesc.ResourceHeapLayout)->GetVkDescriptorSetLayoutHandle();

    VkDescriptorSetAllocateInfo descriptorSetAllocationInfo = {};
    descriptorSetAllocationInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocationInfo.pNext                       = nullptr;
    descriptorSetAllocationInfo.descriptorPool              = _descriptorPools[_descriptorPools.GetLength() - 1];
    descriptorSetAllocationInfo.descriptorSetCount          = 1;
    descriptorSetAllocationInfo.pSetLayouts                 = &descriptorSetLayout;

    auto vkResult = vkAllocateDescriptorSets(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), &descriptorSetAllocationInfo, &vkDescriptorSet);

    if (vkResult != VK_SUCCESS)
        throw ExternalException("Failed to allocate VkDescriptorSet!");

    descriptorSetGroupToReturn = UniquePointer<VulkanDescriptorSetGroup>(Axis::New<VulkanDescriptorSetGroup>(vkDescriptorSet,
                                                                                                             *(VulkanGraphicsDevice*)GetCreatorDevice()));

    _currentAllocation++;

    return descriptorSetGroupToReturn;
}

void VulkanDescriptorPool::ReturnDescriptorSetGroup(UniquePointer<VulkanDescriptorSetGroup>&& descriptorSetGroup) noexcept
{
    std::scoped_lock lockGuard(_vectorMutex);

    _descriptorSetGroups.Append(std::move(descriptorSetGroup));
}

void VulkanDescriptorPool::MarkAllAsNotUpToDate() noexcept
{
    for (const auto& descriptor : _descriptorSetGroups)
    {
        descriptor->UpToDate = false;
    }
}

void VulkanDescriptorPool::AddPool()
{
    const Size currentAllocationGroup = _descriptorPools.GetLength();
    const Size multiplier             = InitialDescriptorSetPoolSize + currentAllocationGroup;
    _currentAllocation                = 0;

    AXIS_ASSERT(_resourceHeapDesc.ResourceHeapLayout, "_resourceDesc.PipelineLayout shouldn't be nullptr!");

    auto CreateVkDescriptorPool = [&]() -> VkDescriptorPool {
        List<VkDescriptorPoolSize> descriptorPoolSizes = {};
        descriptorPoolSizes.ReserveFor(_resourceHeapDesc.ResourceHeapLayout->Description.ResourceBindings.GetLength());

        for (const auto& resourceBinding : _resourceHeapDesc.ResourceHeapLayout->Description.ResourceBindings)
        {
            VkDescriptorPoolSize descriptorPoolSize = {};
            descriptorPoolSize.type                 = VulkanUtility::GetVkDescriptorTypeFromResourceBinding(resourceBinding.Binding);
            descriptorPoolSize.descriptorCount      = (Uint32)multiplier;

            descriptorPoolSizes.Append(std::move(descriptorPoolSize));
        }

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.pNext                      = nullptr;
        descriptorPoolCreateInfo.flags                      = 0;
        descriptorPoolCreateInfo.maxSets                    = (Uint32)multiplier;
        descriptorPoolCreateInfo.poolSizeCount              = (Uint32)descriptorPoolSizes.GetLength();
        descriptorPoolCreateInfo.pPoolSizes                 = descriptorPoolSizes.GetLength() == 0 ? nullptr : descriptorPoolSizes.GetData();

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        auto result = vkCreateDescriptorPool(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), &descriptorPoolCreateInfo, nullptr, &descriptorPool);

        if (result != VK_SUCCESS)
            throw ExternalException("Failed to create VkDescriptorPool!");

        return descriptorPool;
    };

    auto DestroyVkDescriptorPool = [this](VkDescriptorPool vkDescriptorPool) {
        vkDestroyDescriptorPool(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkDescriptorPool, nullptr);
    };

    VkPtr<VkDescriptorPool> descriptorPool = VkPtr<VkDescriptorPool>(CreateVkDescriptorPool, std::move(DestroyVkDescriptorPool));

    _descriptorPools.Append(std::move(descriptorPool));

    _currentAllocation = 0;
}

} // namespace Axis
