/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Vector2.hpp
///
/// \brief Contains \a `Axis::Vector3` mathematical vector template struct.

#ifndef AXIS_SYSTEM_VECTOR2_HPP
#define AXIS_SYSTEM_VECTOR2_HPP
#pragma once

#include "Math.hpp"

namespace Axis
{

/// \brief 2D mathematical vector template definition.
///
/// It's often refers to point position and size which contains width and height.
template <ArithmeticType T>
struct Vector2 final
{
    /// \brief Default constructor
    constexpr Vector2() noexcept = default;

    /// \brief Constructs a 2D vector object.
    ///
    /// \param[in] x X component of the vector.
    /// \param[in] y Y component of the vector.
    template <ArithmeticType U, ArithmeticType W>
    constexpr Vector2(U x,
                      W y) noexcept;

    /// \brief Constructs a 2D vector object from different arithmetic types.
    ///
    /// \param[in] other Other 2D vector object.
    template <ArithmeticType U>
    constexpr Vector2(const Vector2<U>& other) noexcept;

    /// \brief Constructs a 2D vector object.
    ///
    /// \param[in] value Scalar for both X and Y components.
    constexpr explicit Vector2(T value) noexcept;

    /// \brief Gets the magnitude of the vector.
    ///
    /// uses this following formula : \f$\sqrt{x^2 + y^2}\f$
    AXIS_NODISCARD constexpr BigFloat GetMagnitude() const noexcept;

    /// \brief Turns this vector into the unit vector.
    ///
    /// have the magnitude of 1 but still having the same direction
    constexpr void Normalize() noexcept requires(FloatingPointType<T>);

    /// \brief Gets the copy of this normalized vector.
    ///
    /// has the magnitude of 1 but still having the same direction
    AXIS_NODISCARD constexpr Vector2 NormalizeCopy() const noexcept requires(FloatingPointType<T>);

    /// \brief Returns the dot product of this vector and other vector.
    AXIS_NODISCARD constexpr T DotProduct(const Vector2& other) const noexcept;

    /// \brief Addition operator.
    AXIS_NODISCARD constexpr Vector2 operator+(const Vector2& other) const noexcept;

    /// \brief Subtraction operator.
    AXIS_NODISCARD constexpr Vector2 operator-(const Vector2& other) const noexcept;

    /// \brief Multiplication operator.
    AXIS_NODISCARD constexpr Vector2 operator*(const Vector2& other) const noexcept;

    /// \brief Division operator.
    AXIS_NODISCARD constexpr Vector2 operator/(const Vector2& other) const noexcept;

    /// \brief Multiplication operator.
    AXIS_NODISCARD constexpr Vector2 operator*(T value) const noexcept;

    /// \brief Division operator.
    AXIS_NODISCARD constexpr Vector2 operator/(T value) const noexcept;

    /// \brief Equal operator
    AXIS_NODISCARD constexpr Bool operator==(const Vector2& other) const noexcept;

    /// \brief Not equal operator
    AXIS_NODISCARD constexpr Bool operator!=(const Vector2& other) const noexcept;

    /// Compound addition operator.
    constexpr Vector2& operator+=(const Vector2& other) noexcept;

    /// Compound subtraction operator.
    constexpr Vector2& operator-=(const Vector2& other) noexcept;

    /// Compound multiplication operator.
    constexpr Vector2& operator*=(const Vector2& other) noexcept;

    /// Compound division operator.
    constexpr Vector2& operator/=(const Vector2& other) noexcept;

    /// Compound multiplication operator.
    constexpr Vector2& operator*=(T value) noexcept;

    /// Compound division operator.
    constexpr Vector2& operator/=(T value) noexcept;

    /// \brief X component of the vector.
    T X = T{};

    /// \brief Y component of the vector.
    T Y = T{};
};

/// \brief Vector2 using \a `Axis::Float32` as template argument.
typedef Vector2<Float32> Vector2F;

/// \brief Vector2 using \a `Axis::Int32` as template argument.
typedef Vector2<Int32> Vector2I;

/// \brief Vector2 using \a `Axis::Uint32` as template argument.
typedef Vector2<Uint32> Vector2UI;

} // namespace Axis

#include "../../Private/Axis/Vector2Impl.inl"

#endif // AXIS_SYSTEM_VECTOR2_HPP