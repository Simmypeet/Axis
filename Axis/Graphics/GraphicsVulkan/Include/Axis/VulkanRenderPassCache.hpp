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
#include "VulkanRenderPassCache.hpp"
#include <mutex>

namespace Axis
{

/// Forward declarations
class IRenderPass;
class VulkanGraphicsDevice;

/// \brief VulkanRenderPassCache's key
struct VulkanRenderPassCacheKey final
{
public:
    /// \brief Sample count
    Uint8 SampleCount = 0;

    /// \brief Depth stencil view format
    TextureFormat DepthStencilViewFormat = TextureFormat::Unknown;

    /// \brief Render target view formats
    List<TextureFormat> RenderTargetViewFormats = nullptr;

    /// \brief Gets hash key for this render pass key.
    Size GetHash() const noexcept;

    /// \brief Equal operator
    Bool operator==(const VulkanRenderPassCacheKey& RHS) const noexcept;

    /// \brief Not equal operator
    Bool operator!=(const VulkanRenderPassCacheKey& RHS) const noexcept;

    /// \brief Hash functor object
    struct Hash
    {
        Size operator()(const VulkanRenderPassCacheKey& key) const noexcept
        {
            return key.GetHash();
        }
    };
};

/// \brief Helper struct for creating the render pass by using TextureFormats
struct VulkanRenderPassCache final : public DeviceChild
{
public:
    /// \brief Constructor
    VulkanRenderPassCache(VulkanGraphicsDevice& vulkanGraphicsDevice);

    /// \brief Destructor
    ~VulkanRenderPassCache() noexcept;

    VulkanRenderPassCache(const VulkanRenderPassCache&) = delete;
    VulkanRenderPassCache(VulkanRenderPassCache&&)      = delete;
    VulkanRenderPassCache& operator=(const VulkanRenderPassCache&) = delete;
    VulkanRenderPassCache& operator=(VulkanRenderPassCache&&) = delete;

    /// \brief Gets the render pass based on the cache keys.
    SharedPointer<IRenderPass> GetRenderPass(const VulkanRenderPassCacheKey& renderPassCacheKey);

private:
    /// Private members
    HashMap<VulkanRenderPassCacheKey, SharedPointer<IRenderPass>, VulkanRenderPassCacheKey::Hash> _hashCache = {};
    std::mutex                                                                                    _mutex     = {};
};

} // namespace Axis

#endif // AXIS_VULKANRENDERPASSCACHE_HPP