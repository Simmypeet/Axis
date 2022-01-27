/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file WindowExport.hpp
/// 
/// \brief Contains portable export/import macro for \a `Window` module.

#ifndef AXIS_WINDOW_WINDOWEXPORT_HPP
#define AXIS_WINDOW_WINDOWEXPORT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"

/// Portable import / export macro
#if defined(BUILD_AXIS_WINDOW)

#    define AXIS_WINDOW_API AXIS_EXPORT

#else

#    define AXIS_WINDOW_API AXIS_IMPORT

#endif

#endif // AXIS_WINDOW_WINDOWEXPORT_HPP