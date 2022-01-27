/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file DisplayWindow.hpp
///
/// \brief This file includes the header which contains defintinion of \a
///        `Axis::Window::DisplayWindow`in the correct platform.

#ifndef AXIS_WINDOW_DISPLAYWINDOW_HPP
#define AXIS_WINDOW_DISPLAYWINDOW_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"
#include "../../../System/Include/Axis/Rectangle.hpp"
#include "../../../System/Include/Axis/String.hpp"
#include "../../../System/Include/Axis/Utility.hpp"
#include "../../../System/Include/Axis/Vector2.hpp"
#include "Mouse.hpp"
#include "WindowExport.hpp"


namespace Axis
{

/// \brief Enumeration of window styles.
enum class WindowStyle : Uint8
{
    /// \brief No window style.
    None = 0,

    /// \brief Window with title bar.
    TitleBar = AXIS_BIT(1),

    /// \brief Resizeable window with maximize / minimize buttons and title bar.
    Resizeable = AXIS_BIT(2),

    /// \brief Window with close button and title bar.
    CloseButton = AXIS_BIT(3),

    /// \brief Required for enum reflection.
    MaximumEnumValue = CloseButton,
};

/// \brief Enumeration of window styles. (Bit masks)
typedef WindowStyle WindowStyleFlags;

/// \brief Description of the DisplayWindow.
struct WindowDescription
{
    /// \brief Undefined window position
    static constexpr Int32 UndefinedPosition = INT32_MAX;

    /// \brief Size of the window (in pixels) (This excludes the bounds of title
    /// bar and border)
    Vector2UI WindowSize = {};

    /// \brief The position of the window (This excludes the bounds of title bar
    /// and border)
    Vector2I WindowPosition = {};

    /// \brief The window's title name
    WString Title = {};

    /// \brief Window's style flags.
    WindowStyleFlags WindowStyle = {};
};

} // namespace Axis

// DisplayWindow class for win32 platform
#ifdef AXIS_PLATFORM_WINDOWS
#    include "Platform/Win32/Win32DisplayWindow.hpp"
#endif

#endif // AXIS_WINDOW_DISPLAYWINDOW_HPP