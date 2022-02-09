/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanPhysicalDevice.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

namespace Graphics
{

// Default constructor
VulkanPhysicalDevice::VulkanPhysicalDevice() noexcept {}

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice physicalDevice) noexcept :
    _physicalDevice(physicalDevice)
{
    AXIS_ASSERT(physicalDevice, "VkPhysicalDevice handle can't be nullptr!");

    vkGetPhysicalDeviceProperties(_physicalDevice, &_physicalDeviceProperties);

    vkGetPhysicalDeviceFeatures(_physicalDevice, &_physicalDeviceFeatures);

    // Constructs graphics capability from VkPhysicalDeviceLimit
    GraphicsCapability graphicsCapability      = {};
    graphicsCapability.MaxTexture2DSize        = _physicalDeviceProperties.limits.maxImageDimension2D;
    graphicsCapability.MaxVertexInputBinding   = _physicalDeviceProperties.limits.maxVertexInputBindings;
    graphicsCapability.MaxFramebufferDimension = {
        _physicalDeviceProperties.limits.maxFramebufferWidth,
        _physicalDeviceProperties.limits.maxFramebufferHeight,
        _physicalDeviceProperties.limits.maxFramebufferLayers};
    graphicsCapability.MaxPipelineLayoutBinding = _physicalDeviceProperties.limits.maxBoundDescriptorSets;

    Uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

    System::List<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.GetData());

    System::List<DeviceQueueFamily> deviceQueueFamilies(queueFamilyCount);

    Size index = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        deviceQueueFamilies[index].QueueCount = queueFamily.queueCount;
        deviceQueueFamilies[index].QueueType  = VulkanUtility::GetQueueOperationFlagsFromVkQueueFlags(queueFamily.queueFlags);

        index++;
    }

    _graphicsAdapterRepresentation.Capability          = graphicsCapability;
    _graphicsAdapterRepresentation.AdapterType         = VulkanUtility::GetGraphicsAdapterTypeFromVkPhysicalDeviceType(_physicalDeviceProperties.deviceType);
    _graphicsAdapterRepresentation.DeviceQueueFamilies = std::move(deviceQueueFamilies);
    _graphicsAdapterRepresentation.Name                = _physicalDeviceProperties.deviceName;
}

// Default destructor
VulkanPhysicalDevice::~VulkanPhysicalDevice() noexcept {}

SwapChainSpecification VulkanPhysicalDevice::GetSwapChainSpecification(VkSurfaceKHR surface) const noexcept
{
    SwapChainSpecification swapChainSpecification = {};

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, surface, &surfaceCapabilities);

    swapChainSpecification.MaxBackBufferCount = surfaceCapabilities.maxImageCount;
    swapChainSpecification.MinBackBufferCount = surfaceCapabilities.minImageCount;

    Uint32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, nullptr);

    System::List<VkSurfaceFormatKHR> surfaceFormats(formatCount);

    if (formatCount != 0)
    {
        swapChainSpecification.SupportedFormats = System::List<TextureFormat>(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, surfaceFormats.GetData());

        Size index = 0;
        for (const auto& format : surfaceFormats)
        {
            swapChainSpecification.SupportedFormats[index] = VulkanUtility::GetTextureFormatFromVkFormat(format.format);
            index++;
        }
    }

    return swapChainSpecification;
}

} // namespace Graphics

} // namespace Axis