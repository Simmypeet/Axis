/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_UTILITYIMPL_INL
#define AXIS_SYSTEM_UTILITYIMPL_INL
#pragma once

#include "../../Include/Axis/Utility.hpp"

namespace Axis::System
{

template <class T>
constexpr T&& Forward(RemoveReference<T>& arg) noexcept
{
    return static_cast<T&&>(arg);
}

template <class T>
constexpr T&& Forward(RemoveReference<T>&& arg) noexcept
{
    static_assert(!IsLvalueReference<T>, "Couldn't forward l-value reference.");
    return static_cast<T&&>(arg);
}

template <class T>
constexpr T* AddressOf(T& value)
{
    return std::addressof(value);
}

template <class T>
constexpr RemoveReference<T>&& Move(T&& value) noexcept
{
    return static_cast<RemoveReference<T>&&>(value);
}

template <Concept::Pure T>
constexpr ConditionalType<IsNothrowMoveAssignable<T>, T&&, const T&> MoveAssignIfNoThrow(T& value) noexcept
{
    if constexpr (IsNothrowMoveAssignable<T>)
    {
        return static_cast<T&&>(value);
    }
    else
    {
        return static_cast<const T&>(value);
    }
}

template <Concept::Pure T>
constexpr ConditionalType<IsNothrowMoveConstructible<T>, T&&, const T&> MoveConstructIfNoThrow(T& value) noexcept
{
    if constexpr (IsNothrowMoveConstructible<T>)
    {
        return static_cast<T&&>(value);
    }
    else
    {
        return static_cast<const T&>(value);
    }
}


} // namespace Axis::System

#endif // AXIS_SYSTEM_UTILITYIMPL_INL
