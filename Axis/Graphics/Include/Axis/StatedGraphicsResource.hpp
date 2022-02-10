/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_STATEDGRAPHICSRESOURCE_HPP
#define AXIS_GRAPHICS_STATEDGRAPHICSRESOURCE_HPP
#pragma once

#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Graphics resource which able to track down their current resource state.
class AXIS_GRAPHICS_API StatedGraphicsResource : public DeviceChild
{
public:
    /// \brief Gets the current state of the resource.
    inline ResourceState GetCurrentResourceState() const noexcept { return _resourceState; }

    /// \brief Sets the resource state
    void SetResourceState(ResourceState resourceState) noexcept;

protected:
    /// \brief Default constructor
    StatedGraphicsResource() noexcept;

private:
    /// Private members
    ResourceState _resourceState = ResourceState::Undefined;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_STATEDGRAPHICSRESOURCE_HPP