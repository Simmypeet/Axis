/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/StatedGraphicsResource.hpp>

namespace Axis
{

// Default constructor
StatedGraphicsResource::StatedGraphicsResource() noexcept = default;

void StatedGraphicsResource::SetResourceState(ResourceState state) noexcept
{
    _resourceState = state;
}

} // namespace Axis
