/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanBuffer.hpp>
#include <Axis/VulkanCommandBuffer.hpp>
#include <Axis/VulkanCommandPool.hpp>
#include <Axis/VulkanDescriptorPool.hpp>
#include <Axis/VulkanDeviceContext.hpp>
#include <Axis/VulkanDeviceQueueFamily.hpp>
#include <Axis/VulkanFence.hpp>
#include <Axis/VulkanFramebuffer.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsPipeline.hpp>
#include <Axis/VulkanGraphicsSystem.hpp>
#include <Axis/VulkanResourceHeap.hpp>
#include <Axis/VulkanResourceHeapLayout.hpp>
#include <Axis/VulkanTexture.hpp>
#include <Axis/VulkanUtility.hpp>


namespace Axis
{

VulkanDeviceContext::VulkanDeviceContext(Uint32                deviceQueueFamilyIndex,
                                         Uint32                deviceQueueIndex,
                                         QueueOperationFlags   supportedQueueOperations,
                                         VulkanGraphicsDevice& vulkanGraphicsDevice) :
    IDeviceContext(vulkanGraphicsDevice,
                   deviceQueueFamilyIndex,
                   deviceQueueIndex,
                   supportedQueueOperations),
    _deviceQueueFamilyIndex(deviceQueueFamilyIndex),
    _deviceQueueIndex(deviceQueueIndex),
    _supportedQueueOperations(supportedQueueOperations),
    _framebufferCache(vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);

    BufferDescription bufferDescription = {};
    bufferDescription.Usage             = ResourceUsage::Immutable;
    bufferDescription.BufferBinding     = BufferBinding::Vertex | BufferBinding::Index | BufferBinding::Uniform;
    bufferDescription.BufferSize        = 1;

    bufferDescription.DeviceQueueFamilyMask = Math::AssignBitToPosition(bufferDescription.DeviceQueueFamilyMask, _deviceQueueFamilyIndex, true);

    _nullVulkanBuffer = (SharedPointer<VulkanBuffer>)GetCreatorDevice()->CreateBuffer(bufferDescription, nullptr);

    _vulkanCommandPool = UniquePointer<VulkanCommandPool>(Axis::New<VulkanCommandPool>(vulkanGraphicsDevice.GetVulkanDeviceQueueFamily(deviceQueueFamilyIndex),
                                                                                       vulkanGraphicsDevice,
                                                                                       VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT));

    _currentVulkanCommandBuffer = _vulkanCommandPool->GetCommandBuffer();

    _currentVulkanCommandBuffer->BeginRecording(); // Starts recording command buffer.
}

// Default destructor
VulkanDeviceContext::~VulkanDeviceContext() noexcept
{
    // Waits all the operations.
    GetVulkanDeviceQueue().WaitQueueIdle();
}

void VulkanDeviceContext::WaitQueueIdle() const noexcept
{
    GetVulkanDeviceQueue().WaitQueueIdle();
}

void VulkanDeviceContext::TransitTextureState(const SharedPointer<ITexture>& textureResource,
                                              ResourceState                  initialState,
                                              ResourceState                  finalState,
                                              Uint32                         baseArrayIndex,
                                              Uint32                         arrayLevelCount,
                                              Uint32                         baseMipLevel,
                                              Uint32                         mipLevelCount,
                                              Bool                           discardContent,
                                              Bool                           recordState)
{
    // Validates the arguments
    IDeviceContext::TransitTextureState(textureResource,
                                        initialState,
                                        finalState,
                                        baseArrayIndex,
                                        arrayLevelCount,
                                        baseMipLevel,
                                        mipLevelCount,
                                        discardContent,
                                        recordState);

    if (initialState != finalState)
    {
        VkImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext                = nullptr;

        imageMemoryBarrier.image = ((VulkanTexture*)textureResource)->GetVkImageHandle();

        imageMemoryBarrier.oldLayout = VulkanUtility::GetVkImageLayoutFromResourceState(initialState);
        imageMemoryBarrier.newLayout = VulkanUtility::GetVkImageLayoutFromResourceState(finalState);

        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        imageMemoryBarrier.srcAccessMask = VulkanUtility::GetVkAccessFlagsFromResourceState(initialState);
        imageMemoryBarrier.dstAccessMask = VulkanUtility::GetVkAccessFlagsFromResourceState(finalState);

        imageMemoryBarrier.subresourceRange.aspectMask     = VulkanUtility::GetVkImageAspectFlagsFromTextureFormat(textureResource->Description.Format);
        imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayIndex;
        imageMemoryBarrier.subresourceRange.layerCount     = arrayLevelCount;
        imageMemoryBarrier.subresourceRange.baseMipLevel   = baseMipLevel;
        imageMemoryBarrier.subresourceRange.levelCount     = mipLevelCount;

        _currentVulkanCommandBuffer->AddResourceStrongReference(textureResource);

        if (_currentVulkanCommandBuffer->IsRenderPassActivating())
        {
            _currentVulkanCommandBuffer->EndRenderPass(); // Ends the current render pass to avoid that weird render pass and barrier dependency.
            _renderPassUpToDate = false;
        }

        vkCmdPipelineBarrier(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                             VulkanUtility::GetVkPipelineStageFlagsFromResourceState(initialState),
                             VulkanUtility::GetVkPipelineStageFlagsFromResourceState(finalState),
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &imageMemoryBarrier);
    }

    if (recordState)
        textureResource->SetResourceState(finalState);
}

void VulkanDeviceContext::TransitBufferState(const SharedPointer<IBuffer>& bufferResource,
                                             ResourceState                 initialState,
                                             ResourceState                 finalState,
                                             Bool                          discardContent,
                                             Bool                          recordState)
{
    // Validates the arguments
    IDeviceContext::TransitBufferState(bufferResource,
                                       initialState,
                                       finalState,
                                       discardContent,
                                       recordState);

    const MemoryAccess oldAccessMode = Graphics::GetMemoryAccessFromResourceState(initialState);
    const MemoryAccess newAccessMode = Graphics::GetMemoryAccessFromResourceState(finalState);

    // There wouldn't be any needs to insert the barrier in kinds of read then read or write then write operations, right ?
    if (!(((oldAccessMode == MemoryAccess::Read) && (newAccessMode == MemoryAccess::Read)) ||
          ((oldAccessMode == MemoryAccess::Write) && (newAccessMode == MemoryAccess::Write))))
    {
        VkBufferMemoryBarrier bufferMemoryBarrier = {};
        bufferMemoryBarrier.sType                 = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufferMemoryBarrier.pNext                 = nullptr;
        bufferMemoryBarrier.buffer                = ((VulkanBuffer*)bufferResource)->GetVkBufferHandle();

        bufferMemoryBarrier.srcAccessMask = VulkanUtility::GetVkAccessFlagsFromResourceState(initialState);
        bufferMemoryBarrier.dstAccessMask = VulkanUtility::GetVkAccessFlagsFromResourceState(finalState);

        bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        bufferMemoryBarrier.size   = VK_WHOLE_SIZE;
        bufferMemoryBarrier.offset = 0;

        _currentVulkanCommandBuffer->AddResourceStrongReference(bufferResource);

        if (_currentVulkanCommandBuffer->IsRenderPassActivating())
        {
            _currentVulkanCommandBuffer->EndRenderPass();
            _renderPassUpToDate = false;
        }

        vkCmdPipelineBarrier(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                             VulkanUtility::GetVkPipelineStageFlagsFromResourceState(initialState),
                             VulkanUtility::GetVkPipelineStageFlagsFromResourceState(finalState),
                             0,
                             0,
                             nullptr,
                             1,
                             &bufferMemoryBarrier,
                             0,
                             nullptr);
    }

    if (recordState)
        bufferResource->SetResourceState(finalState);
}

void VulkanDeviceContext::CopyBuffer(const SharedPointer<IBuffer>& sourceBuffer,
                                     Size                          sourceOffset,
                                     const SharedPointer<IBuffer>& destBuffer,
                                     Size                          destOffset,
                                     Size                          copySize,
                                     StateTransition               sourceBufferStateTransition,
                                     StateTransition               destBufferStateTransition)
{
    // Validates the arguments
    IDeviceContext::CopyBuffer(sourceBuffer,
                               sourceOffset,
                               destBuffer,
                               destOffset,
                               copySize,
                               sourceBufferStateTransition,
                               destBufferStateTransition);

    if (sourceBufferStateTransition != StateTransition::Transit)
    {
        TransitBufferState(sourceBuffer,
                           sourceBuffer->GetCurrentResourceState(),
                           ResourceState::TransferSource,
                           false,
                           true);
    }

    if (destBufferStateTransition != StateTransition::Transit)
    {
        TransitBufferState(destBuffer,
                           destBuffer->GetCurrentResourceState(),
                           ResourceState::TransferDestination,
                           false,
                           true);
    }

    _currentVulkanCommandBuffer->AddResourceStrongReference(sourceBuffer);
    _currentVulkanCommandBuffer->AddResourceStrongReference(destBuffer);

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset    = sourceOffset;
    bufferCopy.dstOffset    = destOffset;
    bufferCopy.size         = copySize;

    vkCmdCopyBuffer(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                    ((VulkanBuffer*)sourceBuffer)->GetVkBufferHandle(),
                    ((VulkanBuffer*)destBuffer)->GetVkBufferHandle(),
                    1,
                    &bufferCopy);
}

void VulkanDeviceContext::BindPipeline(const SharedPointer<IPipeline>& pipeline)
{
    // Validates the arguments
    IDeviceContext::BindPipeline(pipeline);

    _pipelineBindingUpToDate = false;
}

void VulkanDeviceContext::SetViewport(const RectangleF& viewportArea,
                                      Float32           minDepth,
                                      Float32           maxDepth)
{
    // Validates the arguments
    IDeviceContext::SetViewport(viewportArea,
                                minDepth,
                                maxDepth);

    VkViewport viewport = {};
    viewport.x          = viewportArea.X;
    viewport.y          = viewportArea.Y;
    viewport.width      = viewportArea.Width;
    viewport.height     = viewportArea.Height;
    viewport.minDepth   = minDepth;
    viewport.maxDepth   = maxDepth;

    vkCmdSetViewport(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                     0,
                     1,
                     &viewport);
}

void VulkanDeviceContext::SetScissorRectangle(const RectangleI& rectangle)
{
    // Validates the arguments
    IDeviceContext::SetScissorRectangle(rectangle);

    VkRect2D scissor = {};
    scissor.extent   = {(Uint32)rectangle.Width, (Uint32)rectangle.Height};
    scissor.offset   = {rectangle.X, rectangle.Y};

    vkCmdSetScissor(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(), 0, 1, &scissor);
}

void VulkanDeviceContext::ClearDepthStencilView(const SharedPointer<ITextureView>& depthStencilView,
                                                Uint8                              stencilClearValue,
                                                Float32                            depthClearValue,
                                                ClearDepthStencilFlags             clearDepthStencilFlags,
                                                StateTransition                    stateTransition)
{
    // Validates the arguments
    IDeviceContext::ClearDepthStencilView(depthStencilView,
                                          stencilClearValue,
                                          depthClearValue,
                                          clearDepthStencilFlags,
                                          stateTransition);

    VkClearValue       clearValue       = {};
    VkImageAspectFlags imageAspectFlags = {};

    if ((Bool)(clearDepthStencilFlags & ClearDepthStencil::Depth))
    {
        clearValue.depthStencil.depth = depthClearValue;
        imageAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    if ((Bool)(clearDepthStencilFlags & ClearDepthStencil::Stencil))
    {
        clearValue.depthStencil.stencil = stencilClearValue;
        imageAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    if (GetCurrentRenderTargetBinding().DepthStencilView == depthStencilView)
    {
        // Commits the render target binding.
        CommitRenderPass();

        VkClearRect clearRect    = {};
        clearRect.rect           = {{0, 0}, {_pendingRenderPass.Framebuffer->Description.FramebufferSize.X, _pendingRenderPass.Framebuffer->Description.FramebufferSize.Y}};
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount     = _pendingRenderPass.Framebuffer->Description.FramebufferSize.Z;

        VkClearAttachment clearAttachment = {};
        clearAttachment.aspectMask        = imageAspectFlags;
        clearAttachment.colorAttachment   = VK_ATTACHMENT_UNUSED;
        clearAttachment.clearValue        = clearValue;

        // No need to insert texture barrier inside the render pass.
        vkCmdClearAttachments(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                              1,
                              &clearAttachment,
                              1,
                              &clearRect);
    }
    else
    {
        if (stateTransition == StateTransition::Transit)
        {
            // Transition the image layout into ResourceState::TransferDestination.
            TransitTextureState(depthStencilView->Description.ViewTexture,
                                depthStencilView->Description.ViewTexture->GetCurrentResourceState(),
                                ResourceState::TransferDestination,
                                0,
                                depthStencilView->Description.ViewTexture->Description.ArraySize,
                                0,
                                depthStencilView->Description.ViewTexture->Description.MipLevels,
                                true, /*The previous content will be cleared anyway.....*/
                                true);
        }

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask              = imageAspectFlags;
        subresourceRange.baseMipLevel            = depthStencilView->Description.BaseMipLevel;
        subresourceRange.levelCount              = depthStencilView->Description.MipLevelCount;
        subresourceRange.baseArrayLayer          = depthStencilView->Description.BaseArrayIndex;


        vkCmdClearDepthStencilImage(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                                    ((VulkanTexture*)depthStencilView)->GetVkImageHandle(),
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    &clearValue.depthStencil,
                                    1,
                                    &subresourceRange);
    }
}

void VulkanDeviceContext::ClearRenderTarget(const SharedPointer<ITextureView>& renderTargetView,
                                            const ColorF&                      clearColor,
                                            StateTransition                    stateTransition)

{
    // Validates the arguments
    IDeviceContext::ClearRenderTarget(renderTargetView,
                                      clearColor,
                                      stateTransition);

    Bool   foundRenderTarget = false;
    Uint32 attachmentIndex   = 0;

    for (const auto& renderTargetViewToCheck : GetCurrentRenderTargetBinding().RenderTargetViews)
    {
        if (renderTargetViewToCheck == renderTargetView)
        {
            foundRenderTarget = true;
            break;
        }
        attachmentIndex++;
    }

    constexpr auto GetVkClearValueForRenderTarget = [](TextureFormatComponentType format, const ColorF& clearColor) {
        VkClearColorValue clearValue = {};

        switch (format)
        {
            case TextureFormatComponentType::SignedInt:
                clearValue.int32[0] = (Int32)clearColor.R;
                clearValue.int32[1] = (Int32)clearColor.G;
                clearValue.int32[2] = (Int32)clearColor.B;
                clearValue.int32[3] = (Int32)clearColor.A;
                break;

            case TextureFormatComponentType::UnsignedInt:
                clearValue.uint32[0] = (Uint32)clearColor.R;
                clearValue.uint32[1] = (Uint32)clearColor.G;
                clearValue.uint32[2] = (Uint32)clearColor.B;
                clearValue.uint32[3] = (Uint32)clearColor.A;
                break;

            default:
                clearValue.float32[0] = clearColor.R;
                clearValue.float32[1] = clearColor.G;
                clearValue.float32[2] = clearColor.B;
                clearValue.float32[3] = clearColor.A;
                break;
        }

        return clearValue;
    };

    // Luckily the renderTargetView is already in pending to the RenderPassBinding.
    if (foundRenderTarget)
    {
        auto textureFormat = Graphics::GetTextureFormatComponentTypeFormTextureFormat(renderTargetView->Description.ViewFormat);

        // Commit the render target binding.
        CommitRenderPass();

        VkClearRect clearRect    = {};
        clearRect.rect.offset    = {0, 0};
        clearRect.rect.extent    = {_pendingRenderPass.Framebuffer->Description.FramebufferSize.X, _pendingRenderPass.Framebuffer->Description.FramebufferSize.Y};
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount     = _pendingRenderPass.Framebuffer->Description.FramebufferSize.Z;

        VkClearAttachment clearAttachment = {};
        clearAttachment.aspectMask        = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color  = GetVkClearValueForRenderTarget(textureFormat, clearColor);
        clearAttachment.colorAttachment   = attachmentIndex;

        // No need to insert texture barrier inside the render pass.
        vkCmdClearAttachments(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                              1,
                              &clearAttachment,
                              1,
                              &clearRect);
    }
    else
    {
        auto textureFormat = Graphics::GetTextureFormatComponentTypeFormTextureFormat(renderTargetView->Description.ViewFormat);

        if (stateTransition == StateTransition::Transit)
        {
            // Transition the image layout into ResourceState::TransferDestination.
            TransitTextureState(renderTargetView->Description.ViewTexture,
                                renderTargetView->Description.ViewTexture->GetCurrentResourceState(),
                                ResourceState::TransferDestination,
                                0,
                                renderTargetView->Description.ViewTexture->Description.ArraySize,
                                0,
                                renderTargetView->Description.ViewTexture->Description.MipLevels,
                                true, /*The previous content will be cleared anyway.....*/
                                true);
        }

        VkClearColorValue clearValue = GetVkClearValueForRenderTarget(textureFormat, clearColor);

        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseArrayLayer          = renderTargetView->Description.BaseArrayIndex;
        subresourceRange.layerCount              = renderTargetView->Description.ArrayLevelCount;
        subresourceRange.baseMipLevel            = renderTargetView->Description.BaseMipLevel;
        subresourceRange.levelCount              = renderTargetView->Description.MipLevelCount;

        vkCmdClearColorImage(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                             ((VulkanTexture*)renderTargetView->Description.ViewTexture)->GetVkImageHandle(),
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             &clearValue,
                             1,
                             &subresourceRange);
    }
}

void VulkanDeviceContext::SetRenderTarget(const RenderTargetBinding& renderTargetBinding,
                                          StateTransition            stateTransition)
{
    // Validates the arguments
    IDeviceContext::SetRenderTarget(renderTargetBinding,
                                    stateTransition);

    // Creates framebuffer out of render target views
    VulkanFramebufferCacheKey framebufferCacheKey = {};
    framebufferCacheKey.RenderTargetViews         = List<WeakPointer<ITextureView>>(GetCurrentRenderTargetBinding().RenderTargetViews.GetLength());

    for (Size i = 0; i < GetCurrentRenderTargetBinding().RenderTargetViews.GetLength(); i++)
    {
        framebufferCacheKey.RenderTargetViews[i] = GetCurrentRenderTargetBinding().RenderTargetViews[i];
    }

    framebufferCacheKey.DepthStencilView = GetCurrentRenderTargetBinding().DepthStencilView;

    auto framebuffer = _framebufferCache.GetFramebuffer(framebufferCacheKey);

    try
    {
        for (const auto& renderTarget : renderTargetBinding.RenderTargetViews)
        {
            if (stateTransition == StateTransition::Transit)
                TransitTextureState(renderTarget->Description.ViewTexture,
                                    renderTarget->Description.ViewTexture->GetCurrentResourceState(),
                                    ResourceState::RenderTarget,
                                    0,
                                    renderTarget->Description.ViewTexture->Description.ArraySize,
                                    0,
                                    renderTarget->Description.ViewTexture->Description.MipLevels,
                                    false,
                                    true);
        }

        if (renderTargetBinding.DepthStencilView)
        {
            if (stateTransition == StateTransition::Transit)
                TransitTextureState(renderTargetBinding.DepthStencilView->Description.ViewTexture,
                                    renderTargetBinding.DepthStencilView->Description.ViewTexture->GetCurrentResourceState(),
                                    ResourceState::DepthStencilWrite,
                                    0,
                                    renderTargetBinding.DepthStencilView->Description.ViewTexture->Description.ArraySize,
                                    0,
                                    renderTargetBinding.DepthStencilView->Description.ViewTexture->Description.MipLevels,
                                    false,
                                    true);
        }
    }
    catch (...)
    {
        ResetRenderTarget();
        throw;
    }

    _pendingRenderPass.RenderPass  = framebuffer->Description.RenderPass;
    _pendingRenderPass.Framebuffer = framebuffer;

    _renderPassUpToDate = false;
}

void VulkanDeviceContext::BindVertexBuffers(Uint32                              firstBinding,
                                            const Span<SharedPointer<IBuffer>>& vertexBuffers,
                                            const Span<Size>&                   offsets,
                                            StateTransition                     stateTransition)
{
    // Validates the arguments
    IDeviceContext::BindVertexBuffers(firstBinding,
                                      vertexBuffers,
                                      offsets,
                                      stateTransition);

    try
    {
        // Transits the resource state.
        if (stateTransition == StateTransition::Transit)
        {
            for (const auto& vertexBuffer : vertexBuffers)
            {
                TransitBufferState(vertexBuffer,
                                   vertexBuffer->GetCurrentResourceState(),
                                   ResourceState::Vertex,
                                   false,
                                   true);
            }
        }
    }
    catch (...)
    {
        ResetVertexBuffers();
        throw;
    }

    _vertexBufferBindingUpToDate = false;
}

void VulkanDeviceContext::BindIndexBuffer(const SharedPointer<IBuffer>& indexBuffer,
                                          Size                          offset,
                                          IndexType                     indexType,
                                          StateTransition               stateTransition)
{
    // Validates the arguments
    IDeviceContext::BindIndexBuffer(indexBuffer,
                                    offset,
                                    indexType,
                                    stateTransition);
    try
    {
        if (stateTransition == StateTransition::Transit)
        {
            TransitBufferState(indexBuffer,
                               indexBuffer->GetCurrentResourceState(),
                               ResourceState::Index,
                               false,
                               true);
        }
    }
    catch (...)
    {
        ResetIndexBuffer();
        throw;
    }

    _indexBufferBindingUpToDate = false;
}

void VulkanDeviceContext::BindResourceHeap(const SharedPointer<IResourceHeap>& resourceHeap)
{
    // Validates the arguments
    IDeviceContext::BindResourceHeap(resourceHeap);

    _resourceHeapUpToDate = false;
}

void VulkanDeviceContext::Draw(Uint32          vertexCount,
                               Uint32          instanceCount,
                               Uint32          firstVertex,
                               Uint32          firstInstance,
                               StateTransition bindingResourcesStateTransition)
{
    // Validates the arguments
    IDeviceContext::Draw(vertexCount,
                         instanceCount,
                         firstVertex,
                         firstInstance);

    PreDraw(bindingResourcesStateTransition);

    vkCmdDraw(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
              vertexCount,
              instanceCount,
              firstVertex,
              firstInstance);
}

void VulkanDeviceContext::DrawIndexed(Uint32          indexCount,
                                      Uint32          instanceCount,
                                      Uint32          firstIndex,
                                      Uint32          vertexOffset,
                                      Uint32          firstInstance,
                                      StateTransition bindingResourcesStateTransition)

{
    // Validates the arguments
    IDeviceContext::DrawIndexed(indexCount,
                                instanceCount,
                                firstIndex,
                                vertexOffset,
                                firstInstance);

    PreDrawIndexed(bindingResourcesStateTransition);

    vkCmdDrawIndexed(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                     indexCount,
                     instanceCount,
                     firstIndex,
                     vertexOffset,
                     firstInstance);
}

void VulkanDeviceContext::CopyBufferToTexture(const SharedPointer<IBuffer>&  sourceBuffer,
                                              Size                           bufferOffset,
                                              const SharedPointer<ITexture>& destTexture,
                                              Uint32                         baseArrayIndex,
                                              Uint32                         arrayLevelCount,
                                              Uint32                         mipLevel,
                                              Vector3UI                      textureOffset,
                                              Vector3UI                      textureSize,
                                              StateTransition                bufferStateTransition,
                                              StateTransition                textureStateTransition)
{
    // Validates the arguments
    IDeviceContext::CopyBufferToTexture(sourceBuffer,
                                        bufferOffset,
                                        destTexture,
                                        baseArrayIndex,
                                        arrayLevelCount,
                                        mipLevel,
                                        textureOffset,
                                        textureSize,
                                        bufferStateTransition,
                                        textureStateTransition);

    VkBufferImageCopy bufferImageCopy = {};
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.bufferRowLength   = 0;
    bufferImageCopy.bufferOffset      = bufferOffset;
    bufferImageCopy.imageOffset       = {(Int32)textureOffset.X,
                                   (Int32)textureOffset.Y,
                                   (Int32)textureOffset.Z};

    bufferImageCopy.imageExtent = {textureSize.X,
                                   textureSize.Y,
                                   textureSize.Z};

    bufferImageCopy.imageSubresource.aspectMask     = VulkanUtility::GetVkImageAspectFlagsFromTextureFormat(destTexture->Description.Format);
    bufferImageCopy.imageSubresource.baseArrayLayer = baseArrayIndex;
    bufferImageCopy.imageSubresource.layerCount     = arrayLevelCount;
    bufferImageCopy.imageSubresource.mipLevel       = mipLevel;

    _currentVulkanCommandBuffer->AddResourceStrongReference(sourceBuffer);
    _currentVulkanCommandBuffer->AddResourceStrongReference(destTexture);

    try
    {
        if (bufferStateTransition == StateTransition::Transit)
        {
            TransitBufferState(sourceBuffer,
                               sourceBuffer->GetCurrentResourceState(),
                               ResourceState::TransferSource,
                               false,
                               true);
        }

        if (textureStateTransition == StateTransition::Transit)
        {
            TransitTextureState(destTexture,
                                destTexture->GetCurrentResourceState(),
                                ResourceState::TransferDestination,
                                0,
                                destTexture->Description.ArraySize,
                                0,
                                destTexture->Description.MipLevels,
                                false,
                                true);
        }
    }
    catch (...)
    {
        throw;
    }

    vkCmdCopyBufferToImage(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                           ((VulkanBuffer*)sourceBuffer)->GetVkBufferHandle(),
                           ((VulkanTexture*)destTexture)->GetVkImageHandle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &bufferImageCopy);
}

void VulkanDeviceContext::GenerateMips(const SharedPointer<ITextureView>& textureView,
                                       StateTransition                    stateTransition)
{
    IDeviceContext::GenerateMips(textureView,
                                 stateTransition);

    _currentVulkanCommandBuffer->AddResourceStrongReference(textureView);

    ResourceState originState = textureView->Description.ViewTexture->GetCurrentResourceState();

    for (Size i = 1; i < textureView->Description.MipLevelCount; i++)
    {
        // Source mip as `TransferSource`
        TransitTextureState(textureView->Description.ViewTexture,
                            i == 1 ? originState : ResourceState::TransferDestination,
                            ResourceState::TransferSource,
                            textureView->Description.BaseArrayIndex,
                            textureView->Description.ArrayLevelCount,
                            (Uint32)(i - 1),
                            1,
                            false,
                            false);

        // Dest mip as `TransferDest`
        TransitTextureState(textureView->Description.ViewTexture,
                            originState,
                            ResourceState::TransferDestination,
                            textureView->Description.BaseArrayIndex,
                            textureView->Description.ArrayLevelCount,
                            (Uint32)i,
                            1,
                            true,
                            false);

        VkImageBlit imageBlit = {};

        imageBlit.srcOffsets[0] = {0, 0, 0};

        imageBlit.srcOffsets[1].x = std::max((textureView->Description.ViewTexture->Description.Size.X >> (i - 1)), (Uint32)1);
        imageBlit.srcOffsets[1].y = std::max((textureView->Description.ViewTexture->Description.Size.Y >> (i - 1)), (Uint32)1);
        imageBlit.srcOffsets[1].z = std::max((textureView->Description.ViewTexture->Description.Size.Z >> (i - 1)), (Uint32)1);

        imageBlit.srcSubresource.aspectMask     = VulkanUtility::GetVkImageAspectFlagsFromTextureViewUsageFlags(textureView->Description.ViewUsage);
        imageBlit.srcSubresource.baseArrayLayer = textureView->Description.BaseArrayIndex;
        imageBlit.srcSubresource.layerCount     = textureView->Description.ArrayLevelCount;
        imageBlit.srcSubresource.mipLevel       = (Uint32)(i - 1);

        imageBlit.dstOffsets[0] = {0, 0, 0};

        imageBlit.dstOffsets[1].x = std::max((textureView->Description.ViewTexture->Description.Size.X >> i), (Uint32)1);
        imageBlit.dstOffsets[1].y = std::max((textureView->Description.ViewTexture->Description.Size.Y >> i), (Uint32)1);
        imageBlit.dstOffsets[1].z = std::max((textureView->Description.ViewTexture->Description.Size.Z >> i), (Uint32)1);

        imageBlit.dstSubresource.aspectMask     = VulkanUtility::GetVkImageAspectFlagsFromTextureViewUsageFlags(textureView->Description.ViewUsage);
        imageBlit.dstSubresource.baseArrayLayer = textureView->Description.BaseArrayIndex;
        imageBlit.dstSubresource.layerCount     = textureView->Description.ArrayLevelCount;
        imageBlit.dstSubresource.mipLevel       = (Uint32)i;

        vkCmdBlitImage(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                       ((VulkanTexture*)textureView->Description.ViewTexture)->GetVkImageHandle(),
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       ((VulkanTexture*)textureView->Description.ViewTexture)->GetVkImageHandle(),
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &imageBlit,
                       VK_FILTER_LINEAR);
    }

    // Transits all texture's sub resource regions to transfer source.
    TransitTextureState(textureView->Description.ViewTexture,
                        originState,
                        ResourceState::TransferSource,
                        textureView->Description.BaseArrayIndex,
                        textureView->Description.ArrayLevelCount,
                        textureView->Description.BaseMipLevel + textureView->Description.MipLevelCount - 1,
                        1,
                        false,
                        false);

    textureView->Description.ViewTexture->SetResourceState(ResourceState::TransferSource);
}

PVoid VulkanDeviceContext::MapBuffer(const SharedPointer<IBuffer>& buffer,
                                     MapAccess                     mapAccess,
                                     MapType                       mapType)
{
    if (!Graphics::IsResourceUsageMappable(buffer->Description.Usage))
        throw InvalidArgumentException("`buffer` couldn't be mapped!");

    if (((VulkanBuffer*)buffer)->BufferMapped)
        throw InvalidOperationException("`buffer` has been mapped already!");

    VulkanBuffer* vkBuffer = ((VulkanBuffer*)buffer);

    if (mapAccess == MapAccess::Read)
    {
        if (mapType != MapType::Overwrite)
            throw InvalidArgumentException("Failed to map buffer!");

        PVoid pData = {};

        auto vkResult = vmaMapMemory(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                     vkBuffer->GetVmaAllocation(),
                                     &pData);

        if (vkResult != VK_SUCCESS)
            throw ExternalException("Failed to map buffer memory!");

        VkMemoryPropertyFlags memoryPropertyFlag = {};

        vmaGetMemoryTypeProperties(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                   ((VulkanBuffer*)buffer)->GetVmaAllocationInfo().memoryType,
                                   &memoryPropertyFlag);

        if (memoryPropertyFlag & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            vmaInvalidateAllocation(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                    vkBuffer->GetVmaAllocation(),
                                    0,
                                    vkBuffer->Description.BufferSize);
        }


        vkBuffer->BufferMapped = true;
        vkBuffer->MappedAccess = mapAccess;
        vkBuffer->MappedType   = mapType;

        return pData;
    }
    else if (mapAccess == MapAccess::Write)
    {
        if (mapType == MapType::Overwrite)
        {
            PVoid pData = {};

            auto vkResult = vmaMapMemory(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                         ((VulkanBuffer*)buffer)->GetVmaAllocation(),
                                         &pData);

            if (vkResult != VK_SUCCESS)
                throw ExternalException("Failed to map buffer memory!");

            vkBuffer->BufferMapped = true;
            vkBuffer->MappedAccess = mapAccess;
            vkBuffer->MappedType   = mapType;

            return pData;
        }
        else if (mapType == MapType::Discard)
        {
            BufferDescription stagingBufferDescription = {
                .BufferSize            = buffer->Description.BufferSize,
                .BufferBinding         = BufferBinding::TransferSource,
                .Usage                 = ResourceUsage::StagingSource,
                .DeviceQueueFamilyMask = (Size)Math::AssignBitToPosition(0, DeviceQueueFamilyIndex, true)};

            vkBuffer->StagingBuffer = (SharedPointer<VulkanBuffer>)GetCreatorDevice()->CreateBuffer(stagingBufferDescription, nullptr);

            PVoid pData = {};

            auto vkResult = vmaMapMemory(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                         vkBuffer->StagingBuffer->GetVmaAllocation(),
                                         &pData);

            if (vkResult != VK_SUCCESS)
                throw ExternalException("Failed to map buffer memory!");

            vkBuffer->BufferMapped = true;
            vkBuffer->MappedAccess = mapAccess;
            vkBuffer->MappedType   = mapType;

            return pData;
        }
        else
            throw InvalidArgumentException("`mapType` was invalid!");
    }
    else
        throw InvalidArgumentException("`mapAccess` was invalid!");
}

void VulkanDeviceContext::UnmapBuffer(const SharedPointer<IBuffer>& buffer)
{
    if (!Graphics::IsResourceUsageMappable(buffer->Description.Usage))
        throw InvalidArgumentException("`buffer` couldn't be mapped!");

    if (!((VulkanBuffer*)buffer)->BufferMapped)
        throw InvalidOperationException("`buffer` hasn't been mapped yet!");

    VulkanBuffer* vkBuffer = ((VulkanBuffer*)buffer);

    if (vkBuffer->MappedAccess == MapAccess::Read)
    {
        vmaUnmapMemory(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(), vkBuffer->GetVmaAllocation());
    }
    else if (vkBuffer->MappedAccess == MapAccess::Write)
    {
        VulkanBuffer* bufferToUnmap = vkBuffer->MappedType == MapType::Discard ? (VulkanBuffer*)vkBuffer->StagingBuffer : vkBuffer;

        VkMemoryPropertyFlags memoryPropertyFlag = {};

        vmaGetMemoryTypeProperties(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                                   bufferToUnmap->GetVmaAllocationInfo().memoryType,
                                   &memoryPropertyFlag);

        if (memoryPropertyFlag & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        {
            vmaFlushAllocation(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                               bufferToUnmap->GetVmaAllocation(),
                               0,
                               bufferToUnmap->Description.BufferSize);
        }

        vmaUnmapMemory(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVmaAllocatorHandle(),
                       bufferToUnmap->GetVmaAllocation());

        if (vkBuffer->MappedType == MapType::Discard)
        {
            CopyBuffer(vkBuffer->StagingBuffer,
                       0,
                       buffer,
                       0,
                       buffer->Description.BufferSize,
                       StateTransition::Transit,
                       StateTransition::Transit);
        }
    }

    vkBuffer->StagingBuffer = nullptr;
    vkBuffer->BufferMapped  = false;
}

void VulkanDeviceContext::AppendSignalFence(const SharedPointer<IFence>& fence,
                                            Uint64                       fenceValue)
{
    // Validates the arguments
    IDeviceContext::AppendSignalFence(fence, fenceValue);

    auto vulkanFence = (VulkanFence*)fence;

    // Keeps the strong reference until this command buffer done using them.
    _currentVulkanCommandBuffer->AddResourceStrongReference(fence);

    GetVulkanDeviceQueue().AppendSignalSeamphore(vulkanFence->GetVkSemaphoreHandle(), fenceValue);
}

void VulkanDeviceContext::AppendWaitFence(const SharedPointer<IFence>& fence,
                                          Uint64                       fenceValue)
{
    // Validates the argument
    IDeviceContext::AppendSignalFence(fence, fenceValue);

    auto vulkanFence = (VulkanFence*)fence;

    // Keeps the strong reference until this command buffer done using them.
    _currentVulkanCommandBuffer->AddResourceStrongReference(fence);

    GetVulkanDeviceQueue().AppendWaitSemaphore(vulkanFence->GetVkSemaphoreHandle(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, fenceValue);
}

void VulkanDeviceContext::Flush()
{
    // Gets a new command buffer upfront here, to avoid the command buffer being destroyed while we are using it.
    auto backupVulkanCommand = _vulkanCommandPool->GetCommandBuffer();

    IDeviceContext::Flush();

    // Ends any implicit render pass.
    if (_currentVulkanCommandBuffer->IsRenderPassActivating())
        _currentVulkanCommandBuffer->EndRenderPass();

    _currentVulkanCommandBuffer->EndRecording();

    _renderPassUpToDate          = true;
    _pipelineBindingUpToDate     = true;
    _vertexBufferBindingUpToDate = true;
    _indexBufferBindingUpToDate  = true;
    _resourceHeapUpToDate        = true;
    _framebufferCache.CleanUp();

    try
    {
        // Submits command buffer to the queue.
        GetVulkanDeviceQueue().QueueSubmit(*_currentVulkanCommandBuffer,
                                           ((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle());
    }
    catch (...)
    {
        // There is an exception, so we need to restore the command buffer.
        // The current command buffer is corrupted, so we need to destroy it.

        // Assigns the backup command buffer (current gets destroyed) to the current command buffer.
        _currentVulkanCommandBuffer = std::move(backupVulkanCommand);

        _currentVulkanCommandBuffer->BeginRecording();

        throw;
    }

    // Returns command buffer back to the pool.
    _vulkanCommandPool->ReturnCommandBuffer(std::move(_currentVulkanCommandBuffer));

    _currentVulkanCommandBuffer = std::move(backupVulkanCommand); // Gets new command buffer and starts recording

    _currentVulkanCommandBuffer->BeginRecording();
}

VulkanDeviceQueue& VulkanDeviceContext::GetVulkanDeviceQueue() const noexcept
{
    return ((VulkanGraphicsDevice*)GetCreatorDevice())->GetVulkanDeviceQueueFamily(_deviceQueueFamilyIndex).GetDeviceQueue(_deviceQueueIndex);
}

void VulkanDeviceContext::CommitRenderPass()
{
    if (!_renderPassUpToDate)
    {

        try
        {
            _currentVulkanCommandBuffer->AddResourceStrongReference(_pendingRenderPass.RenderPass);
            _currentVulkanCommandBuffer->AddResourceStrongReference(_pendingRenderPass.Framebuffer);
        }
        catch (...)
        {
            throw;
        }

        if (_currentVulkanCommandBuffer->IsRenderPassActivating())
            _currentVulkanCommandBuffer->EndRenderPass();

        _currentVulkanCommandBuffer->BeginRenderPass(_pendingRenderPass.RenderPass, _pendingRenderPass.Framebuffer, nullptr);

        RectangleF frameSize = {
            0,
            0,
            (Float32)_pendingRenderPass.Framebuffer->Description.Attachments[0]->Description.ViewTexture->Description.Size.X,
            (Float32)_pendingRenderPass.Framebuffer->Description.Attachments[0]->Description.ViewTexture->Description.Size.Y};

        // Set view port.
        SetViewport(frameSize,
                    0.0f,
                    1.0f);

        SetScissorRectangle({0, 0, (int)frameSize.Width, (int)frameSize.Height});

        _renderPassUpToDate = true;
    }
}

void VulkanDeviceContext::CommitPipelineBinding()
{
    if (!_pipelineBindingUpToDate)
    {
        _currentVulkanCommandBuffer->AddResourceStrongReference(GetCurrentBindingPipeline());

        AXIS_ASSERT(GetCurrentBindingPipeline(), "_bindingPipeline is nullptr!");

        VkPipeline pipeline = VK_NULL_HANDLE;

        switch (GetCurrentBindingPipeline()->Binding)
        {
            case PipelineBinding::Graphics:
                pipeline = ((VulkanGraphicsPipeline*)GetCurrentBindingPipeline())->GetVkPipelineHandle();
                break;
        }

        vkCmdBindPipeline(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                          VulkanUtility::GetVkPipelineBindPointFromPipelineBinding(GetCurrentBindingPipeline()->Binding),
                          pipeline);


        _pipelineBindingUpToDate = true;
    }
}

void VulkanDeviceContext::CommitVertexBufferBinding()
{
    if (!_vertexBufferBindingUpToDate)
    {
        List<VkBuffer>        bufferBinding;
        List<Uint64>          bufferBindingOffset;
        List<GraphicsAdapter> graphicsAdapters = GetCreatorDevice()->GraphicsSystem->GetGraphicsAdapters();

        bufferBinding.ReserveFor(GetCurrentBindingVertexBuffers().GetLength());
        bufferBindingOffset.ReserveFor(GetCurrentBindingVertexBuffers().GetLength());

        for (const auto& bindingBuffer : GetCurrentBindingVertexBuffers())
        {
            if (bindingBuffer.VertexBuffer)
            {
                bufferBinding.Append(((VulkanBuffer*)bindingBuffer.VertexBuffer)->GetVkBufferHandle());
                bufferBindingOffset.Append(bindingBuffer.Offset);

                _currentVulkanCommandBuffer->AddResourceStrongReference(bindingBuffer.VertexBuffer);
            }
            else
            {
                bufferBinding.Append(_nullVulkanBuffer->GetVkBufferHandle());
                bufferBindingOffset.Append(0);

                _currentVulkanCommandBuffer->AddResourceStrongReference(_nullVulkanBuffer);
            }
        }

        vkCmdBindVertexBuffers(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                               0,
                               graphicsAdapters[GetCreatorDevice()->GraphicsAdapterIndex].Capability.MaxVertexInputBinding,
                               bufferBinding.GetData(),
                               bufferBindingOffset.GetData());


        _vertexBufferBindingUpToDate = true;
    }
}

void VulkanDeviceContext::CommitIndexBufferBinding() noexcept
{
    if (!_indexBufferBindingUpToDate)
    {
        _currentVulkanCommandBuffer->AddResourceStrongReference(GetCurrentBindingIndexBuffer().IndexBuffer);

        vkCmdBindIndexBuffer(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                             ((VulkanBuffer*)GetCurrentBindingIndexBuffer().IndexBuffer)->GetVkBufferHandle(),
                             GetCurrentBindingIndexBuffer().Offset,
                             VulkanUtility::GetVkIndexTypeFromIndexType(GetCurrentBindingIndexBuffer().Type));


        _indexBufferBindingUpToDate = true;
    }
}

void VulkanDeviceContext::CommitResourceHeapBinding(StateTransition stateTransition) noexcept
{
    if (!_resourceHeapUpToDate)
    {
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        switch (GetCurrentBindingPipeline()->Binding)
        {
            case PipelineBinding::Graphics:
                pipelineLayout = ((VulkanGraphicsPipeline*)GetCurrentBindingPipeline())->GetVkPipelineLayoutHandle();
                break;
        }

        Size index = 0;

        for (const auto& resourceHeap : GetCurrentBindingResourceHeaps())
        {
            if (!resourceHeap)
                continue;

            auto vulkanResourceHeap = (VulkanResourceHeap*)resourceHeap;

            _currentVulkanCommandBuffer->AddResourceStrongReference(resourceHeap);

            vulkanResourceHeap->PrepareResourceHeapBinding(*this,
                                                           stateTransition);

            VkDescriptorSet descriptorSet = vulkanResourceHeap->GetCurrentDescriptorSetGroup()->DescriptorSet;

            vkCmdBindDescriptorSets(_currentVulkanCommandBuffer->GetVkCommandBufferHandle(),
                                    VulkanUtility::GetVkPipelineBindPointFromPipelineBinding(GetCurrentBindingPipeline()->Binding),
                                    pipelineLayout,
                                    (Uint32)index,
                                    1,
                                    &descriptorSet,
                                    0,
                                    nullptr);

            index++;
        }

        _resourceHeapUpToDate = true;
    }
}

void VulkanDeviceContext::PreDraw(StateTransition stateTransition) noexcept
{
    CommitPipelineBinding();

    CommitVertexBufferBinding();

    CommitResourceHeapBinding(stateTransition);

    CommitRenderPass();
}

void VulkanDeviceContext::PreDrawIndexed(StateTransition stateTransition) noexcept
{
    PreDraw(stateTransition);

    CommitIndexBufferBinding();
}

} // namespace Axis