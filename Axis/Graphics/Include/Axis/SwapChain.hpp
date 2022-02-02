/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file SwapChain.hpp
///
/// \brief Contains `Axis::ISwapChain` interface class.

#ifndef AXIS_GRAPHICS_SWAPCHAIN_HPP
#define AXIS_GRAPHICS_SWAPCHAIN_HPP
#pragma once

#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"


namespace Axis
{

/// Forward declarations
class IDeviceContext;
class ITextureView;
class ITexture;
class DisplayWindow;

/// \brief Description of SwapChain.
struct SwapChainDescription
{
    /// \brief The targeted window, which the swap chain will present their images to.
    ///
    SharedPointer<DisplayWindow> TargetWindow = {};

    /// \brief Immediate graphics context uses in the swap chain present command submissions.
    ///
    SharedPointer<IDeviceContext> ImmediateGraphicsContext = {};

    /// \brief The number of back buffer.
    ///
    Uint32 BackBufferCount = {};

    /// \brief TextureFormat for the render target (color) view of the swap chain.
    ///
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

    /// \brief Gets the render target (color) view which will be presented to the window in the next \a `Axis::ISwapChain::Present` call.
    AXIS_NODISCARD virtual SharedPointer<ITextureView> GetCurrentRenderTargetView() = 0;

    /// \brief Gets the depth stencil view.
    AXIS_NODISCARD virtual SharedPointer<ITextureView> GetCurrentDepthStencilView() = 0;

    /// \brief Presents all the rendering to the window.
    virtual void Present(Uint8 syncIntervals) = 0;

protected:
    /// \brief Constructor
    ISwapChain(const SwapChainDescription& description);
};

} // namespace Axis

#endif // AXIS_GRAPHICS_SWAPCHAIN_HPP