/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SPAN_HPP
#define AXIS_SYSTEM_SPAN_HPP
#pragma once

#include "Config.hpp"
#include "List.hpp"
#include "Trait.hpp"

namespace Axis
{

namespace System
{

/// \brief Represents the non-owning, read-only continuous block of memory.
template <RawType T>
struct Span
{
public:
    /// \brief Default constructor
    ///
    /// The span contains no elements if it was constructed with this constructor
    constexpr Span() noexcept;

    /// \brief Constructs the span class with nullptr.
    ///
    /// The span contains no elements if it was constructed with this constructor
    constexpr Span(NullptrType) noexcept;

    /// \brief Constructs the span class with static array (stack allocated array).
    template <Size N>
    constexpr Span(const T (&array)[N]) noexcept;

    /// \brief Constructs the span class with user-defined head and end buffer .
    ///
    /// \param[in] begin The start of the buffer.
    /// \param[in] end The end of the buffer
    constexpr Span(const T* begin,
                   const T* end);

    /// \brief Constructs the span class with \a `Array` class.
    ///        The span stores the whole \a `Array`.
    ///
    /// \param[in] array Array to store in the span
    Span(const List<T>& array) noexcept;

    /// \brief Gets the number of element contained in the span.
    AXIS_NODISCARD constexpr Size GetSize() const noexcept;

    /// \brief Gets pointer to the read-only buffer.
    AXIS_NODISCARD constexpr const T* GetData() const noexcept;

    /// Begin const iterator
    AXIS_NODISCARD constexpr const T* begin() const noexcept;

    /// End const iterator
    AXIS_NODISCARD constexpr const T* end() const noexcept;

    /// \brief Checks if the span is empty (contains no elements).
    ///
    /// \return Returns true if the span is empty.
    AXIS_NODISCARD constexpr Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the span is not empty (contains some elements).
    ///
    /// \return Returns true if the span is not empty.
    AXIS_NODISCARD constexpr Bool operator!=(NullptrType) const noexcept;

    /// \brief Implicit cast conversion, checks if the span is not empty.
    ///
    /// \return Returns true if the span is not empty.
    AXIS_NODISCARD constexpr operator Bool() const noexcept;

    /// \brief Indexing operator
    ///
    /// \param[in] index Index to retrieve the object reference in the array.
    ///
    /// \warning The user should be aware of value of \a `index`. The index should
    ///          be in range of array, else the program has to abort upon out of bound index.
    AXIS_NODISCARD constexpr const T& operator[](Size index) const;

private:
    const T* _begin;
    const T* _end;
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/SpanImpl.inl"

#endif // AXIS_SYSTEM_SPAN_HPP