/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/WindowPch.hpp>

#include <Axis/DisplayWindow.hpp>
#include <Axis/Enum.hpp>
#include <Axis/Mouse.hpp>

namespace Axis
{

ButtonState MouseState::GetLeftButtonState() const noexcept
{
    return Math::ReadBitPosition(
               _buttons, (Uint8)Enum::GetUnderlyingValue(MouseButton::Left)) ?
        ButtonState::Pressed :
        ButtonState::Released;
}

ButtonState MouseState::GetRightButtonState() const noexcept
{
    return Math::ReadBitPosition(
               _buttons, (Uint8)Enum::GetUnderlyingValue(MouseButton::Right)) ?
        ButtonState::Pressed :
        ButtonState::Released;
}

ButtonState MouseState::GetMiddleButtonState() const noexcept
{
    return Math::ReadBitPosition(
               _buttons, (Uint8)Enum::GetUnderlyingValue(MouseButton::Middle)) ?
        ButtonState::Pressed :
        ButtonState::Released;
}

ButtonState MouseState::GetExtraButton1State() const noexcept
{
    return Math::ReadBitPosition(_buttons, (Uint8)Enum::GetUnderlyingValue(MouseButton::ExtraButton1)) ? ButtonState::Pressed : ButtonState::Released;
}

ButtonState MouseState::GetExtraButton2State() const noexcept
{
    return Math::ReadBitPosition(_buttons, (Uint8)Enum::GetUnderlyingValue(MouseButton::ExtraButton2)) ? ButtonState::Pressed : ButtonState::Released;
}

Bool MouseState::operator==(const MouseState& other) const noexcept
{
    return _buttons == other._buttons && _mousePosition == other._mousePosition &&
        _verticalScrollWheelValue == other._verticalScrollWheelValue &&
        _horizontalScrollWheelValue == other._horizontalScrollWheelValue;
}

Bool MouseState::operator!=(const MouseState& other) const noexcept
{
    return _buttons != other._buttons || _mousePosition != other._mousePosition ||
        _verticalScrollWheelValue != other._verticalScrollWheelValue ||
        _horizontalScrollWheelValue != other._horizontalScrollWheelValue;
}

MouseState::MouseState(Uint8 buttons, Vector2I mousePosition, Float32 verticalScrollWheelValue, Float32 horizontalScrollWheelValue) noexcept
    :
    _buttons(buttons), _mousePosition(mousePosition), _verticalScrollWheelValue(verticalScrollWheelValue), _horizontalScrollWheelValue(horizontalScrollWheelValue) {}

} // namespace Axis