/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/System>
#include <Axis/Window>

using namespace Axis;

int main()
{
    // Creates a window with the specified description.
    WindowDescription windowDescription = {
        .WindowSize     = {800, 600},
        .WindowPosition = {WindowDescription::UndefinedPosition, WindowDescription::UndefinedPosition},
        .Title          = L"My first window",
        .WindowStyle    = WindowStyleFlags::TitleBar | WindowStyleFlags::Resizeable | WindowStyleFlags::CloseButton};

    DisplayWindow window(windowDescription);

    // Flags indicates whether if the window should be closed or not.
    Bool windowShouldClose = false;

    auto WindowCloseEventHandler = [&windowShouldClose](DisplayWindow& window) {
        windowShouldClose = true;
    };

    window.GetUserClosedWindowEvent().Add(WindowCloseEventHandler);

    window.ShowWindow();

    while (!windowShouldClose)
    {
        window.WaitEvent();

        if (Keyboard::GetKeyboardState().IsKeyDown(Key::Escape))
            windowShouldClose = true;
    }

    return 0;
}