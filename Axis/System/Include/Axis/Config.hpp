/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Config.hpp
///
/// \brief Contains Axis's basic typedefs and macros to handle
///		   differences of the basic types between compilers and
///        platform.

#ifndef AXIS_SYSTEM_CONFIG_HPP
#define AXIS_SYSTEM_CONFIG_HPP
#pragma once

#include <stdint.h>


/// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

/// \brief Desktop platform
#    define AXIS_PLATFORM_DESKTOP

/// \brief Windows platform
#    define AXIS_PLATFORM_WINDOWS

#elif defined(__APPLE__)

#    include <TargetConditionals.h>

#    if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)

/// \brief Mobile phone platform
#        define AXIS_PLATFORM_MOBILE

/// \brief iOS platform
#        define AXIS_PLATFORM_IOS

#    elif defined(TARGET_OS_MACCATALYST) || defined(TARGET_OS_MAC)

/// \brief Desktop platform
#        define AXIS_PLATFORM_DESKTOP

/// \brief macOS platform
#        define AXIS_PLATFORM_MACOS

#    endif

#elif defined(__unix__)

/// \brief Unix platform
#    define AXIS_PLATFORM_UNIX

#    if defined(__linux__)

/// \brief Desktop platform
#        define AXIS_PLATFORM_DESKTOP

/// \brief Unix platform
#        define AXIS_PLATFORM_LINUX

#    endif

#endif

/// Pointer size detection
#if defined(__LP64__) || defined(_WIN64) ||                            \
    (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || \
    defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) ||     \
    defined(__powerpc64__)

/// \brief The pointer is 64-Bit.
#    define AXIS_64_BIT_PTR
#endif

/// DLL import / export macros for Windows NT
#if defined(AXIS_PLATFORM_WINDOWS)

#    define AXIS_EXPORT __declspec(dllexport)
#    define AXIS_IMPORT __declspec(dllimport)

#else

#    define AXIS_EXPORT
#    define AXIS_IMPORT

#endif

/// Disables in MSVC compilers.
#ifdef _MSC_VER

#    pragma warning(disable : 26812)
#    pragma warning(disable : 4251)
#    pragma warning(disable : 4002)

#endif

#ifndef NDEBUG

#    define AXIS_DEBUG

#endif

namespace Axis
{

/// \brief 8 bits signed integer
typedef signed char Int8;

/// \brief 8 bits unsigned integer
typedef unsigned char Uint8;

/// \brief 16 bits signed integer
typedef signed short Int16;

/// \brief 16 bits unsigned integer
typedef unsigned short Uint16;

/// \brief 32 bits signed integer
typedef signed int Int32;

/// \brief 32 bits unsigned integer
typedef unsigned int Uint32;

#if defined(_MSC_VER)

/// \brief 64 bits signed integer
typedef signed __int64 Int64;

/// \brief 64 bits unsigned integer
typedef unsigned __int64 Uint64;
#else

/// \brief 64 bits signed integer
typedef signed long long Int64;

/// \brief 64 bits unsigned integer
typedef unsigned long long Uint64;
#endif

/// \brief The implicit type of `nullptr` literal
typedef decltype(nullptr) NullptrType;

/// \brief Boolean value type, Only contains 2 values: `true` and `false`
typedef bool Bool;

/// \brief Raw memory pointer (void* pointer)
typedef void* PVoid;

/// \brief Read only raw memory pointer (const void* pointer)
typedef const void* CPVoid;

/// \brief Biggest possible unsigned integer type.
typedef size_t Size;

/// \brief Unsigned 8 bits integer type.
typedef Uint8 Byte;

/// \brief 32 bits floating point number type.
typedef float Float32;

/// \brief 64 bits floating point number type.
typedef double Float64;

/// \brief Largest precision possible floating point number.
///
/// \warning The size may varies between different compilers and platforms.
typedef long double BigFloat;

/// \brief 8-Bits ANSI character
typedef char Char;

/// \brief Wide character
///
/// \warning The size may varies between different platforms and compilers
typedef wchar_t WChar;

#ifdef AXIS_64_BIT_PTR

/// \brief 64 Bits unsigned integer type, can be casted to pointer, back and
/// forth.
typedef Uint64 UintPtr;
#else

/// \brief 32 Bits unsigned integer type, can be casted to pointer, back and
/// forth.
typedef Uint32 UintPtr;
#endif

/// \brief Size of pointer type (in bytes)
constexpr Size PointerSize = sizeof(PVoid);

/// \brief Size of `Axis::Size` integer type (in bytes)
constexpr Size SizeTypeSize = sizeof(Size);

/// \brief Size of C++ defined `wchar_t` type (in bytes)
constexpr Size WideCharSize = sizeof(wchar_t);

/// Integer type size checking
static_assert(sizeof(Int8) == 1, "Size of `Axis::Int8` was not 1 bytes (8 bits)");
static_assert(sizeof(Int16) == 2, "Size of `Axis::Int16` was not 2 bytes (16 bits)");
static_assert(sizeof(Int32) == 4, "Size of `Axis::Int32` was not 4 bytes (32 bits)");
static_assert(sizeof(Int64) == 8, "Size of `Axis::Int64` was not 8 bytes (64 bits)");

static_assert(sizeof(Uint8) == 1, "Size of `Axis::Uint8` was not 1 bytes (8 bits)");
static_assert(sizeof(Uint16) == 2, "Size of `Axis::Uint16` was not 2 bytes (16 bits)");
static_assert(sizeof(Uint32) == 4, "Size of `Axis::Uint32` was not 4 bytes (32 bits)");
static_assert(sizeof(Uint64) == 8, "Size of `Axis::Uint64` was not 8 bytes (64 bits)");
static_assert(sizeof(UintPtr) == PointerSize, "Size of `Axis::UintPtr` was not the same to `Axis::PointerSize`");

/// \brief No discard compiler warning attribute
///
#define AXIS_NODISCARD [[nodiscard]]

} // namespace Axis

#ifdef __clang__
/// \brief Retrieves the function name from the compiler.
#    define AXIS_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
/// \brief Retrieves the function name from the compiler.
#    define AXIS_FUNCTION __FUNCSIG__
#elif defined(__GNUC__)
/// \brief Retrieves the function name from the compiler.
#    define AXIS_FUNCTION __PRETTY_FUNCTION__
#endif

#ifdef AXIS_DEBUG
/// \brief If the macro is defined then the container will check for the
///        arguments validity.
#    define AXIS_CONTAINER_DEBUG
#endif

#ifdef _MSC_VER
/// \brief Note that MSVC x86 compiler by default uses __this call for class
/// member functions
#    define AXIS_CALL_TYPE __cdecl
#else
#    define AXIS_CALL_TYPE
#endif

#endif // AXIS_SYSTEM_CONFIG_HPP