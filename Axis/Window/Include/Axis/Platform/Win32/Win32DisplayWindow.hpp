/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

#ifndef AXIS_WINDOW_WIN32DISPLAYWINDOW_HPP
#define AXIS_WINDOW_WIN32DISPLAYWINDOW_HPP
#pragma once

#include "../../../../../System/Include/Axis/Config.hpp"
#include "../../../../../System/Include/Axis/Event.hpp"
#include "../../../../../System/Include/Axis/SmartPointer.hpp"
#include "../../DisplayWindow.hpp"
#include "../../Input.hpp"
#include "../../Mouse.hpp"
#include "../../WindowExport.hpp"


namespace Axis
{

namespace Window
{

/// \brief Represents the window displaying on the screen.
class AXIS_WINDOW_API DisplayWindow final : public System::ISharedFromThis
{
public:
    /// \brief Raised when the window client size is changed.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `Vector2UI` specifies the new client size (in pixels
    /// for width and height).
    using ClientSizeChangedEvent = System::Event<void(DisplayWindow&, System::Vector2UI)>;

    /// \brief Raised when the window client position is changed.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `Vector2I` specifies the new client position (in
    /// pixels for X and Y).
    using ClientPositionChangedEvent = System::Event<void(DisplayWindow&, System::Vector2I)>;

    /// \brief Raised when the cursor position is changed.
    ///
    /// \note In \a `Windows` platform the event doesn't raise if the cursor is
    /// not in the client bound.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `Vector2I` specifies the new cursor position relative
    /// to the client bound (in pixels for X and Y).
    using CursorPositionChangedEvent = System::Event<void(DisplayWindow&, System::Vector2I)>;

    /// \brief Raised when the mouse buttons changed their state.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `MouseButton` specifies which mouse button has
    /// changed their state.
    /// - Argument \a `ButtonState` specifies the new button state
    /// of the mouse button.
    using MouseButtonStateChangedEvent = System::Event<void(DisplayWindow&, MouseButton, ButtonState)>;

    /// \brief Raised when the mouse cursor leaved or entered the client bound.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `Bool` Specifies whether if the mouse has entered or
    /// entered the window. (true for enter / false for leave)
    using CursorEnteredEvent = System::Event<void(DisplayWindow&, Bool)>;

    /// \brief Raised when the window gained or lost the focus.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `Bool` Specifies whether if the window has gained or
    /// lost the focus. (true for gain / false for lose)
    using WindowFocusEvent = System::Event<void(DisplayWindow&, Bool)>;

    /// \brief Raised when the mouse wheel is scrolled, either \a
    /// `Axis::Window::MouseWheel::Horizontal` or \a
    /// `Axis::Window::MouseWheel::Vertical`.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    /// - Argument \a `MouseWheel` specifies which mouse wheel has
    /// scrolled.
    /// - Argument \a `Float32` specifies the delta value of mouse wheel.
    /// (Positive for up and left, negative for down and right).
    using MouseWheelScrollEvent = System::Event<void(DisplayWindow&, MouseWheel, Float32)>;

    /// \brief The event is raised when user requested to close the window.
    ///
    /// - Argument \a `DisplayWindow&` specifies the window which
    /// raised the event.
    using UserClosedWindowEvent = System::Event<void(DisplayWindow&)>;

    /// \brief Constrcuts new display window.
    ///
    /// \param[in] description Contains the information used in window creation
    /// \param[out] resultCode ResultCode for reporting errors
    ///
    /// \remark The window doesn't show after its construction. To show the window
    ///         calls \a `DisplayWindow::ShowWindow`.
    DisplayWindow(const WindowDescription& description);

    /// \brief Destructor
    ~DisplayWindow() noexcept override;

    /// \brief Sets new window style to the window.
    ///
    /// \param[in] windowStyleFlags New window style to set to the window.
    void SetWindowStyle(WindowStyleFlags windowStyleFlags);

    /// \brief Sets new window title text.
    void SetWindowTitle(const System::StringView<WChar>& windowTitle);

    /// \brief Gets the client rectangle of the window.
    AXIS_NODISCARD System::RectangleI GetClientBounds() const;

    /// \brief Gets the position of the window.
    AXIS_NODISCARD System::Vector2I GetPosition() const;

    /// \brief Gets the size of the window
    AXIS_NODISCARD System::Vector2UI GetSize() const;

    /// \brief Gets the system's low level handle to the window.
    ///
    /// \note In Windows platform the return value is \a `HWND`.
    AXIS_NODISCARD PVoid GetWindowHandle() const noexcept { return _hwnd; }

    /// \brief Sets the position of the client window.
    void SetPosition(const System::Vector2UI& position);

    /// \brief Gets the name of screen device which window is in.
    AXIS_NODISCARD System::String8 GetScreenDeviceName() const;

    /// \brief Shows the window to the screen.
    void ShowWindow();

    /// \brief Polls the events.
    ///
    /// \note This doesn't block the calling thread.
    void PollEvent();

    /// \brief Polls the events or blocks the thread and wait for the next event
    ///        if the event queue is empty.
    void WaitEvent();

    /// \brief Gets the event for \a `ClientSizeChangedEvent`.
    AXIS_NODISCARD inline ClientSizeChangedEvent::Register& GetClientSizeChangedEvent() noexcept { return _clientSizeChangedEvent.EventRegister; }

    /// \brief Gets the event for \a `ClientPositionChangedEvent`.
    AXIS_NODISCARD inline ClientPositionChangedEvent::Register& GetClientPositionChangeEvent() noexcept { return _clientPositionChangedEvent.EventRegister; }

    /// \brief Gets the event for \a `CursorPositionChangedEvent`.
    AXIS_NODISCARD inline CursorPositionChangedEvent::Register& GetCursorPositionChangedEvent() noexcept { return _cursorPositionChangedEvent.EventRegister; }

    /// \brief Gets the event for \a `MouseButtonStateChangedEvent`.
    AXIS_NODISCARD inline MouseButtonStateChangedEvent::Register& GetMouseButtonStateChangedEvent() noexcept { return _mouseButtonStateChangedEvent.EventRegister; }

    /// \brief Gets the event for \a `CursorEnteredEvent`.
    AXIS_NODISCARD inline CursorEnteredEvent::Register& GetCursorEnteredEvent() noexcept { return _cursorEnteredEvent.EventRegister; }

    /// \brief Gets the event for \a `WindowFocusEvent`.
    AXIS_NODISCARD inline WindowFocusEvent::Register& GetWindowFocusEvent() noexcept { return _windowFocusEvent.EventRegister; }

    /// \brief Gets the event for \a `MouseWheelScrollEvent`.
    AXIS_NODISCARD inline MouseWheelScrollEvent::Register& GetMouseWheelScrollEvent() noexcept { return _mouseWheelScrollEvent.EventRegister; }

    /// \brief Gets the event for \a `UserClosedWindowEvent`.
    AXIS_NODISCARD inline UserClosedWindowEvent::Register& GetUserClosedWindowEvent() noexcept { return _userClosedWindowEvent.EventRegister; }

    /// \brief Window class name
    static constexpr const wchar_t* Win32ClassName = L"AxisWin32Class";

private:
    /// Private methods
    void GrabCursor(Bool grabbed) const noexcept;

    /// Private members
    ClientSizeChangedEvent       _clientSizeChangedEvent       = {};
    ClientPositionChangedEvent   _clientPositionChangedEvent   = {};
    CursorPositionChangedEvent   _cursorPositionChangedEvent   = {};
    MouseButtonStateChangedEvent _mouseButtonStateChangedEvent = {};
    CursorEnteredEvent           _cursorEnteredEvent           = {};
    WindowFocusEvent             _windowFocusEvent             = {};
    MouseWheelScrollEvent        _mouseWheelScrollEvent        = {};
    UserClosedWindowEvent        _userClosedWindowEvent        = {};
    PVoid                        _hwnd                         = {};
    System::Vector2UI            _lastSize                     = {};
    Bool                         _resizing                     = false;
    Bool                         _cursorGrab                   = false;
    Bool                         _mouseInside                  = false;

    /// Friend declartions
    friend class Win32Implement;
};

} // namespace Window

} // namespace Axis

#endif // AXIS_WINDOW_WIN32DISPLAYWINDOW_HPP