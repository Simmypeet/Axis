/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/ResourceHeapLayout.hpp>

namespace Axis
{

IResourceHeapLayout::IResourceHeapLayout(const ResourceHeapLayoutDescription& description) :
    Description(description) {}

} // namespace Axis