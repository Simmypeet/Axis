/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/Fence.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsPipeline.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/Pipeline.hpp>
#include <Axis/ResourceHeap.hpp>
#include <Axis/Texture.hpp>
#include <Axis/Utility.hpp>

namespace Axis
{

namespace Graphics
{

// Default constructor
IDeviceContext::IDeviceContext(IGraphicsDevice&    graphicsDevice,
                               Uint32              deviceQueueFamilyIndex,
                               Uint32              deviceIndex,
                               QueueOperationFlags supportedQueueOperations) :
    DeviceQueueFamilyIndex(deviceQueueFamilyIndex),
    DeviceQueueIndex(deviceIndex),
    SupportedQueueOperations(supportedQueueOperations),
    _bindingVertexBuffers(graphicsDevice.GraphicsSystem->GetGraphicsAdapters()[graphicsDevice.GraphicsAdapterIndex].Capability.MaxVertexInputBinding) {}

void IDeviceContext::TransitTextureState(const System::SharedPointer<ITexture>& textureResource,
                                         ResourceState                          initialState,
                                         ResourceState                          finalState,
                                         Uint32                                 baseArrayIndex,
                                         Uint32                                 arrayLevelCount,
                                         Uint32                                 baseMipLevel,
                                         Uint32                                 mipLevelCount,
                                         Bool                                   discardContent,
                                         Bool                                   recordState)
{
    if (!textureResource)
        throw System::InvalidArgumentException("textureResource was nullptr!");

    if (!Graphics::IsTextureState(initialState))
        throw System::InvalidArgumentException("initialState was not a valid texture state!");

    if (!Graphics::IsTextureState(finalState))
        throw System::InvalidArgumentException("finalState was not a valid texture state!");

    if (baseArrayIndex + arrayLevelCount > textureResource->Description.ArraySize)
        throw System::InvalidArgumentException("baseArrayIndex + arrayLevelCount was greater than textureResource->Description.ArraySize!");

    if (baseMipLevel + mipLevelCount > textureResource->Description.MipLevels)
        throw System::InvalidArgumentException("baseMipLevel + mipLevelCount was greater than textureResource->Description.MipLevels!");
}

void IDeviceContext::TransitBufferState(const System::SharedPointer<IBuffer>& bufferResource,
                                        ResourceState                         initialState,
                                        ResourceState                         finalState,
                                        Bool                                  discardContent,
                                        Bool                                  recordState)
{
    if (!bufferResource)
        throw System::InvalidArgumentException("bufferResource was nullptr!");

    if (!Graphics::IsBufferState(initialState))
        throw System::InvalidArgumentException("initialState was not a valid buffer state!");

    if (!Graphics::IsBufferState(finalState))
        throw System::InvalidArgumentException("finalState was not a valid buffer state!");
}

void IDeviceContext::CopyBuffer(const System::SharedPointer<IBuffer>& sourceBuffer,
                                Size                                  sourceOffset,
                                const System::SharedPointer<IBuffer>& destBuffer,
                                Size                                  destOffset,
                                Size                                  copySize,
                                StateTransition                       sourceBufferStateTransition,
                                StateTransition                       destBufferStateTransition)
{
    if (!sourceBuffer)
        throw System::InvalidArgumentException("sourceBuffer was nullptr!");

    if (!destBuffer)
        throw System::InvalidArgumentException("destBuffer was nullptr!");

    if (sourceOffset + copySize > sourceBuffer->Description.BufferSize)
        throw System::InvalidArgumentException("sourceOffset + copySize was greater than sourceBuffer->Description.BufferSize!");

    if (destOffset + copySize > destBuffer->Description.BufferSize)
        throw System::InvalidArgumentException("destOffset + copySize was greater than destBuffer->Description.BufferSize!");
}

void IDeviceContext::BindPipeline(const System::SharedPointer<IPipeline>& pipeline)
{
    if (!pipeline)
        throw System::InvalidArgumentException("pipeline was nullptr!");

    Size resourceHeapLayoutCount = 0;

    switch (pipeline->Binding)
    {
        case PipelineBinding::Graphics:
            if (!(Bool)(SupportedQueueOperations & QueueOperationFlags::Graphics))
                throw System::InvalidArgumentException("pipeline->Binding was PipelineBinding::Graphics but this device context did not support graphics operations!");
            resourceHeapLayoutCount = ((IGraphicsPipeline*)pipeline.GetPointer())->Description.ResourceHeapLayouts.GetSize();
            break;

        default:
            throw System::InvalidArgumentException("pipeline->Binding was not a valid pipeline binding!");
    }

    _bindingResourceHeaps.Clear();
    _bindingResourceHeaps.Reserve(resourceHeapLayoutCount);

    _currentBindingPipeline = pipeline;
}

void IDeviceContext::SetViewport(const System::RectangleF& viewportArea,
                                 Float32                   minDepth,
                                 Float32                   maxDepth)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperationFlags::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (minDepth > maxDepth)
        throw System::InvalidArgumentException("minDepth was greater than maxDepth!");

    if (!System::Math::IsInRange(minDepth, 0.0f, 1.0f))
        throw System::InvalidArgumentException("minDepth was not in range [0.0f, 1.0f]!");

    if (!System::Math::IsInRange(maxDepth, 0.0f, 1.0f))
        throw System::InvalidArgumentException("maxDepth was not in range [0.0f, 1.0f]!");

    _currentViewport.RenderArea = viewportArea;
    _currentViewport.MinDepth   = minDepth;
    _currentViewport.MaxDepth   = maxDepth;
}

void IDeviceContext::SetScissorRectangle(const System::RectangleI& rectangle)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperationFlags::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (rectangle.Width < 0)
        throw System::InvalidArgumentException("rectangle.Width was less than 0!");

    if (rectangle.Height < 0)
        throw System::InvalidArgumentException("rectangle.Height was less than 0!");

    _currentScissorRectangle = rectangle;
}

void IDeviceContext::ClearDepthStencilView(const System::SharedPointer<ITextureView>& depthStencilView,
                                           Uint8                                      stencilClearValue,
                                           Float32                                    depthClearValue,
                                           ClearDepthStencilFlags                     clearDepthStencilFlags,
                                           StateTransition                            stateTransition)
{
    if (!depthStencilView)
        throw System::InvalidArgumentException("depthStencilView was nullptr!");

    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!(Bool)(clearDepthStencilFlags & (ClearDepthStencil::Depth | ClearDepthStencil::Stencil)))
        throw System::InvalidArgumentException("clearDepthStencilFlags did not contain any of the ClearDepthStencil flags!");

    if ((Bool)(clearDepthStencilFlags & ClearDepthStencil::Depth))
    {
        if (!System::Math::IsInRange(depthClearValue, 0.0f, 1.0f))
            throw System::InvalidArgumentException("depthClearValue was not in range [0.0f, 1.0f]!");

        if (!(Bool)(depthStencilView->Description.ViewUsage & TextureViewUsage::Depth))
            throw System::InvalidArgumentException("depthStencilView->Description.ViewUsage did not contain TextureViewUsage::Depth!");
    }

    if ((Bool)(clearDepthStencilFlags & ClearDepthStencil::Stencil))
    {
        if (!(Bool)(depthStencilView->Description.ViewUsage & TextureViewUsage::Stencil))
            throw System::InvalidArgumentException("depthStencilView->Description.ViewUsage did not contain TextureViewUsage::Stencil!");
    }
}

void IDeviceContext::ClearRenderTarget(const System::SharedPointer<ITextureView>& renderTargetView,
                                       const ColorF&                              clearColor,
                                       StateTransition                            stateTransition)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!renderTargetView)
        throw System::InvalidArgumentException("renderTargetView was nullptr!");

    if (!(Bool)(renderTargetView->Description.ViewUsage & TextureViewUsage::RenderTarget))
        throw System::InvalidArgumentException("renderTargetView->Description.ViewUsage did not contain TextureViewUsage::RenderTarget!");
}

void IDeviceContext::SetRenderTarget(const RenderTargetBinding& renderTargetBinding,
                                     StateTransition            stateTransition /*StateTransition::Transit*/)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!renderTargetBinding.RenderTargetViews)
        throw System::InvalidArgumentException("renderTargetBinding.RenderTarget was nullptr!");

    if (renderTargetBinding.DepthStencilView)
    {
        if (!(Bool)(renderTargetBinding.DepthStencilView->Description.ViewUsage & TextureViewUsage::Depth))
            throw System::InvalidArgumentException("renderTargetBinding.DepthStencilView->Description.ViewUsage did not contain TextureViewUsage::Depth!");
    }

    for (const auto& renderTargetView : renderTargetBinding.RenderTargetViews)
    {
        if (!renderTargetView)
            throw System::InvalidArgumentException("renderTargetBinding.RenderTargetViews contained a nullptr!");

        if (!(Bool)(renderTargetView->Description.ViewUsage & TextureViewUsage::RenderTarget))
            throw System::InvalidArgumentException("renderTargetBinding.RenderTargetViews->Description.ViewUsage did not contain TextureViewUsage::RenderTarget!");
    }

    _currentRenderTargetBinding = renderTargetBinding;
}

void IDeviceContext::BindVertexBuffers(Uint32                                              firstBinding,
                                       const System::Span<System::SharedPointer<IBuffer>>& vertexBuffers,
                                       const System::Span<Size>&                           offsets,
                                       StateTransition                                     stateTransition /*StateTransition::Transit*/)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!vertexBuffers)
        throw System::InvalidArgumentException("vertexBuffers was nullptr!");

    if (offsets)
    {
        if (offsets.GetSize() != vertexBuffers.GetSize())
            throw System::InvalidArgumentException("offsets.Size() != vertexBuffers.Size()!");
    }

    if (firstBinding + vertexBuffers.GetSize() > _bindingVertexBuffers.GetSize())
        throw System::InvalidArgumentException("firstBinding + vertexBuffers.GetSize() was greater than GraphicsCapability::MaxVertexInputBinding!");

    Size index = 0;
    for (const auto& vertexBuffer : vertexBuffers)
    {
        if (!vertexBuffer)
            continue;

        if (!(Bool)(vertexBuffer->Description.BufferBinding & BufferBinding::Vertex))
            throw System::InvalidArgumentException("vertexBuffers[index]->Description.BufferBinding did not contain BufferBinding::Vertex!");

        if (offsets)
        {
            if (!System::Math::IsInRange(offsets[index], 0, vertexBuffer->Description.BufferSize - 1))
                throw System::InvalidArgumentException("offsets[index] was not in range [0, vertexBuffers[index]->Description.Size - 1]!");
        }
    }

    index = 0;
    for (const auto& vertexBuffer : vertexBuffers)
    {
        if (offsets)
            _bindingVertexBuffers[firstBinding + index] = {vertexBuffer, offsets[index]};
        else
            _bindingVertexBuffers[firstBinding + index] = {vertexBuffer, 0};
        index++;
    }
}

void IDeviceContext::BindIndexBuffer(const System::SharedPointer<IBuffer>& indexBuffer,
                                     Size                                  offset,
                                     IndexType                             indexType,
                                     StateTransition                       stateTransition)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!indexBuffer)
        throw System::InvalidArgumentException("indexBuffer was nullptr!");

    if (!(Bool)(indexBuffer->Description.BufferBinding & BufferBinding::Index))
        throw System::InvalidArgumentException("indexBuffer->Description.BufferBinding did not contain BufferBinding::Index!");

    if (!System::Math::IsInRange(offset, 0, indexBuffer->Description.BufferSize - 1))
        throw System::InvalidArgumentException("offset was not in range [0, indexBuffer->Description.Size - 1]!");

    _bindingIndexBuffer = {indexBuffer, offset, indexType};
}


void IDeviceContext::BindResourceHeap(const System::SharedPointer<IResourceHeap>& resourceHeap)
{
    if (!resourceHeap)
        throw System::InvalidArgumentException("resourceHeap was nullptr!");

    if (!_currentBindingPipeline)
        throw System::InvalidOperationException("this device context did not have a pipeline bound!");

    const System::List<System::SharedPointer<IResourceHeapLayout>>* resourceHeapLayouts = nullptr;

    switch (_currentBindingPipeline->Binding)
    {
        case PipelineBinding::Graphics:
            resourceHeapLayouts = &((IGraphicsPipeline*)_currentBindingPipeline.GetPointer())->Description.ResourceHeapLayouts;
            break;

        default:
            throw System::InvalidOperationException("this device context did not have a pipeline bound!");
    }

    for (auto& bindingResourceHeap : _bindingResourceHeaps)
    {
        if (bindingResourceHeap->Description.ResourceHeapLayout == resourceHeap->Description.ResourceHeapLayout)
        {
            bindingResourceHeap = resourceHeap;
            return;
        }
    }

    for (const auto& resourceHeapLayout : *resourceHeapLayouts)
    {
        if (resourceHeapLayout == resourceHeap->Description.ResourceHeapLayout)
        {
            _bindingResourceHeaps.Append(resourceHeap);
            return;
        }
    }

    throw System::InvalidArgumentException("resourceHeap->Description.ResourceHeapLayout was not found in the pipeline's resource heap layouts!");
}

void IDeviceContext::Draw(Uint32          vertexCount,
                          Uint32          instanceCount,
                          Uint32          firstVertex,
                          Uint32          firstInstance,
                          StateTransition bindingResourcesStateTransition)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!_currentBindingPipeline)
        throw System::InvalidOperationException("this device context did not have a pipeline bound!");

    if (_currentBindingPipeline->Binding != PipelineBinding::Graphics)
        throw System::InvalidOperationException("this device context did not have a graphics pipeline bound!");

    if (!_currentRenderTargetBinding.RenderTargetViews)
        throw System::InvalidOperationException("this device context did not have a render target bound!");
}

void IDeviceContext::DrawIndexed(Uint32          indexCount,
                                 Uint32          instanceCount,
                                 Uint32          firstIndex,
                                 Uint32          vertexOffset,
                                 Uint32          firstInstance,
                                 StateTransition bindingResourcesStateTransition)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!_currentBindingPipeline)
        throw System::InvalidOperationException("this device context did not have a pipeline bound!");

    if (_currentBindingPipeline->Binding != PipelineBinding::Graphics)
        throw System::InvalidOperationException("this device context did not have a graphics pipeline bound!");

    if (!_currentRenderTargetBinding.RenderTargetViews)
        throw System::InvalidOperationException("this device context did not have a render target bound!");

    if (!_bindingIndexBuffer.IndexBuffer)
        throw System::InvalidOperationException("this device context did not have an index buffer bound!");
}

void IDeviceContext::CopyBufferToTexture(const System::SharedPointer<IBuffer>&  sourceBuffer,
                                         Size                                   bufferOffset,
                                         const System::SharedPointer<ITexture>& destTexture,
                                         Uint32                                 baseArrayIndex,
                                         Uint32                                 arrayLevelCount,
                                         Uint32                                 mipLevel,
                                         System::Vector3UI                      textureOffset,
                                         System::Vector3UI                      textureSize,
                                         StateTransition                        bufferStateTransition,
                                         StateTransition                        textureStateTransition)

{
    if (!sourceBuffer)
        throw System::InvalidArgumentException("sourceBuffer was nullptr!");

    if (!destTexture)
        throw System::InvalidArgumentException("destTexture was nullptr!");

    if (!(Bool)(sourceBuffer->Description.BufferBinding & BufferBinding::TransferSource))
        throw System::InvalidArgumentException("sourceBuffer->Description.BufferBinding did not contain BufferBinding::TransferSource!");

    if (!(Bool)(destTexture->Description.TextureBinding & TextureBinding::TransferDestination))
        throw System::InvalidArgumentException("destTexture->Description.TextureBinding did not contain TextureBinding::TransferDestination!");

    if (bufferOffset >= sourceBuffer->Description.BufferSize)
        throw System::InvalidArgumentException("bufferOffset was greater than or equal to sourceBuffer->Description.Size!");

    if (baseArrayIndex + arrayLevelCount > destTexture->Description.ArraySize)
        throw System::InvalidArgumentException("baseArrayIndex + arrayLevelCount was greater than destTexture->Description.ArraySize!");

    if (mipLevel >= destTexture->Description.MipLevels)
        throw System::InvalidArgumentException("mipLevel was greater than or equal to destTexture->Description.MipLevels!");
}

void IDeviceContext::GenerateMips(const System::SharedPointer<ITextureView>& textureView,
                                  StateTransition                            stateTransition)
{
    if (!(Bool)(SupportedQueueOperations & QueueOperation::Graphics))
        throw System::InvalidOperationException("this device context did not support graphics operations!");

    if (!textureView)
        throw System::InvalidArgumentException("textureView was nullptr!");
}

void IDeviceContext::AppendSignalFence(const System::SharedPointer<IFence>& fence,
                                       Uint64                               fenceValue)
{
    if (!fence)
        throw System::InvalidArgumentException("fence was nullptr!");
}

void IDeviceContext::AppendWaitFence(const System::SharedPointer<IFence>& fence,
                                     Uint64                               fenceValue)
{
    if (!fence)
        throw System::InvalidArgumentException("fence was nullptr!");
}

void IDeviceContext::Flush()
{
    _currentViewport            = {};
    _currentScissorRectangle    = {};
    _currentRenderTargetBinding = {};
    _bindingIndexBuffer         = {};
    _bindingResourceHeaps.Clear();
    _bindingVertexBuffers.Reset();
}

} // namespace Graphics

} // namespace Axis