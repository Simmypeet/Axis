/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_TIMEPERIODIMPL_INL
#define AXIS_SYSTEM_TIMEPERIODIMPL_INL
#pragma once

#include "../../Include/Axis/TimePeriod.hpp"

namespace Axis
{

inline constexpr TimePeriod::TimePeriod(Size microseconds) noexcept :
    Microseconds(microseconds) {}

inline constexpr Size TimePeriod::GetTotalMicroseconds() const noexcept { return Microseconds; }

inline constexpr BigFloat TimePeriod::GetTotalMilliseconds() const noexcept { return Microseconds * MillisecondsPerMicrosecond; }

inline constexpr BigFloat TimePeriod::GetTotalSeconds() const noexcept { return Microseconds * SecondsPerMicrosecond; }

inline constexpr BigFloat TimePeriod::GetTotalMinutes() const noexcept { return Microseconds * MinutesPerMicrosecond; }

inline constexpr BigFloat TimePeriod::GetTotalHours() const noexcept { return Microseconds * HoursPerMicrosecond; }

inline constexpr BigFloat TimePeriod::GetTotalDays() const noexcept { return Microseconds * DaysPerMicrosecond; }

inline constexpr Size TimePeriod::GetMilliseconds() const noexcept { return (Size)((Microseconds / MicrosecondsPerMillisecond) % 1000); }

inline constexpr Size TimePeriod::GetSeconds() const noexcept { return (Size)((Microseconds / MicrosecondsPerSecond) % 60); }

inline constexpr Size TimePeriod::GetMinutes() const noexcept { return (Size)((Microseconds / MicrosecondsPerMinute) % 60); }

inline constexpr Size TimePeriod::GetHours() const noexcept { return (Size)((Microseconds / MicrosecondsPerHour) % 24); }

inline constexpr Size TimePeriod::GetDays() const noexcept { return (Size)(Microseconds / MicrosecondsPerDay); }

inline constexpr TimePeriod TimePeriod::FromMilliseconds(BigFloat milliseconds)
{
    // negative value is not allowed!
    milliseconds = milliseconds < 0 ? -milliseconds : milliseconds;

    return TimePeriod((Size)(milliseconds * TimePeriod::MicrosecondsPerMillisecond));
}

inline constexpr TimePeriod TimePeriod::FromSeconds(BigFloat seconds)
{
    // negative value is not allowed!
    seconds = seconds < 0 ? -seconds : seconds;

    return TimePeriod((Size)(seconds * TimePeriod::MicrosecondsPerSecond));
}

inline constexpr TimePeriod TimePeriod::FromMinutes(BigFloat minutes)
{
    // negative value is not allowed!
    minutes = minutes < 0 ? -minutes : minutes;

    return TimePeriod((Size)(minutes * TimePeriod::MicrosecondsPerMinute));
}

inline constexpr TimePeriod TimePeriod::FromHours(BigFloat hours)
{
    // negative value is not allowed!
    hours = hours < 0 ? -hours : hours;

    return TimePeriod((Size)(hours * TimePeriod::MicrosecondsPerHour));
}

inline constexpr TimePeriod TimePeriod::FromDays(BigFloat days)
{
    // negative value is not allowed!
    days = days < 0 ? -days : days;

    return TimePeriod((Size)(days * TimePeriod::MicrosecondsPerDay));
}

inline constexpr Bool TimePeriod::operator==(const TimePeriod& other) const noexcept { return Microseconds == other.Microseconds; }

inline constexpr Bool TimePeriod::operator!=(const TimePeriod& other) const noexcept { return Microseconds != other.Microseconds; }

inline constexpr Bool TimePeriod::operator>(const TimePeriod& other) const noexcept { return Microseconds > other.Microseconds; }

inline constexpr Bool TimePeriod::operator<(const TimePeriod& other) const noexcept { return Microseconds < other.Microseconds; }

inline constexpr Bool TimePeriod::operator>=(const TimePeriod& other) const noexcept { return Microseconds >= other.Microseconds; }

inline constexpr Bool TimePeriod::operator<=(const TimePeriod& other) const noexcept { return Microseconds <= other.Microseconds; }

inline constexpr TimePeriod TimePeriod::operator+(const TimePeriod& other) const noexcept { return TimePeriod(Microseconds + other.Microseconds); }

inline constexpr TimePeriod TimePeriod::operator-(const TimePeriod& other) const noexcept { return TimePeriod(Microseconds - other.Microseconds); }

inline constexpr TimePeriod& TimePeriod::operator+=(const TimePeriod& other) noexcept
{
    Microseconds += other.Microseconds;

    return *this;
}

inline constexpr TimePeriod& TimePeriod::operator-=(const TimePeriod& other) noexcept
{
    Microseconds -= other.Microseconds;

    return *this;
}

} // namespace Axis

#endif // AXIS_SYSTEM_TIMEPERIODIMPL_INL