/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_HASHMAPIMPL_INL
#define AXIS_SYSTEM_HASHMAPIMPL_INL
#pragma once

#include "../../Include/Axis/HashMap.hpp"

namespace Axis
{

namespace System
{

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, MemoryResourceType MemRes>
inline typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, MemRes>::Find(const TKey& key) noexcept
{
    return HashSetBase::template FindIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, MemoryResourceType MemRes>
inline typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::const_iterator HashMap<TKey, TValue, Hasher, Comparer, MemRes>::Find(const TKey& key) const noexcept
{
    return HashSetBase::template FindIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::const_iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, MemoryResourceType MemRes>
inline Pair<Bool, typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator> HashMap<TKey, TValue, Hasher, Comparer, MemRes>::Remove(const TKey& key) noexcept
{
    return HashSetBase::template RemoveIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, MemoryResourceType MemRes>
inline typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, MemRes>::begin() noexcept
{
    return HashSetBase::NonConstBegin();
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, MemoryResourceType MemRes>
inline typename HashMap<TKey, TValue, Hasher, Comparer, MemRes>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, MemRes>::end() noexcept
{
    return HashSetBase::NonConstEnd();
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_HASHMAPIMPL_INL