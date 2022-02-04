/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/Buffer.hpp>
#include <Axis/Exception.hpp>
#include <Axis/ResourceHeap.hpp>
#include <Axis/ResourceHeapLayout.hpp>
#include <Axis/Texture.hpp>

namespace Axis
{

IResourceHeap::IResourceHeap(const ResourceHeapDescription& description) :
    Description(description) {}

// Default constructor
void IResourceHeap::BindBuffers(Uint32                              bindingIndex,
                                const Span<SharedPointer<IBuffer>>& buffers,
                                const Span<Size>&                   offsets,
                                const Span<Size>&                   sizes,
                                Uint32                              startingArrayIndex)
{
    if (!buffers)
        throw InvalidArgumentException("`buffers` was nullptr!");

    Size arrayIndex = SIZE_MAX;
    Size currIndex  = 0;

    for (const auto& resourceBinding : Description.ResourceHeapLayout->Description.ResourceBindings)
    {
        if (resourceBinding.BindingIndex == bindingIndex)
            arrayIndex = currIndex;
        currIndex++;
    }

    if (arrayIndex == SIZE_MAX)
        throw InvalidArgumentException("`bindingIndex` was not found in the resource heap layout!");

    if (Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].Binding != ResourceBinding::UniformBuffer)
        throw InvalidArgumentException("`bindingIndex` was not a buffer binding!");

    if (startingArrayIndex >= Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].ArraySize)
        throw ArgumentOutOfRangeException("`startingArrayIndex` was out of range!");

    Size index = 0;
    for (const auto& buffer : buffers)
    {
        if (!buffer)
            throw InvalidArgumentException("`buffers` contained a nullptr!");

        if (!(Bool)(buffer->Description.BufferBinding & BufferBinding::Uniform))
            throw InvalidArgumentException("`buffers` were not uniform buffers!");

        index++;
    }

    if (offsets)
    {
        if (offsets.GetLength() != buffers.GetLength())
            throw InvalidArgumentException("`offsets` was not the same size as `buffers`!");

        index = 0;
        for (const auto& offset : offsets)
        {
            if (offset >= buffers[index]->Description.BufferSize)
                throw ArgumentOutOfRangeException("`offset` was out of range!");

            index++;
        }
    }

    if (sizes)
    {
        if (sizes.GetLength() != buffers.GetLength())
            throw InvalidArgumentException("`sizes` was not the same size as `buffers`!");

        index = 0;
        for (const auto& size : sizes)
        {
            if (size > buffers[index]->Description.BufferSize)
                throw ArgumentOutOfRangeException("`size` was out of range!");

            if (offsets)
            {
                if (offsets[index] + size > buffers[index]->Description.BufferSize)
                    throw ArgumentOutOfRangeException("`size` was out of range!");
            }

            index++;
        }
    }
}

void IResourceHeap::BindSamplers(Uint32                                   bindingIndex,
                                 const Span<SharedPointer<ISampler>>&     samplers,
                                 const Span<SharedPointer<ITextureView>>& textureViews,
                                 Uint32                                   startingArrayIndex)
{
    Size arrayIndex = SIZE_MAX;
    Size currIndex  = 0;

    for (const auto& resourceBinding : Description.ResourceHeapLayout->Description.ResourceBindings)
    {
        if (resourceBinding.BindingIndex == bindingIndex)
            arrayIndex = currIndex;
        currIndex++;
    }

    if (arrayIndex == SIZE_MAX)
        throw InvalidArgumentException("`bindingIndex` was not found in the resource heap layout!");

    if (Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].Binding != ResourceBinding::Sampler)
        throw InvalidArgumentException("`bindingIndex` was not a sampler binding!");

    if (!samplers)
        throw InvalidArgumentException("`samplers` was nullptr!");

    if (!textureViews)
        throw InvalidArgumentException("`textureViews` was nullptr!");

    if (samplers.GetLength() != textureViews.GetLength())
        throw InvalidArgumentException("`samplers` and `textureViews` were not the same size!");

    if (startingArrayIndex >= Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].ArraySize)
        throw ArgumentOutOfRangeException("`startingArrayIndex` was out of range!");

    for (Size i = 0; i < samplers.GetLength(); i++)
    {
        if (!samplers[i])
            throw InvalidArgumentException("`samplers` contained a nullptr!");

        if (!textureViews[i])
            throw InvalidArgumentException("`textureViews` contained a nullptr!");

        if (!(Bool)(textureViews[i]->Description.ViewTexture->Description.TextureBinding & TextureBinding::Sampled))
            throw InvalidArgumentException("`textureViews` contained a texture that was not a sampled texture!");
    }
}

} // namespace Axis
