/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/DeviceChild.hpp>
#include <Axis/GraphicsDevice.hpp>

namespace Axis
{

// Constructor
IGraphicsDevice::IGraphicsDevice(const SharedPointer<IGraphicsSystem>& graphicsSystem,
                                 Uint32                                graphicsAdapterIndex) :
    GraphicsSystem(graphicsSystem),
    GraphicsAdapterIndex(graphicsAdapterIndex) {}


void IGraphicsDevice::AddDeviceChild(DeviceChild& deviceChild) noexcept
{
    deviceChild._graphicsDevice = ISharedFromThis::CreateSharedPointerFromThis(*this);
}

} // namespace Axis