/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_TRAIT_HPP
#define AXIS_SYSTEM_TRAIT_HPP
#pragma once

#include "Config.hpp"
#include <concepts>
#include <type_traits>

namespace Axis
{

namespace System
{

/// \brief Checks if two types are the same.
template <class T, class U>
concept SameAs = requires
{
    std::is_same_v<T, U>;
};

/// \brief Checks if a type doesn't have reference, const or array qualifiers.
template <class T>
concept RawType = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T>;

/// \brief Checks if a type doesn't have reference, const or array qualifiers.
template <class T>
concept RawConstableType = !std::is_reference_v<T> && !std::is_array_v<T>;

/// \brief Concept for checking if a type is a callable object.
template <class T, class ReturnType, class... Args>
concept Callable = requires(T t, Args&&... args)
{
    {
        t(std::forward<Args>(args)...)
        } -> SameAs<ReturnType>;
}
&&RawType<T>&& std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_copy_constructible_v<T>;

/// \brief Checks if a type is POD (Plain Old Data).
template <class T>
concept PodType = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_TRAIT_HPP