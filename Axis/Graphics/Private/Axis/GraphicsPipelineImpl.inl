/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_GRAPHICSPIPELINEIMPL_INL
#define AXIS_GRAPHICS_GRAPHICSPIPELINEIMPL_INL
#pragma once

#include "../../Include/Axis/GraphicsPipeline.hpp"

namespace Axis
{

namespace Graphics
{

inline constexpr DepthStencilState DepthStencilState::GetNone() noexcept
{
    return {
        false,
        false,
        CompareFunction::AlwaysTrue,
        false,
        0,
        0,
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
    };
}

inline constexpr DepthStencilState DepthStencilState::GetDefault() noexcept
{
    return {
        true,
        true,
        CompareFunction::LesserEqual,
        false,
        0,
        0,
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
    };
}

inline constexpr DepthStencilState DepthStencilState::GetDepthRead() noexcept
{
    return {
        true,
        false,
        CompareFunction::LesserEqual,
        false,
        0,
        0,
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
        {StencilOperation::Keep,
         StencilOperation::Keep,
         StencilOperation::Keep,
         CompareFunction::AlwaysTrue},
    };
}

inline constexpr RasterizerState RasterizerState::GetCullClockwise() noexcept
{
    return {
        FillMode::Solid,
        FrontFace::Clockwise,
        CullMode::FrontFace,
        false,
        0,
        0,
        0,
        true};
}

inline constexpr RasterizerState RasterizerState::GetCullCounterClockwise() noexcept
{
    return {
        FillMode::Solid,
        FrontFace::Clockwise,
        CullMode::BackFace,
        false,
        0,
        0,
        0,
        true};
}

inline constexpr RasterizerState RasterizerState::GetCullNone() noexcept
{
    return {
        FillMode::Solid,
        FrontFace::Clockwise,
        CullMode::None,
        false,
        0,
        0,
        0,
        true};
}

inline constexpr AttachmentBlendState AttachmentBlendState::GetAlphaBlend() noexcept
{
    return {
        true,
        BlendFactor::SourceAlpha,
        BlendFactor::OneMinusSourceAlpha,
        BlendFactor::SourceAlpha,
        BlendFactor::OneMinusSourceAlpha,
        BlendOperation::Add,
        BlendOperation::Add,
        ColorChannel::Red | ColorChannel::Green | ColorChannel::Blue | ColorChannel::Alpha,
    };
}

inline constexpr AttachmentBlendState AttachmentBlendState::GetAdditiveBlend() noexcept
{
    return {
        true,
        BlendFactor::SourceAlpha,
        BlendFactor::One,
        BlendFactor::SourceAlpha,
        BlendFactor::One,
        BlendOperation::Add,
        BlendOperation::Add,
        ColorChannel::Red | ColorChannel::Green | ColorChannel::Blue | ColorChannel::Alpha,
    };
}

inline constexpr AttachmentBlendState AttachmentBlendState::GetNonPremultiplied() noexcept
{
    return {
        true,
        BlendFactor::SourceAlpha,
        BlendFactor::OneMinusSourceAlpha,
        BlendFactor::SourceAlpha,
        BlendFactor::OneMinusSourceAlpha,
        BlendOperation::Add,
        BlendOperation::Add,
        ColorChannel::Red | ColorChannel::Green | ColorChannel::Blue | ColorChannel::Alpha,
    };
}

inline constexpr AttachmentBlendState AttachmentBlendState::GetOpaque() noexcept
{
    return {
        true,
        BlendFactor::One,
        BlendFactor::Zero,
        BlendFactor::One,
        BlendFactor::Zero,
        BlendOperation::Add,
        BlendOperation::Add,
        ColorChannel::Red | ColorChannel::Green | ColorChannel::Blue | ColorChannel::Alpha,
    };
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_GRAPHICSPIPELINEIMPL_INL