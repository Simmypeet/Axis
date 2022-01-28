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
IGraphicsSystem::IGraphicsSystem(const GraphicsSystemDescription& graphicsSystemDescription,
                                 const List<GraphicsAdapter>&     graphicsAdapters) :
    Description(graphicsSystemDescription),
    GraphicsAdapters(graphicsAdapters) {}

// Create graphics system function pointer type.
typedef void (*CreateGraphicsSystemFunctionPointerType)(IGraphicsSystem**);

} // namespace Axis