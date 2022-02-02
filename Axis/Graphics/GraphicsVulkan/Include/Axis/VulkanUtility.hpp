/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

#ifndef AXIS_VULKANUTILITY_HPP
#define AXIS_VULKANUTILITY_HPP
#pragma once

#include "../../../Include/Axis/DeviceContext.hpp"
#include "../../../Include/Axis/GraphicsCommon.hpp"
#include "../../../Include/Axis/GraphicsPipeline.hpp"
#include "../../../Include/Axis/GraphicsSystem.hpp"
#include "../../../Include/Axis/Pipeline.hpp"
#include "../../../Include/Axis/RenderPass.hpp"
#include "../../../Include/Axis/ResourceHeapLayout.hpp"
#include "../../../Include/Axis/Sampler.hpp"
#include "../../../Include/Axis/SwapChain.hpp"
#include "../../../Include/Axis/Texture.hpp"
#include <VulkanMemoryAllocator/vk_mem_alloc.hpp>
#include <vulkan/vulkan.h>


namespace Axis
{

namespace VulkanUtility
{

constexpr VkPipelineStageFlags AllShaderStages =
    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

inline List<Uint32> ExtractDeviceQueueFamilyIndices(Uint64 deviceQueueFamilyMask) noexcept
{
    if ((deviceQueueFamilyMask & (deviceQueueFamilyMask - 1)) == 0)
        return nullptr;

    // Device queue family indices
    List<Uint32> indices = {};

    Uint64 currentDeviceQueueFamilyMask = deviceQueueFamilyMask;

    while (currentDeviceQueueFamilyMask)
    {
        Uint64 extractedDeviceQueueFamilyMask = Math::GetLeastSignificantBit(currentDeviceQueueFamilyMask);

        Uint32 index = (Uint32)std::log2(extractedDeviceQueueFamilyMask);
        indices.Append(index);

        currentDeviceQueueFamilyMask &= ~extractedDeviceQueueFamilyMask;
    }

    // SHARING_MODE_CONCURENT
    return indices;
}

inline VmaAllocationCreateInfo GetVmaAllocationCreateInfoFromResourceUsage(ResourceUsage usage) noexcept
{
    VmaAllocationCreateInfo allocationCreateInfo = {};

    switch (usage)
    {
        case ResourceUsage::Immutable:
        {
            allocationCreateInfo.usage          = VMA_MEMORY_USAGE_GPU_ONLY;
            allocationCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        }
        case ResourceUsage::Dynamic:
        {
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            break;
        }
        case ResourceUsage::StagingSource:
        {
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            break;
        }
        default:
            AXIS_VALIDATE(false, "`description.Usage` was invalid!");
    }

    return allocationCreateInfo;
}

constexpr VkAccessFlagBits GetVkAccessFlagBitsFromAccessMode(AccessMode accessMode)
{
    // clang-format off

    switch (accessMode)
    {
    case AccessMode::None:              return VK_ACCESS_NONE_KHR;
    case AccessMode::RenderTargetRead:  return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    case AccessMode::RenderTargetWrite: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case AccessMode::MemoryReadWrite:   return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    default: throw InvalidArgumentException("`accessMode` was invalid!");
    }

    // clang-format on
}

constexpr VkPipelineStageFlagBits GetVkPipelineStageFlagBitsFromPipelineStage(PipelineStage pipelineStage)
{
    // clang-format off

    switch (pipelineStage)
    {
    case PipelineStage::TopOfPipeline:     return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    case PipelineStage::DrawIndirect:      return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    case PipelineStage::VertexInput:       return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    case PipelineStage::VertexShader:      return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    case PipelineStage::FragmentShader:    return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    case PipelineStage::EarlyFragmentTest: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    case PipelineStage::LateFragmentTest:  return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    case PipelineStage::RenderTarget:      return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case PipelineStage::BottomOfPipeline:  return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    default: throw InvalidArgumentException("`pipelineStage` was invalid!");
    }

    // clang-format on
}

constexpr VkAttachmentStoreOp GetVkAttachmentStoreOpFromStoreOperation(StoreOperation operation)
{
    // clang-format off

    switch (operation)
	{
	case StoreOperation::Store:    return VK_ATTACHMENT_STORE_OP_STORE;
    case StoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default: throw InvalidArgumentException("`operation` was invalid!");
	}

    // clang-format on
}

constexpr VkAttachmentLoadOp GetVkAttachmentLoadOpFromLoadOperation(LoadOperation operation)
{
    // clang-format off

    switch (operation)
    {
    case LoadOperation::Load:     return VK_ATTACHMENT_LOAD_OP_LOAD;
    case LoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    case LoadOperation::Clear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
    default: throw InvalidArgumentException("`operation` was invalid!");
    }

    // clang-format on
}

constexpr VkImageLayout GetVkImageLayoutFromResourceState(ResourceState resourceState)
{
    // clang-format off

    switch (resourceState)
    {
    case ResourceState::Present:             return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case ResourceState::RenderTarget:        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case ResourceState::DepthStencilWrite:   return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case ResourceState::DepthStencilRead:    return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case ResourceState::TransferDestination: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case ResourceState::TransferSource:      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case ResourceState::Undefined:           return VK_IMAGE_LAYOUT_UNDEFINED;
    case ResourceState::ShaderReadOnly:      return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    default: throw InvalidArgumentException("`resourceState` was invalid!");
    }

    // clang-format on
}

constexpr GraphicsAdapterType GetGraphicsAdapterTypeFromVkPhysicalDeviceType(VkPhysicalDeviceType deviceType)
{
    // clang-format off

    switch (deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_CPU:            return GraphicsAdapterType::CPU;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return GraphicsAdapterType::Dedicated;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return GraphicsAdapterType::Integrated;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return GraphicsAdapterType::Virtual;
    default: throw InvalidArgumentException("`deviceType` was invalid!");
    }

    // clang-format on
}

constexpr QueueOperation GetQueueOperationFromVkQueueFlagBits(VkQueueFlagBits queueFlag)
{
    // clang-format off

    switch (queueFlag)
    {
    case VK_QUEUE_GRAPHICS_BIT:       return QueueOperation::Graphics;
    case VK_QUEUE_TRANSFER_BIT:       return QueueOperation::Transfer;
    case VK_QUEUE_COMPUTE_BIT:        return QueueOperation::Compute;
    case VK_QUEUE_SPARSE_BINDING_BIT: return (QueueOperation)0;
    default: throw InvalidArgumentException("`queueFlag` was invalid!");
    }

    // clang-format on
}

constexpr VkFormat GetVkFormatFromTextureFormat(TextureFormat textureFormat)
{
    // clang-format off

    switch (textureFormat)
    {
    case TextureFormat::UnormR8:                return VK_FORMAT_R8_UNORM;
    case TextureFormat::UnormR8G8B8A8:          return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::UnormB8G8R8A8:          return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::UnormB5G6R5Pack16:      return VK_FORMAT_B5G6R5_UNORM_PACK16;
    case TextureFormat::UnormB5G5R5A1Pack16:    return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
    case TextureFormat::UnormR4G4B4A4Pack16:    return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
    case TextureFormat::UnormA2R10G10B10Pack32: return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
    case TextureFormat::UnormA2B10G10R10Pack32: return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    case TextureFormat::UnormR16G16:            return VK_FORMAT_R16G16_UNORM;
    case TextureFormat::UnormR16G16B16A16:      return VK_FORMAT_R16G16B16A16_UNORM;
    case TextureFormat::FloatR16:               return VK_FORMAT_R16_SFLOAT;
    case TextureFormat::FloatR16G16:            return VK_FORMAT_R16G16_SFLOAT;
    case TextureFormat::FloatR16G16B16A16:      return VK_FORMAT_R16G16B16A16_SFLOAT;
    case TextureFormat::FloatR32:               return VK_FORMAT_R32_SFLOAT;
    case TextureFormat::FloatR32G32:            return VK_FORMAT_R32G32_SFLOAT;
    case TextureFormat::FloatR32G32B32A32:      return VK_FORMAT_R32G32B32_SFLOAT;
    case TextureFormat::UnormR8G8B8A8sRGB:      return VK_FORMAT_R8G8B8A8_SRGB;
    case TextureFormat::UnormB8G8R8A8sRGB:      return VK_FORMAT_B8G8R8A8_SRGB;
    case TextureFormat::X8UnormDepth24Pack32:   return VK_FORMAT_X8_D24_UNORM_PACK32;
    case TextureFormat::UnormStencil8:          return VK_FORMAT_S8_UINT;
    case TextureFormat::UnormDepth16Stencil8:   return VK_FORMAT_D16_UNORM_S8_UINT;
    case TextureFormat::UnormDepth24Stencil8:   return VK_FORMAT_D24_UNORM_S8_UINT;
    case TextureFormat::FloatDepth32:           return VK_FORMAT_D32_SFLOAT;
    default: throw InvalidArgumentException("`textureFormat` was invalid!");
    }

    // clang-format on
}

constexpr TextureFormat GetTextureFormatFromVkFormat(VkFormat format)
{
    // clang-format off

    switch (format)
	{
    case VK_FORMAT_R8_UNORM:                 return TextureFormat::UnormR8;
    case VK_FORMAT_R8G8B8A8_UNORM:           return TextureFormat::UnormR8G8B8A8;
    case VK_FORMAT_B8G8R8A8_UNORM:           return TextureFormat::UnormB8G8R8A8;
    case VK_FORMAT_B5G6R5_UNORM_PACK16:      return TextureFormat::UnormB5G6R5Pack16;
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:    return TextureFormat::UnormB5G5R5A1Pack16;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:    return TextureFormat::UnormR4G4B4A4Pack16;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return TextureFormat::UnormA2R10G10B10Pack32;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return TextureFormat::UnormA2B10G10R10Pack32;
    case VK_FORMAT_R16G16_UNORM:             return TextureFormat::UnormR16G16;
    case VK_FORMAT_R16G16B16A16_UNORM:       return TextureFormat::UnormR16G16B16A16;
    case VK_FORMAT_R16_SFLOAT:               return TextureFormat::FloatR16;
    case VK_FORMAT_R16G16_SFLOAT:            return TextureFormat::FloatR16G16;
    case VK_FORMAT_R16G16B16A16_SFLOAT:      return TextureFormat::FloatR16G16B16A16;
    case VK_FORMAT_R32_SFLOAT:               return TextureFormat::FloatR32;
    case VK_FORMAT_R32G32_SFLOAT:            return TextureFormat::FloatR32G32;
    case VK_FORMAT_R32G32B32_SFLOAT:         return TextureFormat::FloatR32G32B32A32;
    case VK_FORMAT_B8G8R8A8_SRGB:            return TextureFormat::UnormB8G8R8A8sRGB;
    case VK_FORMAT_R8G8B8A8_SRGB:            return TextureFormat::UnormR8G8B8A8sRGB;
    case VK_FORMAT_X8_D24_UNORM_PACK32:      return TextureFormat::X8UnormDepth24Pack32;
    case VK_FORMAT_S8_UINT:                  return TextureFormat::UnormStencil8;
    case VK_FORMAT_D16_UNORM_S8_UINT:        return TextureFormat::UnormDepth16Stencil8;
    case VK_FORMAT_D24_UNORM_S8_UINT:        return TextureFormat::UnormDepth24Stencil8;
    case VK_FORMAT_D32_SFLOAT:               return TextureFormat::FloatDepth32;
    default: throw InvalidArgumentException("`format` was invalid!");
	}

    // clang-format on
}

constexpr VkImageViewType GetVkImageViewTypeFromTextureViewDimension(TextureViewDimension textureViewDimension)
{
    // clang-format off

    switch (textureViewDimension)
    {
    case TextureViewDimension::Texture1D:      return VK_IMAGE_VIEW_TYPE_1D;
    case TextureViewDimension::Texture1DArray: return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case TextureViewDimension::Texture2D:      return VK_IMAGE_VIEW_TYPE_2D;
    case TextureViewDimension::Texture2DArray: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    default: throw InvalidArgumentException("`textureViewDimension` was invalid!");
    }

    // clang-format on
}

constexpr VkImageAspectFlagBits GetVkImageAspectFlagBitsFromTextureViewUsage(TextureViewUsage textureViewUsageFlag)
{
    // clang-format off

    switch (textureViewUsageFlag)
    {
    case TextureViewUsage::RenderTarget: return VK_IMAGE_ASPECT_COLOR_BIT;
    case TextureViewUsage::Depth:        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case TextureViewUsage::Stencil:      return VK_IMAGE_ASPECT_STENCIL_BIT;
    default: throw InvalidArgumentException("`textureViewUsageFlag` was invalid!");
    }

    // clang-format on
}

constexpr VkAccessFlags GetVkAccessFlagsFromResourceState(ResourceState resourceState)
{
    // clang-format off

    switch (resourceState)
    {
    case ResourceState::TransferDestination: return VK_ACCESS_TRANSFER_READ_BIT;
    case ResourceState::TransferSource:      return VK_ACCESS_TRANSFER_WRITE_BIT;
    case ResourceState::Index:               return VK_ACCESS_INDEX_READ_BIT;
    case ResourceState::Vertex:              return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    case ResourceState::DepthStencilRead:    return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    case ResourceState::DepthStencilWrite:   return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    case ResourceState::Uniform:             return VK_ACCESS_UNIFORM_READ_BIT;
    case ResourceState::Present:             return (VkAccessFlagBits)0;
    case ResourceState::RenderTarget:        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case ResourceState::Undefined:           return (VkAccessFlagBits)0;
    case ResourceState::ShaderReadOnly:      return VK_ACCESS_SHADER_READ_BIT;
    default: throw InvalidArgumentException("`resourceState` was invalid!");
    }

    // clang-format on
}

constexpr VkImageAspectFlags GetVkImageAspectFlagsFromTextureFormat(TextureFormat textureFormat)
{
    // clang-format off

    switch (textureFormat)
    {   
    case TextureFormat::UnormR8:
    case TextureFormat::UnormR8G8B8A8:
    case TextureFormat::UnormB5G6R5Pack16:
    case TextureFormat::UnormB5G5R5A1Pack16:
    case TextureFormat::UnormA2B10G10R10Pack32:
    case TextureFormat::UnormR4G4B4A4Pack16:
    case TextureFormat::UnormA2R10G10B10Pack32:
    case TextureFormat::UnormR16G16:
    case TextureFormat::UnormR16G16B16A16:
    case TextureFormat::FloatR16:
    case TextureFormat::FloatR16G16:
    case TextureFormat::FloatR16G16B16A16:
    case TextureFormat::FloatR32:
    case TextureFormat::FloatR32G32:
    case TextureFormat::FloatR32G32B32A32:
    case TextureFormat::UnormR8G8B8A8sRGB:
    case TextureFormat::UnormB8G8R8A8sRGB:
    case TextureFormat::UnormB8G8R8A8:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    case TextureFormat::X8UnormDepth24Pack32:
    case TextureFormat::FloatDepth32:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case TextureFormat::UnormStencil8:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case TextureFormat::UnormDepth16Stencil8:
    case TextureFormat::UnormDepth24Stencil8:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default: throw InvalidArgumentException("`textureFormat` was invalid!");
    }

    // clang-format on
}

constexpr VkPipelineStageFlags GetVkPipelineStageFlagsFromResourceState(ResourceState resourceState)
{
    // clang-format off

    switch (resourceState)
    {
    case ResourceState::TransferDestination: return VK_PIPELINE_STAGE_TRANSFER_BIT;
    case ResourceState::TransferSource:      return VK_PIPELINE_STAGE_TRANSFER_BIT;
    case ResourceState::Index:               return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    case ResourceState::Vertex:              return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    case ResourceState::Uniform:             return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    case ResourceState::DepthStencilRead:    return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    case ResourceState::DepthStencilWrite:   return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    case ResourceState::RenderTarget:        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case ResourceState::Present:             return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    case ResourceState::Undefined:           return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    case ResourceState::ShaderReadOnly:      return AllShaderStages;
    default: throw InvalidArgumentException("`resourceState` was invalid!");
    }

    // clang-format on
}

constexpr VkDescriptorType GetVkDescriptorTypeFromResourceBinding(ResourceBinding resourceBinding)
{
    // clang-format off

    switch (resourceBinding)
    {
    case ResourceBinding::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case ResourceBinding::Sampler:       return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default: throw InvalidArgumentException("`resourceBinding` was invalid!");
    }

    // clang-format on
}

constexpr VkShaderStageFlagBits GetVkShaderStageFlagBitsFromShaderStage(ShaderStage stage)
{
    // clang-format off

    switch (stage)
    {
    case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
    default: throw InvalidArgumentException("`stage` was invalid!");
    }

    // clang-format on
}

constexpr VkFormat GetVkFormatFromShaderDataType(ShaderDataType stage)
{
    // clang-format off

    switch (stage)
	{
	case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
    case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
    case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
    case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
    default: throw InvalidArgumentException("`stage` was invalid!");
	}

    // clang-format on
}

constexpr VkPipelineBindPoint GetVkPipelineBindPointFromPipelineBinding(PipelineBinding pipelineBinding)
{
    // clang-format off

    switch (pipelineBinding)
	{
	case PipelineBinding::Graphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
    default: throw InvalidArgumentException("`pipelineBinding` was invalid!");
	}

    // clang-format on
}

constexpr VkIndexType GetVkIndexTypeFromIndexType(IndexType indexType)
{
    // clang-format off

    switch (indexType)
	{
	case IndexType::Uint16: return VK_INDEX_TYPE_UINT16;
	case IndexType::Uint32: return VK_INDEX_TYPE_UINT32;
    default: throw InvalidArgumentException("`indexType` was invalid!");
	}

    // clang-format on
}

constexpr VkImageType GetVkImageTypeFromTextureDimension(TextureDimension dimension)
{
    // clang-format off

    switch (dimension)
    {
    case TextureDimension::Texture1D: return VK_IMAGE_TYPE_1D;
    case TextureDimension::Texture2D: return VK_IMAGE_TYPE_2D;
    case TextureDimension::Texture3D: return VK_IMAGE_TYPE_3D;
    default: throw InvalidArgumentException("`dimension` was invalid!");
    }

    // clang-format on
}

constexpr VkImageUsageFlagBits GetVkImageUsageFlagBitsFromTextureBinding(TextureBinding binding)
{
    // clang-format off

    switch (binding)
    {
    case TextureBinding::TransferDestination:    return VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    case TextureBinding::TransferSource:         return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    case TextureBinding::Sampled:                return VK_IMAGE_USAGE_SAMPLED_BIT;
    case TextureBinding::RenderTarget:           return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    case TextureBinding::DepthStencilAttachment: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    default: throw InvalidArgumentException("`binding` was invalid!");
    }

    // clang-format on
}

constexpr VkCompareOp GetVkCompareOpFromCompareFunction(CompareFunction function)
{
    // clang-format off

    switch (function)
	{
    case CompareFunction::AlwaysTrue:   return VK_COMPARE_OP_ALWAYS;
    case CompareFunction::AlwaysFalse:  return VK_COMPARE_OP_NEVER;
    case CompareFunction::Greater:      return VK_COMPARE_OP_GREATER;
    case CompareFunction::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareFunction::Lesser:       return VK_COMPARE_OP_LESS;
    case CompareFunction::LesserEqual:  return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareFunction::Equal:        return VK_COMPARE_OP_EQUAL;
    case CompareFunction::NotEqual:     return VK_COMPARE_OP_NOT_EQUAL;
    default: throw InvalidArgumentException("`function` was invalid!");
	}

    // clang-format on
}

constexpr VkStencilOp GetVkStencilOpFromStencilOperation(StencilOperation operation)
{
    // clang-format off

    switch (operation)
	{
    case StencilOperation::Increment:     return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case StencilOperation::IncrementWrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case StencilOperation::Decrement:     return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case StencilOperation::DecrementWrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
    case StencilOperation::Keep:          return VK_STENCIL_OP_KEEP;
    case StencilOperation::Zero:          return VK_STENCIL_OP_ZERO;
    case StencilOperation::Replace:       return VK_STENCIL_OP_REPLACE;
    case StencilOperation::Invert:        return VK_STENCIL_OP_INVERT;
    default: throw InvalidArgumentException("`operation` was invalid!");
	}

    // clang-format on
}

constexpr VkPolygonMode GetVkPolygonModeFromFillMode(FillMode fillMode)
{
    // clang-format off

    switch (fillMode)
    {
    case FillMode::Solid:      return VK_POLYGON_MODE_FILL;
    case FillMode::WiredFrame: return VK_POLYGON_MODE_LINE;
    default: throw InvalidArgumentException("`fillMode` was invalid!");
    }

    // clang-format on
}

constexpr VkFrontFace GetVkFrontFaceFromFrontFace(FrontFace frontFace)
{
    // clang-format off

    switch (frontFace)
    {
    case FrontFace::Clockwise:        return VK_FRONT_FACE_CLOCKWISE;
    case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    default: throw InvalidArgumentException("`frontFace` was invalid!");
    }

    // clang-format on
}

constexpr VkCullModeFlagBits GetVkCullModeFlagBitsFromCullMode(CullMode cullMode)
{
    // clang-format off

    switch (cullMode)
    {
    case CullMode::None:      return VK_CULL_MODE_NONE;
    case CullMode::BackFace:  return VK_CULL_MODE_BACK_BIT;
    case CullMode::FrontFace: return VK_CULL_MODE_FRONT_BIT;
    default: throw InvalidArgumentException("`cullMode` was invalid!");
    }

    // clang-format on
}

constexpr VkBlendOp GetVkBlendOpFromBlendOperation(BlendOperation operation)
{
    // clang-format off
    
    switch (operation)
	{
	case BlendOperation::Add:              return VK_BLEND_OP_ADD;
    case BlendOperation::Subtract:         return VK_BLEND_OP_SUBTRACT;
    case BlendOperation::SubtractReverse:  return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOperation::Min:              return VK_BLEND_OP_MIN;
    case BlendOperation::Max:              return VK_BLEND_OP_MAX;
    default: throw InvalidArgumentException("`operation` was invalid!");
	}

    // clang-format on
}

constexpr VkColorComponentFlagBits GetVkColorComponentFlagBitsFromColorChannel(ColorChannel channel)
{
    // clang-format off
    
    switch (channel)
	{
	case ColorChannel::Red:   return VK_COLOR_COMPONENT_R_BIT;
    case ColorChannel::Green: return VK_COLOR_COMPONENT_G_BIT;
    case ColorChannel::Blue:  return VK_COLOR_COMPONENT_B_BIT;
    case ColorChannel::Alpha: return VK_COLOR_COMPONENT_A_BIT;
    default: throw InvalidArgumentException("`channel` was invalid!");
	}

    // clang-format on
}

constexpr VkBlendFactor GetVkBlendFactorFromBlendFactor(BlendFactor factor)
{
    // clang-format off

    switch (factor)
	{
	case BlendFactor::Zero:                  return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::One:                   return VK_BLEND_FACTOR_ONE;
    case BlendFactor::SourceColor:           return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::OneMinusSourceColor:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DestColor:             return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::OneMinusDestColor:     return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::SourceAlpha:           return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::OneMinusSourceAlpha:   return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DestAlpha:             return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::OneMinusDestAlpha:     return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::ConstantColor:         return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case BlendFactor::OneMinusConstantColor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::ConstantAlpha:         return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::Source1Color:          return VK_BLEND_FACTOR_SRC1_COLOR;
    case BlendFactor::Source1Alpha:          return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BlendFactor::OneMinusSource1Color:  return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::OneMinusSource1Alpha:  return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default: throw InvalidArgumentException("`factor` was invalid!");
	}

    // clang-format on
}

constexpr VkLogicOp GetVkLogicOpFromLogicOperation(LogicOperation operation)
{
    // clang-format off

    switch (operation)
	{
	case LogicOperation::Clear:        return VK_LOGIC_OP_CLEAR;
    case LogicOperation::And:          return VK_LOGIC_OP_AND;
    case LogicOperation::AndReverse:   return VK_LOGIC_OP_AND_REVERSE;
    case LogicOperation::Copy:         return VK_LOGIC_OP_COPY;
    case LogicOperation::AndInverted:  return VK_LOGIC_OP_AND_INVERTED;
    case LogicOperation::NoOperation:  return VK_LOGIC_OP_NO_OP;
    case LogicOperation::ExclusiveOr:  return VK_LOGIC_OP_XOR;
    case LogicOperation::Or:           return VK_LOGIC_OP_OR;
    case LogicOperation::NOR:          return VK_LOGIC_OP_NOR;
    case LogicOperation::Equivalent:   return VK_LOGIC_OP_EQUIVALENT;
    case LogicOperation::Invert:       return VK_LOGIC_OP_INVERT;
    case LogicOperation::OrReverse:    return VK_LOGIC_OP_OR_REVERSE;
    case LogicOperation::CopyInverted: return VK_LOGIC_OP_COPY_INVERTED;
    case LogicOperation::OrInverted:   return VK_LOGIC_OP_OR_INVERTED;
    case LogicOperation::NAND:         return VK_LOGIC_OP_NAND;
    case LogicOperation::Set:          return VK_LOGIC_OP_SET;
    default: throw InvalidArgumentException("`operation` was invalid!");
	}

    // clang-format on
}

constexpr VkFilter GetVkFilterFromSamplerFilter(SamplerFilter samplerFilter)
{
    // clang-format off

    switch (samplerFilter)
    {
    case SamplerFilter::Linear:  return VK_FILTER_LINEAR;
    case SamplerFilter::Nearest: return VK_FILTER_NEAREST;
    default: throw InvalidArgumentException("`samplerFilter` was invalid!");
    }

    // clang-format on
}

constexpr VkSamplerMipmapMode GetVkSamplerMipmapModeFromSamplerFilter(SamplerFilter samplerFilter)
{
    // clang-format off

    switch (samplerFilter)
    {
    case SamplerFilter::Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    case SamplerFilter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    default: throw InvalidArgumentException("`samplerFilter` was invalid!");
    }

    // clang-format on
}

constexpr VkSamplerAddressMode GetVkSamplerAddressModeFromSamplerAddressMode(SamplerAddressMode addressMode)
{
    // clang-format off

    switch (addressMode)
    {
    case SamplerAddressMode::ClampToBorder:  return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case SamplerAddressMode::ClampToEdge:    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case SamplerAddressMode::Repeat:         return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case SamplerAddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    default: throw InvalidArgumentException("`addressMode` was invalid!");
    }

    // clang-format on
}

constexpr VkColorComponentFlags GetVkColorComponentFlagsColorChannelFlags(ColorChannelFlags channels)
{
    VkColorComponentFlags flagToReturn = {};
    while ((Bool)channels)
    {
        ColorChannelFlags currentType = (ColorChannelFlags)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(channels));

        flagToReturn |= GetVkColorComponentFlagBitsFromColorChannel(currentType);

        channels &= ~currentType;
    }
    return flagToReturn;
}

constexpr VkCullModeFlags GetVkCullModeFlagsFromCullModeFlags(CullModeFlags cullModes)
{
    VkCullModeFlags flagToReturn = {};
    while ((Bool)cullModes)
    {
        CullMode currentType = (CullMode)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(cullModes));

        flagToReturn |= GetVkCullModeFlagBitsFromCullMode(currentType);

        cullModes &= ~currentType;
    }
    return flagToReturn;
}

constexpr VkImageUsageFlags GetVkImageUsageFlagsFromTextureBindingFlags(TextureBindingFlags binding)
{
    VkImageUsageFlags flagToReturn = {};
    while ((Bool)binding)
    {
        TextureBinding currentType = (TextureBinding)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(binding));

        flagToReturn |= GetVkImageUsageFlagBitsFromTextureBinding(currentType);

        binding &= ~currentType;
    }
    return flagToReturn;
}

constexpr VkShaderStageFlags GetVkShaderStageFlagsFromShaderStageFlags(ShaderStageFlags stages)
{
    VkShaderStageFlags flagToReturn = {};
    while ((Bool)stages)
    {
        ShaderStage currentType = (ShaderStageFlags)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(stages));

        flagToReturn |= GetVkShaderStageFlagBitsFromShaderStage(currentType);

        stages &= ~currentType;
    }
    return flagToReturn;
}

constexpr QueueOperationFlags GetQueueOperationFlagsFromVkQueueFlags(VkQueueFlags type)
{
    QueueOperationFlags flagToReturn = {};
    while (type)
    {
        VkQueueFlagBits currentType = (VkQueueFlagBits)Math::GetLeastSignificantBit(type);

        flagToReturn |= GetQueueOperationFromVkQueueFlagBits(currentType);

        type &= ~currentType;
    }
    return flagToReturn;
}

constexpr VkImageAspectFlags GetVkImageAspectFlagsFromTextureViewUsageFlags(TextureViewUsageFlags type)
{
    VkImageAspectFlags flagToReturn = {};
    while ((Bool)type)
    {
        TextureViewUsage currentType = (TextureViewUsage)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(type));

        flagToReturn |= GetVkImageAspectFlagBitsFromTextureViewUsage(currentType);

        type &= ~currentType;
    }
    return flagToReturn;
}

constexpr VkPipelineStageFlags GetVkPipelineStageFlagsFromPipelineStageFlags(PipelineStageFlags type)
{
    VkPipelineStageFlags flagToReturn = {};
    while ((Bool)type)
    {
        PipelineStageFlags currentType = (PipelineStageFlags)Math::GetLeastSignificantBit(Enum::GetUnderlyingValue(type));

        flagToReturn |= GetVkPipelineStageFlagBitsFromPipelineStage(type);

        type &= ~currentType;
    }
    return flagToReturn;
}

} // namespace VulkanUtility

} // namespace Axis

#endif
