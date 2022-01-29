/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/Framebuffer.hpp>

namespace Axis
{

// Default constructor
IFramebuffer::IFramebuffer(const FramebufferDescription& description) :
    Description(description) {}

} // namespace Axis