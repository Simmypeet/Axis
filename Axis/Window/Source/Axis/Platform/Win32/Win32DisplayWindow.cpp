/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#include <Axis/WindowPch.hpp>

#include <Axis/Enum.hpp>
#include <Axis/Exception.hpp>
#include <Axis/HashMap.hpp>
#include <Axis/Platform/Win32/Win32DisplayWindow.hpp>

#ifndef UNICODE
#    define UNICODE
#endif

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4244)
#endif

#include <Windows.h>
#include <Windowsx.h>

namespace Axis
{

static std::atomic<Bool> s_ClassCreated = false;
static WNDCLASSEXW       s_WindowClass  = {0};
static std::mutex        s_Mutex        = {};
static std::atomic<Size> s_WindowCount  = 0; // Window count to zero

// Friend declaration hack :)
class Win32Implement final
{
public:
    // WndProc function resolves the callbacks.
    static LRESULT CALLBACK GlobalProcessWindowMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept
    {
        if (!hwnd)
            return 0;

        // Registers our Win32DisplayWindow instance to the hwnd handle.
        if (message == WM_CREATE)
        {
            LONG_PTR window = (LONG_PTR)(((CREATESTRUCT*)lparam)->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, window);
        }

        // Gets the Win32DisplayWindow instance according to the hwnd handle.
        DisplayWindow* win32DisplayWindowInstance =
            hwnd ? (DisplayWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA) : nullptr;

        if (win32DisplayWindowInstance)
        {
            if (!win32DisplayWindowInstance->_hwnd)
                win32DisplayWindowInstance->_hwnd = hwnd;

            ProcessWindowMessage(*win32DisplayWindowInstance, message, wparam,
                                 lparam);
        }
        if (message == WM_CLOSE)
            return 0;

        // Don't forward the menu system command, so that pressing ALT or F10
        // doesn't steal the focus
        if ((message == WM_SYSCOMMAND) && (wparam == SC_KEYMENU))
            return 0;

        return DefWindowProcW(hwnd, message, wparam, lparam);
    }

private:
    static void ProcessWindowMessage(DisplayWindow& This, UINT message, WPARAM wparam, LPARAM lparam) noexcept
    {
        switch (message)
        {
            // Resize event
            case WM_SIZE:
            {
                // Resize event for maximize and minimize
                if (wparam != SIZE_MINIMIZED && !This._resizing &&
                    This._lastSize != This.GetSize())
                {
                    This._lastSize = This.GetSize();

                    This._clientSizeChangedEvent.Invoke(
                        This, Vector2UI(This._lastSize.X, This._lastSize.Y));

                    This.GrabCursor(This._cursorGrab);
                }
                break;
            }

            // Start resizing
            case WM_ENTERSIZEMOVE:
            {
                This._resizing = true;
                This.GrabCursor(false);
                break;
            }

            // Stop resizing
            case WM_EXITSIZEMOVE:
            {
                This._resizing = false;

                // Ignore cases where the window has only been moved
                if (This._lastSize != This.GetSize())
                {
                    // Update the last handled size
                    This._lastSize = This.GetSize();

                    This._clientSizeChangedEvent.Invoke(
                        This, Vector2UI(This._lastSize.X, This._lastSize.Y));
                }

                // Restore/update cursor grabbing
                This.GrabCursor(false);
                break;
            }

            // Position move
            case WM_MOVE:
            {
                auto expectedClientBound = This.GetClientBounds();

                This._clientPositionChangedEvent.Invoke(
                    This, Vector2I(expectedClientBound.X, expectedClientBound.Y));

                break;
            }

            // Mouse move event
            case WM_MOUSEMOVE:
            {
                // Extracts the mouse local coordinates
                int x = (Int16)LOWORD(lparam);
                int y = (Int16)HIWORD(lparam);

                // Gets the client area of the window
                RECT rect;
                GetClientRect((HWND)This._hwnd, &rect);

                // Keep receiving event in case the user drags the mouse over.
                if ((wparam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
                {
                    if (GetCapture() == This._hwnd)
                        ReleaseCapture();
                }
                else if (GetCapture() != This._hwnd)
                {
                    SetCapture((HWND)This._hwnd);
                }

                // Raise mouse enter / leave window event
                if ((x < rect.left) || (x > rect.right) || (y < rect.top) ||
                    (y > rect.bottom))
                {
                    if (This._mouseInside)
                    {
                        This._mouseInside = false;

                        TRACKMOUSEEVENT mouseEvent;
                        mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                        mouseEvent.dwFlags     = TME_CANCEL;
                        mouseEvent.hwndTrack   = (HWND)This._hwnd;
                        mouseEvent.dwHoverTime = HOVER_DEFAULT;
                        TrackMouseEvent(&mouseEvent);

                        This._cursorEnteredEvent.Invoke(This, false);
                    }
                }
                else
                {
                    // and vice-versa
                    if (!This._mouseInside)
                    {
                        This._mouseInside = true;

                        // Look for the mouse leaving the window
                        TRACKMOUSEEVENT mouseEvent;
                        mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                        mouseEvent.dwFlags     = TME_LEAVE;
                        mouseEvent.hwndTrack   = (HWND)This._hwnd;
                        mouseEvent.dwHoverTime = HOVER_DEFAULT;
                        TrackMouseEvent(&mouseEvent);

                        This._cursorEnteredEvent.Invoke(This, true);
                    }
                }

                break;
            }

            // Focus event
            case WM_SETFOCUS:
            {
                // Restore cursor grabbing
                This.GrabCursor(This._cursorGrab);

                This._windowFocusEvent.Invoke(This, true);
                break;
            }

            // Focus event
            case WM_KILLFOCUS:
            {
                // Ungrab the cursor
                This.GrabCursor(false);

                This._windowFocusEvent.Invoke(This, false);
                break;
            }

            // Mouse leave event
            case WM_MOUSELEAVE:
            {
                if (This._mouseInside)
                {
                    This._mouseInside = false;

                    // Generate a MouseLeft event
                    This._cursorEnteredEvent.Invoke(This, false);
                }
                break;
            }

            // Vertical mouse wheel event
            case WM_MOUSEWHEEL:
            {
                Int16 delta = (Int16)(HIWORD(wparam));

                This._mouseWheelScrollEvent.Invoke(This, MouseWheel::Vertical,
                                                   (Float32)delta / 120.0f);

                break;
            }

            // Vertical mouse wheel event
            case WM_MOUSEHWHEEL:
            {
                Int16 delta = (Int16)(HIWORD(wparam));

                This._mouseWheelScrollEvent.Invoke(This, MouseWheel::Horizontal,
                                                   (Float32)-delta / 120.0f);

                break;
            }

            // Mouse left button down event
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            {
                MouseButton mouseButton = {};
                ButtonState buttonState = {};

                switch (message)
                {
                    case WM_LBUTTONDOWN:
                        buttonState = ButtonState::Pressed;
                        mouseButton = MouseButton::Left;
                        break;

                    case WM_LBUTTONUP:
                        buttonState = ButtonState::Released;
                        mouseButton = MouseButton::Left;
                        break;

                    case WM_RBUTTONDOWN:
                        buttonState = ButtonState::Pressed;
                        mouseButton = MouseButton::Right;
                        break;

                    case WM_RBUTTONUP:
                        buttonState = ButtonState::Released;
                        mouseButton = MouseButton::Right;
                        break;

                    case WM_MBUTTONDOWN:
                        buttonState = ButtonState::Pressed;
                        mouseButton = MouseButton::Middle;
                        break;

                    case WM_MBUTTONUP:
                        buttonState = ButtonState::Released;
                        mouseButton = MouseButton::Middle;
                        break;

                    case WM_XBUTTONDOWN:
                        buttonState = ButtonState::Pressed;
                        mouseButton = HIWORD(wparam) == XBUTTON1 ? MouseButton::ExtraButton1 : MouseButton::ExtraButton2;
                        break;

                    case WM_XBUTTONUP:
                        buttonState = ButtonState::Released;
                        mouseButton = HIWORD(wparam) == XBUTTON1 ? MouseButton::ExtraButton1 : MouseButton::ExtraButton2;
                        break;
                }

                This._mouseButtonStateChangedEvent.Invoke(This, std::move(mouseButton),
                                                          std::move(buttonState));

                break;
            }

            // Close event
            case WM_CLOSE:
            {
                This._userClosedWindowEvent.Invoke(This);
                break;
            }
        }
    }
};

DWORD ParseWindowStyleFlags(WindowStyleFlags windowStyleFlags)
{
    // Parses `Axis::Window::WindowStyle` to `DWORD` window style
    DWORD win32WindowStyle = {};

    if (windowStyleFlags == WindowStyle::None)
    {
        win32WindowStyle |= WS_POPUP;
    }
    else
    {
        if ((Bool)(windowStyleFlags & WindowStyle::TitleBar))
            win32WindowStyle |= WS_CAPTION | WS_MINIMIZEBOX;

        if ((Bool)(windowStyleFlags & WindowStyle::Resizeable))
            win32WindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX;

        if ((Bool)(windowStyleFlags & WindowStyle::CloseButton))
            win32WindowStyle |= WS_SYSMENU;
    }

    return win32WindowStyle;
}

DisplayWindow::DisplayWindow(const WindowDescription& description)
{

    {
        std::scoped_lock lockGuard(s_Mutex);

        // Registers class
        if (!s_ClassCreated)
        {
            s_WindowClass.cbSize        = sizeof(WNDCLASSEXW);
            s_WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
            s_WindowClass.lpfnWndProc   = &Win32Implement::GlobalProcessWindowMessage;
            s_WindowClass.cbClsExtra    = 0;
            s_WindowClass.cbWndExtra    = 0;
            s_WindowClass.hInstance     = GetModuleHandleA(NULL);
            s_WindowClass.hIcon         = ::LoadIcon(GetModuleHandle(NULL), NULL);
            s_WindowClass.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
            s_WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            s_WindowClass.lpszMenuName  = NULL;
            s_WindowClass.lpszClassName = DisplayWindow::Win32ClassName;
            s_WindowClass.hIconSm       = ::LoadIcon(GetModuleHandle(NULL), NULL);

            if (!RegisterClassExW(&s_WindowClass))
                throw std::runtime_error("Failed to register Win32 window class!");

            else
                s_ClassCreated = true;
        }
    }

    // Calculates the actual size and position
    HDC screenDC = GetDC(NULL);
    int left =
        (GetDeviceCaps(screenDC, HORZRES) - (int)(description.WindowSize.X)) / 2;
    int top =
        (GetDeviceCaps(screenDC, VERTRES) - (int)(description.WindowSize.Y)) / 2;
    int width  = description.WindowSize.X;
    int height = description.WindowSize.Y;
    ReleaseDC(NULL, screenDC);

    // Parses `Axis::Window::WindowStyle` to `DWORD` window style
    DWORD win32WindowStyle = ParseWindowStyleFlags(description.WindowStyle);

    // Creates the window
    _hwnd = CreateWindowExW(
        WS_EX_APPWINDOW, DisplayWindow::Win32ClassName,
        description.Title.GetCString(),
        win32WindowStyle,
        description.WindowPosition.X == WindowDescription::UndefinedPosition ? CW_USEDEFAULT : left,
        description.WindowPosition.Y == WindowDescription::UndefinedPosition ? CW_USEDEFAULT : top,
        width, height, NULL, NULL, GetModuleHandle(NULL), this);

    // Checks for errors
    if (!_hwnd)
        throw std::runtime_error("Failed to create Win32 window!");

    // Adjust the window size to the real size (At first the window was created
    // with the size which included the tilebar size).
    RECT rectangle = {0, 0, (long)description.WindowSize.X,
                      (long)description.WindowSize.Y};
    AdjustWindowRect(&rectangle, GetWindowLongPtr((HWND)_hwnd, GWL_STYLE), false);
    width  = rectangle.right - rectangle.left;
    height = rectangle.bottom - rectangle.top;

    SetWindowPos((HWND)_hwnd, NULL, 0, 0, width, height,
                 SWP_NOMOVE | SWP_NOZORDER);

    s_WindowCount += 1;
}

DisplayWindow::~DisplayWindow() noexcept
{
    // Destroy the window
    if (_hwnd)
    {
        ::DestroyWindow((HWND)_hwnd);

        s_WindowCount -= 1;

        // The last window...
        if (s_WindowCount == 0)
        {
            // Unregister the window class
            UnregisterClassW(DisplayWindow::Win32ClassName, GetModuleHandleW(NULL));

            s_ClassCreated = false;
        }
    }
}

void DisplayWindow::SetWindowStyle(WindowStyleFlags windowStyleFlags)
{
    SetWindowLongPtr((HWND)_hwnd, GWL_STYLE, WS_SYSMENU); // 3d argument=style

    auto windowBound = GetClientBounds();

    // Adjust the window size to the real size (At first the window was created
    // with the size which included the tilebar size).
    RECT rectangle = {windowBound.X, windowBound.Y,
                      windowBound.X + windowBound.Width,
                      windowBound.Y + windowBound.Height};

    if (!AdjustWindowRect(&rectangle, GetWindowLongPtr((HWND)_hwnd, GWL_STYLE),
                          false))
        throw ExternalException("Failed to adjust window rect!");

    if (!SetWindowPos((HWND)_hwnd, NULL, rectangle.top, rectangle.left,
                      rectangle.right - rectangle.left,
                      rectangle.bottom - rectangle.top, SWP_NOZORDER))
        throw ExternalException("Failed to adjust window position!");
}

void DisplayWindow::SetWindowTitle(const wchar_t* windowTitle)
{
    if (!SetWindowTextW((HWND)_hwnd, windowTitle))
        throw ExternalException("Failed to set window text!");
}

RectangleI DisplayWindow::GetClientBounds() const
{
    RECT rect;

    if (!GetClientRect((HWND)_hwnd, &rect))
        throw ExternalException("Failed to GetClientRect!");

    POINT topLeft  = {rect.top, rect.left};
    POINT botRight = {rect.bottom, rect.right};

    if (!ClientToScreen((HWND)_hwnd, &topLeft))
        throw ExternalException("Failed to ClientToScreen!");

    if (!ClientToScreen((HWND)_hwnd, &botRight))
        throw ExternalException("Failed to ClientToScreen!");

    return {topLeft.x, topLeft.y, botRight.y - topLeft.y, botRight.x - topLeft.x};
}

void DisplayWindow::SetPosition(const Vector2UI& position)
{
    RECT rectangle = {(long)position.X, (long)position.Y, 0, 0};

    if (!AdjustWindowRect(&rectangle, GetWindowLongPtr((HWND)_hwnd, GWL_STYLE), false))
        throw ExternalException("Failed to AdjustWindowRect!");

    if (!SetWindowPos((HWND)_hwnd, NULL, rectangle.top, rectangle.left, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
        throw ExternalException("Failed to SetWindowPos!");
}

String8 DisplayWindow::GetScreenDeviceName() const
{
    return {};
}

void DisplayWindow::ShowWindow()
{
    if (!::ShowWindow((HWND)_hwnd, SW_SHOW))
        throw ExternalException("Failed to ShowWindow!");
}

void DisplayWindow::PollEvent()
{
    MSG message;
    while (PeekMessageW(&message, (HWND)_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

void DisplayWindow::WaitEvent()
{
    MSG msg = {};
    while (GetMessageW(&msg, (HWND)_hwnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

Vector2UI DisplayWindow::GetSize() const
{
    RECT rect;
    GetClientRect((HWND)_hwnd, &rect);

    return {rect.right - rect.left, rect.bottom - rect.top};
}

void DisplayWindow::GrabCursor(Bool grabbed) const noexcept
{
    if (grabbed)
    {
        RECT rect;
        GetClientRect((HWND)_hwnd, &rect);
        MapWindowPoints((HWND)_hwnd, NULL, (LPPOINT)&rect, 2);
        ClipCursor(&rect);
    }
    else
        ClipCursor(NULL);
}

} // namespace Axis


#ifdef _MSC_VER
#    pragma warning(pop)
#endif