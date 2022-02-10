/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/Math.hpp>
#include <Axis/Utility.hpp>
#include <Axis/VulkanFramebuffer.hpp>
#include <Axis/VulkanFramebufferCache.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanRenderPassCache.hpp>
#include <Axis/VulkanTexture.hpp>

namespace Axis
{

namespace Graphics
{

Size VulkanFramebufferCacheKey::GetHash() const noexcept
{
    std::hash<ITextureView*> hasher     = {};
    std::hash<Size>          sizeHasher = {};
    Size                     hash       = hasher(DepthStencilView.GetPointer());
    hash                                = System::Math::HashCombine(hash, sizeHasher(RenderTargetViews.GetLength()));

    for (const auto& renderTargetView : RenderTargetViews)
        hash = System::Math::HashCombine(hash, hasher(renderTargetView.GetPointer()));

    return hash;
}

Bool VulkanFramebufferCacheKey::operator==(const VulkanFramebufferCacheKey& RHS) const noexcept
{
    Bool generalEquality =
        DepthStencilView == RHS.DepthStencilView &&
        RenderTargetViews.GetLength() == RHS.RenderTargetViews.GetLength();

    if (!generalEquality)
        return false;

    for (Size i = 0; i < RenderTargetViews.GetLength(); i++)
    {
        if (RenderTargetViews[i] != RHS.RenderTargetViews[i])
            return false;
    }

    return true;
}

Bool VulkanFramebufferCacheKey::operator!=(const VulkanFramebufferCacheKey& RHS) const noexcept
{
    return !(*this == RHS);
}

VulkanFramebufferCache::VulkanFramebufferCache(VulkanGraphicsDevice& vulkanGraphicsDevice) :
    _vulkanRenderPassCache(vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);
}

// Default destructor
VulkanFramebufferCache::~VulkanFramebufferCache() noexcept {}

System::SharedPointer<IFramebuffer> VulkanFramebufferCache::GetFramebuffer(const VulkanFramebufferCacheKey& framebufferCacheKey)
{
    AXIS_ASSERT(framebufferCacheKey.RenderTargetViews, "framebufferCacheKey.RenderTargetViews wasn't nullptr!");

    auto                                iterator = _hashCache.Find(framebufferCacheKey);
    System::SharedPointer<IFramebuffer> result;

    constexpr auto CreateFramebuffer = [](const VulkanFramebufferCacheKey& cacheKey,
                                          VulkanRenderPassCache&           renderPassCache,
                                          VulkanGraphicsDevice*            vulkanGraphicsDevice) -> System::SharedPointer<IFramebuffer> {
        FramebufferDescription framebufferDesc = {};

        framebufferDesc.Attachments = System::List<System::SharedPointer<ITextureView>>(cacheKey.DepthStencilView == nullptr ? cacheKey.RenderTargetViews.GetLength() : cacheKey.RenderTargetViews.GetLength() + 1);

        if (cacheKey.DepthStencilView != nullptr)
            framebufferDesc.Attachments[0] = cacheKey.DepthStencilView.Generate();

        auto framebufferIndexOffset = cacheKey.DepthStencilView ? 1 : 0;

        for (Size i = 0; i < cacheKey.RenderTargetViews.GetLength(); i++)
        {
            AXIS_ASSERT(cacheKey.RenderTargetViews[i] != nullptr, "framebufferCacheKey is nullptr!");

            framebufferDesc.Attachments[i + framebufferIndexOffset] = cacheKey.RenderTargetViews[i].Generate();
        }

        framebufferDesc.FramebufferSize = {
            framebufferDesc.Attachments[0]->Description.ViewTexture->Description.Size.X,
            framebufferDesc.Attachments[0]->Description.ViewTexture->Description.Size.Y,
            framebufferDesc.Attachments[0]->Description.ArrayLevelCount};

        VulkanRenderPassCacheKey renderPassCacheKey = {};

        renderPassCacheKey.DepthStencilViewFormat  = cacheKey.DepthStencilView == nullptr ? TextureFormat::Unknown : cacheKey.DepthStencilView.GetPointer()->Description.ViewFormat;
        renderPassCacheKey.RenderTargetViewFormats = System::List<TextureFormat>(cacheKey.RenderTargetViews.GetLength());

        for (Size i = 0; i < cacheKey.RenderTargetViews.GetLength(); i++)
            renderPassCacheKey.RenderTargetViewFormats[i] = cacheKey.RenderTargetViews[i].GetPointer()->Description.ViewFormat;

        renderPassCacheKey.SampleCount = cacheKey.RenderTargetViews[0].GetPointer()->Description.ViewTexture->Description.Sample;

        framebufferDesc.RenderPass = renderPassCache.GetRenderPass(renderPassCacheKey);

        return vulkanGraphicsDevice->CreateFramebuffer(framebufferDesc);
    };

    if (iterator == _hashCache.end())
    {
        auto framebuffer = CreateFramebuffer(framebufferCacheKey,
                                             _vulkanRenderPassCache,
                                             (VulkanGraphicsDevice*)GetCreatorDevice());

        if (!framebuffer)
        {
            result = nullptr;
        }
        else
        {
            _hashCache.Insert({framebufferCacheKey, framebuffer});

            result = framebuffer;
        }
    }
    else
    {
        result = iterator->Second;
    }

    return result;
}

void VulkanFramebufferCache::CleanUp()
{
    auto it = _hashCache.begin();
    while (it != _hashCache.end())
    {
        Bool shouldErase = false;
        // strong count is 1 (which means that our cached framebuffer holds that strong reference).
        if (it->First.DepthStencilView.GetStrongCount() == 1)
            shouldErase = true;

        if (!shouldErase)
        {
            for (const auto& renderTargetView : it->First.RenderTargetViews)
            {
                if (renderTargetView.GetStrongCount() == 1)
                {
                    shouldErase = true;
                    break;
                }
            }
        }

        if (shouldErase)
            it = _hashCache.Remove(it->First).Second;
        else
            it++;
    }
}

} // namespace Graphics

} // namespace Axis
