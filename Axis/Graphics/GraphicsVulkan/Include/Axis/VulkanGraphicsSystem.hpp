/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

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
    AXIS_NODISCARD AXIS_GRAPHICSVULKAN_API Axis::Graphics::IGraphicsSystem* AxisCreateVulkanGraphicsSystem();
}

namespace Axis
{

namespace Graphics
{

// An implementation of IGraphicsSystem interface in Vulkan back end.
class VulkanGraphicsSystem final : public IGraphicsSystem
{
public:
    // Creates an instance of VulkanGraphicsSystem
    VulkanGraphicsSystem();

    // Gets all supported instance layers.
    static const System::List<VkLayerProperties>& GetInstanceLayerProperties();

    // Gets all supported instance extensions.
    static const System::List<VkExtensionProperties>& GetInstanceExtensionProperties();

    // An implementation of IGraphicsSystem::GetGraphicsSystemDescription in Vulkan backend.
    GraphicsSystemDescription GetGraphicsSystemDescription() const override final;

    // An implementation of IGraphicsSystem::GetGraphicsAdapters in Vulkan backend.
    System::List<GraphicsAdapter> GetGraphicsAdapters() const override final;

    // An implementation of IGraphicsSystem::CreateGraphicsDeviceAndContexts in Vulkan backend.
    System::Pair<System::SharedPointer<IGraphicsDevice>, System::List<System::SharedPointer<IDeviceContext>>> CreateGraphicsDeviceAndContexts(Uint32                                          adapterIndex,
                                                                                                                                              const System::Span<ImmediateContextCreateInfo>& pImmediateContextCreateInfos) override final;

    // An implementation of IGraphicsSystem::GetSwapChainSpecification in Vulkan backend.
    SwapChainSpecification GetSwapChainSpecification(Uint32                                              adapterIndex,
                                                     const System::SharedPointer<Window::DisplayWindow>& targetWindow) const override final;

    /// Gets VkSurfaceKHR handle from the window.
    ///
    /// All created VkSurfaceKHR will be destroyed automatically after this VulkanGraphicsSystem is destroy
    VkSurfaceKHR GetVkSurfaceKHR(const System::SharedPointer<Window::DisplayWindow>& window) const;

    // Gets the internal VkInstance handle.
    inline VkInstance GetVkInstanceHandle() const noexcept { return _instance; }

#ifdef AXIS_DEBUG
    // Gets the internal VkDebugUtilsMessengerEXT handle.
    inline VkDebugUtilsMessengerEXT GetVkDebugUtilsMessengerEXTHandle() const noexcept { return _debugUtilsMessengerEXT; }
#endif

    // Gets all vulkan phyiscal devices.
    inline const System::List<VulkanPhysicalDevice>& GetVulkanPhysicalDevices() const noexcept { return _vulkanPhysicalDevices; }

    // The name of the application used in VkApplicationInfo
    static constexpr const char* VulkanApplicationName = "AxisApplication";

    // The name of the engine used in VkApplicationInfo
    static constexpr const char* VulkanEngineName = "AxisEngine";

    // Contains all required instance extensions.
    static const System::List<const char*> InstancecExtensions;

    // Contains all required instance layers.
    static const System::List<const char*> InstanceLayers;

private:
    // Private members
    VkPtr<VkInstance> _instance = {};
#ifdef AXIS_DEBUG
    VkPtr<VkDebugUtilsMessengerEXT> _debugUtilsMessengerEXT = {};
#endif
    mutable std::mutex                                                   _mutex                 = {};
    mutable System::HashMap<Window::DisplayWindow*, VkPtr<VkSurfaceKHR>> _windowSurfacePairs    = {};
    System::List<VulkanPhysicalDevice>                                   _vulkanPhysicalDevices = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANGRAPHICSSYSTEM_HPP
