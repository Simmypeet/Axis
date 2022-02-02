/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#ifndef AXIS_VULKANGRAPHICSSYSTEM_HPP
#define AXIS_VULKANGRAPHICSSYSTEM_HPP
#pragma once

#include "../../../../System/Include/Axis/HashMap.hpp"
#include "../../../Include/Axis/GraphicsSystem.hpp"
#include "GraphicsVulkanExport.hpp"
#include "VkPtr.hpp"
#include "VulkanPhysicalDevice.hpp"

extern "C"
{
    /// \brief Creates graphics system in Vulkan backend
    ///
    AXIS_NODISCARD AXIS_GRAPHICSVULKAN_API Axis::IGraphicsSystem* AxisCreateVulkanGraphicsSystem();
}

namespace Axis
{

/// \brief An implementation of IGraphicsSystem interface in Vulkan back end.
class VulkanGraphicsSystem final : public IGraphicsSystem
{
public:
    /// \brief Creates an instance of VulkanGraphicsSystem
    VulkanGraphicsSystem();

    /// \brief Gets all supported instance layers.
    static const List<VkLayerProperties>& GetInstanceLayerProperties();

    /// \brief Gets all supported instance extensions.
    static const List<VkExtensionProperties>& GetInstanceExtensionProperties();

    /// \brief An implementation of IGraphicsSystem::GetGraphicsSystemDescription in Vulkan backend.
    GraphicsSystemDescription GetGraphicsSystemDescription() const override final;

    /// \brief An implementation of IGraphicsSystem::GetGraphicsAdapters in Vulkan backend.
    List<GraphicsAdapter> GetGraphicsAdapters() const override final;

    /// \brief An implementation of IGraphicsSystem::CreateGraphicsDeviceAndContexts in Vulkan backend.
    Pair<SharedPointer<IGraphicsDevice>, List<SharedPointer<IDeviceContext>>> CreateGraphicsDeviceAndContexts(Uint32                                  adapterIndex,
                                                                                                              const Span<ImmediateContextCreateInfo>& pImmediateContextCreateInfos) override final;

    /// \brief An implementation of IGraphicsSystem::GetSwapChainSpecification in Vulkan backend.
    SwapChainSpecification GetSwapChainSpecification(Uint32                              adapterIndex,
                                                     const SharedPointer<DisplayWindow>& targetWindow) const override final;

    /// \brief Gets VkSurfaceKHR handle from the window.
    ///
    /// \note All created VkSurfaceKHR will be destroyed automatically after this VulkanGraphicsSystem is destroy
    VkSurfaceKHR GetVkSurfaceKHR(const SharedPointer<DisplayWindow>& window) const;

    /// \brief Gets the internal VkInstance handle.
    inline VkInstance GetVkInstanceHandle() const noexcept { return _instance; }

#ifdef AXIS_DEBUG
    /// \brief Gets the internal VkDebugUtilsMessengerEXT handle.
    inline VkDebugUtilsMessengerEXT GetVkDebugUtilsMessengerEXTHandle() const noexcept { return _debugUtilsMessengerEXT; }
#endif

    /// \brief Gets all vulkan phyiscal devices.
    inline const List<VulkanPhysicalDevice>& GetVulkanPhysicalDevices() const noexcept { return _vulkanPhysicalDevices; }

    /// \brief The name of the application used in VkApplicationInfo
    static constexpr const char* VulkanApplicationName = "AxisApplication";

    /// \brief The name of the engine used in VkApplicationInfo
    static constexpr const char* VulkanEngineName = "AxisEngine";

    /// \brief Contains all required instance extensions.
    static const List<const char*> InstancecExtensions;

    /// \brief Contains all required instance layers.
    static const List<const char*> InstanceLayers;

private:
    /// Private members
    VkPtr<VkInstance> _instance = {};
#ifdef AXIS_DEBUG
    VkPtr<VkDebugUtilsMessengerEXT> _debugUtilsMessengerEXT = {};
#endif
    mutable std::mutex                                   _mutex                 = {};
    mutable HashMap<DisplayWindow*, VkPtr<VkSurfaceKHR>> _windowSurfacePairs    = {};
    List<VulkanPhysicalDevice>                           _vulkanPhysicalDevices = {};
};

} // namespace Axis

#endif // AXIS_VULKANGRAPHICSSYSTEM_HPP