/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ASSERTEXCEPTION_HPP
#define AXIS_SYSTEM_ASSERTEXCEPTION_HPP
#pragma once

#include "Config.hpp"
#include <cstdlib>
#include <stdio.h>


#ifdef AXIS_DEBUG
#    if defined(__has_builtin) && !defined(__ibmxl__)
#        if __has_builtin(__builtin_debugtrap)
#            define AXIS_DEBUG_TRAP() __builtin_debugtrap()
#        elif __has_builtin(__debugbreak)
#            define AXIS_DEBUG_TRAP() __debugbreak()
#        endif
#    endif
#    if !defined(AXIS_DEBUG_TRAP)
#        if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#            define AXIS_DEBUG_TRAP() __debugbreak()
#        elif defined(__ARMCC_VERSION)
#            define AXIS_DEBUG_TRAP() __breakpoint(42)
#        elif defined(__ibmxl__) || defined(__xlC__)
#            include <builtins.h>
#            define AXIS_DEBUG_TRAP() __trap(42)
#        elif defined(__DMC__) && defined(_M_IX86)
static inline void AXIS_DEBUG_TRAP(void) { __asm int 3h; }
#        elif defined(__i386__) || defined(__x86_64__)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__("int3"); }
#        elif defined(__thumb__)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__(".inst 0xde01"); }
#        elif defined(__aarch64__)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__(".inst 0xd4200000"); }
#        elif defined(__arm__)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__(".inst 0xe7f001f0"); }
#        elif defined(__alpha__) && !defined(__osf__)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__("bpt"); }
#        elif defined(_54_)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__("ESTOP"); }
#        elif defined(_55_)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__(";\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP"); }
#        elif defined(_64P_)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__("SWBP 0"); }
#        elif defined(_6x_)
static inline void AXIS_DEBUG_TRAP(void) { __asm__ __volatile__("NOP\n .word 0x10000000"); }
#        elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#            define AXIS_DEBUG_TRAP() __builtin_trap()
#        else
#            include <signal.h>
#            if defined(SIGTRAP)
#                define AXIS_DEBUG_TRAP() raise(SIGTRAP)
#            else
#                define AXIS_DEBUG_TRAP() raise(SIGABRT)
#            endif
#        endif
#    endif
#else
#    define AXIS_DEBUG_TRAP()
#endif

#ifdef AXIS_DEBUG

/// Aborts the program upon unsatisfied conditions.
#    define AXIS_ASSERT(expression, message)                     \
        if (!(expression))                                       \
        {                                                        \
            fprintf(stderr, "ASSERTION: (%s) => ", #expression); \
            fprintf(stderr, "[%s]\n", message);                  \
            AXIS_DEBUG_TRAP();                                   \
            std::abort();                                        \
        }

#else

/// The symbol is stripped out in release mode
#    define AXIS_ASSERT(expression, message)

#endif

/// Aborts the program upon unsatisfied conditions.
#define AXIS_VALIDATE(expression, message)                   \
    if (!(expression))                                       \
    {                                                        \
        fprintf(stderr, "ASSERTION: (%s) => ", #expression); \
        fprintf(stderr, "[%s]\n", message);                  \
        AXIS_DEBUG_TRAP();                                   \
        std::abort();                                        \
    }


#endif // AXIS_SYSTEM_ASSERTEXCEPTION_HPP