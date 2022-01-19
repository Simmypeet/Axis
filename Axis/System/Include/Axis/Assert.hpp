/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Assert.hpp
///
/// \brief Contains the assertion macros.

#ifndef AXIS_SYSTEM_ASSERTEXCEPTION_HPP
#define AXIS_SYSTEM_ASSERTEXCEPTION_HPP
#pragma once

#include <stdio.h>

/// Aborts the program upon unsatisfied conditions.
#define AXIS_VALIDATE(expression, message)                       \
    if (!(expression))                                           \
    {                                                            \
        fprintf(stderr, "Assertion failure: %s\n", #expression); \
        fprintf(stderr, "Assertion message: %s\n", message);     \
        AXIS_EXIT;                                               \
    }

#ifdef AXIS_DEBUG

#    if defined(AXIS_PLATFORM_WINDOWS)

/// Quick exit
#        define AXIS_EXIT __debugbreak()
#    elif defined(AXIS_PLATFORM_LINUX)
#        include <signal.h>
/// Quick exit
#        define AXIS_EXIT raise(SIGTRAP)
#    endif

#else

#    ifdef __GNUC__

/// Quick exit
#        define AXIS_EXIT _c_exit()
#    else

/// Quick exit
#        define AXIS_EXIT std::quick_exit(EXIT_FAILURE)
#    endif

#endif

#ifdef AXIS_DEBUG

/// Aborts the program upon unsatisfied conditions.
#    define AXIS_ASSERT(expression, message)                         \
        if (!(expression))                                           \
        {                                                            \
            fprintf(stderr, "Assertion failure: %s\n", #expression); \
            fprintf(stderr, "Assertion message: %s\n", message);     \
            AXIS_EXIT;                                               \
        }

#else

/// The symbol is stripped out in release mode
#    define AXIS_ASSERT(expression, message)

#endif

#endif // AXIS_SYSTEM_ASSERTEXCEPTION_HPP