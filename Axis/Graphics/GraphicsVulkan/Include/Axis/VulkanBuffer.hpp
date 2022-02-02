/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANBUFFER_HPP
#define AXIS_VULKANBUFFER_HPP
#pragma once

#include "../../../../System/Include/Axis/Event.hpp"
#include "../../../Include/Axis/Buffer.hpp"
#include "VkPtr.hpp"
#include <VulkanMemoryAllocator/vk_mem_alloc.hpp>

namespace Axis
{

/// Forward declarations
class VulkanGraphicsDevice;
struct BufferInitialData;

/// \brief Implementation of IBuffer interface class in Vulkan platform.
class VulkanBuffer final : public IBuffer
{
public:
    // Forward declaration
    class Internal;

    // Constructor
    VulkanBuffer(const BufferDescription& description,
                 const BufferInitialData* pInitialData,
                 VulkanGraphicsDevice&    graphicsDevice);

    // Gets VkBuffer handle
    inline VkBuffer GetVkBufferHandle() const noexcept { return ((VulkanBufferAllocation)_vulkanBuffer).VulkanBuffer; }

    // Gets VmaAllocation handle
    inline VmaAllocation GetVmaAllocation() const noexcept { return ((VulkanBufferAllocation)_vulkanBuffer).VulkanMemoryAllocation; }

    // Gets VmaAllocationInfo
    inline const VmaAllocationInfo& GetVmaAllocationInfo() const noexcept { return _allocationInfo; }

    // Indicates whether the buffer is being mapped or not.
    Bool BufferMapped = false;

    // Lastest mapped access
    MapAccess MappedAccess = {};

    // Lastest mapped type
    MapType MappedType = {};

    // Staging buffer used for various purposes.
    SharedPointer<VulkanBuffer> StagingBuffer = nullptr;

private:
    struct VulkanBufferAllocation
    {
        VkBuffer      VulkanBuffer           = VK_NULL_HANDLE;
        VmaAllocation VulkanMemoryAllocation = VK_NULL_HANDLE;

        inline constexpr Bool operator==(const VulkanBufferAllocation& other) const noexcept { return VulkanBuffer == other.VulkanBuffer && VulkanMemoryAllocation == other.VulkanMemoryAllocation; }
        inline constexpr Bool operator!=(const VulkanBufferAllocation& other) const noexcept { return VulkanBuffer != other.VulkanBuffer || VulkanMemoryAllocation != other.VulkanMemoryAllocation; }
    };

    VkPtr<VulkanBufferAllocation> _vulkanBuffer   = {};
    VmaAllocationInfo             _allocationInfo = {};
};

} // namespace Axis

#endif // AXIS_VULKANBUFFER_HPP
