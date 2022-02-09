/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANDEVICECONTEXT_HPP
#define AXIS_VULKANDEVICECONTEXT_HPP
#pragma once

#include "../../../Include/Axis/DeviceContext.hpp"
#include "../../../Include/Axis/GraphicsDevice.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanFramebufferCache.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
struct VulkanDeviceQueue;
struct VulkanDeviceQueueFamily;
struct VulkanCommandPool;
struct VulkanCommandBuffer;
class VulkanGraphicsDevice;
class VulkanBuffer;

// An implementation of IDeviceContext interface in Vulkan backend
class VulkanDeviceContext final : public IDeviceContext
{
public:
    // Constructor
    VulkanDeviceContext(Uint32                deviceQueueFamilyIndex,
                        Uint32                deviceQueueIndex,
                        QueueOperationFlags   supportedQueueOperations,
                        VulkanGraphicsDevice& vulkanGraphicsDevice);

    // Destructor
    ~VulkanDeviceContext() noexcept override final;

    // An implementation of IDeviceContext::WaitQueueIdle in Vulkan backend
    void WaitQueueIdle() const noexcept override final;

    // An implementation of IDeviceContext::TransitTextureState in Vulkan backend
    void TransitTextureState(const System::SharedPointer<ITexture>& textureResource,
                             ResourceState                          initialState,
                             ResourceState                          finalState,
                             Uint32                                 baseArrayIndex,
                             Uint32                                 arrayLevelCount,
                             Uint32                                 baseMipLevel,
                             Uint32                                 mipLevelCount,
                             Bool                                   discardContent,
                             Bool                                   recordState) override final;

    // An implementation of IDeviceContext::TransitBufferState in Vulkan backend
    void TransitBufferState(const System::SharedPointer<IBuffer>& bufferResource,
                            ResourceState                         initialState,
                            ResourceState                         finalState,
                            Bool                                  discardContent,
                            Bool                                  recordState) override final;

    // An implementation of IDeviceContext::CopyBuffer in Vulkan backend
    void CopyBuffer(const System::SharedPointer<IBuffer>& sourceBuffer,
                    Size                                  sourceOffset,
                    const System::SharedPointer<IBuffer>& destBuffer,
                    Size                                  destOffset,
                    Size                                  copySize,
                    StateTransition                       sourceBufferStateTransition,
                    StateTransition                       destBufferStateTransition) override final;

    // An implementation of IDeviceContext::BindPipeline in Vulkan backend
    void BindPipeline(const System::SharedPointer<IPipeline>& pipeline) override final;

    // An implementation of IDeviceContext::SetViewport in Vulkan backend
    void SetViewport(const System::RectangleF& viewportArea,
                     Float32                   minDepth,
                     Float32                   maxDepth) override final;

    // An implementation of IDeviceContext::SetScissorRectangle in Vulkan backend
    void SetScissorRectangle(const System::RectangleI& rectangle) override final;

    // An implementation of ClearDepthStencilView::ClearRenderTarget in Vulkan backend
    void ClearDepthStencilView(const System::SharedPointer<ITextureView>& depthStencilView,
                               Uint8                                      stencilClearValue,
                               Float32                                    depthClearValue,
                               ClearDepthStencilFlags                     clearDepthStencilFlags,
                               StateTransition                            stateTransition) override final;

    // An implementation of IDeviceContext::ClearRenderTarget in Vulkan backend
    void ClearRenderTarget(const System::SharedPointer<ITextureView>& renderTargetView,
                           const ColorF&                              clearColor,
                           StateTransition                            stateTransition) override final;

    // An implementation of IDeviceContext::SetRenderTarget in Vulkan backend
    void SetRenderTarget(const RenderTargetBinding& renderTargetBinding,
                         StateTransition            stateTransition) override final;

    // An implementation of IDeviceContext::BindVertexBuffers in Vulkan backend
    void BindVertexBuffers(Uint32                                              firstBinding,
                           const System::Span<System::SharedPointer<IBuffer>>& vertexBuffers,
                           const System::Span<Size>&                           offsets,
                           StateTransition                                     stateTransition) override final;

    // An implementation of IDeviceContext::BindIndexBuffer in Vulkan backend
    ///
    void BindIndexBuffer(const System::SharedPointer<IBuffer>& indexBuffer,
                         Size                                  offset,
                         IndexType                             indexType,
                         StateTransition                       stateTransition) override final;

    // An implementation of IDeviceContext::BindResourceHeap in Vulkan backend
    void BindResourceHeap(const System::SharedPointer<IResourceHeap>& resourceHeap) override final;

    // An implementation of IDeviceContext::Draw in Vulkan backend
    void Draw(Uint32          vertexCount,
              Uint32          instanceCount,
              Uint32          firstVertex,
              Uint32          firstInstance,
              StateTransition bindingResourcesStateTransition) override final;

    // An implementation of IDeviceContext::DrawIndexed in Vulkan backend
    void DrawIndexed(Uint32          indexCount,
                     Uint32          instanceCount,
                     Uint32          firstIndex,
                     Uint32          vertexOffset,
                     Uint32          firstInstance,
                     StateTransition bindingResourcesStateTransition) override final;

    // An implementation of IDevivecContext::CopyBufferToTexture in Vulkan backend
    void CopyBufferToTexture(const System::SharedPointer<IBuffer>&  sourceBuffer,
                             Size                                   bufferOffset,
                             const System::SharedPointer<ITexture>& destTexture,
                             Uint32                                 baseArrayIndex,
                             Uint32                                 arrayLevelCount,
                             Uint32                                 mipLevel,
                             System::Vector3UI                      textureOffset,
                             System::Vector3UI                      textureSize,
                             StateTransition                        bufferStateTransition,
                             StateTransition                        textureStateTransition) override final;

    // An implementation of IDeviceContext::GenerateMips in Vulkan backend
    void GenerateMips(const System::SharedPointer<ITextureView>& textureView,
                      StateTransition                            stateTransition) override final;

    // An implementation of IDeviceContext::MapBuffer in Vulkan backend
    PVoid MapBuffer(const System::SharedPointer<IBuffer>& buffer,
                    MapAccess                             mapAccess,
                    MapType                               mapType) override final;

    // An implementation of IDeviceContext::UnmapMemory in Vulkan backend
    void UnmapBuffer(const System::SharedPointer<IBuffer>& buffer) override final;

    // An implementation of IDeviceContext::AppendSignalFence in Vulkan backend
    void AppendSignalFence(const System::SharedPointer<IFence>& fence,
                           Uint64                               fenceValue) override final;

    // An implementation of IDeviceContext::AppendWaitFence in Vulkan backend
    void AppendWaitFence(const System::SharedPointer<IFence>& fence,
                         Uint64                               fenceValue) override final;

    // An implementation of IDeviceContext::Flush in Vulkan backend
    void Flush() override final;

    // Gets the internal VulkanDeviceQueue object.
    VulkanDeviceQueue& GetVulkanDeviceQueue() const noexcept;

    // Gets the VulkanCommandPool.
    inline VulkanCommandPool* GetVulkanCommandPool() noexcept { return _vulkanCommandPool.GetPointer(); }

    // Gets current VulkanCommandBuffer.
    inline VulkanCommandBuffer* GetVulkanCommandBuffer() noexcept { return _currentVulkanCommandBuffer.GetPointer(); }

private:
    void CommitRenderPass();                                                  // Binds the pending render target and starts an implicit render pass. (Subpass with no dependency)
    void CommitPipelineBinding();                                             // Binds the stored pipeline binding upon draw commands.
    void CommitVertexBufferBinding();                                         // Binds the vertex buffer upon draw commands.
    void CommitIndexBufferBinding() noexcept;                                 // Binds the index buffer upon draw commands.
    void CommitResourceHeapBinding(StateTransition stateTransition) noexcept; // Binds descriptor set upon draw / dispatch commands .
    void PreDraw(StateTransition stateTransition) noexcept;                   // Commits RenderPass, PipelineBinding and VertexBuffers if required.
    void PreDrawIndexed(StateTransition stateTransition) noexcept;            // Commits RenderPass, PipelineBinding, VertexBuffers and IndexBuffer if required.

    Uint32                                     _deviceQueueFamilyIndex      = 0;
    Uint32                                     _deviceQueueIndex            = 0;
    System::UniquePointer<VulkanCommandPool>   _vulkanCommandPool           = {};
    System::UniquePointer<VulkanCommandBuffer> _currentVulkanCommandBuffer  = {};
    Bool                                       _renderPassUpToDate          = true;
    IDeviceContext::RenderPassScope            _pendingRenderPass           = {};
    Bool                                       _pipelineBindingUpToDate     = true;
    Bool                                       _vertexBufferBindingUpToDate = true;
    Bool                                       _indexBufferBindingUpToDate  = true;
    Bool                                       _resourceHeapUpToDate        = true;
    System::SharedPointer<VulkanBuffer>        _nullVulkanBuffer            = nullptr;
    VulkanFramebufferCache                     _framebufferCache;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANDEVICECONTEXT_HPP