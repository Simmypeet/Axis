/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_TIMEPERIOD_HPP
#define AXIS_TIMEPERIOD_HPP
#pragma once

#include "Config.hpp"

namespace Axis
{

namespace System
{

/// \brief Specifies the period/intervals of time.
struct TimePeriod final
{
public:
    /// \brief 1 millisecond contains 1E+3 microseconds.
    static constexpr Size MicrosecondsPerMillisecond = 1000;

    /// \brief 1 second contains 1E+6 microseconds.
    static constexpr Size MicrosecondsPerSecond = 1000000;

    /// \brief 1 minute contains 6E+7 microseconds.
    static constexpr Size MicrosecondsPerMinute = 60000000;

    /// \brief 1 hour contains 3.6E+9 microseconds.
    static constexpr Size MicrosecondsPerHour = 3600000000;

    /// \brief 1 day contains 8.64E+10 microseconds.
    static constexpr Size MicrosecondsPerDay = 86400000000;

    /// \brief 1 microsecond contains 1 / 1E+3 millisecond.
    static constexpr BigFloat MillisecondsPerMicrosecond = 1.0L / MicrosecondsPerMillisecond;

    /// \brief 1 microsecond contains 1 / 1E+6 seconds.
    static constexpr BigFloat SecondsPerMicrosecond = 1.0L / MicrosecondsPerSecond;

    /// \brief 1 microsecond contains 1 / 6E+7 minutes.
    static constexpr BigFloat MinutesPerMicrosecond = 1.0L / MicrosecondsPerMinute;

    /// \brief 1 microsecond contains 1 / 3.6E+9 hours.
    static constexpr BigFloat HoursPerMicrosecond = 1.0L / MicrosecondsPerHour;

    /// \brief 1 microsecond contains 1 / 8.64E+10 days.
    static constexpr BigFloat DaysPerMicrosecond = 1.0L / MicrosecondsPerDay;

    /// \brief Default constructor
    constexpr TimePeriod() noexcept = default;

    /// \brief Creates a new instance of TimePeriod.
    ///
    /// \param microseconds Period of time in microseconds magnitude.
    constexpr explicit TimePeriod(Size microseconds) noexcept;

    /// \brief Gets the total time contained in the TimeSpan as microseconds (Multiple of a second: 1E-6).
    AXIS_NODISCARD constexpr Size GetTotalMicroseconds() const noexcept;

    /// \brief Gets the total time contained in the TimeSpan as microseconds (Multiple of a second: 1E-3).
    AXIS_NODISCARD constexpr BigFloat GetTotalMilliseconds() const noexcept;

    /// \brief Gets the total time contained in the TimeSpan as seconds.
    AXIS_NODISCARD constexpr BigFloat GetTotalSeconds() const noexcept;

    /// \brief Gets the total time contained in the TimeSpan as seconds (Multiple of a second: 60).
    AXIS_NODISCARD constexpr BigFloat GetTotalMinutes() const noexcept;

    /// \brief Gets the total time contained in the TimeSpan as seconds (Multiple of a second: 3.6E+3).
    AXIS_NODISCARD constexpr BigFloat GetTotalHours() const noexcept;

    /// \brief Gets the total time contained in the TimeSpan as seconds (Multiple of a second: 8.64E+4).
    AXIS_NODISCARD constexpr BigFloat GetTotalDays() const noexcept;

    /// \brief Gets the milliseconds component in the format of [HH:MM:SS:FFF]
    AXIS_NODISCARD constexpr Size GetMilliseconds() const noexcept;

    /// \brief Gets the seconds component in the format of [HH:MM:SS:FFF]
    AXIS_NODISCARD constexpr Size GetSeconds() const noexcept;

    /// \brief Gets the minutes component in the format of [HH:MM:SS:FFF]
    AXIS_NODISCARD constexpr Size GetMinutes() const noexcept;

    /// \brief Gets the hours component in the format of [HH:MM:SS:FFF]
    AXIS_NODISCARD constexpr Size GetHours() const noexcept;

    /// \brief Gets the days component in the format of [HH:MM:SS:FFF]
    AXIS_NODISCARD constexpr Size GetDays() const noexcept;

    /// \brief Converts the period of time from milliseconds into TimePeriod.
    AXIS_NODISCARD constexpr static TimePeriod FromMilliseconds(BigFloat milliseconds);

    /// \brief Converts the period of time from seconds into TimePeriod.
    AXIS_NODISCARD constexpr static TimePeriod FromSeconds(BigFloat seconds);

    /// \brief Converts the period of time from minutes into TimePeriod.
    AXIS_NODISCARD constexpr static TimePeriod FromMinutes(BigFloat minutes);

    /// \brief Converts the period of time from hours into TimePeriod.
    AXIS_NODISCARD constexpr static TimePeriod FromHours(BigFloat hours);

    /// \brief Converts the period of time from days into TimePeriod.
    AXIS_NODISCARD constexpr static TimePeriod FromDays(BigFloat days);

    /// \brief Equal operator
    AXIS_NODISCARD constexpr Bool operator==(const TimePeriod& other) const noexcept;

    /// \brief Not equal operator
    AXIS_NODISCARD constexpr Bool operator!=(const TimePeriod& other) const noexcept;

    /// \brief Greater than operator
    AXIS_NODISCARD constexpr Bool operator>(const TimePeriod& other) const noexcept;

    /// \brief Lesser than operator
    AXIS_NODISCARD constexpr Bool operator<(const TimePeriod& other) const noexcept;

    /// \brief Greater than or equal operator
    AXIS_NODISCARD constexpr Bool operator>=(const TimePeriod& other) const noexcept;

    /// \brief Lesser than or equal operator
    AXIS_NODISCARD constexpr Bool operator<=(const TimePeriod& other) const noexcept;

    /// \brief Plus arithmetic operator
    AXIS_NODISCARD constexpr TimePeriod operator+(const TimePeriod& other) const noexcept;

    /// \brief Minus arithmetic operator
    AXIS_NODISCARD constexpr TimePeriod operator-(const TimePeriod& other) const noexcept;

    /// \brief Compoud plus assignment
    constexpr TimePeriod& operator+=(const TimePeriod& other) noexcept;

    /// \brief Compoud minus assignment
    constexpr TimePeriod& operator-=(const TimePeriod& other) noexcept;

    /// \brief Period of time stored in microseconds magnitude.
    Size Microseconds = 0;
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/TimePeriodImpl.inl"

#endif // AXIS_TIMEPERIOD_HPP