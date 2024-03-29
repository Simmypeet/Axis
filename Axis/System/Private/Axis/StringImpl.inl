/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_STRINGIMPL_INL
#define AXIS_SYSTEM_STRINGIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/String.hpp"

namespace Axis
{

namespace System
{

namespace Detail
{

template <Bool NullTerminated, CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept;

template <Bool NullTerminated, CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(std::is_same_v<T, U>)
{
    // use memcpy directly
    std::memcpy(destination, source, sourceSize * sizeof(T));

    if constexpr (NullTerminated)
        destination[sourceSize] = U(0);
}

template <Bool NullTerminated, CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(!std::is_same_v<T, U>)
{
    // Convert each element
    for (Size i = 0; i < sourceSize; ++i)
        destination[i] = U(source[i]);

    if constexpr (NullTerminated)
        destination[sourceSize] = U(0);
}

template <ArithmeticType U, CharType T, AllocatorType Allocator>
inline void AppendNumerics(String<T, Allocator>& str,
                           U                     value) noexcept
{
    if (value >= 10)
        AppendNumerics(str, value / 10);

    U digit = value % 10;

    str.Append(static_cast<T>(digit + '0'));
}

} // namespace Detail

template <CharType T, AllocatorType Allocator>
constexpr Size String<T, Allocator>::GetStringLength(const T* strPtr) noexcept
{
    if (!strPtr)
        return 0;

    Size length = 0;

    while (strPtr[length] != T(0))
        ++length;

    return length;
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::String(NullptrType) noexcept {}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline String<T, Allocator>::String(const U* str) :
    _stringLength(String<U, Allocator>::GetStringLength(str))
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement<true>(str, pointer, _stringLength);
}


template <CharType T, AllocatorType Allocator>
template <CharType U>
inline String<T, Allocator>::String(const U* begin,
                                    const U* end) :
    _stringLength(end - begin)
{
    if (begin > end)
        throw InvalidArgumentException("`begin` was greater than `end`!");

    auto pointer = Reserve(_stringLength);

    Detail::CopyElement<true>(begin, pointer, _stringLength);
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::String(const String<T, Allocator>& other) :
    _stringLength(other._stringLength)
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement<true>(other.GetCString(), pointer, _stringLength);
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::String(String<T, Allocator>&& other) noexcept :
    _stringLength(other._stringLength),
    _isSmallString(other._isSmallString)
{
    if (_isSmallString)
    {
        Detail::CopyElement<true>(other.SmallStringBuffer, SmallStringBuffer, _stringLength);
    }
    else
    {
        DynamicStringBuffer        = other.DynamicStringBuffer;
        DynamicMemoryAllocatedSize = other.DynamicMemoryAllocatedSize;
    }

    other._stringLength  = 0;
    other._isSmallString = true;
}

template <CharType T, AllocatorType Allocator>
template <CharType U, AllocatorType OtherAllocator>
inline String<T, Allocator>::String(const String<U, OtherAllocator>& other) :
    _stringLength(other._stringLength)
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement<true>(other.GetCString(), pointer, _stringLength);
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::~String() noexcept
{
    if (!_isSmallString)
        Allocator::Deallocate(DynamicStringBuffer);
}

template <CharType T, AllocatorType Allocator>
String<T, Allocator>& String<T, Allocator>::operator=(const String<T, Allocator>& other)
{
    if (this == std::addressof(other))
        return *this;

    auto pointer = Reserve(other._stringLength);

    Detail::CopyElement<true>(other.GetCString(), pointer, other._stringLength);

    _stringLength = other._stringLength;

    return *this;
}

template <CharType T, AllocatorType Allocator>
String<T, Allocator>& String<T, Allocator>::operator=(String<T, Allocator>&& other) noexcept
{
    if (this == std::addressof(other))
        return *this;

    // Deallocates the current string
    if (!_isSmallString)
    {
        Allocator::Deallocate(DynamicStringBuffer);
        _isSmallString = true;
    }

    _stringLength  = other._stringLength;
    _isSmallString = other._isSmallString;

    if (_isSmallString)
    {
        Detail::CopyElement<true>(other.SmallStringBuffer, SmallStringBuffer, _stringLength);
    }
    else
    {
        DynamicStringBuffer        = other.DynamicStringBuffer;
        DynamicMemoryAllocatedSize = other.DynamicMemoryAllocatedSize;
    }

    other._stringLength  = 0;
    other._isSmallString = true;

    return *this;
}

template <CharType T, AllocatorType Allocator>
inline T* String<T, Allocator>::GetCString() noexcept
{
    // if _stringLength is zero, returns nullptr
    if (_stringLength == 0)
        return nullptr;

    if (_isSmallString)
        return SmallStringBuffer;
    else
        return DynamicStringBuffer;
}

template <CharType T, AllocatorType Allocator>
inline const T* String<T, Allocator>::GetCString() const noexcept
{
    // if _stringLength is zero, returns nullptr
    if (_stringLength == 0)
        return nullptr;

    if (_isSmallString)
        return SmallStringBuffer;
    else
        return DynamicStringBuffer;
}

template <CharType T, AllocatorType Allocator>
inline void String<T, Allocator>::RemoveAt(Size index,
                                           Size count)

{
    if (index + count > _stringLength || index >= _stringLength)
        throw ArgumentOutOfRangeException("`index` and `count` were out of range!");

    if (count == 0)
        return;

    auto pointer = GetCString();

    // Uses std::memmove to move the elements
    std::memmove(pointer + index, pointer + index + count, (_stringLength - index - count) * sizeof(T));

    _stringLength -= count;

    // inserts the null terminator
    pointer[_stringLength] = T(0);
}

template <CharType T, AllocatorType Allocator>
inline void String<T, Allocator>::ReserveFor(Size count)
{
    Reserve<true>(count);
}

template <CharType T, AllocatorType Allocator>
template <Bool Move>
inline T* String<T, Allocator>::Reserve(Size size)
{
    if (size <= SmallStringSize && _isSmallString)
    {
        return SmallStringBuffer;
    }
    /// Checks if the string is currently using dynamic memory allocation.
    /// If it is, and the new size is smaller than the current size, then
    /// the string is not reallocated.
    else if (!_isSmallString && size <= DynamicMemoryAllocatedSize)
    {
        return DynamicStringBuffer;
    }
    /// Once allocated dynamic memory, the string is always using dynamic
    else
    {
        auto actualDynamicMemoryAllocatedSize = (Math::RoundToNextPowerOfTwo(size + 1)) * sizeof(T);
        auto dynamicMemoryAllocatedSize       = size - 1;

        // Allocates the new memory
        auto newDynamicMemory = (T*)Allocator::Allocate(actualDynamicMemoryAllocatedSize, alignof(T));

        // Copies the old string to the new one
        if constexpr (Move)
        {
            const T* source = GetCString();
            Detail::CopyElement<true>(source, newDynamicMemory, _stringLength);
        }

        // No exception thrown, so we can deallocate the old memory
        // Deallocate the old dynamic memory
        if (!_isSmallString) Allocator::Deallocate(DynamicStringBuffer);

        _isSmallString             = false;
        DynamicMemoryAllocatedSize = dynamicMemoryAllocatedSize;
        DynamicStringBuffer        = newDynamicMemory;
        return DynamicStringBuffer;
    }
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline Bool String<T, Allocator>::operator==(const U* str) const noexcept
{
    auto stringLength = GetStringLength(str);

    if (_stringLength != stringLength)
        return false;

    auto cString = GetCString();

    for (Size i = 0; i < _stringLength; ++i)
        if (str[i] != cString[i])
            return false;

    return true;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline Bool String<T, Allocator>::operator!=(const U* str) const noexcept
{
    return !(*this == str);
}

template <CharType T, AllocatorType Allocator>
inline Bool String<T, Allocator>::operator==(NullptrType) const noexcept
{
    return _stringLength == 0;
}

template <CharType T, AllocatorType Allocator>
inline Bool String<T, Allocator>::operator!=(NullptrType) const noexcept
{
    return _stringLength != 0;
}

template <CharType T, AllocatorType Allocator>
template <CharType U, AllocatorType OtherAllocator>
inline Bool String<T, Allocator>::operator==(const String<U, OtherAllocator>& str) const noexcept
{
    auto stringLength = str._stringLength;

    if (_stringLength != stringLength)
        return false;

    auto cString      = GetCString();
    auto otherCString = str.GetCString();

    for (Size i = 0; i < _stringLength; ++i)
        if (otherCString[i] != cString[i])
            return false;

    return true;
}

template <CharType T, AllocatorType Allocator>
template <CharType U, AllocatorType OtherAllocator>
inline Bool String<T, Allocator>::operator!=(const String<U, OtherAllocator>& str) const noexcept
{
    return !(*this == str);
}

template <CharType T, AllocatorType Allocator>
inline T& String<T, Allocator>::operator[](Size index)
{
    if (index >= _stringLength)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return GetCString()[index];
}

template <CharType T, AllocatorType Allocator>
inline const T& String<T, Allocator>::operator[](Size index) const
{
    if (index >= _stringLength)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return GetCString()[index];
}

template <CharType T, AllocatorType Allocator>
inline const T* String<T, Allocator>::begin() const noexcept
{
    return GetCString();
}

template <CharType T, AllocatorType Allocator>
inline const T* String<T, Allocator>::end() const noexcept
{
    return GetCString() + _stringLength;
}


template <CharType T, AllocatorType Allocator>
inline T* String<T, Allocator>::begin() noexcept
{
    return GetCString();
}

template <CharType T, AllocatorType Allocator>
inline T* String<T, Allocator>::end() noexcept
{
    return GetCString() + _stringLength;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline String<T, Allocator>& String<T, Allocator>::operator+=(const U& character)
{
    Append(std::addressof(character), std::addressof(character) + 1);

    return *this;
}

template <CharType T, AllocatorType Allocator>
template <CharType U, AllocatorType OtherAllocator>
inline String<T, Allocator>& String<T, Allocator>::operator+=(const String<U, OtherAllocator>& string)
{
    Append(string.begin(), string.end());

    return *this;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline String<T, Allocator>& String<T, Allocator>::operator+=(const U* string)
{
    Append(string, string + String<U>::GetStringLength(string));

    return *this;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline void String<T, Allocator>::Insert(const U* begin,
                                         const U* end,
                                         Size     index)
{
    if (index > _stringLength)
        throw ArgumentOutOfRangeException("`index` was out of range!");

    if (begin > end)
        throw ArgumentOutOfRangeException("`begin` was out of range!");

    if (begin == end)
        return;

    if (index == _stringLength)
    {
        // Redirects the insertion to the append function
        Append(begin, end);
        return;
    }

    Size insertSize = end - begin;

    Size newSize = _stringLength + insertSize;

    auto useSmallString   = newSize <= SmallStringSize && _isSmallString;
    auto useDynamicString = !_isSmallString && newSize <= DynamicMemoryAllocatedSize;

    if (useSmallString || useDynamicString)
    {
        T* pointer = useSmallString ? SmallStringBuffer : DynamicStringBuffer;

        // Moves the string starting from the index to the right by the size of the inserted string
        Detail::CopyElement<false>(pointer + index, pointer + index + insertSize, _stringLength - index);

        // Copies the inserted string to the string
        Detail::CopyElement<false>(begin, pointer + index, insertSize);

        pointer[newSize] = 0;
    }
    /// Once allocated dynamic memory, the string is always using dynamic
    else
    {
        auto actualDynamicMemoryAllocatedSize = (Math::RoundToNextPowerOfTwo(newSize + 1)) * sizeof(T);
        auto dynamicMemoryAllocatedSize       = newSize - 1;

        // Allocates the new memory
        auto newDynamicMemory = (T*)Allocator::Allocate(actualDynamicMemoryAllocatedSize, alignof(T));

        // Copies the old string to the new one
        {
            const T* source = GetCString();

            Detail::CopyElement<false>(source, newDynamicMemory, index);

            Detail::CopyElement<false>(begin, newDynamicMemory + index, insertSize);

            Detail::CopyElement<false>(source + index, newDynamicMemory + index + insertSize, _stringLength - index);

            newDynamicMemory[newSize] = 0;
        }

        // No exception thrown, so we can deallocate the old memory
        // Deallocate the old dynamic memory
        if (!_isSmallString)
            Allocator::Deallocate(DynamicStringBuffer);

        _isSmallString             = false;
        DynamicMemoryAllocatedSize = dynamicMemoryAllocatedSize;
        DynamicStringBuffer        = newDynamicMemory;
    }

    _stringLength += insertSize;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline void String<T, Allocator>::Append(const U* begin,
                                         const U* end)
{
    // Checks the argument
    if (begin > end)
        throw ArgumentOutOfRangeException("`begin` was greater than `end`!");

    if (begin == end)
        return;

    // Calculates the size of the string
    Size size = end - begin;

    // Reserves the memory for the string
    auto pointer = Reserve(_stringLength + size);

    // Copies the string to the string
    Detail::CopyElement<true>(begin, pointer + _stringLength, size);

    _stringLength += size;
}

template <CharType T, AllocatorType Allocator>
template <CharType U>
inline void String<T, Allocator>::Append(const U& character)
{
    // Reserves the memory for the string
    auto pointer = Reserve(_stringLength + 1);

    // Copies the string to the string
    Detail::CopyElement<true>(std::addressof(character), pointer + _stringLength, 1);

    _stringLength += 1;
}

template <CharType T, AllocatorType Allocator>
inline Size String<T, Allocator>::GetLength() const noexcept
{
    return _stringLength;
}

template <CharType T, AllocatorType Allocator>
template <ArithmeticType U>
inline String<T, Allocator> String<T, Allocator>::ToString(const U& value)
{
    String<T, Allocator> stringToReturn = {};

    if (value == 0)
    {
        stringToReturn.Reserve(1);

        T zero = T('0');

        stringToReturn.Append(std::addressof(zero), std::addressof(zero) + 1);

        return stringToReturn;
    }

    Size stringSize  = 0;
    U    absIntegral = value;

    if constexpr (std::is_signed_v<U>)
    {
        // Includes minus sign `-`
        if (value < 0)
        {
            absIntegral = value * -1;
            stringSize++;
        }
    }

    // Gets the integer digit count
    stringSize += (Size)std::ceil(std::log10(absIntegral));

    stringToReturn.ReserveFor(stringSize);

    if constexpr (std::is_signed_v<U>)
    {
        // Includes minus sign `-`
        if (value < 0)
            stringToReturn.Append(T('-'));
    }

    Detail::AppendNumerics(stringToReturn, absIntegral);

    return stringToReturn;
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_STRINGIMPL_INL
