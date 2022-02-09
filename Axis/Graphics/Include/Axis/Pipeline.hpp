/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_PIPELINE_HPP
#define AXIS_GRAPHICS_PIPELINE_HPP
#pragma once

#include "../../../System/Include/Axis/List.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class IResourceHeapLayout;

/// \brief Specifies the available pipeline bindings.
enum class PipelineBinding : Uint8
{
    /// \brief Graphics pipeline
    Graphics,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Graphics,
};

/// \brief Base \a `IPipeline` description struct for all types of pipelines.
struct BasePipelineDescription
{
    /// \brief Provides the layout of resource bindings.
    System::List<System::SharedPointer<IResourceHeapLayout>> ResourceHeapLayouts = {};
};

/// \brief Describes the behaviour of each rendering operation step.
class AXIS_GRAPHICS_API IPipeline : public DeviceChild
{
public:
    /// \brief The specifies the binding type of the pipeline.
    const PipelineBinding Binding;

protected:
    /// \brief Constructor
    IPipeline(const PipelineBinding& binding);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_PIPELINE_HPP