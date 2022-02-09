/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_HASHSET_HPP
#define AXIS_SYSTEM_HASHSET_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include "Utility.hpp"

namespace Axis
{

namespace System
{

/// \brief Functor object for calculating hash.
///
/// \tparam T Type of object to calculate hash
///
/// The library provides hash calculation for basic integer types and pointers.
///
/// To provide custom hash object, the class should provides `operator(const T&)` function
/// which returns \a `Size` as result (hash).
template <RawType T>
struct Hash final
{
    /// \brief Calculates hash for the object.
    ///
    /// This function should be pure function, which means the result should be always
    /// the same if the inputs are always the same.
    ///
    /// \param[in] object Object to calculate hash value.
    ///
    /// \return Hash value
    AXIS_NODISCARD Size operator()(const T& object) const noexcept { return (Size)object; }
};

/// \brief Functor object for comparing object equality.
///
/// \tparam T Type of object to calculate hash
///
/// This class uses any corresponded `operator==` as equality comparison.
template <RawType T>
struct EqualityComparer final
{
    /// \brief Compares two objects.
    ///
    /// \param[in] LHS Left-hand side object in the `operator==`;
    /// \param[in] RHS Right-hand side object in the `operator==`;
    ///
    /// \return Returns the value returned from comparing two object with `operator==`;
    AXIS_NODISCARD Bool operator()(const T& LHS, const T& RHS) const noexcept { return LHS == RHS; }
};

/// \brief Functor object for calculating object hash.
template <class T, class U>
concept HasherType = requires(const T& hasher, const U& object)
{
    {
        hasher(object)
        } -> SameAs<Size>;
}
&&std::is_nothrow_default_constructible_v<T>&& std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>&& std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_destructible_v<T>;

/// \brief Functor object for comparing two objects equality.
template <class T, class U>
concept ComparerType = requires(const T& comparer, const U& object)
{
    {
        comparer(object, object)
        } -> SameAs<Bool>;
}
&&std::is_nothrow_default_constructible_v<T>&& std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_copy_assignable_v<T>&& std::is_nothrow_move_assignable_v<T>&& std::is_nothrow_destructible_v<T>;

/// \brief Hash table template container implemented in separate chaining method.
///
/// \a `HashSet` data structure enables to access elements in the container in almost constant time
/// ( average time complexity O(LogN) )
///
/// \tparam T Data type to be contained within the nodes.
/// \tparam Hasher Functor object used for hash calculation from the \a `T` object.
/// \tparam Comparer Functor object used for comparing \a `T` object equality.
/// \tparam Allocator Memory allocator for all dynamic memory allocation in the hash table.
template <RawType T, HasherType<T> Hasher = Hash<T>, ComparerType<T> Comparer = EqualityComparer<T>, AllocatorType Allocator = DefaultAllocator>
class HashSet
{
public:
    /// \brief Default constructor
    HashSet() noexcept = default;

    /// \brief Destructor
    ~HashSet() noexcept;

    /// \brief Copy constructor
    ///
    /// \param[in] other Other object to copy from.
    HashSet(const HashSet&) requires(std::is_copy_constructible_v<T>);

    /// \brief Move constructor
    ///
    /// \param[in] other Other object to move from.
    HashSet(HashSet&&) noexcept;

    /// \brief Copy assignment operator
    ///
    /// \param[in] other Other object to copy from.
    HashSet& operator=(const HashSet& other) requires(std::is_copy_constructible_v<T>);

    /// \brief Move assignment operator
    ///
    /// \param[in] other Other object to move from.
    HashSet& operator=(HashSet&& other) noexcept;

    /// \brief Hash set container iterator
    template <class Reference, class Pointer>
    struct Iterator;

    /// \brief Non-const iterator
    typedef Iterator<T&, T*> iterator;

    /// \brief Const iterator
    typedef Iterator<const T&, const T*> const_iterator;

    /// \brief Gets the number of elements in the hash table.
    ///
    /// \return Number of elements in the hash table.
    AXIS_NODISCARD Size GetSize() const noexcept;

    /// \brief Gets the current load factor of the hash table.
    ///
    /// \return Current load factor of the hash table.
    AXIS_NODISCARD Float32 GetCurrentLoadFactor() const noexcept;

    /// \brief Gets the current max load factor.
    ///
    /// Max load factor indicates the maximum ratio of elements in the container to the container's capacity.
    /// The container is considered full when the ratio of elements to capacity is equal or greater than the max load factor.
    ///
    /// - The default value is 0.75.
    /// - If current load factor is greater than max load factor, the container will be resized.
    AXIS_NODISCARD Float32 GetMaxLoadFactor() const noexcept;

    /// \brief Sets the current max load factor.
    ///
    /// Max load factor indicates the maximum ratio of elements in the container to the container's capacity.
    /// The container is considered full when the ratio of elements to capacity is equal or greater than the max load factor.
    ///
    /// - The default value is 0.75.
    /// - If current load factor is greater than max load factor, the container will be resized.
    ///
    /// \param[in] maxLoadFactor New max load factor.
    ///
    /// \pre maxLoadFactor Should be greater than `0.0` and less than or equal `1.0`.
    Bool SetMaxLoadFactor(Float32 maxLoadFactor) noexcept;

    /// \brief Finds the specified element in the hash table.
    ///
    /// \return Iterator to the found element or `end()` if the element is not found.
    const_iterator Find(const T& element) const noexcept;

    /// \brief Inserts new element into the hash set. If the element is already present, the function does nothing.
    ///
    /// \param[in] value Element to be inserted.
    ///
    /// \return Returns a pair of iterator and \a `Bool` value. The iterator points to the inserted element.
    ///         If the element is already present, the iterator points to the existing element and the \a `Bool` value is \a `false`.
    Pair<Bool, iterator> Insert(const T& value) requires(std::is_copy_constructible_v<T>);

    /// \brief Inserts new element into the hash set. If the element is already present, the function does nothing.
    ///
    /// \param[in] value Element to be inserted.
    ///
    /// \return Returns a pair of iterator and \a `Bool` value. The iterator points to the inserted element.
    ///         If the element is already present, the iterator points to the existing element and the \a `Bool` value is \a `false`.
    Pair<Bool, iterator> Insert(T&& value) requires(std::is_move_constructible_v<T>);

    /// \brief Reserves memory space for the specified number of elements.
    ///
    /// \param[in] elementSize Number of elements to be reserved.
    void Reserve(Size elementSize);

    /// \brief Removes the specified element from the hash set.
    ///
    /// \param[in] value Element to be removed.
    ///
    /// \return Returns pair of iterator and \a `Bool` value. The iterator points to the next element.
    ///         If the element is not found, the iterator points to the end() and the \a `Bool` value is \a `false`.
    Pair<Bool, const_iterator> Remove(const T& element) noexcept;

    /// \brief Clears all elements from the hash set.
    void Clear() noexcept;

    /// \brief Iterator to the first element in the hash set.
    ///
    /// \return Iterator to the first element in the hash set.
    AXIS_NODISCARD const_iterator begin() const noexcept;

    /// \brief Iterator to the end of the hash set.
    ///
    /// \return Iterator to the end of the hash set.
    AXIS_NODISCARD const_iterator end() const noexcept;

    /// \brief Default hash table capacity.
    static constexpr Size DefaultCapacity = 3;

protected:
    /// \brief Node type for the hash set.
    struct Node
    {
        /// \brief Represents the data stored in the node.
        T Data = {};

        /// \brief Represents the next node in the chain.
        Node* Next = nullptr;
    };

    /// \brief Finds the specified element in the hash table.
    ///
    /// Can pass another type into this function to find the element. Ensures that the hasher can
    /// calculates hash for the specified type.
    /// Equality comparer must provides the same functionality for the specified type.
    ///
    /// \return Iterator to the found element or `end()` if the element is not found.
    template <class IndirectType, class IteratorVariant>
    IteratorVariant FindIndirect(const IndirectType& element) const noexcept;

    /// \brief Removes the specified element from the hash set.
    ///
    /// Can pass another type into this function to find the element. Ensures that the hasher can
    /// calculates hash for the specified type.
    /// Equality comparer must provides the same functionality for the specified type.
    ///
    /// \return Iterator to the found element or `end()` if the element is not found.
    template <class IndirectType, class IteratorVariant>
    Pair<Bool, IteratorVariant> RemoveIndirect(const IndirectType& element) noexcept;

    /// \brief Iterator to the first element in the hash set.
    ///
    /// \return Iterator to the first element in the hash set.
    AXIS_NODISCARD iterator NonConstBegin() noexcept;

    /// \brief Iterator to the end of the hash set.
    ///
    /// \return Iterator to the end of the hash set.
    AXIS_NODISCARD iterator NonConstEnd() noexcept;

private:
    template <class IteratorVariant = const_iterator, class... Args> Pair<Bool, IteratorVariant> InsertInternal(Node** pTable, Size tableSize, Args&&... args);
    template <class... Args> Pair<Bool, iterator>                                                InsertPerfectForwarding(Args&&... args);
    template <Bool ClearTable> void                                                              ClearInternal(Node** pTable, Size tableSize);
    template <class IteratorVariant> Pair<Bool, IteratorVariant>                                 RemoveInternal(const T& element) requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>);
    template <class IteratorVariant> IteratorVariant                                             Begin() const requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>);
    template <class IteratorVariant> IteratorVariant                                             End() const requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>);

    Node**   _pTable        = nullptr;
    Size     _capacity      = 0;
    Size     _nodeCount     = 0;
    Float32  _maxLoadFactor = 1.0f;
    Hasher   _hasher        = {};
    Comparer _comparer      = {};
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/HashSetImpl.inl"

#endif // AXIS_SYSTEM_HASHSET_HPP