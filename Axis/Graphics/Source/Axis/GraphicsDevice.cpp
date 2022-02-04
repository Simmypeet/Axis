/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/DeviceChild.hpp>
#include <Axis/DeviceContext.hpp>
#include <Axis/Framebuffer.hpp>
#include <Axis/GraphicsDevice.hpp>
#include <Axis/GraphicsPipeline.hpp>
#include <Axis/GraphicsSystem.hpp>
#include <Axis/RenderPass.hpp>
#include <Axis/ResourceHeap.hpp>
#include <Axis/ResourceHeapLayout.hpp>
#include <Axis/SwapChain.hpp>
#include <Axis/Texture.hpp>

namespace Axis
{

// Constructor
IGraphicsDevice::IGraphicsDevice(const SharedPointer<IGraphicsSystem>& graphicsSystem,
                                 Uint32                                graphicsAdapterIndex) :
    GraphicsSystem(graphicsSystem),
    GraphicsAdapterIndex(graphicsAdapterIndex) {}


void IGraphicsDevice::AddDeviceChild(DeviceChild& deviceChild) noexcept
{
    deviceChild._graphicsDevice = ISharedFromThis::CreateSharedPointerFromThis(*this);
}


void IGraphicsDevice::ValidateCreateSwapChain(const SwapChainDescription& description)
{
    if (!description.ImmediateGraphicsContext)
        throw InvalidArgumentException("description.ImmediateGraphicsContext was nullptr!");

    if (!(Bool)(description.ImmediateGraphicsContext->SupportedQueueOperations & QueueOperation::Graphics))
        throw InvalidArgumentException("description.ImmediateGraphicsContext does not support graphics operations!");

    if (!description.TargetWindow)
        throw InvalidArgumentException("description.TargetWindow was nullptr!");

    SwapChainSpecification specification = GraphicsSystem->GetSwapChainSpecification(GraphicsAdapterIndex, description.TargetWindow);

    if (!Math::IsInRange(description.BackBufferCount, specification.MinBackBufferCount, specification.MaxBackBufferCount))
        throw ArgumentOutOfRangeException("description.BackBufferCount was out of range!");

    constexpr auto CheckFormatValid = [](TextureFormat formatToCheck, const List<TextureFormat>& formats) {
        for (const auto& format : formats)
        {
            if (format == formatToCheck)
                return true;
        }

        return false;
    };

    if (!CheckFormatValid(description.RenderTargetFormat, specification.SupportedFormats))
        throw InvalidArgumentException("description.RenderTargetFormat was not supported!");
}

void IGraphicsDevice::ValidateCreateTextureView(const TextureViewDescription& description)
{
    // TODO: TextureViewDescription validation
}

void IGraphicsDevice::ValidateCreateRenderPass(const RenderPassDescription& description)
{
    if (!description.Attachments)
        throw InvalidArgumentException("description.Attachments was nullptr!");

    if (!description.Subpasses)
        throw InvalidArgumentException("description.Subpasses was nullptr!");

    auto i = 0;

    for (auto& subpass : description.Subpasses)
    {
        if (subpass.DepthStencilReference.Index == AttachmentReference::Unused && !subpass.InputReferences && !subpass.RenderTargetReferences)
            throw InvalidArgumentException("Subpasses contained invalid attachment references!");

        if (subpass.DepthStencilReference.Index != AttachmentReference::Unused && !(subpass.DepthStencilReference.Index < description.Attachments.GetLength()))
            throw InvalidArgumentException("Subpasses contained invalid attachment references!");


        if (subpass.RenderTargetReferences)
        {
            auto j = 0;
            for (const auto& attachment : subpass.RenderTargetReferences)
            {
                if (!(attachment.Index < description.Attachments.GetLength()))
                    throw InvalidArgumentException("Subpasses contained invalid attachment references!");
                j++;
            }
        }

        if (subpass.InputReferences)
        {
            auto j = 0;
            for (const auto& attachment : subpass.InputReferences)
            {
                if (!(attachment.Index < description.Attachments.GetLength()))
                    throw InvalidArgumentException("Subpasses contained invalid attachment references!");
                j++;
            }
        }
        i++;
    }

    i = 0;

    if (description.Dependencies)
    {
        for (const auto& dependency : description.Dependencies)
        {
            if (dependency.DestSubpassIndex != SubpassDependency::SubpassExternal && !(dependency.DestSubpassIndex < description.Attachments.GetLength()))
                throw InvalidArgumentException("description.Dependencies contained invalid subpass indices!");

            if (dependency.SourceSubpassIndex != SubpassDependency::SubpassExternal && !(dependency.SourceSubpassIndex < description.Attachments.GetLength()))
                throw InvalidArgumentException("description.Dependencies contained invalid subpass indices!");
        }
    }
}

void IGraphicsDevice::ValidateCreateFramebuffer(const FramebufferDescription& description)
{
    if (!(description.Attachments))
        throw InvalidArgumentException("description.Attachments was nullptr!");

    if (!description.RenderPass)
        throw InvalidArgumentException("description.RenderPass was nullptr!");

    auto i = 0;
    for (const auto& attachment : description.Attachments)
    {
        if (!attachment)
            throw InvalidArgumentException("description.Attachments contained nullptr!");
        i++;
    }

    const auto& framebufferDescription = description;
    const auto& renderPassDescription  = description.RenderPass->Description;

    if (framebufferDescription.Attachments.GetLength() != renderPassDescription.Attachments.GetLength())
        throw InvalidArgumentException("description.Attachments and description.RenderPass.Attachments did not match!");

    for (Size i = 0; i < framebufferDescription.Attachments.GetLength(); i++)
    {
        const auto& frameBufferAttachmentDescription = framebufferDescription.Attachments[i]->Description;
        const auto& renderPassAttachment             = renderPassDescription.Attachments[i];

        if (frameBufferAttachmentDescription.ViewFormat != renderPassAttachment.Format)
            throw InvalidArgumentException("frameBuffer attachments' format didn't math render pass attachments' format!");
    }
}

void IGraphicsDevice::ValidateCompileShaderModule(const ShaderModuleDescription& description,
                                                  const StringView<Char>&        sourceCode)
{
    if (!sourceCode)
        throw InvalidArgumentException("sourceCode was nullptr!");
}

void IGraphicsDevice::ValidateCreateResourceHeapLayout(const ResourceHeapLayoutDescription& description)
{
    HashSet<Uint32> bindingIndices;
    auto            graphicsAdapters = GraphicsSystem->GetGraphicsAdapters();

    Size index = 0;
    for (const auto& resourceBinding : description.ResourceBindings)
    {
        if (!(resourceBinding.BindingIndex < graphicsAdapters[GraphicsAdapterIndex].Capability.MaxVertexInputBinding))
            throw ArgumentOutOfRangeException("description.ResourceBindings contained out of range binding indices!");

        bindingIndices.Insert(resourceBinding.BindingIndex);
        index++;
    }

    if (bindingIndices.GetSize() != description.ResourceBindings.GetLength())
        throw InvalidArgumentException("description.ResourceBindings contained duplicate binding indices!");
}

void IGraphicsDevice::ValidateCreateGraphicsPipeline(const GraphicsPipelineDescription& description)
{
    auto graphicsAdapters = GraphicsSystem->GetGraphicsAdapters();

    if (description.VertexBindingDescriptions)
    {
        HashSet<Uint32> hashSet;
        Uint32          index = 0;

        for (const auto& vertexBindingDescription : description.VertexBindingDescriptions)
        {
            if (!(vertexBindingDescription.BindingSlot < graphicsAdapters[GraphicsAdapterIndex].Capability.MaxVertexInputBinding))
                throw ArgumentOutOfRangeException("description.VertexBindingDescriptions contained out of range binding indices!");


            if (!vertexBindingDescription.Attributes)
                throw InvalidArgumentException("description.VertexBindingDescriptions contained nullptr!");

            hashSet.Insert(vertexBindingDescription.BindingSlot);

            index++;
        }

        if (hashSet.GetSize() != description.VertexBindingDescriptions.GetLength())
            throw InvalidArgumentException("description.VertexBindingDescriptions contained duplicate binding indices!");
    }

    if (!description.FragmentShader)
        throw InvalidArgumentException("description.FragmentShader was nullptr!");

    if (!description.VertexShader)
        throw InvalidArgumentException("description.VertexShader was nullptr!");

    if (description.RenderPass)
    {
        if (!(description.SubpassIndex < description.RenderPass->Description.Subpasses.GetLength()))
            throw ArgumentOutOfRangeException("description.SubpassIndex was out of range!");
    }
    else
    {
        if (!description.RenderTargetViewFormats)
            throw InvalidArgumentException("description.RenderTargetViewFormats was nullptr!");
    }
}

void IGraphicsDevice::ValidateCreateBuffer(const BufferDescription& description,
                                           const BufferInitialData* pInitialData)
{

    if (description.BufferSize == 0)
        throw InvalidArgumentException("description.BufferSize was zero!");

    if (description.DeviceQueueFamilyMask == 0)
        throw InvalidArgumentException("description.DeviceQueueFamilyMask was zero!");

    if (pInitialData)
    {
        if (!pInitialData->Data)
            throw InvalidArgumentException("pInitialData->Data was nullptr!");

        if (pInitialData->DataSize + pInitialData->Offset > description.BufferSize)
            throw ArgumentOutOfRangeException("pInitialData->DataSize + pInitialData->Offset was out of range!");

        if (!pInitialData->ImmediateContext)
            throw InvalidArgumentException("pInitialData->ImmediateContext was nullptr!");
    }
}

void IGraphicsDevice::ValidateCreateTexture(const TextureDescription& description)
{
    // TODO: TextureDescription validation
}

void IGraphicsDevice::ValidateCreateResourceHeap(const ResourceHeapDescription& description)
{
    if (!description.ResourceHeapLayout)
        throw InvalidArgumentException("description.ResourceHeapLayout was nullptr!");
}

} // namespace Axis
