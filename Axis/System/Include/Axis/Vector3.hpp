/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Vector3.hpp
///
/// \brief Contains \a `Axis::Vector3` mathematical vector template struct.

#ifndef AXIS_SYSTEM_VECTOR3_HPP
#define AXIS_SYSTEM_VECTOR3_HPP
#pragma once

#include "Math.hpp"

namespace Axis
{

/// \brief 3D mathematical vector template definition. Contains 3 components
///        X, Y and Z
template <ArithmeticType T>
struct Vector3 final
{
    /// \brief Default constructor
    constexpr Vector3() noexcept = default;

    /// \brief Constructs a 3D vector object.
    ///
    /// \param[in] x X component of the vector.
    /// \param[in] y Y component of the vector.
    /// \param[in] z Z component of the vector.
    template <ArithmeticType U, ArithmeticType V, ArithmeticType W>
    constexpr Vector3(U x,
                      V y,
                      W z) noexcept;

    /// \brief Constructs a 3D vector object from different arithmetic types.
    ///
    /// \param[in] other Other 3D vector object.
    template <ArithmeticType U>
    constexpr Vector3(const Vector3<U>& other) noexcept;

    /// \brief Constructs a 3D vector object.
    ///
    /// \param[in] value Scalar for both X, Y and Z components.
    constexpr explicit Vector3(T value) noexcept;

    /// \brief Gets the magnitude of the vector.
    ///
    /// uses this following formula : \f$\sqrt{x^2 + y^2 + z^2}\f$
    AXIS_NODISCARD constexpr BigFloat GetMagnitude() const noexcept;

    /// \brief Turns this vector into the unit vector.
    ///
    /// has the magnitude of 1 but still having the same direction
    constexpr void Normalize() noexcept requires(FloatingPointType<T>);

    /// \brief Gets the copy of this normalized vector.
    ///
    /// has the magnitude of 1 but still having the same direction
    AXIS_NODISCARD constexpr Vector3 NormalizeCopy() const noexcept requires(FloatingPointType<T>);

    /// \brief Returns the cross product of this vector and other vector.
    AXIS_NODISCARD constexpr Vector3 CrossProduct(const Vector3& other) const noexcept;

    /// \brief Returns the dot product of this vector and other vector.
    AXIS_NODISCARD constexpr T DotProduct(const Vector3& other) const noexcept;

    /// \brief Addition operator.
    AXIS_NODISCARD constexpr Vector3 operator+(const Vector3& other) const noexcept;

    /// \brief Subtraction operator.
    AXIS_NODISCARD constexpr Vector3 operator-(const Vector3& other) const noexcept;

    /// \brief Multiplication operator.
    AXIS_NODISCARD constexpr Vector3 operator*(const Vector3& other) const noexcept;

    /// \brief Division operator.
    AXIS_NODISCARD constexpr Vector3 operator/(const Vector3& other) const noexcept;

    /// \brief Multiplication operator.
    AXIS_NODISCARD constexpr Vector3 operator*(T value) const noexcept;

    /// \brief Division operator.
    AXIS_NODISCARD constexpr Vector3 operator/(T value) const noexcept;

    /// \brief Equal operator
    AXIS_NODISCARD constexpr Bool operator==(const Vector3& other) const noexcept;

    /// \brief Not equal operator
    AXIS_NODISCARD constexpr Bool operator!=(const Vector3& other) const noexcept;

    /// Compound addition operator.
    constexpr Vector3& operator+=(const Vector3& other) noexcept;

    /// Compound subtraction operator.
    constexpr Vector3& operator-=(const Vector3& other) noexcept;

    /// Compound multiplication operator.
    constexpr Vector3& operator*=(const Vector3& other) noexcept;

    /// Compound division operator.
    constexpr Vector3& operator/=(const Vector3& other) noexcept;

    /// Compound multiplication operator.
    constexpr Vector3& operator*=(T value) noexcept;

    /// Compound division operator.
    constexpr Vector3& operator/=(T value) noexcept;

    /// \brief X component of the vector.
    T X = T{};

    /// \brief Y component of the vector.
    T Y = T{};

    /// \brief Z component of the vector.
    T Z = T{};
};

/// \brief Vector3 using \a `Axis::Float32` as template argument.
typedef Vector3<Float32> Vector3F;

/// \brief Vector3 using \a `Axis::Int32` as template argument.
typedef Vector3<Int32> Vector3I;

/// \brief Vector3 using \a `Axis::Uint32` as template argument.
typedef Vector3<Uint32> Vector3UI;

} // namespace Axis

#include "../../Private/Axis/Vector3Impl.inl"

#endif // AXIS_SYSTEM_VECTOR3_HPP