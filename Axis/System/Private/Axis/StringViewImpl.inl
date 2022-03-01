/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file `LICENSE`, which is part of this source code package.

#ifndef AXIS_SYSTEM_STRINGVIEWIMPL_INL
#define AXIS_SYSTEM_STRINGVIEWIMPL_INL
#pragma once

#include "../../Include/Axis/StringView.hpp"
#include "../../Include/Axis/Utility.hpp"

namespace Axis
{

namespace System
{

template <CharType T>
inline constexpr StringView<T>::StringView(NullptrType) noexcept :
    _stringBuffer(nullptr),
    _stringLength(0) {}

template <CharType T>
inline constexpr StringView<T>::StringView(const T* string) noexcept :
    _stringBuffer(string),
    _nullTerminatedView(true) {}

template <CharType T>
inline constexpr StringView<T>::StringView(const T* string,
                                           Size     stringLength) noexcept :
    _stringBuffer(string),
    _stringLength(stringLength),
    _nullTerminatedView(false) {}

template <CharType T>
inline constexpr StringView<T>::StringView(const T* begin, const T* end) :
    _stringBuffer(begin),
    _stringLength(end - begin),
    _nullTerminatedView(false)
{
    if (begin > end)
        throw InvalidArgumentException("`begin` was greather than `end`!");
}

template <CharType T>
template <MemoryResourceType MemRes>
inline constexpr StringView<T>::StringView(const String<T, MemRes>& string) noexcept :
    _stringBuffer(string.GetCString()),
    _stringLength(string.GetSize()),
    _nullTerminatedView(true) {}

template <CharType T>
inline constexpr Size StringView<T>::GetSize() const noexcept
{
    // String length calculation is defferred for the null terminated string
    if (_nullTerminatedView && _stringLength == 0)
        _stringLength = String<T>::GetStringLength(_stringBuffer);

    return _stringLength;
}

template <CharType T>
inline constexpr const T* StringView<T>::GetCString() const noexcept
{
    return _stringBuffer;
}

template <CharType T>
inline constexpr Bool StringView<T>::IsNullOrEmpty() const noexcept
{
    return !(Bool)GetSize();
}

template <CharType T>
inline constexpr StringView<T>::operator Bool() const noexcept
{
    return GetSize();
}

template <CharType T>
inline constexpr Bool StringView<T>::operator==(NullptrType) const noexcept
{
    return _stringBuffer == nullptr;
}

template <CharType T>
inline constexpr Bool StringView<T>::operator!=(NullptrType) const noexcept
{
    return _stringBuffer != nullptr;
}

template <CharType T>
inline constexpr const T* StringView<T>::begin() const noexcept
{
    return _stringBuffer;
}

template <CharType T>
inline constexpr const T* StringView<T>::end() const noexcept
{
    return _stringBuffer + GetSize();
}

template <CharType T>
inline constexpr Bool StringView<T>::IsNullTerminated() const noexcept
{
    return _nullTerminatedView;
}

template <CharType T>
inline constexpr const T& StringView<T>::operator[](Size index) const
{
    if (index >= GetSize())
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _stringBuffer[index];
}

template <CharType T>
template <MemoryResourceType MemRes>
inline constexpr StringView<T>::operator String<T, MemRes>() const
{
    return String<T, MemRes>(_stringBuffer, _stringBuffer + GetSize());
}

} // namespace System

} // namespace Axis

template <Axis::System::CharType T, Axis::System::CharType U, Axis::System::MemoryResourceType MemRes>
Axis::System::String<T, MemRes>& operator+=(Axis::System::String<T, MemRes>& LHS, const Axis::System::StringView<U>& RHS)
{
    LHS.Insert(RHS.begin(),
               RHS.end(),
               LHS.GetSize());
    return LHS;
}

/// StringView ostream overloading.
template <class OStream, class T>
OStream& operator<<(OStream& stream, const Axis::System::StringView<T>& stringView)
{
    for (Axis::Size i = 0; i < stringView.GetSize(); i++)
    {
        stream << stringView[i];
    }

    return stream;
}

#endif // AXIS_SYSTEM_STRINGVIEWIMPL_INLs
