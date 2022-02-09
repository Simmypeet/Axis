/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_FRAMEBUFFER_HPP
#define AXIS_GRAPHICS_FRAMEBUFFER_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "../../../System/Include/Axis/Vector3.hpp"
#include "DeviceChild.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IRenderPass;
class ITextureView;

/// \brief Description IFramebuffer resource.
struct FramebufferDescription final
{
    /// \brief The size of the FrameBuffer's attachments. X and Y preserved for FrameBuffer's width and height, Z preserved for layer.
    System::Vector3UI FramebufferSize = {};

    /// \brief The RenderPass which is compatible with the Framebuffer.
    System::SharedPointer<IRenderPass> RenderPass = {};

    /// \brief All the attachments associated with the FrameBuffer.
    System::List<System::SharedPointer<ITextureView>> Attachments = {};
};

/// \brief Represents the collections of TextureViews (attachments) that a RenderPass instances will use.
class AXIS_GRAPHICS_API IFramebuffer : public DeviceChild
{
public:
    /// \brief The description of IFramebuffer resource.
    const FramebufferDescription Description;

protected:
    /// \brief Constructor
    IFramebuffer(const FramebufferDescription& Description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_FRAMEBUFFER_HPP