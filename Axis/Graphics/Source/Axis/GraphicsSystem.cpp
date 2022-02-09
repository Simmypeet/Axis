/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/SwapChain.hpp>

namespace Axis
{

namespace Graphics
{

// Default constructor
IGraphicsSystem::IGraphicsSystem() noexcept = default;

void IGraphicsSystem::ValidateCreateGraphicsDeviceAndContexts(Uint32                                          adapterIndex,
                                                              const System::Span<ImmediateContextCreateInfo>& immediateContextCreateInfos)
{
    auto graphicsAdapters = GetGraphicsAdapters();

    if (adapterIndex >= graphicsAdapters.GetLength())
        throw System::ArgumentOutOfRangeException("`adapterIndex` was out of range!");

    if (!immediateContextCreateInfos)
        throw System::InvalidArgumentException("`immediateContextCreateInfos` was nullptr!");

    System::List<DeviceQueueFamily> currentDeviceQueueFamilies = graphicsAdapters[adapterIndex].DeviceQueueFamilies;

    for (Uint64 i = 0; i < immediateContextCreateInfos.GetLength(); i++)
    {
        if (immediateContextCreateInfos[i].DeviceQueueFamilyIndex >= currentDeviceQueueFamilies.GetLength())
            throw System::InvalidArgumentException("`immediateContextCreateInfos` contained out of range device queue family indices!");

        if (!currentDeviceQueueFamilies[immediateContextCreateInfos[i].DeviceQueueFamilyIndex].QueueCount)
            throw System::InvalidArgumentException("`immediateContextCreateInfos` requested more device queue than provided!");

        currentDeviceQueueFamilies[immediateContextCreateInfos[i].DeviceQueueFamilyIndex].QueueCount--;
    }
}

} // namespace Graphics

} // namespace Axis