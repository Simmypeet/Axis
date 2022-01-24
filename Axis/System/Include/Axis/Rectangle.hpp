/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Rectangle.hpp
///
/// \brief Contains `Axis::Rectangle` template class.

#ifndef AXIS_MATH_RECTANGLE_HPP
#define AXIS_MATH_RECTANGLE_HPP
#pragma once

#include "Math.hpp"

namespace Axis
{

/// \brief 2D Rectangle area data structure template for all numeric types.
///
/// \tparam T Numerics type, contains arithmetic operators.
template <ArithmeticType T>
struct Rectangle final
{
    /// \brief Default constructor
    constexpr Rectangle() noexcept = default;

    /// \brief Constructs the Rectangle with specified position and area.
    template <ArithmeticType A, ArithmeticType B, ArithmeticType C, ArithmeticType D>
    constexpr Rectangle(A x,
                        B y,
                        C width,
                        D height) noexcept;

    /// \brief Equal operator
    AXIS_NODISCARD constexpr Bool operator==(const Rectangle& other) const noexcept;

    /// \brief Not equal operator
    AXIS_NODISCARD constexpr Bool operator!=(const Rectangle& other) const noexcept;

    /// \brief X Position of the rectangle.
    T X = {};

    /// \brief Y Position of the rectangle.
    T Y = {};

    /// \brief Width of the rectangle.
    T Width = {};

    /// \brief Height of the rectangle.
    T Height = {};
};

/// \brief Rectangle data structure using 32-Bit signed integer as template parameter.
typedef Rectangle<Int32> RectangleI;

/// \brief Rectangle data structure using 32-Bit unsigned integer as template parameter.
typedef Rectangle<Uint32> RectangleUI;

/// \brief Rectangle data structure using 32-Bit floating point number as template parameter.
typedef Rectangle<Float32> RectangleF;

} // namespace Axis

#include "../../Private/Axis/RectangleImpl.inl"

#endif // AXIS_MATH_RECTANGLE_HPP
