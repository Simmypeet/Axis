/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANDESCRIPTORPOOL_HPP
#define AXIS_VULKANDESCRIPTORPOOL_HPP
#pragma once

#include "../../../../System/Include/Axis/List.hpp"
#include "../../../../System/Include/Axis/Math.hpp"
#include "../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../Include/Axis/ResourceHeap.hpp"
#include "VkPtr.hpp"
#include <atomic>

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IFence;
class VulkanGraphicsDevice;
class VulkanDescriptorPool;
class VulkanDeviceContext;
class VulkanSampler;
class VulkanBuffer;
class VulkanTextureView;
enum class StateTransition : Uint8;

// Specifies the group of required descriptor sets requested from resource heap.
class VulkanDescriptorSetGroup final : public DeviceChild
{
public:
    // Constructor
    VulkanDescriptorSetGroup(VkDescriptorSet       descriptorSet,
                             VulkanGraphicsDevice& vulkanGraphicsDevice);

    // Vulkan descriptor set representation.
    const VkDescriptorSet DescriptorSet;

    // Sets the fences.
    void PrepareBindDescriptorSetGroup(VulkanDeviceContext& vulkanDeviceContext);

    // Checks if this descriptor set group is available for use
    Bool IsAvailable() const noexcept;

    // Checks whether if the descriptor should be updated for the next use
    Bool UpToDate = false;

private:
    System::SharedPointer<IFence> _descriptorFinished      = nullptr;
    Uint64                        _expectedValue           = 0;
    ResourceHeapDescription       _resourceHeapDescription = {};
};

// Descriptor pool recycling the descriptor set group.
class VulkanDescriptorPool final : public DeviceChild
{
public:
    // Constructor
    VulkanDescriptorPool(const ResourceHeapDescription& description,
                         VulkanGraphicsDevice&          vulkanGraphicsDevice);

    // Gets the existing descriptor group in the pool / allocates new descriptor set group.
    System::UniquePointer<VulkanDescriptorSetGroup> GetDescriptorSetGroup();

    // Returns the descriptor set group back to the pool for future uses.
    void ReturnDescriptorSetGroup(System::UniquePointer<VulkanDescriptorSetGroup>&& descriptorSetGroup) noexcept;

    // Makes all pooled descriptors marked as not up to date.
    void MarkAllAsNotUpToDate() noexcept;

private:
    inline Size GetCurrentGroupSize() const noexcept { return InitialDescriptorSetPoolSize + _descriptorPools.GetSize() - 1; }
    void        AddPool();

    System::List<VkPtr<VkDescriptorPool>>                         _descriptorPools     = {};
    System::List<System::UniquePointer<VulkanDescriptorSetGroup>> _descriptorSetGroups = {};
    std::atomic<Size>                                             _currentAllocation   = 0;
    ResourceHeapDescription                                       _resourceHeapDesc    = {};
    std::mutex                                                    _vectorMutex         = {};

    static constexpr Size InitialDescriptorSetPoolSize = 3;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANDESCRIPTORPOOL_HPP
