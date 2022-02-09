/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_SWAPCHAIN_HPP
#define AXIS_GRAPHICS_SWAPCHAIN_HPP
#pragma once

#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"

namespace Axis
{

namespace Window
{

class DisplayWindow;

} // namespace Window

namespace Graphics
{

// Forward declarations
class IDeviceContext;
class ITextureView;
class ITexture;

/// \brief Description of SwapChain.
struct SwapChainDescription
{
    /// \brief The targeted window, which the swap chain will present their images to.
    System::SharedPointer<Window::DisplayWindow> TargetWindow = {};

    /// \brief Immediate graphics context uses in the swap chain present command submissions.
    System::SharedPointer<IDeviceContext> ImmediateGraphicsContext = {};

    /// \brief The number of back buffer.
    Uint32 BackBufferCount = {};

    /// \brief TextureFormat for the render target (color) view of the swap chain.
    TextureFormat RenderTargetFormat = {};

    /// \brief TextureFormat for the depth stencil view of the swap chain.
    ///        set \a `TextureFormmat::Unknown` to not create depth stencil view.
    TextureFormat DepthStencilFormat = {};
};

/// \brief Responsible for presenting the images to the window.
class AXIS_GRAPHICS_API ISwapChain : public DeviceChild
{
public:
    /// \brief The description of ISwapChain.
    const SwapChainDescription Description;

    /// \brief Gets the render target (color) view which will be presented to the window in the next \a `ISwapChain::Present` call.
    AXIS_NODISCARD virtual System::SharedPointer<ITextureView> GetCurrentRenderTargetView() = 0;

    /// \brief Gets the depth stencil view.
    AXIS_NODISCARD virtual System::SharedPointer<ITextureView> GetCurrentDepthStencilView() = 0;

    /// \brief Presents all the rendering to the window.
    virtual void Present(Uint8 syncIntervals) = 0;

protected:
    /// \brief Constructor
    ISwapChain(const SwapChainDescription& description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_SWAPCHAIN_HPP