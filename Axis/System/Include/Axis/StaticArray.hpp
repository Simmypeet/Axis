/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.


#ifndef AXIS_SYSTEM_STATICARRAY_HPP
#define AXIS_SYSTEM_STATICARRAY_HPP
#pragma once

#include "Config.hpp"
#include "Trait.hpp"

namespace Axis
{

namespace System
{

/// \brief The wrapper over the compile-time known size array.
///
/// \warning This class doesn't provide strong exception safety.
template <RawType T, Size N>
struct StaticArray
{
public:
    /// \brief The length of the static array.
    static constexpr Size Length = N;

    /// \brief Gets the length of the array.
    AXIS_NODISCARD constexpr Size GetLength() const noexcept;

    /// \brief Indexing operator. Gets the reference to the element at the specified index.
    ///
    /// \param[in] index The index of the element.
    ///
    /// \return The reference to the element at the specified index.
    AXIS_NODISCARD constexpr T& operator[](Size index);

    /// \brief Indexing operator. Gets the reference to the element at the specified index.
    ///
    AXIS_NODISCARD constexpr const T& operator[](Size index) const;

    /// \brief Iterator begin
    AXIS_NODISCARD constexpr T* begin() noexcept;

    /// \brief Const iterator begin
    AXIS_NODISCARD constexpr const T* begin() const noexcept;

    /// \brief Iterator end
    AXIS_NODISCARD constexpr T* end() noexcept;

    /// \brief Const iterator end
    AXIS_NODISCARD constexpr const T* end() const noexcept;

    /// \brief The internal stack allocated array.
    T _Elements[N] = {}; // Initializes array with default constructor
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/StaticArrayImpl.inl"

#endif // AXIS_SYSTEM_STATICARRAY_HPP