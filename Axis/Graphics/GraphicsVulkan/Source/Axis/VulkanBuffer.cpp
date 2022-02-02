/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Utility.hpp>
#include <Axis/VulkanBuffer.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

inline Tuple<List<Uint32>, VkBufferCreateInfo> GetBufferCreationInfosFromBufferDescription(const BufferDescription& description)
{
    VkBufferUsageFlags bufferUsage = {};

    // Appropriately set the usage flag
    if (Enum::GetUnderlyingValue(description.BufferBinding & BufferBinding::Index))
        bufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    if (Enum::GetUnderlyingValue(description.BufferBinding & BufferBinding::Vertex))
        bufferUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    if (Enum::GetUnderlyingValue(description.BufferBinding & BufferBinding::Uniform))
        bufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if (Enum::GetUnderlyingValue(description.BufferBinding & BufferBinding::TransferDestination))
        bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (Enum::GetUnderlyingValue(description.BufferBinding & BufferBinding::TransferSource))
        bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    auto indices = VulkanUtility::ExtractDeviceQueueFamilyIndices(description.DeviceQueueFamilyMask);

    bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // Vulkan buffer
    VkBufferCreateInfo bufferCreateInfo    = {};
    bufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext                 = nullptr;
    bufferCreateInfo.size                  = description.BufferSize;
    bufferCreateInfo.usage                 = bufferUsage;
    bufferCreateInfo.sharingMode           = indices ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = (Uint32)indices.GetLength();
    bufferCreateInfo.pQueueFamilyIndices   = indices.GetData();

    return {std::move(indices), bufferCreateInfo};
}

inline BufferDescription AppendTransferDestination(const BufferDescription& description, Bool append)
{
    BufferDescription copiedDescription = description;
    if (append)
        copiedDescription.BufferBinding |= BufferBinding::TransferDestination;
    return copiedDescription;
}

VulkanBuffer::VulkanBuffer(const BufferDescription& description,
                           const BufferInitialData* pInitialData,
                           VulkanGraphicsDevice&    vulkanGraphicsDevice) :
    IBuffer(AppendTransferDestination(description, (pInitialData != nullptr && description.Usage == ResourceUsage::Immutable) ? true : false))
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    auto CreateVulkanBufferAllocation = [&]() -> VulkanBufferAllocation {
        VulkanBufferAllocation vulkanBufferAllocation  = {};
        auto                   vkBufferCreateInfo      = GetBufferCreationInfosFromBufferDescription(Description);
        auto                   vmaAllocationCreateInfo = VulkanUtility::GetVmaAllocationCreateInfoFromResourceUsage(Description.Usage);

        auto vkResult = vmaCreateBuffer(vulkanGraphicsDevice.GetVmaAllocatorHandle(),
                                        &Axis::GetTuple<1>(vkBufferCreateInfo),
                                        &vmaAllocationCreateInfo,
                                        &vulkanBufferAllocation.VulkanBuffer,
                                        &vulkanBufferAllocation.VulkanMemoryAllocation,
                                        &_allocationInfo);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to create VkBuffer!");

        return vulkanBufferAllocation;
    };

    auto DestroyVulkanBufferAllocation = [this](VulkanBufferAllocation vulkanBufferAllocation) {
        vmaDestroyBuffer(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(), vulkanBufferAllocation.VulkanBuffer, vulkanBufferAllocation.VulkanMemoryAllocation);
    };

    auto bufferCreateInfos = GetBufferCreationInfosFromBufferDescription(Description);

    _vulkanBuffer = VkPtr<VulkanBufferAllocation>(CreateVulkanBufferAllocation, std::move(DestroyVulkanBufferAllocation));
}

} // namespace Axis