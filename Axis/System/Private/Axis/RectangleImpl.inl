/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_MATH_RECTANGLEIMPL_INL
#define AXIS_MATH_RECTANGLEIMPL_INL
#pragma once

#include "../../Include/Axis/Rectangle.hpp"

namespace Axis
{

namespace System
{

template <ArithmeticType T>
template <ArithmeticType A, ArithmeticType B, ArithmeticType C, ArithmeticType D>
inline constexpr Rectangle<T>::Rectangle(A x,
                                         B y,
                                         C width,
                                         D height) noexcept :
    X((T)x),
    Y((T)y),
    Width((T)width),
    Height((T)height)
{}

template <ArithmeticType T>
inline constexpr Bool Rectangle<T>::operator==(const Rectangle& other) const noexcept
{
    return X == other.X &&
        Y == other.Y &&
        Width == other.Width &&
        Height == other.Height;
}

template <ArithmeticType T>
inline constexpr Bool Rectangle<T>::operator!=(const Rectangle& other) const noexcept
{
    return X != other.X ||
        Y != other.Y ||
        Width != other.Width ||
        Height != other.Height;
}

template <ArithmeticType T>
inline constexpr Bool Rectangle<T>::Collides(const Rectangle& other) const noexcept
{
    return X < other.X + other.Width &&
        X + Width > other.X &&
        Y < other.Y + other.Height &&
        Y + Height > other.Y;
}

} // namespace System

} // namespace Axis

#endif // AXIS_MATH_RECTANGLEIMPL_INL