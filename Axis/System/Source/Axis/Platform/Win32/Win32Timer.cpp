/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \remark The code is mostly taken from SFML library.
///
/// \ref https://github.com/SFML/SFML/blob/master/src/SFML/System/Win32/ClockImpl.cpp

#include <Axis/SystemPch.hpp>

#include <Axis/Config.hpp>
#include <Axis/Platform/Win32/Win32Timer.hpp>
#include <Axis/TimePeriod.hpp>
#include <Windows.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <synchapi.h>

static LARGE_INTEGER GetFrequency()
{
    LARGE_INTEGER frequency;

    QueryPerformanceFrequency(&frequency);

    return frequency;
}

static Axis::Bool IsWindowXpOrOlder()
{

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4996)
#endif


    // Windows XP was the last 5.x version of Windows
    return static_cast<DWORD>(LOBYTE(LOWORD(GetVersion()))) < 6;

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif
}

static Axis::System::TimePeriod GetCurrentTimePeriod()
{
    // Calculate inverse of frequency multiplied by 1000000 to prevent overflow in final calculation
    // Frequency is constant across the program lifetime
    static const Axis::BigFloat inverse = 1000000.0 / GetFrequency().QuadPart;

    // Detect if we are on Windows XP or older
    static const Axis::Bool oldWindows = IsWindowXpOrOlder();

    LARGE_INTEGER time;

    if (oldWindows)
    {
        struct MutexWrapper
        {
            MutexWrapper()
            {
                InitializeCriticalSection(&Mutex);
            }

            ~MutexWrapper()
            {
                DeleteCriticalSection(&Mutex);
            }

            void Lock()
            {
                EnterCriticalSection(&Mutex);
            }

            void Unlock()
            {
                LeaveCriticalSection(&Mutex);
            }

            CRITICAL_SECTION Mutex;
        };

        static MutexWrapper oldWindowsMutex;

        // Acquire a lock (CRITICAL_SECTION) to prevent travelling back in time
        oldWindowsMutex.Lock();

        // Get the current time
        QueryPerformanceCounter(&time);

        oldWindowsMutex.Unlock();
    }
    else
    {
        // Get the current time
        QueryPerformanceCounter(&time);
    }

    return Axis::System::TimePeriod((Axis::Uint64)(time.QuadPart * inverse));
}

namespace Axis
{

namespace System
{

// Default constructor
Timer::Timer() noexcept :
    _lastestTime(GetCurrentTimePeriod()) {}

// Default destructor
Timer::~Timer() noexcept {}

TimePeriod Timer::GetElapsedTimePeriod() const noexcept
{
    return GetCurrentTimePeriod() - _lastestTime;
}

TimePeriod Timer::Reset() noexcept
{
    const TimePeriod currentTimePeriod = GetCurrentTimePeriod();
    const TimePeriod elapsed           = currentTimePeriod - _lastestTime;
    _lastestTime                       = currentTimePeriod;

    return elapsed;
}

} // namespace System

} // namespace Axis
