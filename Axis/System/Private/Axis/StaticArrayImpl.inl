/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_STATICARRAYIMPL_INL
#define AXIS_SYSTEM_STATICARRAYIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/StaticArray.hpp"

namespace Axis
{

template <RawType T, Size N>
inline constexpr Size StaticArray<T, N>::GetLength() const noexcept
{
    return N;
}

template <RawType T, Size N>
inline constexpr T& StaticArray<T, N>::operator[](Size index)
{
    if (index >= N)
        throw ArgumentOutOfRangeException("`index` was out of range");

    return _Elements[index];
}

template <RawType T, Size N>
inline constexpr const T& StaticArray<T, N>::operator[](Size index) const
{
    if (index >= N)
        throw ArgumentOutOfRangeException("`index` was out of range");

    return _Elements[index];
}

template <RawType T, Size N>
inline constexpr T* StaticArray<T, N>::begin() noexcept
{
    return _Elements;
}

template <RawType T, Size N>
inline constexpr const T* StaticArray<T, N>::begin() const noexcept
{
    return _Elements;
}

template <RawType T, Size N>
inline constexpr T* StaticArray<T, N>::end() noexcept
{
    return _Elements + N;
}

template <RawType T, Size N>
inline constexpr const T* StaticArray<T, N>::end() const noexcept
{
    return _Elements + N;
}

} // namespace Axis

#endif // AXIS_SYSTEM_STATICARRAYIMPL_INL
