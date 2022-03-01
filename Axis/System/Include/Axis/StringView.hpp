/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_SYSTEM_STRINGVIEW_HPP
#define AXIS_SYSTEM_STRINGVIEW_HPP
#pragma once

#include "String.hpp"

namespace Axis
{

namespace System
{

/// \brief Represents the view to the non-owning string sequences.
template <CharType T>
struct StringView
{
public:
    /// Default constructor
    constexpr StringView() noexcept = default;

    /// \brief Constructs the object with nullptr, zero string length, empty string.
    constexpr StringView(NullptrType) noexcept;

    /// \brief Constructs the object with null terminated string sequence.
    ///        the length will be cached into object.
    constexpr StringView(const T* string) noexcept;

    /// \brief Constructs the StringView object with defined range.
    ///
    /// \param[in] string Pointer to the string buffer to initialize. Null terminated string is not required.
    /// \param[in] stringLength Length of the string sequence.
    constexpr StringView(const T* string, Size stringLength) noexcept;

    /// \brief Constructs the StringView object with defined range.
    ///
    /// \param[in] begin Pointer to the begining of the string.
    /// \param[in] end Pointer to the end of the string.
    constexpr StringView(const T* begin,
                         const T* end);

    /// \brief Constructs the StringView object from the string object.
    ///
    /// \param[in] string String to view
    template <MemoryResourceType MemRes>
    constexpr StringView(const String<T, MemRes>& string) noexcept;

    /// \brief Gets the length of the string. The length was cached into the object
    ///        since the construction.
    AXIS_NODISCARD constexpr Size GetSize() const noexcept;

    /// \brief Gets the internal string buffer.
    AXIS_NODISCARD constexpr const T* GetCString() const noexcept;

    /// \brief Checks whether the string is nullptr string or empty (null terminated at the start).
    AXIS_NODISCARD inline constexpr Bool IsNullOrEmpty() const noexcept;

    /// \brief Implicit cast conversion: Returns true if the string is not null or empty.
    AXIS_NODISCARD inline constexpr operator Bool() const noexcept;

    /// \brief Checks whether the string is nullptr string or empty (null terminated at the start).
    AXIS_NODISCARD inline constexpr Bool operator==(NullptrType) const noexcept;

    /// \brief Checks whether the string is nullptr string or empty (null terminated at the start).
    AXIS_NODISCARD inline constexpr Bool operator!=(NullptrType) const noexcept;

    /// \brief Indexing operator. Gets the element of the string at the specified index.
    AXIS_NODISCARD constexpr const T& operator[](Size index) const;

    /// \brief Const iterator begin
    AXIS_NODISCARD constexpr const T* begin() const noexcept;

    /// \brief Const iterator end
    AXIS_NODISCARD constexpr const T* end() const noexcept;

    /// \brief Checks if the current view on the string is null terminated.
    AXIS_NODISCARD constexpr Bool IsNullTerminated() const noexcept;

    /// \brief Constructs a new string object from the view.
    template <MemoryResourceType MemRes = DefaultMemoryResource>
    AXIS_NODISCARD constexpr operator String<T, MemRes>() const;

private:
    const T*     _stringBuffer       = nullptr; // Pointer to null terminated string
    mutable Size _stringLength       = 0;       // Cached string length
    Bool         _nullTerminatedView = false;   // Indicates whether the viewed string was taken from null terminated string sequence
};

} // namespace System

} // namespace Axis

/// \brief Appends the string view to the string
///
/// \param[in] LHS Left hand side string to be appended.
/// \param[in] RHS Right hand side string to append.
template <Axis::System::CharType T, Axis::System::CharType U, Axis::System::MemoryResourceType MemRes>
Axis::System::String<T, MemRes>& operator+=(Axis::System::String<T, MemRes>& LHS, const Axis::System::StringView<U>& RHS);

#include "../../Private/Axis/StringViewImpl.inl"

#endif // AXIS_SYSTEM_STRINGVIEW_HPP
