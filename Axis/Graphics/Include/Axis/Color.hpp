/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_GRAPHICS_COLOR_HPP
#define AXIS_GRAPHICS_COLOR_HPP
#pragma once

#include "../../../System/Include/Axis/Math.hpp"

namespace Axis
{

namespace Graphics
{

/// \brief Graphical Color template data structure for all numeric types.
template <System::ArithmeticType T>
struct Color
{
    /// \brief Default constructor
    constexpr Color() noexcept = default;

    /// \brief Constructs the Color data structure with specified RGBA values.
    template <System::ArithmeticType U, System::ArithmeticType V, System::ArithmeticType W, System::ArithmeticType X>
    constexpr Color(U r,
                    V g,
                    W b,
                    X a) noexcept;

    /// \brief Constructs the Color data structure with specfied value for all
    ///        RGBA channels.
    ///
    /// \param[in] value Value to assign all RGBA values.
    constexpr explicit Color(T value) noexcept;

    /// \brief Equality operator.
    ///
    /// \param[in] rhs Right hand side operand.
    ///
    /// \returns True if the two colors are equal, false otherwise.
    ///
    constexpr Bool operator==(const Color& rhs) const noexcept;

    /// \brief Inequality operator.
    ///
    /// \param[in] rhs Right hand side operand.
    ///
    /// \returns True if the two colors are not equal, false otherwise.
    ///
    constexpr Bool operator!=(const Color& rhs) const noexcept;

    /// \brief R-Color channel value.
    ///
    T R = {};

    /// \brief G-Color channel value.
    ///
    T G = {};

    /// \brief B-Color channel value.
    ///
    T B = {};

    /// \brief Alpha channel value.
    ///
    T A = {};
};

/// \brief Defines a Color struct stores RGBA value in `Axis::Float32`.
///
typedef Color<Float32> ColorF;

/// \brief Defines a Color struct stores RGBA value in `Axis::Uint8`.
///
typedef Color<Uint8> ColorUI8;

} // namespace Graphics

} // namespace Axis

#include "../../Private/Axis/ColorImpl.hpp"

#endif // AXIS_GRAPHICS_COLOR_HPP