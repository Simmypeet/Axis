/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \ref This code is taken from `electronicarts/EASTL https://github.com/electronicarts/EASTL/blob/master/include/EASTL/bonus/call_traits.h`.
///      The original code is licensed under the BSD-3-Clause License.

#ifndef AXIS_SYSTEM_CALLTRAITS_INL
#define AXIS_SYSTEM_CALLTRAITS_INL
#pragma once

#include "../../Include/Axis/Trait.hpp"

namespace Axis
{

namespace System
{

namespace Detail
{

template <typename T, bool small_>
struct CallTraitsImpl1
{
    typedef const T& ParamType;
};

template <typename T>
struct CallTraitsImpl1<T, true>
{
    typedef const T ParamType;
};

template <typename T, bool isp, bool b1>
struct CallTraitsImpl
{
    typedef const T& ParamType;
};

template <typename T, bool isp>
struct CallTraitsImpl<T, isp, true>
{
    typedef typename CallTraitsImpl1<T, sizeof(T) <= sizeof(void*)>::ParamType ParamType;
};

template <typename T, bool b1>
struct CallTraitsImpl<T, true, b1>
{
    typedef T const ParamType;
};

template <typename T>
struct CallTraits
{
public:
    typedef T                                                                    ValueType;
    typedef T&                                                                   ReferenceType;
    typedef const T&                                                             ConstReferenceType;
    typedef typename CallTraitsImpl<T, IsPointer<T>, IsArithmetic<T>>::ParamType ParamType;
};


template <typename T>
struct CallTraits<T&>
{
    typedef T&       ValueType;
    typedef T&       ReferenceType;
    typedef const T& ConstReferenceType;
    typedef T&       ParamType;
};


template <typename T, Size N>
struct CallTraits<T[N]>
{
private:
    typedef T array_type[N];

public:
    typedef const T*          ValueType;
    typedef array_type&       ReferenceType;
    typedef const array_type& ConstReferenceType;
    typedef const T* const    ParamType;
};


template <typename T, size_t N>
struct CallTraits<const T[N]>
{
private:
    typedef const T array_type[N];

public:
    typedef const T*          ValueType;
    typedef array_type&       ReferenceType;
    typedef const array_type& ConstReferenceType;
    typedef const T* const    ParamType;
};

} // namespace Detail

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_CALLTRAITS_INL