/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file String.hpp
///
/// \brief Contains the definition of the string template class.

#ifndef AXIS_SYSTEM_STRING_HPP
#define AXIS_SYSTEM_STRING_HPP

#include "Memory.hpp"
#include "Trait.hpp"
#pragma once

namespace Axis
{

/// \brief Type of character with different size.
template <class T>
concept CharType = std::is_same_v<T, Char> || std::is_same_v<T, WChar> || std::is_same_v<T, char8_t> || std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>;

/// \brief Counts the element in string pointer until the
///        null terminated character is found. Null terminated character
///        is not included.
///
/// \tparam T string data type.
///
/// \return Returns the number of elements in C-style null terminated string.
template <CharType T>
constexpr Size GetStringLength(const T* strPtr) noexcept;

/// \brief Container which contains a null terminated character sequence.
///
/// \tparam T internal string data type.
template <CharType T, AllocatorType Allocator = DefaultAllocator>
class String final
{
public:
    /// \brief Number of elements in the string that would apply
    ///        the small string optimization.
    static constexpr Size SmallStringSize = (Size)((sizeof(T*) + sizeof(Size)) / sizeof(T)) - 1;

    /// \brief Constructs an empty string.
    String(NullptrType) noexcept;

    /// \brief Default constructor, constructs an empty string.
    String() noexcept = default;

    /// \brief Constructs a string from null terminated character sequence.
    ///
    /// \param[in] str null terminated character sequence.
    template <CharType U>
    String(const U* str) noexcept;

    /// \brief Copy constructor
    ///
    /// \param[in] other instance to copy
    String(const String& other) noexcept;

    /// \brief Move constructor
    ///
    /// \param[in] other instance to move
    String(String&& other) noexcept;

    /// \brief Constructs a string from different character type.
    ///
    /// \param[in] other instance to copy
    template <CharType U, AllocatorType OtherAllocator>
    String(const String<U, OtherAllocator>& other) noexcept;

    /// \brief Destructor
    ~String() noexcept;

    /// \brief Copy assignment operator
    ///
    /// \param[in] other instance to copy
    String& operator=(const String& other) noexcept;

    /// \brief Move assignment operator
    ///
    /// \param[in] other instance to move
    String& operator=(String&& other) noexcept;

    /// \brief Gets pointer to the internal string buffer.
    ///
    /// \return pointer to the internal string buffer.
    AXIS_NODISCARD T* GetCString() noexcept;

    /// \brief Gets pointer to the internal string buffer.
    ///
    /// \return pointer to the internal string buffer.
    AXIS_NODISCARD const T* GetCString() const noexcept;

    /// \brief Checks if the string is equal to another string.
    ///
    /// \param[in] str null terminated character sequence to compare.
    ///
    /// \return Returns true if the string is equal to the other string.
    template <CharType U>
    AXIS_NODISCARD Bool operator==(const U* str) const noexcept;

    /// \brief Checks if the string is not equal to another string.
    ///
    /// \param[in] str null terminated character sequence to compare.
    ///
    /// \return Returns true if the string is not equal to the other string.
    template <CharType U>
    AXIS_NODISCARD Bool operator!=(const U* str) const noexcept;

    /// \brief Checks if the string is null or empty.
    ///
    /// \return Returns true if the string is null or empty.
    AXIS_NODISCARD Bool operator==(NullptrType) const noexcept;

    /// \brief Checks if the string is not null or empty.
    ///
    /// \return Returns true if the string is not null or empty.
    AXIS_NODISCARD Bool operator!=(NullptrType) const noexcept;

    /// \brief Equality operator.
    ///
    /// \param[in] other instance to compare.
    ///
    /// \return Returns true if the string is equal to the other string.
    template <CharType U, AllocatorType OtherAllocator>
    AXIS_NODISCARD Bool operator==(const String<U, OtherAllocator>& other) const noexcept;

    /// \brief Inequality operator.
    ///
    /// \param[in] other instance to compare.
    ///
    /// \return Returns true if the string is equal to the other string.
    template <CharType U, AllocatorType OtherAllocator>
    AXIS_NODISCARD Bool operator!=(const String<U, OtherAllocator>& other) const noexcept;

    /// \brief Array subscript operator
    ///
    /// \param[in] index index of the element to get.
    ///
    /// \return Returns the element at the specified index.
    AXIS_NODISCARD T& operator[](Size index);

    /// \brief Array subscript operator
    ///
    /// \param[in] index index of the element to get.
    ///
    /// \return Returns the element at the specified index.
    AXIS_NODISCARD const T& operator[](Size index) const;

    /// \brief Gets the number of elements in the string (null terminated character is not included).
    ///
    /// \return Returns the number of elements in the string.
    AXIS_NODISCARD Size GetLength() const noexcept;

private:
    T* Reserve(Size size) noexcept;

    union
    {
        struct
        {
            T*   DynamicStringBuffer;        // Dynamic buffer allocation for the string.
            Size DynamicMemoryAllocatedSize; // Size of dynamic allocated memory (per character! not bytes!), included null terminated character.
        };

        T SmallStringBuffer[SmallStringSize + 1]; // Stack allocated buffer memory.
    };

    Size _stringLength  = 0;    // Caches the string length to this variable. not included null terminated character.
    Bool _isSmallString = true; // Indicates whether the current string is using small string optimization.

    template <CharType, AllocatorType>
    friend class String;
};

/// \brief Data structure which contains null terminated `Axis::Char` sequence.
using String8 = String<Char, DefaultAllocator>;

/// \brief Data structure which contains null terminated `Axis::WChar` sequence.
using WString = String<WChar, DefaultAllocator>;

/// \brief Data structure which contains null terminated `char8_t` sequence.
using StringU8 = String<char8_t, DefaultAllocator>;

/// \brief Data structure which contains null terminated `char16_t` sequence.
using StringU16 = String<char16_t, DefaultAllocator>;

} // namespace Axis

#include "../../Private/Axis/StringImpl.inl"

#endif // AXIS_SYSTEM_STRING_HPP