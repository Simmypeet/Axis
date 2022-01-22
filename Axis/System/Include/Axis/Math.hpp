/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE.txt', which is part of this source code package.

/// \file Math.hpp
///
/// \brief Contains basic mathematical functions.

#ifndef AXIS_SYSTEM_MATH_HPP
#define AXIS_SYSTEM_MATH_HPP
#pragma once

#include "Config.hpp"
#include <type_traits>

namespace Axis
{

/// \brief For both signed and unsigned numerics integral types.
template <class T>
concept IntegralType = std::is_integral_v<T>;

/// \brief For any numerics arithmetic types. (Floats, integers)
template <class T>
concept ArithmeticType = std::is_arithmetic_v<T>;

/// \brief For any floating point number types.
template <class T>
concept FloatingPointType = std::is_floating_point_v<T>;

/// \brief For unsigned integral types.
template <class T>
concept UnsignedIntegralType = std::is_unsigned_v<T> && std::is_integral_v<T>;

namespace Math
{

    /// \brief Assigns value to a specified bit position.
    ///
    /// \param[in] bitStorage Target variable to modify the bit.
    /// \param[in] position Position of bit. (starts from 0 from the lowest).
    /// \param[in] value Bit value.
    template <IntegralType T>
    AXIS_NODISCARD constexpr T AssignBitToPosition(T     bitStorage,
                                                   Uint8 position,
                                                   Bool  value) noexcept;

    /// \brief Reads a bit in variable at specific position.
    ///
    /// \param[in] bitStorage Target variable to read the bit.
    /// \param[in] position Position of bit. (starts from 0 from the lowest).
    template <IntegralType T>
    AXIS_NODISCARD constexpr Bool ReadBitPosition(T     bitStorage,
                                                  Uint8 position) noexcept;

    /// \brief Gets the value of the least significant bit.
    ///
    /// Example:
    /// from 0000 0000 0001 01010 = 42 to 0000 0000 0000 00010 = 2
    ///
    /// \param[in] value Value to get the least significant bit value.
    template <IntegralType T>
    AXIS_NODISCARD constexpr T GetLeastSignificantBit(T value) noexcept;

    /// \brief Clamps the value to be in the specific range.
    ///
    /// \param[in] value Value to clamp.
    /// \param[in] min Minimum value allowed.
    /// \param[in] max Maximum value allowed.
    template <ArithmeticType T, ArithmeticType U, ArithmeticType V>
    AXIS_NODISCARD constexpr T Clamp(T value,
                                     U min,
                                     V max) noexcept;

    /// \brief Checks if the value is in the specified range.
    ///
    /// \param[in] value Value to check.
    /// \param[in] min Minimum value allowed.
    /// \param[in] max Maximum value allowed.
    template <ArithmeticType T, ArithmeticType U, ArithmeticType V>
    AXIS_NODISCARD constexpr Bool IsInRange(T value,
                                            U min,
                                            V max) noexcept;

    /// \brief Gets the maximum value in the specified arguments.
    template <ArithmeticType T>
    AXIS_NODISCARD constexpr T Max(T first, T second) noexcept;

    /// \brief Gets the minumum value in the specified arguments.
    template <ArithmeticType T>
    AXIS_NODISCARD constexpr T Min(T first, T second) noexcept;

    /// \brief Combines the existing hash to another.
    AXIS_NODISCARD constexpr Size HashCombine(Size hash,
                                              Size anotherHash) noexcept;

    /// \brief Gets fast approximated value of inverse square root.
    ///
    /// \param[in] value the value to substitute in the formula of ( 1/ sqrt(X) )
    AXIS_NODISCARD inline Float32 FastInverseSquareRoot(Float32 value) noexcept;

    /// \brief Gets absolute value.
    template <ArithmeticType T>
    AXIS_NODISCARD constexpr T Abs(T value) noexcept;

    /// \brief Rounds the number up to the multiple of the specified number.
    ///
    /// \param[in] numToRound The number to round.
    /// \param[in] multipleOf The number of multiple.
    template <IntegralType T>
    AXIS_NODISCARD constexpr T RoundUp(T numToRound, T multipleOf) noexcept;

    /// \brief Rounds the number down to the multiple of the specified number.
    ///
    /// \param[in] numToRound The number to round.
    /// \param[in] multipleOf The number of multiple.
    template <IntegralType T>
    AXIS_NODISCARD constexpr T RoundDown(T numToRound, T multipleOf) noexcept;

    /// \brief Finds the remainder, works with integer and floating point type.
    template <ArithmeticType T>
    AXIS_NODISCARD constexpr auto Modulo(T x, T y) noexcept->decltype(auto);

    /// \brief Turns the degree value into radian.
    template <ArithmeticType T, FloatingPointType FloatingPointPrecision = Float64>
    AXIS_NODISCARD constexpr FloatingPointPrecision ToRadians(T degree) noexcept;

    /// \brief Returns the number that was rounded up to the next power of two.
    template <UnsignedIntegralType T>
    AXIS_NODISCARD constexpr T RoundToNextPowerOfTwo(T num) noexcept;

    /// \brief Checks whether the number is prime.
    template <UnsignedIntegralType T>
    AXIS_NODISCARD constexpr T IsPrime(T num) noexcept;

    /// \brief Gets the next prime number.
    template <UnsignedIntegralType T>
    AXIS_NODISCARD constexpr T NextPrime(T num) noexcept;

} // namespace Math

} // namespace Axis

#include "../../Private/Axis/MathImpl.inl"

#endif // AXIS_SYSTEM_MATH_HPP