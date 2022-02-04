/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/DisplayWindow.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/GraphicsVulkanPch.hpp>
#include <Axis/Utility.hpp>
#include <Axis/VkPtr.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanDeviceQueueFamily.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsSystem.hpp>
#include <Axis/VulkanPhysicalDevice.hpp>
#include <iostream>
#include <vulkan/vulkan_core.h>


#ifdef AXIS_PLATFORM_WINDOWS
#    include <Windows.h>
#    include <vulkan/vulkan_win32.h>
#endif

#ifdef AXIS_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL AxisDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void*                                       pUserData)
{
    if (pCallbackData->messageIdNumber == -400166253)
        return VK_FALSE;

    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            std::cout << "ValidationLayer: " << pCallbackData->pMessage;
            break;

        default:
            std::cout << "ValidationLayer: " << pCallbackData->pMessage;
    }

    return VK_FALSE;
}
#endif

extern "C"
{
    Axis::IGraphicsSystem* AxisCreateVulkanGraphicsSystem()
    {
        try
        {
            return Axis::New<Axis::VulkanGraphicsSystem>();
        }
        catch (...)
        {
            // Any caught exception in here is ignored, returns nullptr
            return nullptr;
        }
    }
}

namespace Axis
{

const List<const char*> VulkanGraphicsSystem::InstancecExtensions = {
#ifdef AXIS_DEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef AXIS_PLATFORM_WINDOWS
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
};

const List<const char*> VulkanGraphicsSystem::InstanceLayers = {
#ifdef AXIS_DEBUG
    "VK_LAYER_KHRONOS_validation"
#endif
};

VulkanGraphicsSystem::VulkanGraphicsSystem()
{
    // Checks if required instance extensions are supported!
    for (const auto& requiredLayer : VulkanGraphicsSystem::InstancecExtensions)
    {
        Bool found = false;

        for (const auto& extension : VulkanGraphicsSystem::GetInstanceExtensionProperties())
        {
            if (std::strcmp(requiredLayer, extension.extensionName))
            {
                found = true;
                break;
            }
        }

        // The hardware doesn't have the engine's required instance extensions.
        if (!found)
            throw ExternalException("The hardware didn't support this graphics system!");
    }

    // Checks if required instance layers are supported!
    for (const auto& requiredLayer : VulkanGraphicsSystem::InstanceLayers)
    {
        Bool found = false;

        for (const auto& extension : VulkanGraphicsSystem::GetInstanceLayerProperties())
        {
            if (std::strcmp(requiredLayer, extension.layerName))
            {
                found = true;
                break;
            }
        }

        // The hardware doesn't have the engine's required instance layers.
        if (!found)
            throw ExternalException("The hardware didn't support this graphics system!");
    }

    auto CreateVkInstance = []() -> VkInstance {
        // instance.
        VkInstance instance = VK_NULL_HANDLE;

        // App info
        VkApplicationInfo appInfo  = {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = Axis::VulkanGraphicsSystem::VulkanApplicationName;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = Axis::VulkanGraphicsSystem::VulkanEngineName;
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_2;

        // Instance create info
        VkInstanceCreateInfo instanceCreateInfo    = {};
        instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo        = &appInfo;
        instanceCreateInfo.enabledLayerCount       = (Axis::Uint32)Axis::VulkanGraphicsSystem::InstanceLayers.GetLength();
        instanceCreateInfo.ppEnabledLayerNames     = Axis::VulkanGraphicsSystem::InstanceLayers.GetData();
        instanceCreateInfo.enabledExtensionCount   = (Axis::Uint32)Axis::VulkanGraphicsSystem::InstancecExtensions.GetLength();
        instanceCreateInfo.ppEnabledExtensionNames = Axis::VulkanGraphicsSystem::InstancecExtensions.GetData();

        auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

        if (result != VK_SUCCESS)
            return VK_NULL_HANDLE;
        else
            return instance;
    };

    auto DeleteVkInstance = [](VkInstance vkInstance) -> void {
        vkDestroyInstance(vkInstance, nullptr);
    };

    // Creates vkInstance
    _instance = VkPtr<VkInstance>(CreateVkInstance, std::move(DeleteVkInstance));

#ifdef AXIS_DEBUG
    auto CreateVkDebugUtilsMessengerEXT = [this]() -> VkDebugUtilsMessengerEXT {
        // debug utils messenger.
        VkDebugUtilsMessengerEXT debugUtilsMessengerEXT = VK_NULL_HANDLE;

        // Creates debug utils messenger in debug mode.
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT = {};
        debugUtilsMessengerCreateInfoEXT.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsMessengerCreateInfoEXT.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCreateInfoEXT.messageType                        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsMessengerCreateInfoEXT.pfnUserCallback                    = &AxisDebugCallback;

        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

        if (!vkCreateDebugUtilsMessengerEXT)
            return VK_NULL_HANDLE;

        auto result = vkCreateDebugUtilsMessengerEXT(_instance,
                                                     &debugUtilsMessengerCreateInfoEXT,
                                                     nullptr,
                                                     &debugUtilsMessengerEXT);

        if (result != VK_SUCCESS)
            return VK_NULL_HANDLE;
        else
            return debugUtilsMessengerEXT;
    };

    auto DestroyVkDebugUtilsMessengerEXT = [this](VkDebugUtilsMessengerEXT debugUtilsMessengerEXT) {
        auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

        if (vkDestroyDebugUtilsMessengerEXT)
            vkDestroyDebugUtilsMessengerEXT(_instance, _debugUtilsMessengerEXT, nullptr);
    };

    _debugUtilsMessengerEXT = VkPtr<VkDebugUtilsMessengerEXT>(CreateVkDebugUtilsMessengerEXT, std::move(DestroyVkDebugUtilsMessengerEXT));
#endif

    Uint32 physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, nullptr);

    // Doesn't have any physical devices which support Vulkan
    if (physicalDeviceCount == 0)
        throw ExternalException("The hardware didn't support this graphics system!");

    // Retrieves VkPhysicalDevice handles.
    List<VkPhysicalDevice> vkPhyiscalDevices;
    vkPhyiscalDevices.Resize(physicalDeviceCount);

    vkEnumeratePhysicalDevices(_instance, &physicalDeviceCount, vkPhyiscalDevices.GetData());

    // Creates Axis's vulkan physical device wrapper
    _vulkanPhysicalDevices.ReserveFor(physicalDeviceCount);

    for (const auto& vkPhyiscalDevice : vkPhyiscalDevices)
        _vulkanPhysicalDevices.EmplaceBack(vkPhyiscalDevice);
}

const List<VkLayerProperties>& VulkanGraphicsSystem::GetInstanceLayerProperties()
{
    static List<VkLayerProperties> s_InstanceLayers = nullptr;

    if (!s_InstanceLayers)
    {
        Uint32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        s_InstanceLayers = List<VkLayerProperties>(layerCount);

        vkEnumerateInstanceLayerProperties(&layerCount, s_InstanceLayers.GetData());
    }

    return s_InstanceLayers;
}

const List<VkExtensionProperties>& VulkanGraphicsSystem::GetInstanceExtensionProperties()
{
    static List<VkExtensionProperties> s_InstanceExtensions = nullptr;

    if (!s_InstanceExtensions)
    {
        Uint32 extensionCount;

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        s_InstanceExtensions = List<VkExtensionProperties>(extensionCount);

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, s_InstanceExtensions.GetData());
    }

    return s_InstanceExtensions;
}

GraphicsSystemDescription VulkanGraphicsSystem::GetGraphicsSystemDescription() const
{
    return {
        .EngineGraphicsAPI = GraphicsAPI::Vulkan};
}

List<GraphicsAdapter> VulkanGraphicsSystem::GetGraphicsAdapters() const
{
    List<GraphicsAdapter> graphicsAdapters = {};

    for (const auto& physicalDevice : _vulkanPhysicalDevices)
        graphicsAdapters.Append(physicalDevice.GetGraphicsAdapterRepresentation());

    return graphicsAdapters;
}

Pair<SharedPointer<IGraphicsDevice>, List<SharedPointer<IDeviceContext>>> VulkanGraphicsSystem::CreateGraphicsDeviceAndContexts(Uint32                                  adapterIndex,
                                                                                                                                const Span<ImmediateContextCreateInfo>& pImmediateContextCreateInfos)
{
    IGraphicsSystem::ValidateCreateGraphicsDeviceAndContexts(adapterIndex,
                                                             pImmediateContextCreateInfos);

    auto vulkanGraphicsSystem = ISharedFromThis::CreateSharedPointerFromThis(*this);

    if (!vulkanGraphicsSystem)
        throw InvalidOperationException("This graphics system hasn't been assigned to any reference counted system!");

    auto graphicsDevice = Axis::MakeShared<VulkanGraphicsDevice>(vulkanGraphicsSystem,
                                                                 adapterIndex,
                                                                 pImmediateContextCreateInfos);

    return {graphicsDevice, graphicsDevice->GetDeviceContexts()};
}

SwapChainSpecification VulkanGraphicsSystem::GetSwapChainSpecification(Uint32                              adapterIndex,
                                                                       const SharedPointer<DisplayWindow>& targetWindow) const
{
    if (adapterIndex >= _vulkanPhysicalDevices.GetLength())
        throw ArgumentOutOfRangeException("`adapterIndex` was out of range!");

    return _vulkanPhysicalDevices[adapterIndex].GetSwapChainSpecification(GetVkSurfaceKHR(targetWindow));
}


VkSurfaceKHR VulkanGraphicsSystem::GetVkSurfaceKHR(const SharedPointer<DisplayWindow>& window) const
{
    auto windowSurfacePair = _windowSurfacePairs.Find(window.GetPointer());

    if (windowSurfacePair == _windowSurfacePairs.end())
    {
        {
            std::scoped_lock lockGuard(_mutex);

#ifdef AXIS_PLATFORM_WINDOWS
            auto CreateVkSurfaceKHR = [&]() -> VkSurfaceKHR {
                VkSurfaceKHR surfaceKhr = VK_NULL_HANDLE;

                VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
                win32SurfaceCreateInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                win32SurfaceCreateInfo.pNext                       = nullptr;
                win32SurfaceCreateInfo.flags                       = 0;
                win32SurfaceCreateInfo.hinstance                   = GetModuleHandle(NULL);
                win32SurfaceCreateInfo.hwnd                        = (HWND)window->GetWindowHandle();


                if (vkCreateWin32SurfaceKHR(_instance, &win32SurfaceCreateInfo, nullptr, &surfaceKhr) != VK_SUCCESS)
                    throw ExternalException("Failed to create VkSurfaceKHR!");
                else
                    return surfaceKhr;
            };

            auto DestroyVkSurfaceKHR = [this](VkSurfaceKHR surface) {
                vkDestroySurfaceKHR(_instance, surface, nullptr);
            };
#endif

            VkPtr<VkSurfaceKHR> vkSurfaceKHR(CreateVkSurfaceKHR, std::move(DestroyVkSurfaceKHR));

            auto it = _windowSurfacePairs.Insert({window.GetPointer(), std::move(vkSurfaceKHR)});

            return it.Second->Second;
        }
    }

    return windowSurfacePair->Second;
}

} // namespace Axis
