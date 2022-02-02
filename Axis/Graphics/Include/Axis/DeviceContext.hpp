/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file DeviceContext.hpp
///
/// \brief Contains `Axis::IDeviceContext` interface class.

#ifndef AXIS_GRAPHICS_DEVICECONTEXT_HPP
#define AXIS_GRAPHICS_DEVICECONTEXT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"
#include "../../../System/Include/Axis/Enum.hpp"
#include "../../../System/Include/Axis/List.hpp"
#include "../../../System/Include/Axis/Rectangle.hpp"
#include "../../../System/Include/Axis/Span.hpp"
#include "../../../System/Include/Axis/Vector3.hpp"
#include "Color.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"


namespace Axis
{

/// Forward declaration
class ITexture;
class ITextureView;
class IFence;
class IRenderPass;
class IFramebuffer;
class IPipeline;
class IBuffer;
class IGraphicsDevice;
class IResourceHeap;
enum class PipelineBinding : Uint8;

/// \brief Render target binding information.
struct RenderTargetBinding final
{
    /// \brief Uses for color attachments.
    ///
    /// \note This filed is required (should not be nullptr or empty)
    ///
    /// Each of the attachment in the array is corresponded to the out index in the fragment shader.
    List<SharedPointer<ITextureView>> RenderTargetViews = {};

    /// \brief Uses for depth stencil attachment. (optional)
    SharedPointer<ITextureView> DepthStencilView = nullptr;
};

/// \brief Integer types available to use as index buffer data.
enum class IndexType : Uint8
{
    /// \brief Unsigned 16-Bit integer.
    Uint16,

    /// \brief Unsigned 32-Bit integer.
    Uint32,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Uint32,
};

/// \brief Specifies the depth stencil buffer clear values.
enum class ClearDepthStencil : Uint8
{
    /// \brief Clears the depth value.
    Depth = AXIS_BIT(1),

    /// \brief Clears the stencil value.
    Stencil = AXIS_BIT(2),

    /// \brief Required for enum reflection.
    MaximumEnumValue = Stencil,
};

/// \brief Memory access type when maps the resource memory.
enum class MapAccess : Uint8
{
    /// \brief Maps buffer for read operations.
    Read = AXIS_BIT(1),

    /// \brief Maps buffer for write operations.
    Write = AXIS_BIT(2),

    /// \brief Maps buffer for read and write operations
    ReadWrite = Read | Write,
};

/// \brief Resource mapping type;
enum class MapType : Uint8
{
    /// \brief Default resource binding.
    Default = 0,

    /// \brief Discards the old buffer content; looks for new memory region
    ///        to map immediately.
    Discard = 1,

    /// \brief Returns the mapped memory pointer immediately; be careful with
    ///        synchronization issues cause, GPU might be using the resource while
    ///        it's content is being modified.
    Overwrite = 2,
};

/// \brief Specifies the depth stencil buffer clear values. (Bit mask)
typedef ClearDepthStencil ClearDepthStencilFlags;

/// \brief Represents the port of the execution of graphics commands.
class AXIS_GRAPHICS_API IDeviceContext : public DeviceChild
{
public:
    /// \brief Indicates which device queue family index will this device context
    ///        execute their commands to.
    const Uint32 DeviceQueueFamilyIndex = {};

    /// \brief The index of the device queue in the device queue family.
    const Uint32 DeviceQueueIndex = {};

    /// \brief Specifies all the supported operations for this device context.
    const QueueOperationFlags SupportedQueueOperations = {};

    /// \brief Blocks the current thread until all the command submissions are done.
    virtual void WaitQueueIdle() const = 0;

    /// \brief Transits the texture's resource state.
    ///
    /// \param[in] textureResource Texture to transits the resource, If the texture is in managed state, the engine
    ///                            will automatically track the resource state which texture transited into.
    /// \param[in] initialState The resource state which texture is in.
    /// \param[in] finalState The resource state which texture will be transited to.
    /// \param[in] baseArrayIndex The index of the texture's array to start transition to.
    /// \param[in] arrayLevelCount The number of texture's array to transition, starts from the \a baseArrayIndex.
    /// \param[in] baseMipLevel The mip level of the texture to start transition to.
    /// \param[in] mipLevelCount The number of texture's mip level to transition, starts from the \a baseMipLevel.
    /// \param[in] discardContent Discards the content of the resource if possible for the better performance.
    /// \param[in] recordState Specifies whether to record the \a `finalState` into the resource.
    virtual void TransitTextureState(const SharedPointer<ITexture>& textureResource,
                                     ResourceState                  initialState,
                                     ResourceState                  finalState,
                                     Uint32                         baseArrayIndex,
                                     Uint32                         arrayLevelCount,
                                     Uint32                         baseMipLevel,
                                     Uint32                         mipLevelCount,
                                     Bool                           discardContent,
                                     Bool                           recordState) = 0;

    /// \brief Transits the buffer's resource state.
    ///
    /// \param[in] bufferResource Buffer to transits the resource, If the buffer is in managed state, the engine
    ///                            will automatically track the resource state which buffer transited into.
    /// \param[in] initialState The resource state which buffer is in.
    /// \param[in] finalState The resource state which buffer will be transited to.
    /// \param[in] discardContent Discards the content of the resource if possible for the better performance.
    /// \param[in] recordState Specifies whether to record the \a `finalState` into the resource.
    virtual void TransitBufferState(const SharedPointer<IBuffer>& bufferResource,
                                    ResourceState                 initialState,
                                    ResourceState                 finalState,
                                    Bool                          discardContent,
                                    Bool                          recordState) = 0;

    /// \brief Copies the data from one buffer to the another.
    ///
    /// \note Supported queue types : Graphics, Compute, Transfer
    ///       Render pass scope     : Outside
    ///
    /// \param[in] sourceBuffer The source of data transference.
    /// \param[in] sourceOffset The memory offset in the \a sourceBuffer to copy the data from.
    /// \param[in] destBuffer The destination of the data transference.
    /// \param[in] destOffset The memory offset in the \a destOffset to copy data into.
    /// \param[in] copySize The amount of memory to copy.
    /// \param[in] sourceBufferStateTransition Specifies the resource's state transition behavior of \a sourceBuffer.
    /// \param[in] destBufferStateTransition Specifies the resource's state transition behavior of \a destBuffer.
    virtual void CopyBuffer(const SharedPointer<IBuffer>& sourceBuffer,
                            Size                          sourceOffset,
                            const SharedPointer<IBuffer>& destBuffer,
                            Size                          destOffset,
                            Size                          copySize,
                            StateTransition               sourceBufferStateTransition = StateTransition::Transit,
                            StateTransition               destBufferStateTransition   = StateTransition::Transit) = 0;

    /// \brief Binds the pipeline to the context.
    ///
    /// \note Supported queue types : Graphics, Compute
    ///       Render pass scope     : Both
    ///
    /// \param[in] pipeline IPipeline instance to bind to the context. \a `IPipeline::Description::Binding `will specify which Queue type is required.
    virtual void BindPipeline(const SharedPointer<IPipeline>& pipeline) = 0;

    /// \brief Set the Viewport.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] viewportArea X, Y are reserved for offset, Width and Height are reserved for extend.
    /// \param[in] minDepth Minimum of depth range, must be in range of [0.0, 1.0]
    /// \param[in] maxDepth Maximum of depth range, must be in range of [0.0, 1.0]
    virtual void SetViewport(const RectangleF& viewportArea,
                             Float32           minDepth,
                             Float32           maxDepth) = 0;

    /// \brief Set the scissor rectangle.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] rectangle X, Y are reserved for offset, Width and Height are reserved for extend.
    virtual void SetScissorRectangle(const RectangleI& rectangle) = 0;

    /// \brief Clear the depth stencil attachment to the specified values.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] depthStencilView Depth stencil view to clear.
    /// \param[in] stencilClearValue Clear value for stencil.
    /// \param[in] depthClearValue Clear value for depth.
    /// \param[in] clearDepthStencilFlags Indicates which values to clear.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    virtual void ClearDepthStencilView(const SharedPointer<ITextureView>& depthStencilView,
                                       Uint8                              stencilClearValue,
                                       Float32                            depthClearValue,
                                       ClearDepthStencilFlags             clearDepthStencilFlags = ClearDepthStencil::Depth | ClearDepthStencil::Stencil,
                                       StateTransition                    stateTransition        = StateTransition::Transit) = 0;

    /// \brief Clear the render target attachment to the specified color.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] renderTargetView Render target to clear color.
    /// \param[in] clearColor Clear color.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    ///
    virtual void ClearRenderTarget(const SharedPointer<ITextureView>& renderTargetView,
                                   const ColorF&                      clearColor,
                                   StateTransition                    stateTransition = StateTransition::Transit) = 0;

    /// \brief Sets the render target to the context, also sets the viewport and scissor rectangle.
    ///        to match the render target view.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Outside
    ///
    /// \param[in] renderTargetBinding Contains the information needed to set render target.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    ///
    virtual void SetRenderTarget(const RenderTargetBinding& renderTargetBinding,
                                 StateTransition            stateTransition = StateTransition::Transit) = 0;

    /// \brief Binds the vertex buffers to the context.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] firstBinding The index of first vertex binding input.
    /// \param[in] vertexBuffers Array of buffers to be bound as vertex buffer.
    ///                          All the buffers in the array must include the binding flag of BufferBinding::Vertex.
    /// \param[in] offsets Offset of the vertex buffers (in bytes). If passes nullptr (empty) span the engine we assume
    ///                    that the offsets for all vertex buffers are zero, if not, the span length should matches the length of
    ///                    \a vertexBuffers span.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    ///
    virtual void BindVertexBuffers(Uint32                              firstBinding,
                                   const Span<SharedPointer<IBuffer>>& vertexBuffers,
                                   const Span<Size>&                   offsets,
                                   StateTransition                     stateTransition = StateTransition::Transit) = 0;

    /// \brief Binds the index buffer to the context.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] indexBuffer The buffer to be bound as index buffer, must includes the binding flag of BufferBinding::Index.
    /// \param[in] offset Offset (in bytes) of the buffer to be bound as index data.
    /// \param[in] indexType Integer type that contained in the index buffer.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    ///
    virtual void BindIndexBuffer(const SharedPointer<IBuffer>& indexBuffer,
                                 Size                          offset,
                                 IndexType                     indexType,
                                 StateTransition               stateTransition = StateTransition::Transit) = 0;

    /// \brief Binds the resource heap to the context.
    ///
    /// \note Supported queue types : Graphics | Compute
    ///       Render pass scope     : Both
    ///
    /// \param[in] resourceHeap The resource heap to bind.
    ///
    virtual void BindResourceHeap(const SharedPointer<IResourceHeap>& resourceHeap) = 0;

    /// \brief Draw primitives.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Inside
    ///
    /// \param[in] vertexCount Number of vertices to draw.
    /// \param[in] instanceCount Number of instances to draw.
    /// \param[in] firstVertex The index of the first vertex to draw.
    /// \param[in] firstInstance The number of the first instance to draw.
    /// \param[in] bindingResourcesStateTransition Specifies the state transition of resources that are bound to
    ///                                            the resource heap. If the \a `StateTransition::Explicit` is used, all the resources
    ///                                            that are bound to the resource heaps need to be explicitly transited to the required
    ///                                            resource state.
    virtual void Draw(Uint32          vertexCount,
                      Uint32          instanceCount,
                      Uint32          firstVertex,
                      Uint32          firstInstance,
                      StateTransition bindingResourcesStateTransition = StateTransition::Transit) = 0;

    /// \brief Draw indexed primitives.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Both
    ///
    /// \param[in] indexCount Number of vertices to draw.
    /// \param[in] instanceCount Number of instances to draw.
    /// \param[in] firstIndex The index of the first vertex to draw.
    /// \param[in] vertexOffset The value added to the vertex index before indexing into the vertex buffer.
    /// \param[in] firstInstance The number of the first instance to draw.
    /// \param[in] bindingResourcesStateTransition Specifies the state transition of resources that are bound to
    ///                                            the resource heap. If the \a `StateTransition::Explicit` is used, all the resources
    ///                                            that are bound to the resource heaps need to be explicitly transited to the required
    ///                                            resource state.
    virtual void DrawIndexed(Uint32          indexCount,
                             Uint32          instanceCount,
                             Uint32          firstIndex,
                             Uint32          vertexOffset,
                             Uint32          firstInstance,
                             StateTransition bindingResourcesStateTransition = StateTransition::Transit) = 0;

    /// \brief Copies the data from the buffer to texture.
    ///
    /// \note Supported queue types : Graphics | Transfer | Compute
    ///       Render pass scope     : Outside
    ///
    /// \param[in] sourceBuffer Buffer to copy the data from.
    /// \param[in] bufferOffset Offset in the buffer to start copy the data from.
    /// \param[in] destTexture Texture to copy the data into.
    /// \param[in] baseArrayIndex The index of the texture's array to start copy data into.
    /// \param[in] arrayLevelCount The number of texture's array to copy data into, starts from the \a baseArrayIndex.
    /// \param[in] mipLevel Specifies at which  mip level to copy data into.
    /// \param[in] textureOffset The offset of the texture to start copy the data into.
    /// \param[in] textureSize The size of the texture to copy the data into.
    /// \param[in] bufferStateTransition Specifies the resource's state transition behavior of \a sourceBuffer.
    /// \param[in] textureStateTransition Specifies the resource's state transition behavior of \a destTexture.
    virtual void CopyBufferToTexture(const SharedPointer<IBuffer>&  sourceBuffer,
                                     Size                           bufferOffset,
                                     const SharedPointer<ITexture>& destTexture,
                                     Uint32                         baseArrayIndex,
                                     Uint32                         arrayLevelCount,
                                     Uint32                         mipLevel,
                                     Vector3UI                      textureOffset,
                                     Vector3UI                      textureSize,
                                     StateTransition                bufferStateTransition  = StateTransition::Transit,
                                     StateTransition                textureStateTransition = StateTransition::Transit) = 0;

    /// \brief Generates full mip map chains, starts from the base.
    ///
    /// \note Supported queue types : Graphics
    ///       Render pass scope     : Outside
    ///
    /// \param[in] textureView View to the texture to generate the mips.
    /// \param[in] stateTransition Specifies the resource's state transition behavior.
    virtual void GenerateMips(const SharedPointer<ITextureView>& textureView,
                              StateTransition                    stateTransition = StateTransition::Transit) = 0;

    /// \brief Mpas the buffer for read or write operations.
    ///
    /// \note Supported queu types : All
    ///       Render pass scope    : Outside
    ///
    /// \param[in] buffer Buffer to map the memory.
    /// \param[in] mapAccess Specifies which memory access to operate to the mapped memory.
    /// \param[in] mapType Specifies meomry mapping behaviour.
    virtual PVoid MapBuffer(const SharedPointer<IBuffer>& buffer,
                            MapAccess                     mapAccess,
                            MapType                       mapType) = 0;

    /// \brief Unmaps the buffer memory
    ///
    /// \note Supported queu types : All
    ///       Render pass scope    : Outside
    ///
    /// \param[in] buffer Buffer to unmap its memory.
    virtual void UnmapBuffer(const SharedPointer<IBuffer>& buffer) = 0;

    /// \brief Appends fence to signal upon the next Flush call and all works are done.
    ///
    /// \param[in] fence Fence to be signaled.
    /// \param[in] fenceValue Value to set to the fence.
    ///
    virtual void AppendSignalFence(const SharedPointer<IFence>& fence,
                                   Uint64                       fenceValue) = 0;

    /// \brief Appends fence to wait for the specified value upon the next flush call.
    ///        GPU (NOT CPU!) will block its executions until the fence's value is satisfied
    ///
    /// \param[in] fence Fence to wait.
    /// \param[in] fenceValue Value to wait for the fence.
    ///
    virtual void AppendWaitFence(const SharedPointer<IFence>& fence,
                                 Uint64                       fenceValue) = 0;

    /// \brief Flushes all pending commands to execute.
    ///
    virtual void Flush() = 0;

protected:
    /// \brief Constructor
    ///
    IDeviceContext(IGraphicsDevice&    graphicsDevice,
                   Uint32              deviceQueueFamilyIndex,
                   Uint32              deviceIndex,
                   QueueOperationFlags supportedQueueOperations);

    /// \brief Current activating render pass scope
    ///
    /// \private
    ///
    struct RenderPassScope final
    {
        /// \brief Current render pass instance
        ///
        SharedPointer<IRenderPass> RenderPass = nullptr;

        /// \brief Target framebuffer
        ///
        SharedPointer<IFramebuffer> Framebuffer = nullptr;
    };

    /// \brief Viewport binding info.
    ///
    /// \private
    ///
    struct Viewport
    {
        /// \brief Rendering area in the 2D TextureView
        ///
        RectangleF RenderArea = {};

        /// \brief Minimum depth range.
        ///
        Float32 MinDepth = 0.0f;

        /// \brief Maximum depth range.
        ///
        Float32 MaxDepth = 0.0f;
    };

    /// \brief Represents the vertex buffer binding
    ///
    /// \private
    ///
    struct VertexBufferBinding
    {
        /// \brief Binding vertex buffer.
        ///
        SharedPointer<IBuffer> VertexBuffer = {};

        /// \brief Vertex buffer binding offset.
        ///
        Size Offset = {};
    };
    /// \brief Current binding index buffer
    ///
    /// \private
    ///
    struct IndexBufferBinding
    {
        /// \brief Strong reference to index buffer.
        ///
        SharedPointer<IBuffer> IndexBuffer = nullptr;

        /// \brief Index buffer offset.
        ///
        Size Offset = 0;

        /// \brief Index type.
        ///
        IndexType Type = IndexType::Uint16;
    };

    // SetRenderTarget
    inline const RenderTargetBinding& GetCurrentRenderTargetBinding() const noexcept { return _currentRenderTargetBinding; }
    // BindPipeline
    inline const SharedPointer<IPipeline>& GetCurrentBindingPipeline() const noexcept { return _currentBindingPipeline; }
    // BindVertexBuffers
    inline const List<VertexBufferBinding>& GetCurrentBindingVertexBuffers() const noexcept { return _bindingVertexBuffers; }
    // BindIndexBuffer
    inline const IndexBufferBinding& GetCurrentBindingIndexBuffer() const noexcept { return _bindingIndexBuffer; }
    // BindResourceHeap
    inline const List<SharedPointer<IResourceHeap>>& GetCurrentBindingResourceHeaps() const noexcept { return _bindingResourceHeaps; }
    // Resets the storing render target state.
    inline void ResetRenderTarget() noexcept { _currentRenderTargetBinding = {}; }
    // Resets the storing vertex buffer.
    inline void ResetVertexBuffers() noexcept { _bindingVertexBuffers.Reset(); }
    // Resets the storing index buffer.
    inline void ResetIndexBuffer() noexcept { _bindingIndexBuffer = {}; }

private:
    /// Private members
    ///
    RenderTargetBinding                _currentRenderTargetBinding = {};
    IDeviceContext::Viewport           _currentViewport            = {};
    RectangleI                         _currentScissorRectangle    = {};
    SharedPointer<IPipeline>           _currentBindingPipeline     = {};
    List<VertexBufferBinding>          _bindingVertexBuffers       = {};
    IndexBufferBinding                 _bindingIndexBuffer         = {};
    List<SharedPointer<IResourceHeap>> _bindingResourceHeaps       = {};
};

} // namespace Axis

#endif // AXIS_GRAPHICS_DEVICECONTEXT_HPP