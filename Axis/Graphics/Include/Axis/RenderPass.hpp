/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_RENDERPASS_HPP
#define AXIS_GRAPHICS_RENDERPASS_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Specifies how the content should be treated at the first use of subpass.
enum class LoadOperation : Uint8
{
    /// \brief Previous content of the image will be preserved.
    Load,

    /// \brief Content will be cleared with specified constant value.
    Clear,

    /// \brief The content will be undefined it might be cleared or preserved, depends on the implementation.
    DontCare,

    /// \brief Required for enum reflection.
    MaximumEnumValue = DontCare,
};

/// \brief Specifies how the content should be treated at the last use of subpass.
enum class StoreOperation : Uint8
{
    /// \brief The content will be written into the memory.
    Store,

    /// \brief The content is not needed after the rendering, and might be discarded.
    DontCare,

    /// \brief Required for enum reflection.
    MaximumEnumValue = DontCare,
};

/// \brief The attachment which will be used in RenderPass.
struct RenderPassAttachment final
{
    /// \brief Format of the texture view.
    TextureFormat Format = {};

    /// \brief Specifies the number of image sampling.
    Uint32 Samples = {};

    /// \brief LoadOperation to use with Color and Depth attachment (If provided).
    LoadOperation ColorDepthLoadOperation = {};

    /// \brief StoreOperation to use with Color and Depth attachment (If provided).
    StoreOperation ColorDepthStoreOperation = {};

    /// \brief LoadOperation to use with Stencil attachment (If provided).
    LoadOperation StencilLoadOperation = {};

    /// \brief StoreOperation to use with Stencil attachment (If provided).
    StoreOperation StencilStoreOperation = {};

    /// \brief The resource state of attachment will be in when render pass instance begin.
    ResourceStateFlags InitialState = {};

    /// \brief The resource state of attachment will be transited into when the render pass instance ended.
    ResourceStateFlags FinalState = {};
};

/// \brief Reference (index) to the render pass's attachments.
struct AttachmentReference final
{
    /// \brief Specified that this attachment reference won't be used.
    static constexpr Uint32 Unused = ~0U;

    /// \brief Index reference corresponding in the RenderPass's attachments array.
    Uint32 Index = {};

    /// \brief The layout of texture view will to be transited to in the subpass.
    ResourceStateFlags SubpassState = {};
};

/// \brief RenderPass's Subpass specification
struct SubpassDescription final
{
    /// \brief References to the RenderTarget attachments, The array correspond to the out index in fragment shader.
    System::List<AttachmentReference> RenderTargetReferences = {};

    /// \brief References to the Input attachments.
    System::List<AttachmentReference> InputReferences = {};

    /// \brief Reference to the DepthStencil attachments.
    AttachmentReference DepthStencilReference = {};
};

/// \brief Specifies memory and execution dependencies between the subpasses
struct SubpassDependency final
{
    /// \brief Specifies to implicit(external) subpass.
    static constexpr Uint32 SubpassExternal = (~0);

    /// \brief The index of dependant subpass in the dependency. (after)
    Uint32 DestSubpassIndex = {};

    /// \brief The index of source subpass in the dependency. (before)
    Uint32 SourceSubpassIndex = {};

    /// \brief At which stage of destination subpass to start waiting for the source subpass. (after)
    PipelineStageFlags DestStages = {};

    /// \brief At which stage of source subpass for destination subpass to wait for. (before)
    PipelineStageFlags SourceStages = {};

    /// \brief At which memory access stage of destination subpass to start waiting for the source subpass. (after)
    AccessMode DestAccessMode = {};

    /// \brief At which memory access stage of source subpass for destination subpass to wait for. (before)
    AccessMode SourceAccessMode = {};
};

/// \brief Description of IRenderPass resource.
struct RenderPassDescription final
{
    /// \brief Specifies all attachments which Subpasses will reference to.
    System::List<RenderPassAttachment> Attachments = {};

    /// \brief All subpasses contained in the RenderPass.
    System::List<SubpassDescription> Subpasses = {};

    /// \brief All dependencies contained in the RenderPass.
    System::List<SubpassDependency> Dependencies = {};
};

/// \brief Represents collections of Subpasses and Attachments and describes how attachments will be use over the courses of Subpasses.
///        This contains the informations needed to finish the rendering operations.
class AXIS_GRAPHICS_API IRenderPass : public DeviceChild
{
public:
    /// \brief The description of IRenderPass resource.
    const RenderPassDescription Description;

protected:
    /// \brief Constructor
    IRenderPass(const RenderPassDescription& Description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_RENDERPASS_HPP