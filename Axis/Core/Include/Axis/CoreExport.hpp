/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.
///

/// \file CoreExport.hpp
///
/// \brief Contains portable export/import macro for \a `Core` module.
///

#ifndef AXIS_CORE_COREEXPORT_HPP
#define AXIS_CORE_COREEXPORT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"

/// Portable import / export macro
#if defined(BUILD_AXIS_CORE)

#    define AXIS_CORE_API AXIS_EXPORT

#else

#    define AXIS_CORE_API AXIS_IMPORT

#endif

#endif // AXIS_CORE_COREEXPORT_HPP