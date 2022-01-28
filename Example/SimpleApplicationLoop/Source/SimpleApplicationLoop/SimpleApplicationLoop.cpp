#include <Axis/DisplayWindow.hpp>
#include <Axis/Enum.hpp>
#include <Axis/Keyboard.hpp>

using namespace Axis;

Int32 ExampleMain()
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

#ifdef AXIS_PLATFORM_WINDOWS
#    include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow)
{
    return ExampleMain();
}
#else
int main()
{
    return ExampleMain();
}
#endif