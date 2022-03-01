/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/Math.hpp>
#include <Axis/RenderPass.hpp>
#include <Axis/Utility.hpp>
#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanRenderPassCache.hpp>


namespace Axis
{

namespace Graphics
{

Size VulkanRenderPassCacheKey::GetHash() const noexcept
{
    std::hash<Uint8> hasher = {};

    Size hash = hasher(SampleCount);

    hash = System::Math::HashCombine(hash, hasher(System::Enum::GetUnderlyingValue(DepthStencilViewFormat)));

    if (RenderTargetViewFormats)
    {
        for (const auto& format : RenderTargetViewFormats)
            hash = System::Math::HashCombine(hash, hasher(System::Enum::GetUnderlyingValue(format)));
    }

    return hash;
}

Bool VulkanRenderPassCacheKey::operator==(const VulkanRenderPassCacheKey& RHS) const noexcept
{
    const Bool generalEquality =
        SampleCount == RHS.SampleCount &&
        DepthStencilViewFormat == RHS.DepthStencilViewFormat &&
        RenderTargetViewFormats.GetSize() == RHS.RenderTargetViewFormats.GetSize();

    if (!generalEquality)
        return false;

    for (Size i = 0; i < RenderTargetViewFormats.GetSize(); i++)
    {
        if (RenderTargetViewFormats[i] != RHS.RenderTargetViewFormats[i])
            return false;
    }

    return true;
}

Bool VulkanRenderPassCacheKey::operator!=(const VulkanRenderPassCacheKey& RHS) const noexcept
{
    return !(*this == RHS);
}

VulkanRenderPassCache::VulkanRenderPassCache(VulkanGraphicsDevice& vulkanGraphicsDevice)
{
    vulkanGraphicsDevice.AddDeviceChild(*this);
}

// Default destructor
VulkanRenderPassCache::~VulkanRenderPassCache() noexcept {}

System::SharedPointer<IRenderPass> VulkanRenderPassCache::GetRenderPass(const VulkanRenderPassCacheKey& renderPassCacheKey)
{
    AXIS_ASSERT(renderPassCacheKey.RenderTargetViewFormats, "renderPassCacheKey.RenderTargetViewFormats can't be nullptr!");

    std::scoped_lock lockGuard(_mutex);

    auto                               iterator = _hashCache.Find(renderPassCacheKey);
    System::SharedPointer<IRenderPass> result;

    if (iterator == _hashCache.end())
    {
        RenderPassDescription renderPassDesc = {};
        renderPassDesc.Attachments           = System::List<RenderPassAttachment>(renderPassCacheKey.DepthStencilViewFormat == TextureFormat::Unknown ? renderPassCacheKey.RenderTargetViewFormats.GetSize() : renderPassCacheKey.RenderTargetViewFormats.GetSize() + 1);
        renderPassDesc.Subpasses             = System::List<SubpassDescription>(1);

        auto& allAttachments = renderPassDesc.Attachments;

        if (renderPassCacheKey.DepthStencilViewFormat != TextureFormat::Unknown)
        {
            allAttachments[0].Samples = renderPassCacheKey.SampleCount;
            allAttachments[0].Format  = renderPassCacheKey.DepthStencilViewFormat;

            allAttachments[0].ColorDepthLoadOperation  = LoadOperation::Load;
            allAttachments[0].ColorDepthStoreOperation = StoreOperation::Store;

            allAttachments[0].StencilLoadOperation  = LoadOperation::Load;
            allAttachments[0].StencilStoreOperation = StoreOperation::Store;

            allAttachments[0].InitialState = ResourceState::DepthStencilWrite;
            allAttachments[0].FinalState   = ResourceState::DepthStencilWrite;

            renderPassDesc.Subpasses[0].DepthStencilReference.Index        = 0;
            renderPassDesc.Subpasses[0].DepthStencilReference.SubpassState = ResourceState::DepthStencilWrite;
        }

        renderPassDesc.Subpasses[0].RenderTargetReferences = System::List<AttachmentReference>(renderPassCacheKey.RenderTargetViewFormats.GetSize());

        for (Size i = 0; i < renderPassCacheKey.RenderTargetViewFormats.GetSize(); i++)
        {
            Size indexNow = renderPassCacheKey.DepthStencilViewFormat == TextureFormat::Unknown ? i : i + 1;

            allAttachments[indexNow].Samples = renderPassCacheKey.SampleCount;
            allAttachments[indexNow].Format  = renderPassCacheKey.RenderTargetViewFormats[i];

            allAttachments[indexNow].ColorDepthLoadOperation  = LoadOperation::Load;
            allAttachments[indexNow].ColorDepthStoreOperation = StoreOperation::Store;

            allAttachments[indexNow].StencilLoadOperation  = LoadOperation::DontCare;
            allAttachments[indexNow].StencilStoreOperation = StoreOperation::DontCare;

            allAttachments[indexNow].InitialState = ResourceState::RenderTarget;
            allAttachments[indexNow].FinalState   = ResourceState::RenderTarget;

            renderPassDesc.Subpasses[0].RenderTargetReferences[i].Index        = (Uint32)indexNow;
            renderPassDesc.Subpasses[0].RenderTargetReferences[i].SubpassState = ResourceState::RenderTarget;
        }

        System::SharedPointer<IRenderPass> renderPass = GetCreatorDevice()->CreateRenderPass(renderPassDesc);

        _hashCache.Insert({renderPassCacheKey, renderPass});

        result = renderPass;
    }
    else
    {
        result = iterator->Second;
    }

    return result;
}

} // namespace Graphics

} // namespace Axis
