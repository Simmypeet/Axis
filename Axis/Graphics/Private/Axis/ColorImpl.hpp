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
    return !(*this == rhs);
}

} // namespace Graphics

} // namespace Axis

#endif // AXIS_GRAPHICS_COLORIMPL_INL