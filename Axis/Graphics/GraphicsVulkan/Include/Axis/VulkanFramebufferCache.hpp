/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANFRAMEBUFFERCACHE_HPP
#define AXIS_VULKANFRAMEBUFFERCACHE_HPP
#pragma once

#include "../../../../System/Include/Axis/HashMap.hpp"
#include "../../../../System/Include/Axis/List.hpp"
#include "../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../../../System/Include/Axis/Utility.hpp"
#include "../../../Include/Axis/DeviceChild.hpp"
#include "VulkanRenderPassCache.hpp"
#include <mutex>

namespace Axis
{

namespace Graphics
{

// Forward declarations
class ITextureView;
class IFramebuffer;
class IGraphicsDevice;

// Key for framebuffer cache.
struct VulkanFramebufferCacheKey final
{
    // Render target texture views
    System::List<System::WeakPointer<ITextureView>> RenderTargetViews = nullptr;

    // Depth stencil texture view
    System::WeakPointer<ITextureView> DepthStencilView = nullptr;

    // Gets hash for this framebuffer cache key.
    Size GetHash() const noexcept;

    // Equality comparisons
    Bool operator==(const VulkanFramebufferCacheKey& RHS) const noexcept;

    // Inequality comparisons
    Bool operator!=(const VulkanFramebufferCacheKey& RHS) const noexcept;

    // VulkanFramebufferCacheKey's hasher
    struct Hash
    {
        // Gets the hash via functor
        Size operator()(const VulkanFramebufferCacheKey& key) const noexcept
        {
            return key.GetHash();
        }
    };
};

// Gets VulkanFramebuffer from the specified framebuffer cache key
struct VulkanFramebufferCache final : public DeviceChild
{
public:
    // Constructor
    VulkanFramebufferCache(VulkanGraphicsDevice& vulkanGraphicsDevice);

    // Destructor
    ~VulkanFramebufferCache() noexcept;

    VulkanFramebufferCache(const VulkanFramebufferCache&) = delete;
    VulkanFramebufferCache(VulkanFramebufferCache&&)      = delete;
    VulkanFramebufferCache& operator=(const VulkanFramebufferCache&) = delete;
    VulkanFramebufferCache& operator=(VulkanFramebufferCache&&) = delete;

    // Gets the framebuffer based on the cache keys.
    System::SharedPointer<IFramebuffer> GetFramebuffer(const VulkanFramebufferCacheKey& renderPassCacheKey);

    // Cleans up framebuffers that should be destroyed.
    void CleanUp();

private:
    VulkanRenderPassCache                                                                                            _vulkanRenderPassCache;
    System::HashMap<VulkanFramebufferCacheKey, System::SharedPointer<IFramebuffer>, VulkanFramebufferCacheKey::Hash> _hashCache = {};
    std::mutex                                                                                                       _mutex     = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANFRAMEBUFFERCACHE_HPP