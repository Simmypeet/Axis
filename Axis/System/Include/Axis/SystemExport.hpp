/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

/// \file SystemExport.hpp
///
/// \brief Contains portable export/import macro for \a `System` module.

#ifndef AXIS_SYSTEM_SYSTEMEXPORT_HPP
#define AXIS_SYSTEM_SYSTEMEXPORT_HPP
#pragma once

#include "Config.hpp"

/// Portable import / export macro
#if defined(BUILD_AXIS_SYSTEM)

#    define AXIS_SYSTEM_API AXIS_EXPORT

#else

#    define AXIS_SYSTEM_API AXIS_IMPORT

#endif

#endif // AXIS_SYSTEM_SYSTEMEXPORT_HPP