/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#ifndef AXIS_VULKANPHYSICALDEVICE_HPP
#define AXIS_VULKANPHYSICALDEVICE_HPP
#pragma once

#include "../../../Include/Axis/GraphicsSystem.hpp"
#include <vulkan/vulkan.h>

namespace Axis
{

/// \brief The wrapper over the VkPhysicalDevice handle.
///
struct VulkanPhysicalDevice final
{
public:
    /// Default constructor
    ///
    VulkanPhysicalDevice() noexcept;

    /// Constructor
    ///
    VulkanPhysicalDevice(VkPhysicalDevice physicalDevice) noexcept;

    /// Destructor
    ///
    ~VulkanPhysicalDevice() noexcept;

    /// \brief Gets the swap chain specification from the window surface.
    ///
    SwapChainSpecification GetSwapChainSpecification(VkSurfaceKHR surface) const noexcept;

    /// \brief Gets the internal VkPhysicalDevice handle.
    ///
    inline VkPhysicalDevice GetVkPhysicalDeviceHandle() const noexcept { return _physicalDevice; }

    /// \brief Gets the VkPhysicalDeviceProperties of this physical device.
    ///
    inline const VkPhysicalDeviceProperties& GetVkPhysicalDeviceProperties() const noexcept { return _physicalDeviceProperties; }

    /// \brief Gets the VkPhysicalDeviceFeatures of this physical device.
    ///
    inline const VkPhysicalDeviceFeatures& GetVkPhysicalDeviceFeatures() const noexcept { return _physicalDeviceFeatures; }

    /// \brief Gets the GraphicsAdapter representation of this physical device.
    ///
    inline const GraphicsAdapter& GetGraphicsAdapterRepresentation() const noexcept { return _graphicsAdapterRepresentation; }

private:
    /// Private members
    ///
    VkPhysicalDevice           _physicalDevice                = {}; // Internal VkPhysicalDevice handle
    VkPhysicalDeviceProperties _physicalDeviceProperties      = {}; // Cached VkPhysicalDeviceProperties
    VkPhysicalDeviceFeatures   _physicalDeviceFeatures        = {}; // Cached VkPhysicalDeviceFeatures
    GraphicsAdapter            _graphicsAdapterRepresentation = {}; // GraphicsAdapter representation
};

} // namespace Axis

#endif // AXIS_VULKANPHYSICALDEVICE_HPP