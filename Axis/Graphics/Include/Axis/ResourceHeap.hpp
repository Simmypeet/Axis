/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_RESOURCEHEAP_HPP
#define AXIS_GRAPHICS_RESOURCEHEAP_HPP
#pragma once

#include "../../../System/Include/Axis/Span.hpp"
#include "DeviceChild.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IResourceHeapLayout;
class IBuffer;
class ISampler;
class ITextureView;

/// \brief Description IResourceHeap resource.
struct ResourceHeapDescription final
{
    /// \brief Describes the resource binding layout in the pipeline.
    System::SharedPointer<IResourceHeapLayout> ResourceHeapLayout = {};
};

/// \brief Represents the resource binding in the shader.
class AXIS_GRAPHICS_API IResourceHeap : public DeviceChild
{
public:
    /// \brief The description of IResourceHeap resource.
    const ResourceHeapDescription Description;

    /// \brief Binds the buffer to the resource heap at specified index.
    ///
    /// \param[in] bindingIndex The resource binding index to bind.
    /// \param[in] buffers The span of buffers to bind to the resource heap.
    /// \param[in] offsets The offset of the buffer in each of \a `buffers` span to bind. (Leaves `nullptr` to set all the offsets as 0.)
    /// \param[in] sizes The size of the buffer in each of \a `buffers` span to bind. (Leaves `nullptr` to set all the sizes as the size of each buffer.)
    /// \param[in] startingArrayIndex The index at resource heap's array to start binding the resources.
    virtual void BindBuffers(Uint32                                              bindingIndex,
                             const System::Span<System::SharedPointer<IBuffer>>& buffers,
                             const System::Span<Size>&                           offsets            = nullptr,
                             const System::Span<Size>&                           sizes              = nullptr,
                             Uint32                                              startingArrayIndex = 0) = 0;

    /// \brief Binds the sampler to the resource heap at specified index.
    ///
    /// \param[in] bindingIndex The resource binding index to bind.
    /// \param[in] samplers The span of sampler objects to bind to the resource heap.
    /// \param[in] textureViews The span of texture views to bind to the resource heap.
    /// \param[in] startingArrayIndex The index at resource heap's array to start binding the resources.
    virtual void BindSamplers(Uint32                                                   bindingIndex,
                              const System::Span<System::SharedPointer<ISampler>>&     samplers,
                              const System::Span<System::SharedPointer<ITextureView>>& textureViews,
                              Uint32                                                   startingArrayIndex = 0) = 0;

protected:
    IResourceHeap(const ResourceHeapDescription& description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_RESOURCEHEAP_HPP