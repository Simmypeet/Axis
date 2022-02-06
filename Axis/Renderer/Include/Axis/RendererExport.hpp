/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_RENDERER_RENDEREREXPORT_HPP
#define AXIS_RENDERER_RENDEREREXPORT_HPP
#pragma once

#include "../../../System/Include/Axis/Config.hpp"

/// Portable import / export macro
#if defined(BUILD_AXIS_RENDERER)

#    define AXIS_RENDERER_API AXIS_EXPORT

#else

#    define AXIS_RENDERER_API AXIS_IMPORT

#endif

#endif // AXIS_RENDERER_RENDEREREXPORT_HPP