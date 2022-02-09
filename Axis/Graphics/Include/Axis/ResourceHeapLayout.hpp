/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_RESOURCEHEAPLAYOUT_HPP
#define AXIS_GRAPHICS_RESOURCEHEAPLAYOUT_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Specifies the type of resource binding.
enum class ResourceBinding : Uint8
{
    /// \brief Unifrom buffer resource binding.
    UniformBuffer,

    /// \brief Texture sampler object resource binding.
    Sampler,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Sampler,
};

/// \brief Specifies the resource binding.
struct ResourceLayoutBinding
{
    /// \brief Resource binding index in the shader.
    Uint32 BindingIndex = {};

    /// \brief Which shader stages can access this resource. (Axis::ShaderStageFlag can be combined with bitwise OR operator)
    ShaderStageFlags StageFlags = {};

    /// \brief Specifies the type of resource binding.
    ResourceBinding Binding = {};

    /// \brief Specifies the number of an array resource.
    Uint32 ArraySize = {};
};

/// \brief Describes the specification of IPipelineLayout resource.
struct ResourceHeapLayoutDescription final
{
    /// \brief List of all resource bindings.
    System::List<ResourceLayoutBinding> ResourceBindings = {};
};

/// \brief Describes the resources that will be used over the course of pipeline.
class AXIS_GRAPHICS_API IResourceHeapLayout : public DeviceChild
{
public:
    /// \brief The description of IPipelineLayout resource.
    const ResourceHeapLayoutDescription Description;

protected:
    /// \brief Constructor
    IResourceHeapLayout(const ResourceHeapLayoutDescription& Description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_RESOURCEHEAPLAYOUT_HPP