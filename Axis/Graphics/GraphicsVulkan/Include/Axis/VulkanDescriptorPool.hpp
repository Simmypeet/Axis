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

/// Forward declarations
class IFence;
class VulkanGraphicsDevice;
class VulkanDescriptorPool;
class VulkanDeviceContext;
class VulkanSampler;
class VulkanBuffer;
class VulkanTextureView;
enum class StateTransition : Uint8;

/// \brief Specifies the group of required descriptor sets requested from resource heap.
class VulkanDescriptorSetGroup final : public DeviceChild
{
public:
    /// Constructor
    VulkanDescriptorSetGroup(VkDescriptorSet       descriptorSet,
                             VulkanGraphicsDevice& vulkanGraphicsDevice);

    /// \brief Vulkan descriptor set representation.
    const VkDescriptorSet DescriptorSet;

    /// \brief Sets the fences.
    void PrepareBindDescriptorSetGroup(VulkanDeviceContext& vulkanDeviceContext);

    /// \brief Checks if this descriptor set group is available for use
    Bool IsAvailable() const noexcept;

    // Checks whether if the descriptor should be updated for the next use
    Bool UpToDate = false;

private:
    SharedPointer<IFence>   _descriptorFinished      = nullptr;
    Uint64                  _expectedValue           = 0;
    ResourceHeapDescription _resourceHeapDescription = {};
};

/// \brief Descriptor pool recycling the descriptor set group.
class VulkanDescriptorPool final : public DeviceChild
{
public:
    /// Constructor
    VulkanDescriptorPool(const ResourceHeapDescription& description,
                         VulkanGraphicsDevice&          vulkanGraphicsDevice);

    /// \brief Gets the existing descriptor group in the pool / allocates new descriptor set group.
    ///
    /// \return Returns nullptr if the error occurred.
    UniquePointer<VulkanDescriptorSetGroup> GetDescriptorSetGroup();

    /// \brief Returns the descriptor set group back to the pool for future uses.
    void ReturnDescriptorSetGroup(UniquePointer<VulkanDescriptorSetGroup>&& descriptorSetGroup) noexcept;

    /// \brief Makes all pooled descriptors marked as not up to date.
    void MarkAllAsNotUpToDate() noexcept;

private:
    /// Private methods
    inline Size GetCurrentGroupSize() const noexcept { return InitialDescriptorSetPoolSize + _descriptorPools.GetLength() - 1; }
    void        AddPool();

    List<VkPtr<VkDescriptorPool>>                 _descriptorPools     = {};
    List<UniquePointer<VulkanDescriptorSetGroup>> _descriptorSetGroups = {};
    std::atomic<Size>                             _currentAllocation   = 0;
    ResourceHeapDescription                       _resourceHeapDesc    = {};
    std::mutex                                    _vectorMutex         = {};

    static constexpr Size InitialDescriptorSetPoolSize = 3;
};

} // namespace Axis

#endif // AXIS_VULKANDESCRIPTORPOOL_HPP
