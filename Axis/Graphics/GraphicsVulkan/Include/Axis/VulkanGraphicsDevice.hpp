/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file VulkanGraphicsDevice.hpp
///
/// \brief Contains an implementation of \a `Axis::IGraphicsDevice` interface
///        in Vulkan backend

#ifndef AXIS_VULKANGRAPHICSDEVICE_HPP
#define AXIS_VULKANGRAPHICSDEVICE_HPP
#pragma once

#include "../../../../System/Include/Axis/List.hpp"
#include "../../../../System/Include/Axis/Span.hpp"
#include "../../../Include/Axis/GraphicsDevice.hpp"
#include "VkPtr.hpp"
#include "VulkanDeviceQueueFamily.hpp"
#include "VulkanFramebufferCache.hpp"
#include "VulkanRenderPassCache.hpp"
#include <VulkanMemoryAllocator/vk_mem_alloc.hpp>

namespace Axis
{

/// Forward declarations
class VulkanGraphicsSystem;
struct VulkanDeviceQueueFamily;
class VulkanDeviceContext;
class IDeviceContext;
struct ImmediateContextCreateInfo;

/// \brief An implementation of IGraphicsDevice interface in Vulkan backend.
///
class VulkanGraphicsDevice final : public IGraphicsDevice
{
public:
    /// \brief Constructor
    VulkanGraphicsDevice(const SharedPointer<VulkanGraphicsSystem>& vulkanGraphicsSystem,
                         Uint32                                     adapterIndex,
                         const Span<ImmediateContextCreateInfo>&    pImmediateContextCreateInfos = nullptr);

    /// \brief An implementation of IGraphicsDevice::GetGraphicsAdapterIndex in Vulkan backend.
    SharedPointer<ISwapChain> CreateSwapChain(const SwapChainDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateTextureView in Vulkan backend.
    SharedPointer<ITextureView> CreateTextureView(const TextureViewDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateRenderPass in Vulkan backend.
    SharedPointer<IRenderPass> CreateRenderPass(const RenderPassDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateFramebuffer in Vulkan backend.
    SharedPointer<IFramebuffer> CreateFramebuffer(const FramebufferDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CompileShaderModule in Vulkan backend.
    SharedPointer<IShaderModule> CompileShaderModule(const ShaderModuleDescription& description,
                                                     const StringView<Char>&        sourceCode) override final;

    /// \brief An implementation of IGraphicsDevice::CreateResourceHeapLayout in Vulkan backend.
    SharedPointer<IResourceHeapLayout> CreateResourceHeapLayout(const ResourceHeapLayoutDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateGraphicsPipeline in Vulkan backend.
    SharedPointer<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateBuffer in Vulkan backend.
    SharedPointer<IBuffer> CreateBuffer(const BufferDescription& description,
                                        const BufferInitialData* pInitialData) override final;

    /// \brief An implementation of IGraphicsDevice::CreateTexture in Vulkan backend.
    SharedPointer<ITexture> CreateTexture(const TextureDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateResourceHeap in Vulkan backend.
    SharedPointer<IResourceHeap> CreateResourceHeap(const ResourceHeapDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateSampler in Vulkan backend.
    ///
    SharedPointer<ISampler> CreateSampler(const SamplerDescription& description) override final;

    /// \brief An implementation of IGraphicsDevice::CreateFence in Vulkan backend.
    SharedPointer<IFence> CreateFence(Uint64 initialValue) override final;

    /// \brief An implementation of IGraphicsDevice::WaitDeviceIdle in Vulkan backend.
    void WaitDeviceIdle() const noexcept override final;

    /// \brief Gets the internal VkDevice handle.
    inline VkDevice GetVkDeviceHandle() const noexcept { return _vulkanLogicalDevice; }

    /// \brief Gets the internal VmaAllocator handle.
    inline VmaAllocator GetVmaAllocatorHandle() const noexcept { return _vulkanMemoryAllocator; }

    /// \brief Gets the pointer to the vulkan device queue family object.
    inline VulkanDeviceQueueFamily& GetVulkanDeviceQueueFamily(Uint32 deviceQueueFamilyIndex)
    {
        auto it = _deviceQueueFamilies.Find(deviceQueueFamilyIndex);

        return it->Second;
    }

    /// \brief Gets the array of created immediate device contexts.
    const List<WeakPointer<IDeviceContext>>& GetCreatedImmediateDeviceContexts() const override final { return _vulkanDeviceContexts; }

    /// \brief Gets immediate device contexts
    List<SharedPointer<IDeviceContext>> GetDeviceContexts();

    /// \brief Gets all required device extensions.
    static const List<const char*> DeviceExtensions;

private:
    List<WeakPointer<IDeviceContext>>        _vulkanDeviceContexts  = {}; // Weak references of all created immediate device contexts
    VkPtr<VkDevice>                          _vulkanLogicalDevice   = {}; // Internal VkDevice handle
    VkPtr<VmaAllocator>                      _vulkanMemoryAllocator = {}; // Vulkan's memory allocator
    HashMap<Uint32, VulkanDeviceQueueFamily> _deviceQueueFamilies   = {}; // All device queue families
};

} // namespace Axis

#endif // AXIS_VULKANGRAPHICSDEVICE_HPP
