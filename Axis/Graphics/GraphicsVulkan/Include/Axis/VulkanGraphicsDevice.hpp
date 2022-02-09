/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

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

namespace Graphics
{

// Forward declarations
class VulkanGraphicsSystem;
struct VulkanDeviceQueueFamily;
class VulkanDeviceContext;
class IDeviceContext;
struct ImmediateContextCreateInfo;

// An implementation of IGraphicsDevice interface in Vulkan backend.
class VulkanGraphicsDevice final : public IGraphicsDevice
{
public:
    // Constructor
    VulkanGraphicsDevice(const System::SharedPointer<VulkanGraphicsSystem>& vulkanGraphicsSystem,
                         Uint32                                             adapterIndex,
                         const System::Span<ImmediateContextCreateInfo>&    pImmediateContextCreateInfos = nullptr);

    // An implementation of IGraphicsDevice::GetGraphicsAdapterIndex in Vulkan backend.
    System::SharedPointer<ISwapChain> CreateSwapChain(const SwapChainDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateTextureView in Vulkan backend.
    System::SharedPointer<ITextureView> CreateTextureView(const TextureViewDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateRenderPass in Vulkan backend.
    System::SharedPointer<IRenderPass> CreateRenderPass(const RenderPassDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateFramebuffer in Vulkan backend.
    System::SharedPointer<IFramebuffer> CreateFramebuffer(const FramebufferDescription& description) override final;

    // An implementation of IGraphicsDevice::CompileShaderModule in Vulkan backend.
    System::SharedPointer<IShaderModule> CompileShaderModule(const ShaderModuleDescription&  description,
                                                             const System::StringView<Char>& sourceCode) override final;

    // An implementation of IGraphicsDevice::CreateResourceHeapLayout in Vulkan backend.
    System::SharedPointer<IResourceHeapLayout> CreateResourceHeapLayout(const ResourceHeapLayoutDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateGraphicsPipeline in Vulkan backend.
    System::SharedPointer<IGraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateBuffer in Vulkan backend.
    System::SharedPointer<IBuffer> CreateBuffer(const BufferDescription& description,
                                                const BufferInitialData* pInitialData) override final;

    // An implementation of IGraphicsDevice::CreateTexture in Vulkan backend.
    System::SharedPointer<ITexture> CreateTexture(const TextureDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateResourceHeap in Vulkan backend.
    System::SharedPointer<IResourceHeap> CreateResourceHeap(const ResourceHeapDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateSampler in Vulkan backend.
    ///
    System::SharedPointer<ISampler> CreateSampler(const SamplerDescription& description) override final;

    // An implementation of IGraphicsDevice::CreateFence in Vulkan backend.
    System::SharedPointer<IFence> CreateFence(Uint64 initialValue) override final;

    // An implementation of IGraphicsDevice::WaitDeviceIdle in Vulkan backend.
    void WaitDeviceIdle() const noexcept override final;

    // Gets the internal VkDevice handle.
    inline VkDevice GetVkDeviceHandle() const noexcept { return _vulkanLogicalDevice; }

    // Gets the internal VmaAllocator handle.
    inline VmaAllocator GetVmaAllocatorHandle() const noexcept { return _vulkanMemoryAllocator; }

    // Gets the pointer to the vulkan device queue family object.
    inline VulkanDeviceQueueFamily& GetVulkanDeviceQueueFamily(Uint32 deviceQueueFamilyIndex)
    {
        auto it = _deviceQueueFamilies.Find(deviceQueueFamilyIndex);

        return it->Second;
    }

    // Gets the array of created immediate device contexts.
    const System::List<System::WeakPointer<IDeviceContext>>& GetCreatedImmediateDeviceContexts() const override final { return _vulkanDeviceContexts; }

    // Gets immediate device contexts
    System::List<System::SharedPointer<IDeviceContext>> GetDeviceContexts();

    // Gets all required device extensions.
    static const System::List<const char*> DeviceExtensions;

private:
    System::List<System::WeakPointer<IDeviceContext>> _vulkanDeviceContexts  = {}; // Weak references of all created immediate device contexts
    VkPtr<VkDevice>                                   _vulkanLogicalDevice   = {}; // Internal VkDevice handle
    VkPtr<VmaAllocator>                               _vulkanMemoryAllocator = {}; // Vulkan's memory allocator
    System::HashMap<Uint32, VulkanDeviceQueueFamily>  _deviceQueueFamilies   = {}; // All device queue families
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANGRAPHICSDEVICE_HPP
