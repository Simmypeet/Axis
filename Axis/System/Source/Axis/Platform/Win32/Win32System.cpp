/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#include <Axis/System.hpp>
#include <Axis/SystemPch.hpp>
#include <Axis/Timer.hpp>
#include <Windows.h>
#include <ios>
#include <iostream>

namespace Axis
{

namespace System
{

/// \ref https://github.com/SFML/SFML/blob/master/src/SFML/System/Win32/SleepImpl.cpp
void Sleep(const TimePeriod& period) noexcept
{
    static thread_local Timer s_timer = {};

    s_timer.Reset();

    TIMECAPS timecpas;
    timeGetDevCaps(&timecpas, sizeof(timecpas));

    timeBeginPeriod(timecpas.wPeriodMin);

    ::Sleep(static_cast<DWORD>(period.GetTotalMilliseconds()));

    auto timeSlept = s_timer.GetElapsedTimePeriod();

    while (timeSlept < period)
    {
        ::Sleep(static_cast<DWORD>((period - timeSlept).GetTotalMilliseconds()));
        timeSlept = s_timer.GetElapsedTimePeriod();
    }

    timeEndPeriod(timecpas.wPeriodMin);
}

Bool CreateConsole() noexcept
{
    Bool result = false;

    // Release any current console and redirect IO to NUL
    Axis::System::DestroyConsole();

    // Attempt to create new console
    if (AllocConsole())
    {

        // Set the screen buffer to be big enough to scroll some text
        CONSOLE_SCREEN_BUFFER_INFO conInfo;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
        if (conInfo.dwSize.Y < 1024)
            conInfo.dwSize.Y = 1024;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);

        FILE* fp;

        // Redirect STDIN if the console has an input handle
        if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
                result = false;
            else
                setvbuf(stdin, NULL, _IONBF, 0);
        }
        // Redirect STDOUT if the console has an output handle
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
                result = false;
            else
                setvbuf(stdout, NULL, _IONBF, 0);
        }
        // Redirect STDERR if the console has an error handle
        if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
        {
            if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
                result = false;
            else
                setvbuf(stderr, NULL, _IONBF, 0);
        }
        // Make C++ standard streams point to console as well.
        std::ios::sync_with_stdio(true);

        // Clear the error state for each of the C++ standard streams.
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();
    }

    return result;
}

Bool DestroyConsole() noexcept
{
    Bool  result = true;
    FILE* fp;

    // Just to be safe, redirect standard IO to NUL before releasing.

    // Redirect STDIN to NUL
    if (freopen_s(&fp, "NUL:", "r", stdin) != 0)
        result = false;
    else
        setvbuf(stdin, NULL, _IONBF, 0);

    // Redirect STDOUT to NUL
    if (freopen_s(&fp, "NUL:", "w", stdout) != 0)
        result = false;
    else
        setvbuf(stdout, NULL, _IONBF, 0);

    // Redirect STDERR to NUL
    if (freopen_s(&fp, "NUL:", "w", stderr) != 0)
        result = false;
    else
        setvbuf(stderr, NULL, _IONBF, 0);

    // Detach from console
    if (!FreeConsole())
        result = false;

    return result;
}


} // namespace System

} // namespace Axis