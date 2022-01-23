/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_VECTOR2IMPL_INL
#define AXIS_SYSTEM_VECTOR2IMPL_INL
#pragma once

#include "../../Include/Axis/Vector2.hpp"

namespace Axis
{

template <ArithmeticType T>
template <ArithmeticType U, ArithmeticType W>
inline constexpr Vector2<T>::Vector2(U x,
                                     W y) noexcept :
    X(T(x)),
    Y(T(y)) {}

template <ArithmeticType T>
template <ArithmeticType U>
inline constexpr Vector2<T>::Vector2(const Vector2<U>& other) noexcept :
    X((T)other.X),
    Y((T)other.Y) {}

template <ArithmeticType T>
inline constexpr Vector2<T>::Vector2(T value) noexcept :
    X(value),
    Y(value) {}

template <ArithmeticType T>
inline constexpr BigFloat Vector2<T>::GetMagnitude() const noexcept
{
    return std::sqrt((X * X) + (Y * Y));
}

template <ArithmeticType T>
inline constexpr void Vector2<T>::Normalize() noexcept requires(FloatingPointType<T>)
{
    T magnitude = (T)GetMagnitude();

    X /= magnitude;
    Y /= magnitude;
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::NormalizeCopy() const noexcept requires(FloatingPointType<T>)
{
    T magnitude = (T)GetMagnitude();

    return Vector2(X / magnitude, Y / magnitude);
}

template <ArithmeticType T>
inline constexpr T Vector2<T>::DotProduct(const Vector2<T>& other) const noexcept
{
    return (X * other.X) + (Y * other.Y);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator+(const Vector2& other) const noexcept
{
    return Vector2(X + other.X, Y + other.Y);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator-(const Vector2& other) const noexcept
{
    return Vector2(X - other.X, Y - other.Y);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator*(const Vector2& other) const noexcept
{
    return Vector2(X * other.X, Y * other.Y);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator/(const Vector2& other) const noexcept
{
    return Vector2(X / other.X, Y / other.Y);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator*(T value) const noexcept
{
    return Vector2(X * value, Y * value);
}

template <ArithmeticType T>
inline constexpr Vector2<T> Vector2<T>::operator/(T value) const noexcept
{
    return Vector2(X / value, Y / value);
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& other) noexcept
{
    X += other.X;
    Y += other.Y;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2& other) noexcept
{
    X -= other.X;
    Y -= other.Y;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2& other) noexcept
{
    X *= other.X;
    Y *= other.Y;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2& other) noexcept
{
    X /= other.X;
    Y /= other.Y;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator*=(T value) noexcept
{
    X *= value;
    Y *= value;

    return *this;
}

template <ArithmeticType T>
inline constexpr Vector2<T>& Vector2<T>::operator/=(T value) noexcept
{
    X /= value;
    Y /= value;

    return *this;
}

template <ArithmeticType T>
inline constexpr Bool Vector2<T>::operator==(const Vector2<T>& other) const noexcept
{
    return X == other.X && Y == other.Y;
}

template <ArithmeticType T>
inline constexpr Bool Vector2<T>::operator!=(const Vector2<T>& other) const noexcept
{
    return X != other.X || Y != other.Y;
}

} // namespace Axis

#endif // AXIS_SYSTEM_VECTOR2IMPL_INL