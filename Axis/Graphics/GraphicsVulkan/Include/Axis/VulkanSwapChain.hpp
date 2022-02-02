/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANSWAPCHAIN_HPP
#define AXIS_VULKANSWAPCHAIN_HPP
#pragma once

#include "../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../Include/Axis/GraphicsSystem.hpp"
#include "../../../Include/Axis/SwapChain.hpp"
#include "VkPtr.hpp"

namespace Axis
{

/// Forward declarations
class VulkanDeviceContext;
class VulkanGraphicsDevice;
class VulkanTextureView;
class IFence;

/// \brief An implementation of ISwapChain interface in Vulkan backend.
class VulkanSwapChain final : public ISwapChain
{
public:
    /// Constructor
    VulkanSwapChain(const SwapChainDescription& description,
                    VulkanGraphicsDevice&       vulkanGraphicsDevice);

    /// Destructor
    ~VulkanSwapChain() noexcept override final;

    /// \brief An implementation of ISwapChain::GetCurrentRenderTargetView in Vulkan backend.
    SharedPointer<ITextureView> GetCurrentRenderTargetView() override final;

    /// \brief An implementation of ISwapChain::GetCurrentDepthStencilView in Vulkan backend.
    SharedPointer<ITextureView> GetCurrentDepthStencilView() override final;

    /// \brief An implementation of ISwapChain::Present in Vulkan backend.
    void Present(Uint8 syncIntervals) override final;

    /// \brief Gets the internal VkSwapchainKHR handle.
    inline VkSwapchainKHR GetVkSwapchainKHRHandle() const noexcept { return _vulkanSwapchain; }

    /// \brief Number of Frame in swapChain that can be in flight in the same time.
    static constexpr Uint32 MaxFrameInFlight = 2;

private:
    void RecreateSwapChain(Bool vsync);
    void OnWindowSizeChanged(const Vector2UI& newSize) noexcept;
    void StartFrame();
    void EndFrame();

    /// Private members
    VkPtr<VkSwapchainKHR>             _vulkanSwapchain           = {};             // The internal VkSwapchainKHR handle.
    VkSurfaceKHR                      _vulkanSurface             = VK_NULL_HANDLE; // The surface handle from the Window.
    Bool                              _vSyncEnabled              = true;           // The lastest sync interval
    List<SharedPointer<ITextureView>> _renderTargetViews         = nullptr;        // Swapchain's render target back buffers
    List<SharedPointer<ITextureView>> _depthStencilViews         = nullptr;        // Swapchain's depth stencil back buffers
    List<VkPtr<VkSemaphore>>          _imageAvailableSemaphores  = {};             // The semaphores to signal that image has acquired and is ready for rendering.
    List<SharedPointer<IFence>>       _inFlightFences            = {};             // In flight fences
    List<SharedPointer<IFence>>       _imagesInFlight            = {};             // Images in flight fences
    List<Uint64>                      _inFlightFenceValues       = {};             // Fence wait values
    Uint64                            _currentImageInFlightValue = 0;              // Wait value for Fence
    Uint32                            _currentInFlightFrame      = 0;              // Current in flight frame number.
    Uint32                            _frameIndex                = 0;              // Current frame buffer index to use.
    Bool                              _windowResized             = false;          // Flags indicates whether if the window has resized or not.
};

} // namespace Axis

#endif // AXIS_VULKANSWAPCHAIN_HPP