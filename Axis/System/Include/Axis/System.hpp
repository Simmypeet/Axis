/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_SYSTEM_SYSTEM_HPP
#define AXIS_SYSTEM_SYSTEM_HPP
#pragma once

#include "SystemExport.hpp"
#include "TimePeriod.hpp"

namespace Axis
{

namespace System
{

/// \brief Sleeps / blocks the current thread for the specified period of time.
///
/// \param[in] period Period of time to sleep / block the time.
void AXIS_SYSTEM_API Sleep(const TimePeriod& period) noexcept;

/// \brief Creates new console window if haven't one yet.
///
/// \return true if the new console is created, false if failed to create console
///         or there are already existing console activating.
Bool AXIS_SYSTEM_API CreateConsole() noexcept;

/// \brief Destroys the current console window.
///
/// \return true if the console is successfully destroyed, false if failed to destroy console
///         or there aren't any console to destroy.
Bool AXIS_SYSTEM_API DestroyConsole() noexcept;

} // namespace System

} // namespace Axis

#endif