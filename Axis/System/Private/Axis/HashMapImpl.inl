/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_HASHMAPIMPL_INL
#define AXIS_SYSTEM_HASHMAPIMPL_INL
#pragma once

#include "../../Include/Axis/HashMap.hpp"

namespace Axis
{

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, AllocatorType Allocator>
inline typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, Allocator>::Find(const TKey& key) noexcept
{
    return HashSetBase::template FindIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, AllocatorType Allocator>
inline typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::const_iterator HashMap<TKey, TValue, Hasher, Comparer, Allocator>::Find(const TKey& key) const noexcept
{
    return HashSetBase::template FindIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::const_iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, AllocatorType Allocator>
inline Pair<Bool, typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator> HashMap<TKey, TValue, Hasher, Comparer, Allocator>::Remove(const TKey& key) noexcept
{
    return HashSetBase::template RemoveIndirect<TKey, typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator>(key);
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, AllocatorType Allocator>
inline typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, Allocator>::begin() noexcept
{
    return HashSetBase::NonConstBegin();
}

template <RawType TKey, RawType TValue, HasherType<TKey> Hasher, ComparerType<TKey> Comparer, AllocatorType Allocator>
inline typename HashMap<TKey, TValue, Hasher, Comparer, Allocator>::HashSetBase::iterator HashMap<TKey, TValue, Hasher, Comparer, Allocator>::end() noexcept
{
    return HashSetBase::NonConstEnd();
}

} // namespace Axis

#endif // AXIS_SYSTEM_HASHMAPIMPL_INL