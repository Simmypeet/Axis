/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_STRINGIMPL_INL
#define AXIS_SYSTEM_STRINGIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Math.hpp"
#include "../../Include/Axis/String.hpp"

namespace Axis
{

namespace Detail
{

template <CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept;

template <CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(sizeof(T) == sizeof(U))
{
    // use memcpy directly
    std::memcpy(destination, source, sourceSize * sizeof(T));

    // terminate the string
    destination[sourceSize] = U(0);
}

template <CharType T, CharType U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(sizeof(T) != sizeof(U))
{
    // Convert each element
    for (Size i = 0; i < sourceSize; ++i)
        destination[i] = U(source[i]);

    // terminate the string
    destination[sourceSize] = U(0);
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
inline String<T, Allocator>::String(const U* str) noexcept :
    _stringLength(::Axis::String<U, Allocator>::GetStringLength(str))
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement(str, pointer, _stringLength);
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::String(const String<T, Allocator>& other) noexcept :
    _stringLength(other._stringLength)
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement(other.GetCString(), pointer, _stringLength);
}

template <CharType T, AllocatorType Allocator>
inline String<T, Allocator>::String(String<T, Allocator>&& other) noexcept :
    _stringLength(other._stringLength),
    _isSmallString(other._isSmallString)
{
    if (_isSmallString)
    {
        Detail::CopyElement(other.SmallStringBuffer, other.SmallStringBuffer, _stringLength);
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
inline String<T, Allocator>::String(const String<U, OtherAllocator>& other) noexcept :
    _stringLength(other._stringLength)
{
    auto pointer = Reserve(_stringLength);

    Detail::CopyElement(other.GetCString(), pointer, _stringLength);
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

    Detail::CopyElement(other.GetCString(), pointer, other._stringLength);

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
        Detail::CopyElement(other.SmallStringBuffer, SmallStringBuffer, _stringLength);
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
        auto dynamicMemoryAllocatedSize       = actualDynamicMemoryAllocatedSize - sizeof(T);

        // Allocates the new memory
        auto newDynamicMemory = (T*)Allocator::Allocate(actualDynamicMemoryAllocatedSize, alignof(T));

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
inline Size String<T, Allocator>::GetLength() const noexcept { return _stringLength; }

} // namespace Axis

#endif // AXIS_SYSTEM_STRINGIMPL_INL