/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.
///

/// \file GraphicsExport.hpp
///
/// \brief Contains portable export/import macro for \a `Graphics` module.
///


#ifndef AXIS_GRAPHICS_GRAPHICSEXPORT_HPP
#define AXIS_GRAPHICS_GRAPHICSEXPORT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"

/// Portable import / export macro
#if defined(BUILD_AXIS_GRAPHICS)

#    define AXIS_GRAPHICS_API AXIS_EXPORT

#else

#    define AXIS_GRAPHICS_API AXIS_IMPORT

#endif

#endif // AXIS_GRAPHICS_GRAPHICSEXPORT_HPP