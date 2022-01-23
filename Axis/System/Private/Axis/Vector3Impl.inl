/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_MATH_VECTOR3IMPL_INL
#define AXIS_MATH_VECTOR3IMPL_INL
#pragma once

#include "../../Include/Axis/Vector3.hpp"

namespace Axis
{

template <ArithmeticType T>
template <ArithmeticType U, ArithmeticType V, ArithmeticType W>
inline constexpr Vector3<T>::Vector3(U x,
                                     V y,
                                     W z) noexcept :
    X((T)x),
    Y((T)y),
    Z((T)z) {}

template <ArithmeticType T>
template <ArithmeticType U>
inline constexpr Vector3<T>::Vector3(const Vector3<U>& other) noexcept :
    X((T)other.X),
    Y((T)other.Y),
    Z((T)other.Z) {}

template <ArithmeticType T>
inline constexpr Vector3<T>::Vector3(T value) noexcept :
    X(value),
    Y(value),
    Z(value) {}

template <ArithmeticType T>
inline constexpr BigFloat Vector3<T>::GetMagnitude() const noexcept
{
    return std::sqrt((X * X) + (Y * Y) + (Z * Z));
}

template <ArithmeticType T>
inline constexpr void Vector3<T>::Normalize() noexcept requires(FloatingPointType<T>)
{
    T magnitude = (T)GetMagnitude();

    X /= magnitude;
    Y /= magnitude;
    Z /= magnitude;
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::NormalizeCopy() const noexcept requires(FloatingPointType<T>)
{
    T magnitude = (T)GetMagnitude();

    return Vector3((T)(X / magnitude), (T)(Y / magnitude), (T)(Z / magnitude));
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::CrossProduct(const Vector3& other) const noexcept
{
    return Vector3(
        Y * other.Z - other.Y * Z,
        Z * other.X - other.Z * X,
        X * other.Y - other.X * Y);
}

template <ArithmeticType T>
inline constexpr T Vector3<T>::DotProduct(const Vector3& other) const noexcept
{
    return (X * other.X) + (Y * other.Y) + (Z * other.Z);
}


template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator+(const Vector3& other) const noexcept
{
    return Vector3(X + other.X, Y + other.Y, Z + other.Z);
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator-(const Vector3& other) const noexcept
{
    return Vector3(X - other.X, Y - other.Y, Z - other.Z);
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator*(const Vector3& other) const noexcept
{
    return Vector3(X * other.X, Y * other.Y, Z * other.Z);
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator/(const Vector3& other) const noexcept
{
    return Vector3(X / other.X, Y / other.Y, Z / other.Z);
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator*(T scalar) const noexcept
{
    return Vector3(X * scalar, Y * scalar, Z * scalar);
}

template <ArithmeticType T>
inline constexpr Vector3<T> Vector3<T>::operator/(T scalar) const noexcept
{
    return Vector3(X / scalar, Y / scalar, Z / scalar);
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator+=(const Vector3& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator-=(const Vector3& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator*=(const Vector3& other) noexcept
{
    X *= other.X;
    Y *= other.Y;
    Z *= other.Z;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator/=(const Vector3& other) noexcept
{
    X /= other.X;
    Y /= other.Y;
    Z /= other.Z;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator*=(T scalar) noexcept
{
    X *= scalar;
    Y *= scalar;
    Z *= scalar;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector3<T>& Vector3<T>::operator/=(T scalar) noexcept
{
    X /= scalar;
    Y /= scalar;
    Z /= scalar;

    return *this;
}

template <ArithmeticType T>
inline constexpr bool Vector3<T>::operator==(const Vector3& other) const noexcept
{
    return (X == other.X) && (Y == other.Y) && (Z == other.Z);
}

template <ArithmeticType T>
inline constexpr bool Vector3<T>::operator!=(const Vector3& other) const noexcept
{
    return (X != other.X) || (Y != other.Y) || (Z != other.Z);
}

} // namespace Axis

#endif // AXIS_MATH_VECTOR3IMPL_INL