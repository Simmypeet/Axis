/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

/// \file Sampler.hpp
///
/// \brief Contains `Axis::ISampler` interface class.
///

#ifndef AXIS_GRAPHICS_SAMPLER_HPP
#define AXIS_GRAPHICS_SAMPLER_HPP
#pragma once

#include "Color.hpp"
#include "DeviceChild.hpp"
#include "GraphicsCommon.hpp"
#include "GraphicsExport.hpp"

namespace Axis
{

/// \brief Specifies the filter used in texture sampling.
enum class SamplerFilter : Uint8
{
    /// \brief Sample the color from the nearest pixel.
    Nearest,

    /// \brief Linearly interpolates the color between neighboring pixels.
    Linear,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Linear
};

/// \brief Specifies the sampling behaviour with texture coordinate
///        outside the texture.
enum class SamplerAddressMode : Uint8
{
    /// \brief Wraps the coordinate back to the begining and sample the texture.
    Repeat,

    /// \brief Wraps the coordinate back to the end and sample the texture.
    MirroredRepeat,

    /// \brief Samples the color at the edge of the texture.
    ClampToEdge,

    /// \brief Samples the color from the specified border color.
    ClampToBorder,

    /// \brief Required for enum reflection.
    MaximumEnumValue = ClampToBorder
};

/// \brief Description of ISampler resource.
struct SamplerDescription
{
    /// \brief Indicates that maximum LOD clamping should not be performed.
    static constexpr Float32 NoLODClamp = 1000.0f;

    /// \brief Is a \a `Axis::SamplerFilter` value specifying the
    ///        filter to apply to minification sampling.
    SamplerFilter MinFilter = {};

    /// \brief Is a \a `Axis::SamplerFilter` value specifying the
    ///        filter to apply to magnification sampling.
    SamplerFilter MagFilter = {};

    /// \brief Is a \a `Axis::SamplerFilter` value specifying the
    ///        filter to apply to mip sampling.
    SamplerFilter MipFilter = {};

    /// \brief Is a \a `Axis::SamplerAddressMode` value specifying the
    ///        address mode to apply when sampling the texture outside of the texture
    ///        U coordinate.
    SamplerAddressMode AddressModeU = {};

    /// \brief Is a \a `Axis::SamplerAddressMode` value specifying the
    ///        address mode to apply when sampling the texture outside of the texture
    ///        V coordinate.
    SamplerAddressMode AddressModeV = {};

    /// \brief Is a \a `Axis::SamplerAddressMode` value specifying the
    ///        address mode to apply when sampling the texture outside of the texture
    ///        W coordinate.
    SamplerAddressMode AddressModeW = {};

    /// \brief Is the bias to be added to mipmap LOD (level-of-detail) calculation before sampling
    ///        the texture.
    Float32 MipLODBias = {};

    /// \brief Speccifies whether to enable anisotropic filtering feature or not.
    Bool AnisotropyEnable = {};

    /// \brief Maximum anisotropy value for anisotropic filtering.
    Uint32 MaxAnisotropyLevel = {};

    /// \brief Is used to clamp the minimum of the computed LOD value.
    Float32 MinLOD = {};

    /// \brief Is used to clamp the maximum of the computed LOD value. To avoid LOD clamping,
    ///        specifies \a NoLODClamp.
    Float32 MaxLOD = {};

    /// \brief Border color for \a `Axis::SamplerAddressMode::ClampToBorder`.
    ColorF BorderColor = {};

    /// \brief Gets \a `Axis::SamplerDescription` for linear filtering
    ///        and texture coordinate clamping.
    inline static SamplerDescription GetLinearClamp() noexcept
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

    /// \brief Gets \a `Axis::SamplerDescription` for linear filtering
    ///        and texture coordinate wraping.
    inline static SamplerDescription GetLinearWrap() noexcept
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

    /// \brief Gets \a `Axis::SamplerDescription` for nearest filtering
    ///        and texture coordinate clamping.
    inline static SamplerDescription GetPointClamp() noexcept
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

    /// \brief Gets \a `Axis::SamplerDescription` for nearest filtering
    ///        and texture coordinate wraping.
    inline static SamplerDescription GetPointWrap() noexcept
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
};

/// \brief Represents the texture sampler object which can be sampled
///        in the shader.
class AXIS_GRAPHICS_API ISampler : public DeviceChild
{
public:
    /// \brief The description of ISampler resource.
    const SamplerDescription Description;

protected:
    /// \brief Constructor
    ISampler(const SamplerDescription& description);
};

} // namespace Axis

#endif // AXIS_GRAPHICS_SAMPLER_HPP