/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file WindowPch.hpp
///
/// \brief Precompiled header file for \a `Window` module

#ifndef AXIS_WINDOW_WINDOWPCH_HPP
#define AXIS_WINDOW_WINDOWPCH_HPP
#pragma once

#include "../../System/Include/Axis/System"
#include <atomic>
#include <mutex>
#include <thread>

#ifdef AXIS_PLATFORM_WINDOWS
#    ifndef UNICODE
#        define UNICODE
#    endif

#    include <Windows.h>
#endif

#endif // AXIS_WINDOW_WINDOWPCH_HPP