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

namespace Graphics
{

IResourceHeap::IResourceHeap(const ResourceHeapDescription& description) :
    Description(description) {}

// Default constructor
void IResourceHeap::BindBuffers(Uint32                                              bindingIndex,
                                const System::Span<System::SharedPointer<IBuffer>>& buffers,
                                const System::Span<Size>&                           offsets,
                                const System::Span<Size>&                           sizes,
                                Uint32                                              startingArrayIndex)
{
    if (!buffers)
        throw System::InvalidArgumentException("`buffers` was nullptr!");

    Size arrayIndex = SIZE_MAX;
    Size currIndex  = 0;

    for (const auto& resourceBinding : Description.ResourceHeapLayout->Description.ResourceBindings)
    {
        if (resourceBinding.BindingIndex == bindingIndex)
            arrayIndex = currIndex;
        currIndex++;
    }

    if (arrayIndex == SIZE_MAX)
        throw System::InvalidArgumentException("`bindingIndex` was not found in the resource heap layout!");

    if (Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].Binding != ResourceBinding::UniformBuffer)
        throw System::InvalidArgumentException("`bindingIndex` was not a buffer binding!");

    if (startingArrayIndex >= Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].ArraySize)
        throw System::ArgumentOutOfRangeException("`startingArrayIndex` was out of range!");

    Size index = 0;
    for (const auto& buffer : buffers)
    {
        if (!buffer)
            throw System::InvalidArgumentException("`buffers` contained a nullptr!");

        if (!(Bool)(buffer->Description.BufferBinding & BufferBinding::Uniform))
            throw System::InvalidArgumentException("`buffers` were not uniform buffers!");

        index++;
    }

    if (offsets)
    {
        if (offsets.GetLength() != buffers.GetLength())
            throw System::InvalidArgumentException("`offsets` was not the same size as `buffers`!");

        index = 0;
        for (const auto& offset : offsets)
        {
            if (offset >= buffers[index]->Description.BufferSize)
                throw System::ArgumentOutOfRangeException("`offset` was out of range!");

            index++;
        }
    }

    if (sizes)
    {
        if (sizes.GetLength() != buffers.GetLength())
            throw System::InvalidArgumentException("`sizes` was not the same size as `buffers`!");

        index = 0;
        for (const auto& size : sizes)
        {
            if (size > buffers[index]->Description.BufferSize)
                throw System::ArgumentOutOfRangeException("`size` was out of range!");

            if (offsets)
            {
                if (offsets[index] + size > buffers[index]->Description.BufferSize)
                    throw System::ArgumentOutOfRangeException("`size` was out of range!");
            }

            index++;
        }
    }
}

void IResourceHeap::BindSamplers(Uint32                                                   bindingIndex,
                                 const System::Span<System::SharedPointer<ISampler>>&     samplers,
                                 const System::Span<System::SharedPointer<ITextureView>>& textureViews,
                                 Uint32                                                   startingArrayIndex)
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
        throw System::InvalidArgumentException("`bindingIndex` was not found in the resource heap layout!");

    if (Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].Binding != ResourceBinding::Sampler)
        throw System::InvalidArgumentException("`bindingIndex` was not a sampler binding!");

    if (!samplers)
        throw System::InvalidArgumentException("`samplers` was nullptr!");

    if (!textureViews)
        throw System::InvalidArgumentException("`textureViews` was nullptr!");

    if (samplers.GetLength() != textureViews.GetLength())
        throw System::InvalidArgumentException("`samplers` and `textureViews` were not the same size!");

    if (startingArrayIndex >= Description.ResourceHeapLayout->Description.ResourceBindings[arrayIndex].ArraySize)
        throw System::ArgumentOutOfRangeException("`startingArrayIndex` was out of range!");

    for (Size i = 0; i < samplers.GetLength(); i++)
    {
        if (!samplers[i])
            throw System::InvalidArgumentException("`samplers` contained a nullptr!");

        if (!textureViews[i])
            throw System::InvalidArgumentException("`textureViews` contained a nullptr!");

        if (!(Bool)(textureViews[i]->Description.ViewTexture->Description.TextureBinding & TextureBinding::Sampled))
            throw System::InvalidArgumentException("`textureViews` contained a texture that was not a sampled texture!");
    }
}

} // namespace Graphics

} // namespace Axis
