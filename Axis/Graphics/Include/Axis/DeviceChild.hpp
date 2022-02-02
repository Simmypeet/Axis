/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

/// \file DeviceChild.hpp
///
/// \brief Contains `Axis::DeviceChild` class.
///

#ifndef AXIS_GRAPHICS_DEVICECHILD_HPP
#define AXIS_GRAPHICS_DEVICECHILD_HPP
#pragma once

#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

/// Forward declarations
class IGraphicsDevice;

/// \brief Represents the resource which was created by graphics device.
class AXIS_GRAPHICS_API DeviceChild : public ISharedFromThis
{
public:
    /// \brief Gets the IGraphicsDevice handle which created this resource.
    AXIS_NODISCARD inline const SharedPointer<IGraphicsDevice>& GetCreatorDevice() const noexcept { return _graphicsDevice; }

protected:
    /// \brief Default constructor
    DeviceChild() noexcept;

private:
    SharedPointer<IGraphicsDevice> _graphicsDevice = nullptr;

    /// Forward declarations
    friend class IGraphicsDevice;
};

} // namespace Axis

#endif // AXIS_GRAPHICS_DEVICECHILD_HPP