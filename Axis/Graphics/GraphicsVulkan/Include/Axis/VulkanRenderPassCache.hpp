/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANRENDERPASSCACHE_HPP
#define AXIS_VULKANRENDERPASSCACHE_HPP
#pragma once

#include "../../../../System/Include/Axis/HashMap.hpp"
#include "../../../../System/Include/Axis/List.hpp"
#include "../../../Include/Axis/DeviceChild.hpp"
#include "../../../Include/Axis/GraphicsCommon.hpp"
#include <mutex>

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IRenderPass;
class VulkanGraphicsDevice;

// VulkanRenderPassCache's key
struct VulkanRenderPassCacheKey final
{
public:
    // Sample count
    Uint8 SampleCount = 0;

    // Depth stencil view format
    TextureFormat DepthStencilViewFormat = TextureFormat::Unknown;

    // Render target view formats
    System::List<TextureFormat> RenderTargetViewFormats = nullptr;

    // Gets hash key for this render pass key.
    Size GetHash() const noexcept;

    // Equal operator
    Bool operator==(const VulkanRenderPassCacheKey& RHS) const noexcept;

    // Not equal operator
    Bool operator!=(const VulkanRenderPassCacheKey& RHS) const noexcept;

    // Hash functor object
    struct Hash
    {
        Size operator()(const VulkanRenderPassCacheKey& key) const noexcept
        {
            return key.GetHash();
        }
    };
};

// Helper struct for creating the render pass by using TextureFormats
struct VulkanRenderPassCache final : public DeviceChild
{
public:
    // Constructor
    VulkanRenderPassCache(VulkanGraphicsDevice& vulkanGraphicsDevice);

    // Destructor
    ~VulkanRenderPassCache() noexcept;

    VulkanRenderPassCache(const VulkanRenderPassCache&) = delete;
    VulkanRenderPassCache(VulkanRenderPassCache&&)      = delete;
    VulkanRenderPassCache& operator=(const VulkanRenderPassCache&) = delete;
    VulkanRenderPassCache& operator=(VulkanRenderPassCache&&) = delete;

    // Gets the render pass based on the cache keys.
    System::SharedPointer<IRenderPass> GetRenderPass(const VulkanRenderPassCacheKey& renderPassCacheKey);

private:
    System::HashMap<VulkanRenderPassCacheKey, System::SharedPointer<IRenderPass>, VulkanRenderPassCacheKey::Hash> _hashCache = {};
    std::mutex                                                                                                    _mutex     = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANRENDERPASSCACHE_HPP