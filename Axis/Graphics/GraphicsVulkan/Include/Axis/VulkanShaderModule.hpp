/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_VULKANSHADERMODULE_HPP
#define AXIS_VULKANSHADERMODULE_HPP
#pragma once

#include "../../../Include/Axis/ShaderModule.hpp"
#include "VkPtr.hpp"

namespace Axis
{

namespace Graphics
{

// Forward declarations
class VulkanGraphicsDevice;

// An implementation of IShaderModule interface in Vulkan backend.
class VulkanShaderModule final : public IShaderModule
{
public:
    // Constructor
    VulkanShaderModule(const ShaderModuleDescription& description,
                       const System::StringView<Char>&        sourceCode,
                       VulkanGraphicsDevice&          vulkanGraphicsDevice);

    // Gets internal VkShaderModule handle
    inline VkShaderModule GetVkShaderModuleHandle() const noexcept { return _vulkanShaderModule; }

private:
    VkPtr<VkShaderModule> _vulkanShaderModule = {};
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_VULKANSHADERMODULE_HPP
