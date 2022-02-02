/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file Win32Timer.hpp
///
/// \brief Contains definition of `Axis::System::Timer` class in Win32 platform.

#ifndef AXIS_SYSTEM_WIN32TIMER_HPP
#define AXIS_SYSTEM_WIN32TIMER_HPP
#pragma once

#include "../../SystemExport.hpp"
#include "../../TimePeriod.hpp"

namespace Axis
{

/// \brief Represents the high resolution timer.
struct AXIS_SYSTEM_API Timer final
{
public:
    /// \brief Default constructor
    Timer() noexcept;

    /// \brief Destructor
    ~Timer() noexcept;

    /// \brief Gets the total elapsed time period since the construction / last Reset.
    AXIS_NODISCARD TimePeriod GetElapsedTimePeriod() const noexcept;

    /// \brief Gets the total elapsed time period since the construction / last Reset, and reset the current time.
    TimePeriod Reset() noexcept;

private:
    TimePeriod _lastestTime = {};
};

} // namespace Axis

#endif // AXIS_SYSTEM_WIN32TIMER_HPP