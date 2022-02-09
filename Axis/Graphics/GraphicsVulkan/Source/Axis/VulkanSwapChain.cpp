/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/DisplayWindow.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Math.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanDeviceQueueFamily.hpp>
#include <Axis/VulkanFence.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsSystem.hpp>
#include <Axis/VulkanSwapChain.hpp>
#include <Axis/VulkanTexture.hpp>
#include <Axis/VulkanUtility.hpp>

namespace Axis
{

namespace Graphics
{

static void CreateSwapChain(const SwapChainDescription&                        description,
                            Bool                                               vSync,
                            VulkanGraphicsDevice&                              vulkanGraphicsDevice,
                            VkSurfaceKHR&                                      surfaceOut,
                            VkPtr<VkSwapchainKHR>&                             currentSwapchainKHR,
                            System::List<System::SharedPointer<ITextureView>>& renderTargetViewsOut,
                            System::List<System::SharedPointer<ITextureView>>& depthStencilViewsOut)
{
    auto vulkanGraphicsSystem = (VulkanGraphicsSystem*)vulkanGraphicsDevice.GraphicsSystem;

    surfaceOut = vulkanGraphicsSystem->GetVkSurfaceKHR(description.TargetWindow);

    // Validates the surface
    VkBool32 supported = VK_FALSE;

    vkGetPhysicalDeviceSurfaceSupportKHR(vulkanGraphicsSystem->GetVulkanPhysicalDevices()[vulkanGraphicsDevice.GraphicsAdapterIndex].GetVkPhysicalDeviceHandle(),
                                         description.ImmediateGraphicsContext->DeviceQueueFamilyIndex,
                                         surfaceOut,
                                         &supported);

    if (!supported)
        throw System::ExternalException("physicalDevice didn't support!");

    const auto& vulkanPhysicalDevice = vulkanGraphicsSystem->GetVulkanPhysicalDevices()[vulkanGraphicsDevice.GraphicsAdapterIndex];

    // All preferred swap chain info
    VkPresentModeKHR              preferredPresentMode     = {};                          // Presentation mode
    VkExtent2D                    preferredImageExtent     = {};                          // Image size
    Uint32                        preferredBackBufferCount = description.BackBufferCount; // Back buffer count (image count)
    VkSurfaceFormatKHR            preferredSurfaceFormat   = {};                          // Preferred surface format
    VkSurfaceTransformFlagBitsKHR surfaceTransform         = {};

    // Chooses image extent and back buffer count
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanPhysicalDevice.GetVkPhysicalDeviceHandle(), surfaceOut, &surfaceCapabilities);

        surfaceTransform = surfaceCapabilities.currentTransform;

        auto windowSize = description.TargetWindow->GetSize();

        // Tries to get the nearest image extent as the window size
        preferredImageExtent.width = System::Math::Clamp(windowSize.X,
                                                         surfaceCapabilities.minImageExtent.width,
                                                         surfaceCapabilities.maxImageExtent.width);


        preferredImageExtent.height = System::Math::Clamp(windowSize.Y,
                                                          surfaceCapabilities.minImageExtent.height,
                                                          surfaceCapabilities.maxImageExtent.height);

        // Clamps the back buffer count into the capability's range
        preferredBackBufferCount = System::Math::Clamp(preferredBackBufferCount,
                                                       surfaceCapabilities.minImageCount,
                                                       surfaceCapabilities.maxImageCount);
    }

    // Chooses format
    {
        Uint32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice.GetVkPhysicalDeviceHandle(), surfaceOut, &formatCount, nullptr);

        System::List<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanPhysicalDevice.GetVkPhysicalDeviceHandle(), surfaceOut, &formatCount, surfaceFormats.GetData());

        for (const auto& surfaceForamt : surfaceFormats)
        {
            if (surfaceForamt.format == VulkanUtility::GetVkFormatFromTextureFormat(description.RenderTargetFormat))
            {
                preferredSurfaceFormat = surfaceForamt;
                break;
            }
        }
    }

    // Chooses VkPresetModeKHR
    {
        Uint32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice.GetVkPhysicalDeviceHandle(), surfaceOut, &presentModeCount, nullptr);

        System::List<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanPhysicalDevice.GetVkPhysicalDeviceHandle(), surfaceOut, &presentModeCount, presentModes.GetData());

        System::List<VkPresentModeKHR> preferredPresentModes;

        if (vSync)
        {
            preferredPresentModes.ReserveFor(2);
            preferredPresentModes.Append(VK_PRESENT_MODE_FIFO_KHR);
        }
        else
        {
            preferredPresentModes.ReserveFor(3);
            preferredPresentModes.Append(VK_PRESENT_MODE_MAILBOX_KHR);   // Tipple buffering
            preferredPresentModes.Append(VK_PRESENT_MODE_IMMEDIATE_KHR); // Simple Vsync disabled
            preferredPresentModes.Append(VK_PRESENT_MODE_FIFO_KHR);      // FIFO is guaranteed to support in all devices
        }

        for (const auto& searchingPresentMode : preferredPresentModes)
        {
            Bool found = false;
            for (auto presentMode : presentModes)
            {
                if (presentMode == searchingPresentMode)
                {
                    preferredPresentMode = presentMode;
                    found                = true;
                    break;
                }
            }

            if (found)
                break;
        }
    }

    auto CreateVkSwapchainKHR = [&]() -> VkSwapchainKHR {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext                    = nullptr;
        swapchainCreateInfo.surface                  = surfaceOut;
        swapchainCreateInfo.minImageCount            = preferredBackBufferCount;
        swapchainCreateInfo.imageFormat              = preferredSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace          = preferredSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent              = preferredImageExtent;
        swapchainCreateInfo.imageArrayLayers         = 1;
        swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchainCreateInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount    = 0;
        swapchainCreateInfo.pQueueFamilyIndices      = nullptr;
        swapchainCreateInfo.preTransform             = surfaceTransform;
        swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode              = preferredPresentMode;
        swapchainCreateInfo.clipped                  = VK_TRUE;
        swapchainCreateInfo.oldSwapchain             = currentSwapchainKHR;

        VkSwapchainKHR vkSwapchainKHR = {};

        VkResult result = vkCreateSwapchainKHR(vulkanGraphicsDevice.GetVkDeviceHandle(), &swapchainCreateInfo, nullptr, &vkSwapchainKHR);

        if (result != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkSwapchainKHR!");

        return vkSwapchainKHR;
    };

    auto DestroyVkSwapchainKHR = [&](VkSwapchainKHR swapchainKHR) {
        vkDestroySwapchainKHR(vulkanGraphicsDevice.GetVkDeviceHandle(), swapchainKHR, nullptr);
    };

    VkPtr<VkSwapchainKHR> createdSwapchain(CreateVkSwapchainKHR, std::move(DestroyVkSwapchainKHR));

    Uint32 imageCount = 0;
    vkGetSwapchainImagesKHR(vulkanGraphicsDevice.GetVkDeviceHandle(), createdSwapchain, &imageCount, nullptr);

    if (imageCount == 0)
        throw System::ExternalException("Failed to get vkSwapChainKHR's images!");

    System::List<VkImage> swapchainImages(imageCount);
    vkGetSwapchainImagesKHR(vulkanGraphicsDevice.GetVkDeviceHandle(), createdSwapchain, &imageCount, swapchainImages.GetData());

    Uint64 deviceQueueFamilyMask = 0;

    deviceQueueFamilyMask = System::Math::AssignBitToPosition(deviceQueueFamilyMask, description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);

    // Swapchain image's description
    TextureDescription swapchainImageDescription    = {};
    swapchainImageDescription.Dimension             = TextureDimension::Texture2D;
    swapchainImageDescription.Size                  = {preferredImageExtent.width, preferredImageExtent.height, 1};
    swapchainImageDescription.Format                = description.RenderTargetFormat;
    swapchainImageDescription.MipLevels             = 1;
    swapchainImageDescription.Sample                = 1;
    swapchainImageDescription.ArraySize             = 1;
    swapchainImageDescription.Usage                 = ResourceUsage::Immutable;
    swapchainImageDescription.DeviceQueueFamilyMask = deviceQueueFamilyMask;

    // Clears all existing texture view.
    auto newRenderTargetViews = System::List<System::SharedPointer<ITextureView>>(preferredBackBufferCount, nullptr);
    auto newDepthStencilViews = System::List<System::SharedPointer<ITextureView>>(preferredBackBufferCount, nullptr);

    Size index = 0;

    for (const auto& image : swapchainImages)
    {
        // VulkanTexture with no memory backed to it. (Created from swap chain)
        System::SharedPointer<VulkanTexture> vulkanTexture = Axis::System::MakeShared<VulkanTexture>(swapchainImageDescription,
                                                                                                     image,
                                                                                                     vulkanGraphicsDevice);

        // Render target (color attachment) description
        TextureViewDescription swapchainTextureViewDescription = {};
        swapchainTextureViewDescription.ViewTexture            = vulkanTexture;
        swapchainTextureViewDescription.ViewDimension          = TextureViewDimension::Texture2D;
        swapchainTextureViewDescription.ViewUsage              = TextureViewUsage::RenderTarget;
        swapchainTextureViewDescription.ViewFormat             = vulkanTexture->Description.Format;
        swapchainTextureViewDescription.BaseMipLevel           = 0;
        swapchainTextureViewDescription.MipLevelCount          = 1;
        swapchainTextureViewDescription.BaseArrayIndex         = 0;
        swapchainTextureViewDescription.ArrayLevelCount        = 1;

        newRenderTargetViews[index] = vulkanGraphicsDevice.CreateTextureView(swapchainTextureViewDescription);

        index++;
    }

    if (description.DepthStencilFormat != TextureFormat::Unknown)
    {
        TextureDescription depthStencilTextureDescription = {};
        depthStencilTextureDescription.Dimension          = TextureDimension::Texture2D;
        // In cases of user resized the window to 0 or minimizes the window.
        depthStencilTextureDescription.Size           = {preferredImageExtent.width == 0 ? 1 : preferredImageExtent.width, preferredImageExtent.height == 0 ? 1 : preferredImageExtent.height, 1};
        depthStencilTextureDescription.TextureBinding = TextureBinding::DepthStencilAttachment;
        depthStencilTextureDescription.Format         = description.DepthStencilFormat;
        depthStencilTextureDescription.MipLevels      = 1;
        depthStencilTextureDescription.Sample         = 1;
        depthStencilTextureDescription.ArraySize      = 1;
        depthStencilTextureDescription.Usage          = ResourceUsage::Immutable;

        depthStencilTextureDescription.DeviceQueueFamilyMask = System::Math::AssignBitToPosition(depthStencilTextureDescription.DeviceQueueFamilyMask, description.ImmediateGraphicsContext->DeviceQueueFamilyIndex, true);

        index = 0;

        for (Size i = 0; i < preferredBackBufferCount; i++)
        {
            System::SharedPointer<ITexture> depthStencilTexture = vulkanGraphicsDevice.CreateTexture(depthStencilTextureDescription);

            // Depth stencil description
            TextureViewDescription swapChainDepthStencilView = {};
            swapChainDepthStencilView.ViewTexture            = depthStencilTexture;
            swapChainDepthStencilView.ViewDimension          = TextureViewDimension::Texture2D;
            swapChainDepthStencilView.ViewUsage              = TextureViewUsage::Depth | TextureViewUsage::Stencil;
            swapChainDepthStencilView.ViewFormat             = depthStencilTexture->Description.Format;
            swapChainDepthStencilView.BaseMipLevel           = 0;
            swapChainDepthStencilView.MipLevelCount          = 1;
            swapChainDepthStencilView.BaseArrayIndex         = 0;
            swapChainDepthStencilView.ArrayLevelCount        = 1;

            newDepthStencilViews[index] = vulkanGraphicsDevice.CreateTextureView(swapChainDepthStencilView);

            index++;
        }
    }

    currentSwapchainKHR  = std::move(createdSwapchain);
    renderTargetViewsOut = std::move(newRenderTargetViews);
    depthStencilViewsOut = std::move(newDepthStencilViews);
}

static void CreateSynchronizationObjects(VulkanGraphicsDevice&                        vulkanGraphicsDevice,
                                         System::List<VkPtr<VkSemaphore>>&            imageAvailableSemaphoresOut,
                                         System::List<System::SharedPointer<IFence>>& inFlightFencesOut) noexcept
{
    auto newImageAvailableSemaphores = System::List<VkPtr<VkSemaphore>>(VulkanSwapChain::MaxFrameInFlight);
    auto newInFlightFences           = System::List<System::SharedPointer<IFence>>(VulkanSwapChain::MaxFrameInFlight);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (Size i = 0; i < VulkanSwapChain::MaxFrameInFlight; i++)
    {
        auto CreateVkSemaphore = [&]() -> VkSemaphore {
            VkSemaphore vkSemaphore = {};

            if (vkCreateSemaphore(vulkanGraphicsDevice.GetVkDeviceHandle(), &semaphoreInfo, nullptr, &vkSemaphore) != VK_SUCCESS)
                throw System::ExternalException("Failed to create VkSemaphore!");
            return vkSemaphore;
        };

        auto DestroyVkSemaphore = [&](VkSemaphore vkSemaphore) {
            vkDestroySemaphore(vulkanGraphicsDevice.GetVkDeviceHandle(), vkSemaphore, nullptr);
        };

        newImageAvailableSemaphores[i] = VkPtr<VkSemaphore>(CreateVkSemaphore, std::move(DestroyVkSemaphore));
        newInFlightFences[i]           = vulkanGraphicsDevice.CreateFence(0);
    }

    imageAvailableSemaphoresOut = std::move(newImageAvailableSemaphores);
    inFlightFencesOut           = std::move(newInFlightFences);
}

VulkanSwapChain::VulkanSwapChain(const SwapChainDescription& description,
                                 VulkanGraphicsDevice&       vulkanGraphicsDevice) :
    ISwapChain(description)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    CreateSwapChain(description,
                    true,
                    vulkanGraphicsDevice,
                    _vulkanSurface,
                    _vulkanSwapchain,
                    _renderTargetViews,
                    _depthStencilViews);


    CreateSynchronizationObjects(vulkanGraphicsDevice,
                                 _imageAvailableSemaphores,
                                 _inFlightFences);


    _imagesInFlight      = System::List<System::SharedPointer<IFence>>(_renderTargetViews.GetLength(), nullptr);
    _inFlightFenceValues = System::List<Uint64>(_renderTargetViews.GetLength(), 0);

    StartFrame();
}

VulkanSwapChain::~VulkanSwapChain() noexcept
{
    GetCreatorDevice()->WaitDeviceIdle();
}

System::SharedPointer<ITextureView> VulkanSwapChain::GetCurrentRenderTargetView()
{
    return _renderTargetViews[_frameIndex];
}

System::SharedPointer<ITextureView> VulkanSwapChain::GetCurrentDepthStencilView()
{
    return _depthStencilViews[_frameIndex];
}

void VulkanSwapChain::Present(Uint8 syncIntervals)
{
    Bool vSync = syncIntervals != 0;

    if (vSync != _vSyncEnabled)
    {
        _vSyncEnabled = vSync;

        // Ends the current frame
        EndFrame();

        // Creates new swap chain
        RecreateSwapChain(_vSyncEnabled);

        // Starts new frame
        StartFrame();
    }

    EndFrame();

    StartFrame();
}

void VulkanSwapChain::RecreateSwapChain(Bool vsync)
{
    System::List<VkPtr<VkSemaphore>>            newImageAvailableSemaphores;
    System::List<System::SharedPointer<IFence>> newInFlightFences;

    CreateSynchronizationObjects(*((VulkanGraphicsDevice*)GetCreatorDevice()),
                                 newImageAvailableSemaphores,
                                 newInFlightFences);
    CreateSwapChain(Description,
                    vsync,
                    *((VulkanGraphicsDevice*)GetCreatorDevice()),
                    _vulkanSurface,
                    _vulkanSwapchain,
                    _renderTargetViews,
                    _depthStencilViews);

    _imagesInFlight.Reset();
    _inFlightFenceValues.Reset(0);

    _imageAvailableSemaphores = std::move(newImageAvailableSemaphores);
    _inFlightFences           = std::move(newInFlightFences);
}

void VulkanSwapChain::OnWindowSizeChanged(const System::Vector2UI& newSize) noexcept { _windowResized = true; }

void VulkanSwapChain::StartFrame()
{
    _inFlightFences[_currentInFlightFrame]->WaitForValue(_inFlightFenceValues[_currentInFlightFrame]);

    // Timeline semaphore values exceeds 64-bit int values.
    // Creates new timeline semaphore.
    if (_inFlightFenceValues[_currentInFlightFrame] == UINT64_MAX) // it won't happen, ikr.
    {
        CreateSynchronizationObjects(*((VulkanGraphicsDevice*)GetCreatorDevice()),
                                     _imageAvailableSemaphores,
                                     _inFlightFences);

        _imagesInFlight.Reset();
        _inFlightFenceValues.Reset();
    }

    VkResult result = vkAcquireNextImageKHR(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(),
                                            _vulkanSwapchain,
                                            UINT64_MAX,
                                            _imageAvailableSemaphores[_currentInFlightFrame],
                                            VK_NULL_HANDLE,
                                            &_frameIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain(_vSyncEnabled);

        // Starts a frame again
        StartFrame();
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw System::ExternalException("Failed to acquire next swap chain image!");
    }
}

void VulkanSwapChain::EndFrame()
{
    System::SharedPointer<VulkanDeviceContext> vulkanDeviceContext = (System::SharedPointer<VulkanDeviceContext>)Description.ImmediateGraphicsContext;

    if (_imagesInFlight[_frameIndex])
        _imagesInFlight[_frameIndex]->WaitForValue(_currentImageInFlightValue);

    // Transits the texture's resource state to the `Present` state.
    vulkanDeviceContext->TransitTextureState(_renderTargetViews[_frameIndex]->Description.ViewTexture,
                                             _renderTargetViews[_frameIndex]->Description.ViewTexture->GetCurrentResourceState(),
                                             ResourceState::Present,
                                             0,
                                             1,
                                             0,
                                             1,
                                             false,
                                             true);

    vulkanDeviceContext->GetVulkanDeviceQueue().AppendWaitSemaphore(_imageAvailableSemaphores[_currentInFlightFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0 /*Ignored*/);
    vulkanDeviceContext->AppendSignalFence(_inFlightFences[_currentInFlightFrame], _inFlightFenceValues[_currentInFlightFrame] + 1);

    auto signalSemaphore = vulkanDeviceContext->GetVulkanCommandBuffer()->GetSignalVkSemaphore();

    // Flushes all pending commands
    vulkanDeviceContext->Flush();

    _imagesInFlight[_frameIndex] = _inFlightFences[_currentInFlightFrame];
    _currentImageInFlightValue   = _inFlightFenceValues[_currentInFlightFrame];

    _inFlightFenceValues[_currentInFlightFrame]++;

    VkSwapchainKHR swapchains = _vulkanSwapchain;

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &signalSemaphore;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchains;
    presentInfo.pImageIndices      = &_frameIndex;

    VkResult result = vkQueuePresentKHR(vulkanDeviceContext->GetVulkanDeviceQueue().GetVkQueueHandle(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _windowResized)
    {
        RecreateSwapChain(_vSyncEnabled);

        _windowResized = false;
    }
    else if (result != VK_SUCCESS)
    {
        throw System::ExternalException("Failed to present swap chain image!");
    }

    _currentInFlightFrame = (_currentInFlightFrame + 1) % MaxFrameInFlight;
}

} // namespace Graphics

} // namespace Axis
