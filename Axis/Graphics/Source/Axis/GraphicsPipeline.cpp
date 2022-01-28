/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/GraphicsPipeline.hpp>

namespace Axis
{

Size VertexBindingDescription::GetStride() const
{
    if (!Attributes)
        return 0;

    Size size = 0;
    for (const auto& attrib : Attributes)
    {
        size += GetShaderDataTypeSize(attrib.Type);
    }

    return size;
}

IGraphicsPipeline::IGraphicsPipeline(const GraphicsPipelineDescription& description) :
    IPipeline(PipelineBinding::Graphics),
    Description(description) {}

} // namespace Axis