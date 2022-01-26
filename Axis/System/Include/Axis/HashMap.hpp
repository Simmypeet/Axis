/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file HashMap.hpp
///
/// \brief Contains \a `Axis::HashMap` class template.

#ifndef AXIS_SYSTEM_HASHMAP_HPP
#define AXIS_SYSTEM_HASHMAP_HPP
#pragma once

#include "HashSet.hpp"

namespace Axis
{

namespace Detail
{

/// \brief Hasher for key-value paired object types. Hash is based
///        on the key's hash code.
///
/// \private
template <RawType TKey, RawType TValue, HasherType<TKey> Hasher>
struct HashMapHasher
{
    /// \brief Calculates hash for only `key` object.
    AXIS_NODISCARD Size operator()(const Pair<const TKey, TValue>& pair) const noexcept { return Hash(pair.First); }

    /// \brief Calculates hash for only `key` object.
    AXIS_NODISCARD Size operator()(const TKey& key) const noexcept { return Hash(key); }

    Hasher Hash = {}; ///< Hasher for key object.
};

/// \brief Comparer for key-value paired object types. Equality is based
///        on the key's hash code.
///
/// \private
template <RawType TKey, RawType TValue, ComparerType<TKey> Comparer>
struct HashMapComparer
{
public:
    /// \brief Compares equality for only `key` object
    AXIS_NODISCARD Bool operator()(const Pair<const TKey, TValue>& LHS,
                                   const Pair<const TKey, TValue>& RHS) const noexcept { return Compare(LHS.First, RHS.First); }

    /// \brief Compares equality for only `key` object
    AXIS_NODISCARD Bool operator()(const Pair<const TKey, TValue>& pair,
                                   const TKey&                     key) const noexcept { return Compare(pair.First, key); }

    Comparer Compare = {}; ///< Comparer for key object.
};


} // namespace Detail

/// \brief Key-value paired object container for fast lookup using
///        hash code.
///
/// \tparam TKey Type of key object.
/// \tparam TValue Type of value object.
/// \tparam Hasher Type of hasher object.
/// \tparam Comparer Type of comparer object.
template <RawType TKey, RawType TValue, HasherType<TKey> Hasher = Hash<TKey>, ComparerType<TKey> Comparer = EqualityComparer<TKey>, AllocatorType Allocator = DefaultAllocator>
class HashMap final : private HashSet<Pair<const TKey, TValue>, Detail::HashMapHasher<TKey, TValue, Hasher>, Detail::HashMapComparer<TKey, TValue, Comparer>, Allocator>
{
public:
    /// \brief Internal hash table type
    using HashSetBase = HashSet<Pair<const TKey, TValue>, Detail::HashMapHasher<TKey, TValue, Hasher>, Detail::HashMapComparer<TKey, TValue, Comparer>, Allocator>;

    /// \brief Default constructor
    HashMap() = default;

    /// \brief Copy constructor
    HashMap(const HashMap& other) = default;

    /// \brief Move constructor
    HashMap(HashMap&& other) = default;

    /// \brief Finds the key-value pair in the hash map by the given key.
    ///
    /// \param key Key object to find.
    ///
    /// \return Iterator to the key-value pair in the hash map if found, otherwise `end()`.
    typename HashSetBase::iterator Find(const TKey& key) noexcept;

    /// \brief Finds the key-value pair in the hash map by the given key.
    ///
    /// \param key Key object to find.
    ///
    /// \return Iterator to the key-value pair in the hash map if found, otherwise `end()`.
    typename HashSetBase::const_iterator Find(const TKey& key) const noexcept;

    /// \brief Removes the key-value pair in the hash map by the given key.
    ///
    /// \param key Key object to remove.
    ///
    /// \return Returns a paired object containing `Bool` value indicating whether the key-value pair was removed or not.
    ///         The first element of the pair is `True` if the key-value pair was removed, otherwise `False`. The second
    ///         element of the pair is the iterator to the next key-value pair in the hash map.
    Pair<Bool, typename HashSetBase::iterator> Remove(const TKey& key) noexcept;

    /// \brief Non-const iterator to the first key-value pair in the hash map.
    typename HashSetBase::iterator begin() noexcept;

    /// \brief Const iterator to the first key-value pair in the hash map.
    typename HashSetBase::iterator end() noexcept;

    using HashSetBase::operator=;            ///< Inherits base class assignment operators.
    using HashSetBase::begin;                ///< Inherits base class iterators.
    using HashSetBase::end;                  ///< Inherits base class iterators.
    using HashSetBase::GetSize;              ///< Inherits base class size getter.
    using HashSetBase::GetCurrentLoadFactor; ///< Inherits base class load factor getter.
    using HashSetBase::GetMaxLoadFactor;     ///< Inherits base class max load factor getter.
    using HashSetBase::SetMaxLoadFactor;     ///< Inherits base class max load factor setter.
    using HashSetBase::Insert;               ///< Inherits base class insert function.
    using HashSetBase::Reserve;              ///< Inherits base class reserve function.
};

} // namespace Axis

#include "../../Private/Axis/HashMapImpl.inl"

#endif // AXIS_SYSTEM_HASHMAP_HPP