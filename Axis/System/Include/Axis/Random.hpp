/////////////////////////////////////////////////////////////////
/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE.txt', which is part of this source code package.
///
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// \file Random.hpp
///
/// \brief Contains pseudo random number generator \a `Axis::Math::Random` class.
///
/////////////////////////////////////////////////////////////////

#ifndef AXIS_SYSTEM_RANDOM_HPP
#define AXIS_SYSTEM_RANDOM_HPP
#pragma once

#include "SystemExport.hpp"

namespace Axis
{

/// \brief Pseudo random number generator class.
///
/// \remark This is a direct C++ port of C# System.Random class.
///
/// \see https://referencesource.microsoft.com/#mscorlib/system/random.cs
class AXIS_SYSTEM_API Random final
{
public:
    /// \brief Default constructor, the seed is captured from
    ///        The system time.
    Random() noexcept;

    /// \brief Constructs a new Random class with defined seed value.
    constexpr explicit Random(Int32 seed) noexcept;

    /// \brief Gets the next random value, range from the
    ///        0 to maximum value of \a `Axis::Int32`.
    AXIS_NODISCARD constexpr Int32 Next() noexcept;

    /// \brief Gets the next random value.
    ///
    /// \param[in] minValue Minimum possible return value.
    /// \param[in] maxValue Maximum possible return value.
    AXIS_NODISCARD constexpr Int32 Next(Int32 minValue, Int32 maxValue);

    /// \brief Gets the next random value.
    ///
    /// \param[in] maxValue Maximum possible positive return value.
    AXIS_NODISCARD constexpr Int32 Next(Int32 maxValue);

    /// \brief Gets the next random Float64 value, ranges from 0.0 to 1.0
    AXIS_NODISCARD constexpr Float64 NextDouble() noexcept;

    /// \brief Gets the next random \a `Axis::Unit8` value.
    AXIS_NODISCARD constexpr Uint8 NextByte() noexcept;

    /// \brief Gets the next random \a `Axis::Size` value.
    AXIS_NODISCARD constexpr Size NextSize() noexcept;

private:
    constexpr Float64 Sample() noexcept;
    constexpr Float64 GetSampleForLargeRange() noexcept;

    static constexpr Int32 MBIG  = 2147483647;
    static constexpr Int32 MSEED = 161803398;
    static constexpr Int32 MZ    = 0;

    Int32 inext         = {};
    Int32 inextp        = {};
    Int32 SeedArray[56] = {};
};

} // namespace Axis

#include "../../Private/Axis/RandomImpl.inl"

#endif // AXIS_SYSTEM_RANDOM_HPP