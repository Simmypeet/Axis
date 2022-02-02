/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

/// \file Assembly.hpp
///
/// \brief This file includes the header which contains defintinion of \a `Axis::Assembly`
///        in the correct platform.

#ifndef AXIS_SYSTEM_ASSEMBLY_HPP
#define AXIS_SYSTEM_ASSEMBLY_HPP
#pragma once

#include "Config.hpp"

// Assembly class for win32 platform
#ifdef AXIS_PLATFORM_WINDOWS
#    include "Platform/Win32/Win32Assembly.hpp"
#endif

#endif // AXIS_SYSTEM_ASSEMBLY_HPP