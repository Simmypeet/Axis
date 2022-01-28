/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SPANIMPL_INL
#define AXIS_SYSTEM_SPANIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Span.hpp"

namespace Axis
{

template <RawType T>
inline constexpr Span<T>::Span() noexcept :
    _begin(nullptr),
    _end(nullptr) {}

template <RawType T>
inline constexpr Span<T>::Span(NullptrType) noexcept :
    _begin(nullptr),
    _end(nullptr) {}

template <RawType T>
inline constexpr Span<T>::Span(const T* begin,
                               const T* end) :
    _begin(begin),
    _end(end)
{
    if (end > begin)
        throw InvalidArgumentException("`begin` was greater than `end`!");
}

template <RawType T>
inline Span<T>::Span(const List<T>& array) noexcept :
    _begin(array.GetData()),
    _end(array.GetData() + array.GetLength()) {}

template <RawType T>
template <Size N>
inline constexpr Span<T>::Span(const T (&array)[N]) noexcept :
    _begin(array),
    _end(array + N) {}

template <RawType T>
inline constexpr Size Span<T>::GetLength() const noexcept
{
    return (Size)(_end - _begin);
}

template <RawType T>
inline constexpr const T* Span<T>::GetData() const noexcept
{
    return _begin;
}

template <RawType T>
inline constexpr const T* Span<T>::begin() const noexcept
{
    return _begin;
}

template <RawType T>
inline constexpr const T* Span<T>::end() const noexcept
{
    return _end;
}

template <RawType T>
inline constexpr Bool Span<T>::operator==(NullptrType) const noexcept
{
    return !GetLength();
}

template <RawType T>
inline constexpr Bool Span<T>::operator!=(NullptrType) const noexcept
{
    return GetLength();
}

template <RawType T>
inline constexpr Span<T>::operator Bool() const noexcept
{
    return (Bool)_begin;
}

template <RawType T>
inline constexpr const T& Span<T>::operator[](Size index) const
{
    if (index >= GetLength())
        throw ArgumentOutOfRangeException("`index` was out of range!");

    return _begin[index];
}

} // namespace Axis

#endif // AXIS_SYSTEM_SPANIMPL_INL