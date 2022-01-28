/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file GraphicsDevice.hpp
///
/// \brief Contains `Axis::IGraphicsDevice` interface class.

#ifndef AXIS_GRAPHICS_GRAPHICSDEVICE_HPP
#define AXIS_GRAPHICS_GRAPHICSDEVICE_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"
#include "../../../System/Include/Axis/List.hpp"
#include "../../../System/Include/Axis/SmartPointer.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

/// Forward declarations
class DisplayWindow;



#define AXIS_DECLARE_GRAPHICSRESOURCE(type) \
    class I##type;                          \
    struct type##Description;

AXIS_DECLARE_GRAPHICSRESOURCE(Texture)
AXIS_DECLARE_GRAPHICSRESOURCE(TextureView)
AXIS_DECLARE_GRAPHICSRESOURCE(Framebuffer)
AXIS_DECLARE_GRAPHICSRESOURCE(RenderPass)
AXIS_DECLARE_GRAPHICSRESOURCE(ShaderModule)
AXIS_DECLARE_GRAPHICSRESOURCE(ShaderModule)
AXIS_DECLARE_GRAPHICSRESOURCE(ResourceHeapLayout)
AXIS_DECLARE_GRAPHICSRESOURCE(GraphicsPipeline)
AXIS_DECLARE_GRAPHICSRESOURCE(Buffer)
AXIS_DECLARE_GRAPHICSRESOURCE(ResourceHeap)
AXIS_DECLARE_GRAPHICSRESOURCE(Sampler)

#undef AXIS_DECLARE_GRAPHICSRESOURCE

/// Forward declarations
class DeviceChild;
class ISwapChain;
class IGraphicsSystem;
struct SwapChainDescription;
class IDeviceContext;
class IFence;

/// \brief Used in IGraphicsDevice::CreateBuffer to initialize data into the immutable buffer.
struct BufferInitialData final
{
    /// \brief The pointer to the array of bytes use in the buffer initialization.
    CPVoid Data = nullptr;

    /// \brief The size of the data to initialize (in bytes).
    Size DataSize = 0;

    /// \brief The offset from the start of the buffer (in bytes) to start initialize data into.
    Size Offset = 0;

    /// \brief The immediate context used in the data transferring.
    ///        The context provided here is required to support transfer operation.
    SharedPointer<IDeviceContext> ImmediateContext = nullptr;
};

/// \brief Used in IGraphicsDevice::CreateBuffer to initialize data into the immutable buffer.
struct TextureInitialData final
{
    /// \brief The pointer to the array of bytes use in the buffer initialization.
    CPVoid Data = nullptr;

    /// \brief The size of the data to initialize (in bytes).
    Size DataSize = 0;

    /// \brief The immediate context used in the data transferring.
    ///        The context provided here is required to support transfer operation.
    SharedPointer<IDeviceContext> ImmediateContext = nullptr;
};

/// \brief Responsible for graphics resources creation.
///
class AXIS_GRAPHICS_API IGraphicsDevice : public ISharedFromThis
{
public:
    /// \brief Graphics system which created graphics device.
    const SharedPointer<IGraphicsSystem> GraphicsSystem = nullptr;

    /// \brief Index of graphics adapter which created this graphics device.
    const Uint32 GraphicsAdapterIndex = 0;

    /// \brief Creates ISwapChain.
    ///
    /// \param[in] description Description of ISwapChain used in its creation.
    AXIS_NODISCARD virtual SharedPointer<ISwapChain> CreateSwapChain(const SwapChainDescription& description) = 0;

    /// \brief Creates ITextureView resource.
    ///
    /// \param[in] description Description of ITextureView resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<ITextureView> CreateTextureView(const TextureViewDescription& description) = 0;

    /// \brief Creates IRenderPass resource.
    ///
    /// \param[in] description Description of IRenderPass resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<IRenderPass> CreateRenderPass(const RenderPassDescription& description) = 0;

    /// \brief Creates IFramebuffer resource.
    ///
    /// \param[in] description Description of IFramebuffer resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<IFramebuffer> CreateFramebuffer(const FramebufferDescription& description) = 0;

    /// \brief Compiles the shader source code into shader module.
    ///
    /// \param[in] description Description of IShaderModule resource used in its creation.
    /// \param[in] sourceCode Shader program's source code to compile.
    AXIS_NODISCARD virtual SharedPointer<IShaderModule> CompileShaderModule(const ShaderModuleDescription& description,
                                                                            const char*                    sourceCode) = 0;

    /// \brief Creates IResourceHeapLayout resource.
    ///
    /// \param[in] description Description of IResourceHeapLayout resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<IResourceHeapLayout> CreateResourceHeapLayout(const ResourceHeapLayoutDescription& description) = 0;

    /// \brief Creates IGraphicsPipeline resource.
    ///
    /// \param[in] description Description of IGraphicsPipeline resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription& description) = 0;

    /// \brief Creates the IBuffer resource.
    ///
    /// \param[in] description Description of IBuffer resource used in its creation.
    /// \param[in] pInitialData Contains the information to initialize data into the buffer.
    AXIS_NODISCARD virtual SharedPointer<IBuffer> CreateBuffer(const BufferDescription& description,
                                                               const BufferInitialData* pInitialData) = 0;

    /// \brief Creates the ITexture resource.
    ///
    /// \param[in] description Description of ITexture resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<ITexture> CreateTexture(const TextureDescription& description) = 0;

    /// \brief Creates IResourceHeap resource.
    ///
    /// \param[in] description Description of IResourceHeap resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<IResourceHeap> CreateResourceHeap(const ResourceHeapDescription& description) = 0;

    /// \brief Creates ISampler resource.
    ///
    /// \param[in] description Description of ISampler resource used in its creation.
    AXIS_NODISCARD virtual SharedPointer<ISampler> CreateSampler(const SamplerDescription& description) = 0;

    /// \brief Creates IFence synchronization object.
    ///
    /// \param[in] initialValue The initial value of IFence.
    AXIS_NODISCARD virtual SharedPointer<IFence> CreateFence(Uint64 initialValue) = 0;

    /// \brief Gets all created immediate device context.
    AXIS_NODISCARD virtual const List<WeakPointer<IDeviceContext>>& GetCreatedImmediateDeviceContexts() const = 0;

    /// \brief Blocks the current thread until the device is idle.
    virtual void WaitDeviceIdle() const = 0;

    /// \brief Adds the DeviceChild to this graphics device.
    ///
    /// \note This will store the strong reference of this graphics device on the device child.
    void AddDeviceChild(DeviceChild& deviceChild) noexcept;

protected:
    /// \brief Constructor
    IGraphicsDevice(const SharedPointer<IGraphicsSystem>& graphicsSystem,
                    Uint32                                graphicsAdapterIndex);
};

} // namespace Axis

#endif // AXIS_GRAPHICS_GRAPHICSDEVICE_HPP