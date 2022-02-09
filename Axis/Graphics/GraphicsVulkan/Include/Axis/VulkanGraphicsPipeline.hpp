/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANGRAPHICSPIPELINE_HPP
#define AXIS_VULKANGRAPHICSPIPELINE_HPP
#pragma once

#include "../../../Include/Axis/GraphicsPipeline.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// An implementation of IGraphicsPipeline interface in Vulkan backend.
class VulkanGraphicsPipeline final : public IGraphicsPipeline
{
public:
    // Constructor
    VulkanGraphicsPipeline(const GraphicsPipelineDescription& description,
                           VulkanGraphicsDevice&              vulkanGraphicsDevice);

    // Gets the internal VkPipeline handle.
    inline VkPipeline GetVkPipelineHandle() const noexcept { return _vulkanPipeline; }

    // Gets the internal VkPipelineLayout handle.
    inline VkPipelineLayout GetVkPipelineLayoutHandle() const noexcept { return _vulkanPipelineLayout; }

private:
    VkPtr<VkPipelineLayout>    _vulkanPipelineLayout = {};
    VkPtr<VkPipeline>          _vulkanPipeline       = {};
    System::SharedPointer<IRenderPass> _implicitRenderPass   = nullptr;
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANGRAPHICSPIPELINE_HPP