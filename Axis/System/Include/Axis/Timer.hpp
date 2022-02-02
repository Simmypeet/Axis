/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Timer.hpp
///
/// \brief This file includes the header which contains defintinion of \a `Axis::Timer`
///        in the correct platform.

#ifndef AXIS_SYSTEM_TIMER_HPP
#define AXIS_SYSTEM_TIMER_HPP

#include "Config.hpp"

// Timer class for win32 platform
#ifdef AXIS_PLATFORM_WINDOWS
#    include "Platform/Win32/Win32Timer.hpp"
#endif

#endif // AXIS_SYSTEM_TIMER_HPP