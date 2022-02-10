/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_SAMPLERIMPL_INL
#define AXIS_GRAPHICS_SAMPLERIMPL_INL
#pragma once

#include "../../Include/Axis/Sampler.hpp"

namespace Axis
{

namespace Graphics
{

inline constexpr SamplerDescription SamplerDescription::GetLinearClamp() noexcept
{
    return {
        SamplerFilter::Linear,
        SamplerFilter::Linear,
        SamplerFilter::Linear,
        SamplerAddressMode::ClampToEdge,
        SamplerAddressMode::ClampToEdge,
        SamplerAddressMode::ClampToEdge,
        0,
        false,
        0,
        0,
        NoLODClamp,
        {0.0f, 0.0f, 0.0f, 0.0f}};
}

inline constexpr SamplerDescription SamplerDescription::GetLinearWrap() noexcept
{
    return {
        SamplerFilter::Linear,
        SamplerFilter::Linear,
        SamplerFilter::Linear,
        SamplerAddressMode::Repeat,
        SamplerAddressMode::Repeat,
        SamplerAddressMode::Repeat,
        0,
        false,
        0,
        0,
        NoLODClamp,
        {0.0f, 0.0f, 0.0f, 0.0f}};
}

inline constexpr SamplerDescription SamplerDescription::GetPointClamp() noexcept
{
    return {
        SamplerFilter::Nearest,
        SamplerFilter::Nearest,
        SamplerFilter::Nearest,
        SamplerAddressMode::ClampToEdge,
        SamplerAddressMode::ClampToEdge,
        SamplerAddressMode::ClampToEdge,
        0,
        false,
        0,
        0,
        NoLODClamp,
        {0.0f, 0.0f, 0.0f, 0.0f}};
}

inline constexpr SamplerDescription SamplerDescription::GetPointWrap() noexcept
{
    return {
        SamplerFilter::Nearest,
        SamplerFilter::Nearest,
        SamplerFilter::Nearest,
        SamplerAddressMode::Repeat,
        SamplerAddressMode::Repeat,
        SamplerAddressMode::Repeat,
        0,
        false,
        0,
        0,
        NoLODClamp,
        {0.0f, 0.0f, 0.0f, 0.0f}};
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_SAMPLERIMPL_INL
