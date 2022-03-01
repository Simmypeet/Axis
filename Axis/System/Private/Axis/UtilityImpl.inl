/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_UTILITYIMPL_INL
#define AXIS_SYSTEM_UTILITYIMPL_INL
#pragma once

#include "../../Include/Axis/Utility.hpp"

namespace Axis
{

namespace System
{

template <class T>
inline constexpr T&& Forward(RemoveReference<T>& arg) noexcept
{
    return static_cast<T&&>(arg);
}

template <class T>
inline constexpr T&& Forward(RemoveReference<T>&& arg) noexcept requires(!IsLvalueReference<T>)
{
    return static_cast<T&&>(arg);
}

template <class T>
inline constexpr T* AddressOf(T& value)
{
    return std::addressof(value);
}

template <class T>
inline constexpr RemoveReference<T>&& Move(T&& value) noexcept
{
    return static_cast<RemoveReference<T>&&>(value);
}

template <RawType T>
inline constexpr ConditionalType<IsNoThrowMoveAssignable<T>, T&&, const T&> MoveAssignIfNoThrow(T& value) noexcept
{
    if constexpr (IsNoThrowMoveAssignable<T>)
    {
        return static_cast<T&&>(value);
    }
    else
    {
        return static_cast<const T&>(value);
    }
}

template <RawType T>
inline constexpr ConditionalType<IsNoThrowMoveConstructible<T>, T&&, const T&> MoveConstructIfNoThrow(T& value) noexcept
{
    if constexpr (IsNoThrowMoveConstructible<T>)
    {
        return static_cast<T&&>(value);
    }
    else
    {
        return static_cast<const T&>(value);
    }
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_UTILITYIMPL_INL