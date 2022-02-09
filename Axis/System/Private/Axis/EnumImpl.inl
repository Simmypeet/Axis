/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ENUMIMPL_INL
#define AXIS_SYSTEM_ENUMIMPL_INL
#pragma once

#include "../../Include/Axis/Enum.hpp"

namespace Axis
{

namespace System
{

namespace Enum
{

template <EnumType T>
inline constexpr auto GetUnderlyingValue(T enumVal) noexcept -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(enumVal);
}

namespace Operator
{

template <EnumType T>
inline constexpr T operator|(T LHS, T RHS) noexcept
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(LHS) | static_cast<std::underlying_type_t<T>>(RHS));
}

template <EnumType T>
inline constexpr T& operator|=(T& LHS, T RHS) noexcept
{
    LHS = static_cast<T>(static_cast<std::underlying_type_t<T>>(LHS) | static_cast<std::underlying_type_t<T>>(RHS));

    return LHS;
}

template <EnumType T>
inline constexpr T operator&(T LHS, T RHS) noexcept
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(LHS) & static_cast<std::underlying_type_t<T>>(RHS));
}

template <EnumType T>
inline constexpr T& operator&=(T& LHS, T RHS) noexcept
{
    LHS = static_cast<T>(static_cast<std::underlying_type_t<T>>(LHS) & static_cast<std::underlying_type_t<T>>(RHS));

    return LHS;
}

template <EnumType T>
inline constexpr T operator~(T val) noexcept
{
    return static_cast<T>(~(static_cast<std::underlying_type_t<T>>(val)));
}

} // namespace Operator

} // namespace Enum

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_ENUMIMPL_INL