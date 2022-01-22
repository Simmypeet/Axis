/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Trait.hpp
///
/// \brief Contains Axis's common concept definitions.

#ifndef AXIS_SYSTEM_TRAIT_HPP
#define AXIS_SYSTEM_TRAIT_HPP
#pragma once

#include "Config.hpp"
#include <type_traits>

namespace Axis
{

/// \brief Checks if two types are the same.
template <class T, class U>
concept SameAs = requires
{
    std::is_same_v<T, U>;
};

/// \brief Checks if a type doesn't have reference, const or array qualifiers.
template <class T>
concept RawType = !std::is_reference_v<T> && !std::is_const_v<T> && !std::is_array_v<T> && std::is_nothrow_destructible_v<std::decay_t<T>>;

} // namespace Axis

#endif // AXIS_SYSTEM_TRAIT_HPP