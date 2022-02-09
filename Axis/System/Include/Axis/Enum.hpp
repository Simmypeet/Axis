/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ENUM_HPP
#define AXIS_SYSTEM_ENUM_HPP
#pragma once

#include "Config.hpp"
#include "Trait.hpp"

namespace Axis
{

namespace System
{

namespace Enum
{

/// \brief Type is scoped or un-scoped enum.
template <class T>
concept EnumType = std::is_enum_v<T>;

/// \brief Gets the underlying numeric value of the enum value.
///
/// \param[in] enumVal Enum value to retrieve the underlying numeric value.
template <EnumType T>
constexpr auto GetUnderlyingValue(T enumVal) noexcept -> typename std::underlying_type<T>::type;

namespace Operator
{

/// Bitwise OR(|) operator
template <EnumType T>
constexpr T operator|(T LHS, T RHS) noexcept;

/// Compound assignment bitwise OR(|) operator
template <EnumType T>
constexpr T& operator|=(T& LHS, T RHS) noexcept;

/// Bitwise AND(&) operator
template <EnumType T>
constexpr T operator&(T LHS, T RHS) noexcept;

/// Compound assignment bitwise AND(&) operator
template <EnumType T>
constexpr T& operator&=(T& LHS, T RHS) noexcept;

/// Bitwise NOT(~) operator
template <EnumType T>
constexpr T operator~(T val) noexcept;

} // namespace Operator

} // namespace Enum

} // namespace System

} // namespace Axis

using namespace Axis::System::Enum::Operator;

#include "../../Private/Axis/EnumImpl.inl"

#endif // AXIS_SYSTEM_ENUM_HPP