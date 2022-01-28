/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/WindowPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Exception.hpp>
#include <Axis/Platform/Win32/Win32DisplayWindow.hpp>
#include <Axis/Platform/Win32/Win32Mouse.hpp>
#include <Windows.h>

namespace Axis
{

Mouse::Mouse(const SharedPointer<DisplayWindow>& window)
{
    if (!window)
        throw InvalidArgumentException("window was nullptr!");

    _window = window;

    auto mouseWheelScrollEventHandler = [this](DisplayWindow& window, MouseWheel mouseWheel, Float32 deltaVal) {
        if (mouseWheel == MouseWheel::Horizontal)
            _horizontalScrollWheelValue += deltaVal;
        else if (mouseWheel == MouseWheel::Vertical)
            _verticalScrollWheelValue += deltaVal;
    };

    _eventToken = _window->GetMouseWheelScrollEvent().Add(mouseWheelScrollEventHandler, (UintPtr)this);
}

Mouse::~Mouse()
{
    if (_window)
        _window->GetMouseWheelScrollEvent().Remove(_eventToken);
}

MouseState Mouse::GetMouseState() const
{
    constexpr auto GetButtonState = [](MouseButton mouseButton) {
        int vkey = 0;
        switch (mouseButton)
        {
            case MouseButton::Left:
                vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
                break;
            case MouseButton::Right:
                vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
                break;
            case MouseButton::Middle:
                vkey = VK_MBUTTON;
                break;
            case MouseButton::ExtraButton1:
                vkey = VK_XBUTTON1;
                break;
            case MouseButton::ExtraButton2:
                vkey = VK_XBUTTON2;
                break;
            default:
                return false;
        }
        return (GetAsyncKeyState(vkey) & 0x8000) != 0;
    };

    Uint8 buttons = 0;
    buttons       = Math::AssignBitToPosition(buttons, Enum::GetUnderlyingValue(MouseButton::Left), GetButtonState(MouseButton::Left));
    buttons       = Math::AssignBitToPosition(buttons, Enum::GetUnderlyingValue(MouseButton::Right), GetButtonState(MouseButton::Right));
    buttons       = Math::AssignBitToPosition(buttons, Enum::GetUnderlyingValue(MouseButton::Middle), GetButtonState(MouseButton::Middle));
    buttons       = Math::AssignBitToPosition(buttons, Enum::GetUnderlyingValue(MouseButton::ExtraButton1), GetButtonState(MouseButton::ExtraButton1));
    buttons       = Math::AssignBitToPosition(buttons, Enum::GetUnderlyingValue(MouseButton::ExtraButton2), GetButtonState(MouseButton::ExtraButton2));

    HWND hwnd = (HWND)_window->GetWindowHandle();

    POINT mouseCursorPosition;
    
    if (!GetCursorPos(&mouseCursorPosition))
        throw ExternalException("Failed to GetCursorPos!");

    if (!ScreenToClient(hwnd, &mouseCursorPosition))
        throw ExternalException("Failed to ScreenToClient!");

    return MouseState(buttons, Vector2I(mouseCursorPosition.x, mouseCursorPosition.y), _verticalScrollWheelValue, _horizontalScrollWheelValue);
}

} // namespace Axis
