/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_COLORIMPL_INL
#define AXIS_GRAPHICS_COLORIMPL_INL
#pragma once

#include "../../Include/Axis/Color.hpp"

namespace Axis
{

namespace Graphics
{

template <System::ArithmeticType T>
template <System::ArithmeticType U, System::ArithmeticType V, System::ArithmeticType W, System::ArithmeticType X>
inline constexpr Color<T>::Color(U r,
                                 V g,
                                 W b,
                                 X a) noexcept :
    R((T)r),
    G((T)g),
    B((T)b),
    A((T)a)
{}

template <System::ArithmeticType T>
inline constexpr Color<T>::Color(T value) noexcept :
    R(value),
    G(value),
    B(value),
    A(value) {}

template <System::ArithmeticType T>
inline constexpr Bool Color<T>::operator==(const Color<T>& rhs) const noexcept
{
    return (R == rhs.R) && (G == rhs.G) && (B == rhs.B) && (A == rhs.A);
}

template <System::ArithmeticType T>
inline constexpr Bool Color<T>::operator!=(const Color<T>& rhs) const noexcept
{
    return (R != rhs.R) || (G != rhs.G) || (B != rhs.B) || (A != rhs.A);
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetWhite() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(255, 255, 255, 255);
    }
}


template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetBlack() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(0, 0, 0, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetRed() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(255, 0, 0, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetGreen() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(0, 255, 0, 255);
    }
}


template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetBlue() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(0, 0, 255, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetYellow() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(1.0f, 1.0f, 0.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(255, 255, 0, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetCyan() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(0.0f, 1.0f, 1.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(0, 255, 255, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetMagenta() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>(1.0f, 0.0f, 1.0f, 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(255, 0, 255, 255);
    }
}

template <System::ArithmeticType T>
inline constexpr Color<T> Color<T>::GetCornflowerBlue() noexcept requires(System::SameAs<T, Float32> || System::SameAs<T, Uint8>)
{
    if constexpr (System::SameAs<T, Float32>)
    {
        return Color<T>((100.0f / 255.0f), (149.0f / 255.0f), (237.0f / 255.0f), 1.0f);
    }
    else if constexpr (System::SameAs<T, Uint8>)
    {
        return Color<T>(100, 149, 237, 255);
    }
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_COLORIMPL_INL