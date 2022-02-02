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

/// Forward declarations
class ITextureView;
class IFramebuffer;
class IGraphicsDevice;

/// \brief Key for framebuffer cache.
struct VulkanFramebufferCacheKey final
{
    /// \brief Render target texture views
    List<WeakPointer<ITextureView>> RenderTargetViews = nullptr;

    /// \brief Depth stencil texture view
    WeakPointer<ITextureView> DepthStencilView = nullptr;

    /// \brief Gets hash for this framebuffer cache key.
    Size GetHash() const noexcept;

    /// Equality comparisons
    Bool operator==(const VulkanFramebufferCacheKey& RHS) const noexcept;

    /// Inequality comparisons
    Bool operator!=(const VulkanFramebufferCacheKey& RHS) const noexcept;

    /// \brief VulkanFramebufferCacheKey's hasher
    struct Hash
    {
        /// \brief Gets the hash via functor
        Size operator()(const VulkanFramebufferCacheKey& key) const noexcept
        {
            return key.GetHash();
        }
    };
};

/// \brief Gets VulkanFramebuffer from the specified framebuffer cache key
struct VulkanFramebufferCache final : public DeviceChild
{
public:
    /// Constructor
    VulkanFramebufferCache(VulkanGraphicsDevice& vulkanGraphicsDevice);

    /// Destructor
    ~VulkanFramebufferCache() noexcept;

    VulkanFramebufferCache(const VulkanFramebufferCache&) = delete;
    VulkanFramebufferCache(VulkanFramebufferCache&&)      = delete;
    VulkanFramebufferCache& operator=(const VulkanFramebufferCache&) = delete;
    VulkanFramebufferCache& operator=(VulkanFramebufferCache&&) = delete;

    /// \brief Gets the framebuffer based on the cache keys.
    ///
    /// \return Returns nullptr upon error.
    SharedPointer<IFramebuffer> GetFramebuffer(const VulkanFramebufferCacheKey& renderPassCacheKey);

    /// \brief Cleans up framebuffers that should be destroyed.
    void CleanUp();

private:
    /// Private members
    VulkanRenderPassCache                                                                            _vulkanRenderPassCache;
    HashMap<VulkanFramebufferCacheKey, SharedPointer<IFramebuffer>, VulkanFramebufferCacheKey::Hash> _hashCache = {};
    std::mutex                                                                                       _mutex     = {};
};

} // namespace Axis

#endif // AXIS_VULKANFRAMEBUFFERCACHE_HPP