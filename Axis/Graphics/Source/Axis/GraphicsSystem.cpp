/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/SwapChain.hpp>

namespace Axis
{

// Default constructor
IGraphicsSystem::IGraphicsSystem() noexcept = default;

void IGraphicsSystem::ValidateCreateGraphicsDeviceAndContexts(Uint32                                  adapterIndex,
                                                              const Span<ImmediateContextCreateInfo>& immediateContextCreateInfos)
{
    auto graphicsAdapters = GetGraphicsAdapters();

    if (adapterIndex >= graphicsAdapters.GetLength())
        throw ArgumentOutOfRangeException("`adapterIndex` was out of range!");

    if (!immediateContextCreateInfos)
        throw InvalidArgumentException("`immediateContextCreateInfos` was nullptr!");

    List<DeviceQueueFamily> currentDeviceQueueFamilies = graphicsAdapters[adapterIndex].DeviceQueueFamilies;

    for (Uint64 i = 0; i < immediateContextCreateInfos.GetLength(); i++)
    {
        if (immediateContextCreateInfos[i].DeviceQueueFamilyIndex >= currentDeviceQueueFamilies.GetLength())
            throw InvalidArgumentException("`immediateContextCreateInfos` contained out of range device queue family indices!");

        if (!currentDeviceQueueFamilies[immediateContextCreateInfos[i].DeviceQueueFamilyIndex].QueueCount)
            throw InvalidArgumentException("`immediateContextCreateInfos` requested more device queue than provided!");

        currentDeviceQueueFamilies[immediateContextCreateInfos[i].DeviceQueueFamilyIndex].QueueCount--;
    }
}

} // namespace Axis