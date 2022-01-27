/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Mouse.hpp
/// 
/// \brief Contains the functions and classes for retrieving mouse inputs.

#ifndef AXIS_WINDOW_MOUSE_HPP
#define AXIS_WINDOW_MOUSE_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "Input.hpp"
#include "WindowExport.hpp"

namespace Axis
{

/// \brief Available different mouse buttons.
enum class MouseButton : Uint8
{
    /// \brief Mouse's left button.
    Left,

    /// \brief Mouse's right button.
    Right,

    /// \brief Mouse's middle button (scroll wheel).
    Middle,

    /// \brief Mouse's extra button 1.
    ExtraButton1,

    /// \brief Mouse's extra button 2.
    ExtraButton2,
};

/// \brief Available mouse wheel types
enum class MouseWheel : Uint8
{
    /// \brief Vertical mouse wheel
    Vertical,

    /// \brief Horizontal mouse wheel
    Horizontal,

    /// \brief Required for enum reflection.
    MaximumEnumValue = Horizontal
};

/// \brief Holds mouse state information. Allows to retrieve buttons' state in a
/// mouse and cursor's position.
struct AXIS_WINDOW_API MouseState final
{
public:
    /// \brief Gets \a `Axis::Window::ButtonState` of \a
    ///        `Axis::Window::MouseButton::Left`.
    AXIS_NODISCARD ButtonState GetLeftButtonState() const noexcept;

    /// \brief Gets \a `Axis::Window::ButtonState` of \a
    ///        `Axis::Window::MouseButton::Right`.
    AXIS_NODISCARD ButtonState GetRightButtonState() const noexcept;

    /// \brief Gets \a `Axis::Window::ButtonState` of \a
    ///        `Axis::Window::MouseButton::Middle`.
    AXIS_NODISCARD ButtonState GetMiddleButtonState() const noexcept;

    /// \brief Gets \a `Axis::Window::ButtonState` of \a
    ///        `Axis::Window::MouseButton::ExtraButton1`.
    AXIS_NODISCARD ButtonState GetExtraButton1State() const noexcept;

    /// \brief Gets \a `Axis::Window::ButtonState` of \a
    ///        `Axis::Window::MouseButton::ExtraButton2`.
    AXIS_NODISCARD ButtonState GetExtraButton2State() const noexcept;

    /// \brief Gets scroll wheel value for vertical scroll wheel.
    AXIS_NODISCARD inline Float32 GetScrollWheelValue() const noexcept { return _verticalScrollWheelValue; }

    /// \brief Gets scroll wheel value for horizontal scroll wheel.
    AXIS_NODISCARD inline Float32 GetHorizontalScrollWheelValue() const noexcept { return _horizontalScrollWheelValue; }

    /// \brief Gets mouse's position relative to the specified \a
    /// `Axis::Window::DisplayWindow`.
    AXIS_NODISCARD inline Vector2I GetPosition() const noexcept { return _mousePosition; }

    /// \brief Equal operator
    AXIS_NODISCARD Bool operator==(const MouseState& other) const noexcept;

    /// \brief Not equal operator
    AXIS_NODISCARD Bool operator!=(const MouseState& other) const noexcept;

private:
    /// Private constructor
    MouseState(Uint8    buttons,
               Vector2I mousePosition,
               Float32  verticalScrollWheelValue,
               Float32  horizontalScrollWheelValue) noexcept;

    /// Private members
    Uint8    _buttons                    = 0;
    Vector2I _mousePosition              = {};
    Float32  _verticalScrollWheelValue   = {};
    Float32  _horizontalScrollWheelValue = {};

    /// Friend declarations
    friend class Mouse;
};

} // namespace Axis

// Mouse class for win32 platform
#ifdef AXIS_PLATFORM_WINDOWS
#    include "Platform/Win32/Win32Mouse.hpp"
#endif

#endif // AXIS_WINDOW_MOUSE_HPP