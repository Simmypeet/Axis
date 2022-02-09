/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_SHADERMODULE_HPP
#define AXIS_GRAPHICS_SHADERMODULE_HPP
#pragma once

#include "../../../System/Include/Axis/String.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Supported Shader languages.
enum class ShaderLanguage : Uint8
{
    /// \brief Khronos's GLSL shader language.
    GLSL,

    /// \brief Khronos's SPIR-V binary format shader language.
    SPIRV,

    /// \brief Microsoft's HLSL shader language.
    HLSL,

    /// \brief Required for enum reflection.
    MaximumEnumValue = HLSL,
};

/// \brief Description of IShaderModule resource.
struct ShaderModuleDescription final
{
    /// \brief The language of the written shader code.
    ShaderLanguage Language = {};

    /// \brief The stage which ShaderModule belongs to. (Can only belong to one shader stage!)
    ShaderStage Stage = {};

    /// \brief The function name to be used as the entry point.
    System::String8 EntryPoint = {};
};

/// \brief Represents the program designed to on some stage in the GraphicsPipeline.
class AXIS_GRAPHICS_API IShaderModule : public DeviceChild
{
public:
    /// \brief The description of IShaderModule resource.
    const ShaderModuleDescription Description;

protected:
    /// \brief Constructor
    IShaderModule(const ShaderModuleDescription& Description);
};

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_SHADERMODULE_HPP