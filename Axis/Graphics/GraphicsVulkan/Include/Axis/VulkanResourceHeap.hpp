/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANRESOURCEHEAP_HPP
#define AXIS_VULKANRESOURCEHEAP_HPP
#pragma once

#include "../../../Include/Axis/GraphicsCommon.hpp"
#include "../../../Include/Axis/ResourceHeap.hpp"
#include "../../../System/Include/Axis/List.hpp"
#include "VulkanDescriptorPool.hpp"
#include <vulkan/vulkan.h>

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;
class VulkanDeviceContext;
class VulkanFence;
class VulkanDescriptorPool;
class VulkanDescriptorSetGroup;
class VulkanBuffer;
class VulkanSampler;
class VulkanTextureView;

// Implementation of IResourceHeap interface class in Vulkan platform.
class VulkanResourceHeap final : public IResourceHeap
{
public:
    // Constructor
    VulkanResourceHeap(const ResourceHeapDescription& description,
                       VulkanGraphicsDevice&          vulkanGraphicsDevice);

    // Implementation of IResourceHeap::BindBuffers class in Vulkan platform.
    void BindBuffers(Uint32                                              bindingIndex,
                     const System::Span<System::SharedPointer<IBuffer>>& buffers,
                     const System::Span<Size>&                           offsets,
                     const System::Span<Size>&                           sizes,
                     Uint32                                              startingArrayIndex) override final;

    // Implementation of IResourceHeap::BindSamplers class in Vulkan platform.
    void BindSamplers(Uint32                                                   bindingIndex,
                      const System::Span<System::SharedPointer<ISampler>>&     samplers,
                      const System::Span<System::SharedPointer<ITextureView>>& textureViews,
                      Uint32                                                   startingArrayIndex) final;

    // Inserts the barrier for the resources. Prepare for binding.
    void PrepareResourceHeapBinding(VulkanDeviceContext& deviceContext,
                                    StateTransition      stateTransition);

    // Gets current using VulkanDescriptorSetGroup.
    inline const System::UniquePointer<VulkanDescriptorSetGroup>& GetCurrentDescriptorSetGroup() const noexcept { return _currentDescriptorSetGroup; }

private:
    void InternalBindResources(VulkanDeviceContext&      vulkanDeviceContext,
                               StateTransition           stateTransition,
                               VulkanDescriptorSetGroup& descriptorSetGroup);

    // The location where the resource existed in the descriptor set
    struct ResourceLocation
    {
        Uint32 BindingIndex = {};
        Uint32 ArrayIndex   = {};

        inline constexpr Size GetHash() const noexcept
        {
            Size hash = BindingIndex;
            hash      = System::Math::HashCombine(hash, ArrayIndex);
            return hash;
        }

        inline constexpr Bool operator==(const ResourceLocation& other) const noexcept { return BindingIndex == other.BindingIndex && ArrayIndex == other.ArrayIndex; }
        inline constexpr Bool operator!=(const ResourceLocation& other) const noexcept { return BindingIndex != other.BindingIndex || ArrayIndex != other.ArrayIndex; }

        struct Hash
        {
            inline constexpr Size operator()(const ResourceLocation& obj) const noexcept { return obj.GetHash(); }
        };
    };

    // Buffer binding info
    struct VulkanBufferBinding
    {
        System::SharedPointer<VulkanBuffer> VulkanBuffer = {};
        Size                                Offset       = {};
        Size                                BufferSize   = {};
    };

    // Sampler binding info
    struct VulkanSamplerBinding
    {
        System::SharedPointer<VulkanTextureView> VulkanTextureView = nullptr;
        System::SharedPointer<VulkanSampler>     VulkanSampler     = nullptr;
    };

    System::HashMap<ResourceLocation, VulkanBufferBinding, ResourceLocation::Hash>  _vulkanBufferBindings      = {};
    System::HashMap<ResourceLocation, VulkanSamplerBinding, ResourceLocation::Hash> _vulkanSamplerBindings     = {};
    System::UniquePointer<VulkanDescriptorSetGroup>                                 _currentDescriptorSetGroup = nullptr;
    VulkanDescriptorPool                                                            _descriptorPool;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANRESOURCEHEAP_HPP