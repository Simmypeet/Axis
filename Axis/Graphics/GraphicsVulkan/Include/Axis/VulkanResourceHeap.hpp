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

/// Forward declarations
class VulkanGraphicsDevice;
class VulkanDeviceContext;
class VulkanFence;
class VulkanDescriptorPool;
class VulkanDescriptorSetGroup;
class VulkanBuffer;
class VulkanSampler;
class VulkanTextureView;

/// \brief Implementation of IResourceHeap interface class in Vulkan platform.
class VulkanResourceHeap final : public IResourceHeap
{
public:
    /// Constructor
    VulkanResourceHeap(const ResourceHeapDescription& description,
                       VulkanGraphicsDevice&          vulkanGraphicsDevice);

    /// \brief Implementation of IResourceHeap::BindBuffers class in Vulkan platform.
    void BindBuffers(Uint32                              bindingIndex,
                     const Span<SharedPointer<IBuffer>>& buffers,
                     const Span<Size>&                   offsets,
                     const Span<Size>&                   sizes,
                     Uint32                              startingArrayIndex) override final;

    /// \brief Implementation of IResourceHeap::BindSamplers class in Vulkan platform.
    void BindSamplers(Uint32                                   bindingIndex,
                      const Span<SharedPointer<ISampler>>&     samplers,
                      const Span<SharedPointer<ITextureView>>& textureViews,
                      Uint32                                   startingArrayIndex) final;

    /// \brief Inserts the barrier for the resources. Prepare for binding.
    void PrepareResourceHeapBinding(VulkanDeviceContext& deviceContext,
                                    StateTransition      stateTransition);

    /// \brief Gets current using VulkanDescriptorSetGroup.
    inline const UniquePointer<VulkanDescriptorSetGroup>& GetCurrentDescriptorSetGroup() const noexcept { return _currentDescriptorSetGroup; }

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
            hash      = Math::HashCombine(hash, ArrayIndex);
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
        SharedPointer<VulkanBuffer> VulkanBuffer = {};
        Size                        Offset       = {};
        Size                        BufferSize   = {};
    };

    // Sampler binding info
    struct VulkanSamplerBinding
    {
        SharedPointer<VulkanTextureView> VulkanTextureView = nullptr;
        SharedPointer<VulkanSampler>     VulkanSampler     = nullptr;
    };

    HashMap<ResourceLocation, VulkanBufferBinding, ResourceLocation::Hash>  _vulkanBufferBindings      = {};
    HashMap<ResourceLocation, VulkanSamplerBinding, ResourceLocation::Hash> _vulkanSamplerBindings     = {};
    UniquePointer<VulkanDescriptorSetGroup>                                 _currentDescriptorSetGroup = nullptr;
    VulkanDescriptorPool                                                    _descriptorPool;
};

} // namespace Axis

#endif // AXIS_VULKANRESOURCEHEAP_HPP