/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/GraphicsVulkanPch.hpp>

#include <Axis/VulkanGraphicsDevice.hpp>
#include <Axis/VulkanGraphicsSystem.hpp>
#include <Axis/VulkanShaderModule.hpp>
#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/DirStackFileIncluder.h>
#include <iostream>
#include <spirv-tools/libspirv.h>

namespace Axis
{

namespace Graphics
{

VulkanShaderModule::VulkanShaderModule(const ShaderModuleDescription&  description,
                                       const System::StringView<Char>& sourceCode,
                                       VulkanGraphicsDevice&           vulkanGraphicsDevice) :
    IShaderModule(description)
{
    constexpr const TBuiltInResource DefaultTBuiltInResource = {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,

        /* .limits = */ {
            /* .nonInductiveForLoops = */ 1,
            /* .whileLoops = */ 1,
            /* .doWhileLoops = */ 1,
            /* .generalUniformIndexing = */ 1,
            /* .generalAttributeMatrixVectorIndexing = */ 1,
            /* .generalVaryingIndexing = */ 1,
            /* .generalSamplerIndexing = */ 1,
            /* .generalVariableIndexing = */ 1,
            /* .generalConstantMatrixVectorIndexing = */ 1,
        }};

    vulkanGraphicsDevice.AddDeviceChild(*this);

    static Bool s_GlslangLibInitialized = false;

    if (!s_GlslangLibInitialized)
    {
        s_GlslangLibInitialized = true;
        Bool result             = glslang::InitializeProcess();

        if (!result)
            throw System::ExternalException("Failed to initialize glslang library!");
    }

    auto CreateVkShaderModule = [&]() -> VkShaderModule {
        constexpr auto GetShaderStage = [](ShaderStage stage) -> EShLanguage {
            // clang-format off

        switch (stage)
        {

        case ShaderStage::Vertex:
            return EShLanguage::EShLangVertex;
        case ShaderStage::Fragment:
            return EShLanguage::EShLangFragment;
        default:
            throw System::InvalidArgumentException("`ShaderStage` was invalid!");

        }

            // clang-format on
        };

        constexpr auto GetShaderLanguage = [](ShaderLanguage stage) -> glslang::EShSource {
            // clang-format off

        switch (stage)
        {

        case ShaderLanguage::GLSL:
            return  glslang::EShSource::EShSourceGlsl;
        case ShaderLanguage::HLSL:
            return glslang::EShSource::EShSourceHlsl;
        default:
            throw System::InvalidArgumentException("`ShaderLanguage` was invalid!");
        }

            // clang-format on
        };


        EShLanguage        shaderStage    = GetShaderStage(description.Stage);
        glslang::EShSource shaderLanguage = GetShaderLanguage(description.Language);

        glslang::TShader shader = glslang::TShader(shaderStage);

        const char* sourceCodeCopy = sourceCode.GetCString();

        shader.setStrings(&sourceCodeCopy, 1);
        shader.setEntryPoint(description.EntryPoint.GetCString());
        shader.setEnvInput(shaderLanguage, shaderStage, glslang::EShClient::EShClientVulkan, 450);

        auto        vulkanGraphicsSystem = (VulkanGraphicsSystem*)(vulkanGraphicsDevice.GraphicsSystem);
        const auto& vulkanPhysicalDevice = vulkanGraphicsSystem->GetVulkanPhysicalDevices()[vulkanGraphicsDevice.GraphicsAdapterIndex];

        glslang::EShTargetClientVersion targetVersion = {};

        auto apiVersion = vulkanPhysicalDevice.GetVkPhysicalDeviceProperties().apiVersion;


        switch (VK_API_VERSION_MINOR(apiVersion))
        {
            case 0:
                targetVersion = glslang::EShTargetVulkan_1_0;
                break;

            case 1:
                targetVersion = glslang::EShTargetVulkan_1_1;
                break;

            case 2:
                targetVersion = glslang::EShTargetVulkan_1_2;
                break;
        }

        shader.setEnvClient(glslang::EShClient::EShClientVulkan, targetVersion);
        shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_0);

        std::string          preprocessedGlsl;
        DirStackFileIncluder Includer;

        Bool glslangResult = shader.preprocess(&DefaultTBuiltInResource,
                                               450,
                                               EProfile::ENoProfile,
                                               false,
                                               false,
                                               EShMessages::EShMsgDefault,
                                               &preprocessedGlsl,
                                               Includer);

        if (!glslangResult)
            throw System::ExternalException("glslang failed to compile shader!");


        if (!shader.parse(&DefaultTBuiltInResource,
                          450,
                          false,
                          EShMessages::EShMsgDefault))
            throw System::ExternalException("glslang failed to compile shader!");

        glslang::TProgram shaderProgram;

        shaderProgram.addShader(&shader);


        if (!shaderProgram.link(EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules))
            throw System::ExternalException("glslang failed to compile shader!");

        spv::SpvBuildLogger logger;
        glslang::SpvOptions spvOptions;
        spvOptions.validate = true;

        const auto*               intermediate = shaderProgram.getIntermediate(shaderStage);
        std::vector<unsigned int> spirv        = {};

        ::glslang::GlslangToSpv(*intermediate, spirv, &logger, &spvOptions);

#ifdef AXIS_DEBUG
        auto message = logger.getAllMessages();
        if (!message.empty())
            std::cout << message;
#endif

        VkShaderModule shaderModule = {};

        VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
        shaderModuleCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize                 = spirv.size() * sizeof(Uint32);
        shaderModuleCreateInfo.pCode                    = spirv.data();

        auto vkResult = vkCreateShaderModule(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(),
                                             &shaderModuleCreateInfo,
                                             nullptr,
                                             &shaderModule);

        if (vkResult != VK_SUCCESS)
            throw System::ExternalException("Failed to create VkShaderModule!");

        return shaderModule;
    };

    auto DestroyVkShaderModule = [this](VkShaderModule vkShaderModule) {
        vkDestroyShaderModule(((VulkanGraphicsDevice*)GetCreatorDevice())->GetVkDeviceHandle(), vkShaderModule, nullptr);
    };

    _vulkanShaderModule = VkPtr<VkShaderModule>(CreateVkShaderModule, std::move(DestroyVkShaderModule));
}

} // namespace Graphics

} // namespace Axis
