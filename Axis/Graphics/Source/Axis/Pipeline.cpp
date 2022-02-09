/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/GraphicsPch.hpp>

#include <Axis/Pipeline.hpp>

namespace Axis
{

namespace Graphics
{

IPipeline::IPipeline(const PipelineBinding& binding) :
    Binding(binding) {}

} // namespace Graphics

} // namespace Axis