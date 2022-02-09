/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_HASHSETIMPL_INL
#define AXIS_SYSTEM_HASHSETIMPL_INL
#pragma once

#include "../../Include/Axis/HashSet.hpp"
#include "../../Include/Axis/Math.hpp"

namespace Axis
{

namespace System
{

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline HashSet<T, Hasher, Comparer, Allocator>::~HashSet() noexcept
{
    if (_pTable)
        ClearInternal<true>(_pTable, _capacity);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline HashSet<T, Hasher, Comparer, Allocator>::HashSet(const HashSet<T, Hasher, Comparer, Allocator>& other) requires(std::is_copy_constructible_v<T>) :
    _maxLoadFactor(other._maxLoadFactor),
    _hasher(other._hasher),
    _comparer(other._comparer)
{
    if constexpr (std::is_nothrow_copy_constructible_v<T>)
    {
        Reserve(other._capacity);

        for (Size i = 0; i < other._capacity; ++i)
        {
            Node* currentNode = other._pTable[i];

            while (currentNode)
            {
                InsertPerfectForwarding(currentNode->Data);

                currentNode = currentNode->Next;
            }
        }
    }
    else
    {
        try
        {
            Reserve(other._capacity);

            for (Size i = 0; i < other._capacity; ++i)
            {
                Node* currentNode = other._pTable[i];

                while (currentNode)
                {
                    InsertPerfectForwarding(currentNode->Value);

                    currentNode = currentNode->Next;
                }
            }
        }
        catch (...)
        {
            if (_pTable)
                ClearInternal<true>(_pTable, _capacity);

            throw;
        }
    }
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline HashSet<T, Hasher, Comparer, Allocator>::HashSet(HashSet<T, Hasher, Comparer, Allocator>&& other) noexcept :
    _maxLoadFactor(other._maxLoadFactor),
    _hasher(std::move(other._hasher)),
    _comparer(std::move(other._comparer))
{
    _pTable    = other._pTable;
    _capacity  = other._capacity;
    _nodeCount = other._nodeCount;

    other._pTable    = nullptr;
    other._capacity  = 0;
    other._nodeCount = 0;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline HashSet<T, Hasher, Comparer, Allocator>& HashSet<T, Hasher, Comparer, Allocator>::operator=(const HashSet<T, Hasher, Comparer, Allocator>& other) requires(std::is_copy_constructible_v<T>)
{
    if (this == &other)
        return *this;

    if constexpr (std::is_nothrow_copy_constructible_v<T>)
    {
        // If other node size is less than current node size, then we can simply clear the current node and copy the other node.
        if (other._nodeCount < _capacity)
        {
            ClearInternal<false>(_pTable, _capacity);

            _nodeCount = 0;

            for (Size i = 0; i < other._capacity; ++i)
            {
                Node* currentNode = other._pTable[i];

                while (currentNode)
                {
                    InsertPerfectForwarding(currentNode->Value);

                    currentNode = currentNode->Next;
                }
            }
        }
        else
        {
            ClearInternal<true>(_pTable, _capacity);

            _nodeCount = 0;
            _capacity  = 0;
            _pTable    = nullptr;

            Reserve(other._capacity);

            for (Size i = 0; i < other._capacity; ++i)
            {
                Node* currentNode = other._pTable[i];

                while (currentNode)
                {
                    InsertPerfectForwarding(currentNode->Value);

                    currentNode = currentNode->Next;
                }
            }
        }
    }
    else
    {
        Node** backupTable     = _pTable;
        Size   backupCapacity  = _capacity;
        Size   backupNodeCount = _nodeCount;

        _pTable    = nullptr;
        _capacity  = 0;
        _nodeCount = 0;

        try
        {
            Reserve(other._capacity);

            for (Size i = 0; i < other._capacity; ++i)
            {
                Node* currentNode = other._pTable[i];

                while (currentNode)
                {
                    InsertPerfectForwarding(currentNode->Value);

                    currentNode = currentNode->Next;
                }
            }
        }
        catch (...)
        {
            ClearInternal<true>(_pTable, _capacity);

            _pTable    = backupTable;
            _capacity  = backupCapacity;
            _nodeCount = backupNodeCount;

            throw;
        }

        ClearInternal<true>(backupTable, backupCapacity);
    }

    return *this;
}


template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline HashSet<T, Hasher, Comparer, Allocator>& HashSet<T, Hasher, Comparer, Allocator>::operator=(HashSet<T, Hasher, Comparer, Allocator>&& other) noexcept
{
    if (this == &other)
        return *this;

    ClearInternal<true>(_pTable, _capacity);

    _pTable    = other._pTable;
    _capacity  = other._capacity;
    _nodeCount = other._nodeCount;

    other._pTable    = nullptr;
    other._capacity  = 0;
    other._nodeCount = 0;

    return *this;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class Reference, class Pointer>
struct HashSet<T, Hasher, Comparer, Allocator>::Iterator
{
public:
    using reference  = Reference; ///< Reference type
    using pointer    = Pointer;   ///< Pointer type
    using value_type = T;         ///< Value type

    // Array operator
    pointer operator->() const noexcept { return &_currentNode->Data; }

    // Dereference operator
    reference operator*() const noexcept { return _currentNode->Data; }

    // Pre-increment operator
    Iterator& operator++() noexcept
    {
        _currentNode = _currentNode->Next;

        // If the current node is nullptr, we are at the end of the linked list chain
        // and we need to find the next non-empty bucket

        if (_currentNode == nullptr)
        {
            _tableIndex++;

            // Find the next non-empty bucket
            while (_tableIndex < _tableSize)
            {
                // If the bucket is not empty, we found the next non-empty bucket
                if (_pTable[_tableIndex] != nullptr)
                {
                    _currentNode = _pTable[_tableIndex];
                    break;
                }

                _tableIndex++;
            }
        }

        return *this;
    }

    // Post-increment operator
    Iterator operator++(int) noexcept
    {
        Iterator temp = *this;
        ++*this;
        return temp;
    }

    // Equality operator
    template <class OtherReference, class OtherPointer>
    bool operator==(const Iterator<OtherReference, OtherPointer>& other) const noexcept
    {
        return _currentNode == other._currentNode;
    }

    // Inequality operator
    template <class OtherReference, class OtherPointer>
    bool operator!=(const Iterator<OtherReference, OtherPointer>& other) const noexcept
    {
        return _currentNode != other._currentNode;
    }

private:
    // Private constructor
    Iterator(Node** pTable,
             Size   tableSize,
             Size   tableIndex,
             Node*  currentNode) noexcept :
        _pTable(pTable),
        _tableSize(tableSize),
        _tableIndex(tableIndex),
        _currentNode(currentNode) {}


    Node** _pTable      = nullptr; ///< Pointer to the table
    Size   _tableSize   = 0;       ///< Size of the table
    Size   _tableIndex  = 0;       ///< Index of the current node in the table
    Node*  _currentNode = nullptr; ///< Pointer to the current node

    friend class HashSet;

    template <class, class>
    friend struct Iterator;
};

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Size HashSet<T, Hasher, Comparer, Allocator>::GetSize() const noexcept
{
    return _nodeCount;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Float32 HashSet<T, Hasher, Comparer, Allocator>::GetCurrentLoadFactor() const noexcept
{
    return (Float32)_nodeCount / _capacity;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Float32 HashSet<T, Hasher, Comparer, Allocator>::GetMaxLoadFactor() const noexcept
{
    return _maxLoadFactor;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Bool HashSet<T, Hasher, Comparer, Allocator>::SetMaxLoadFactor(Float32 maxLoadFactor) noexcept
{
    if (maxLoadFactor <= 0.0f || maxLoadFactor > 1.0f)
    {
        return false;
    }

    _maxLoadFactor = maxLoadFactor;

    return true;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline typename HashSet<T, Hasher, Comparer, Allocator>::const_iterator HashSet<T, Hasher, Comparer, Allocator>::Find(const T& value) const noexcept
{
    return FindIndirect<T, const_iterator>(value);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Pair<Bool, typename HashSet<T, Hasher, Comparer, Allocator>::iterator> HashSet<T, Hasher, Comparer, Allocator>::Insert(const T& value) requires(std::is_copy_constructible_v<T>)
{
    return InsertPerfectForwarding<const T&>(value);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Pair<Bool, typename HashSet<T, Hasher, Comparer, Allocator>::iterator> HashSet<T, Hasher, Comparer, Allocator>::Insert(T&& value) requires(std::is_move_constructible_v<T>)
{
    return InsertPerfectForwarding<T&&>(std::move(value));
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline void HashSet<T, Hasher, Comparer, Allocator>::Reserve(Size newSize)
{
    if (newSize <= _capacity)
        return;

    newSize = Math::RoundToNextPowerOfTwo(newSize);

    // Allocate the new memory
    auto newTable = (Node**)Allocator::Allocate(newSize * Axis::PointerSize, 1);

    for (Size i = 0; i < newSize; ++i)
        newTable[i] = nullptr;

    // Iterates over the old table and copies the elements to the new table
    for (Size i = 0; i < _capacity; ++i)
    {
        Node* currentNode = _pTable[i];

        while (currentNode != nullptr)
        {
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                InsertInternal(newTable, newSize, std::move(currentNode->Data));
            else
            {
                if constexpr (std::is_nothrow_copy_constructible_v<T>)
                    InsertInternal(newTable, newSize, currentNode->Data);
                else
                {
                    try
                    {
                        InsertInternal(newTable, newSize, currentNode->Data);
                    }
                    catch (...)
                    {
                        ClearInternal<true>(newTable, newSize);

                        throw;
                    }
                }
            }
            // Move to the next node
            currentNode = currentNode->Next;
        }
    }

    if (_pTable)
        // Deallocate the old table
        ClearInternal<true>(_pTable, _capacity);

    // Update the table
    _pTable   = newTable;
    _capacity = newSize;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline Pair<Bool, typename HashSet<T, Hasher, Comparer, Allocator>::const_iterator> HashSet<T, Hasher, Comparer, Allocator>::Remove(const T& element) noexcept
{
    return RemoveIndirect<T, const_iterator>(element);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline void HashSet<T, Hasher, Comparer, Allocator>::Clear() noexcept
{
    ClearInternal<false>(_pTable, _capacity);

    _nodeCount = 0;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline typename HashSet<T, Hasher, Comparer, Allocator>::const_iterator HashSet<T, Hasher, Comparer, Allocator>::begin() const noexcept
{
    return Begin<const_iterator>();
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline typename HashSet<T, Hasher, Comparer, Allocator>::const_iterator HashSet<T, Hasher, Comparer, Allocator>::end() const noexcept
{
    return End<const_iterator>();
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline typename HashSet<T, Hasher, Comparer, Allocator>::iterator HashSet<T, Hasher, Comparer, Allocator>::NonConstBegin() noexcept
{
    return Begin<iterator>();
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
inline typename HashSet<T, Hasher, Comparer, Allocator>::iterator HashSet<T, Hasher, Comparer, Allocator>::NonConstEnd() noexcept
{
    return End<iterator>();
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class IndirectType, class IteratorVariant>
inline IteratorVariant HashSet<T, Hasher, Comparer, Allocator>::FindIndirect(const IndirectType& element) const noexcept
{
    if (_capacity == 0)
        return End<IteratorVariant>();

    // Calculate the hash
    auto hash = _hasher(element);

    // Calculate the index
    auto index = hash % _capacity;

    // Find the node
    auto node = _pTable[index];

    while (node)
    {
        if (_comparer(node->Data, element))
            return IteratorVariant(_pTable, _capacity, index, node);

        node = node->Next;
    }

    return IteratorVariant(_pTable, _capacity, index, nullptr);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class IndirectType, class IteratorVariant>
inline Pair<Bool, IteratorVariant> HashSet<T, Hasher, Comparer, Allocator>::RemoveIndirect(const IndirectType& element) noexcept
{
    if (_capacity == 0)
        return {false, End<IteratorVariant>()};

    // Calculate the hash
    auto hash = _hasher(element);

    // Calculate the index
    auto index = hash % _capacity;

    // Find the node
    auto node = _pTable[index];

    Node* previousNode = nullptr;

    while (node)
    {
        if (_comparer(node->Data, element))
        {
            // Remove the node
            if (previousNode)
                previousNode->Next = node->Next;
            else
                _pTable[index] = node->Next;

            // Decrement the node count
            --_nodeCount;

            Node* nextNode = node->Next;

            // Calls the element destructor
            node->Data.~T();

            // Deallocate the node
            Allocator::Deallocate(node);

            // Checks if the next node is nullptr and if so, looks for the next non-nullptr node
            if (nextNode == nullptr)
            {
                while (index < _capacity)
                {
                    if (_pTable[index])
                    {
                        nextNode = _pTable[index];
                        break;
                    }

                    ++index;
                }
            }

            return {true, IteratorVariant(_pTable, _capacity, index, nextNode)};
        }

        previousNode = node;
        node         = node->Next;
    }

    return {false, IteratorVariant(_pTable, _capacity, _capacity, nullptr)};
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class IteratorVariant, class... Args>
inline Pair<Bool, IteratorVariant> HashSet<T, Hasher, Comparer, Allocator>::InsertInternal(Node** pTable, Size tableSize, Args&&... args)
{
    // Calculate the hash
    auto hash = _hasher(args...);

    // Calculate the index
    auto index = hash % tableSize;

    // Find the node
    auto node = pTable[index];

    while (node)
    {
        if (_comparer(node->Data, args...))
            return {false, IteratorVariant(pTable, tableSize, index, node)};

        node = node->Next;
    }

    // Create the node
    node = (Node*)Allocator::Allocate(sizeof(Node), alignof(Node));

    if constexpr (std::is_nothrow_constructible_v<T, Args...>)
        new (&node->Data) T(std::forward<Args>(args)...);
    else
    {
        try
        {
            new (&node->Data) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            Allocator::Deallocate(node);
            throw;
        }
    }

    // Insert the node
    node->Next    = pTable[index];
    pTable[index] = node;

    return {true, IteratorVariant(pTable, tableSize, index, node)};
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class... Args>
inline Pair<Bool, typename HashSet<T, Hasher, Comparer, Allocator>::iterator> HashSet<T, Hasher, Comparer, Allocator>::InsertPerfectForwarding(Args&&... args)
{
    // Load factor after insertion
    auto newLoadFactor = (Float32)_nodeCount / _capacity;

    // Check if we need to resize
    if (newLoadFactor > _maxLoadFactor || _capacity == 0)
        Reserve(_capacity == 0 ? 8 : _capacity * 2);

    auto it = InsertInternal<iterator>(_pTable, _capacity, std::forward<Args>(args)...);

    if (it.First)
        ++_nodeCount;

    return it;
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <Bool ClearTable>
inline void HashSet<T, Hasher, Comparer, Allocator>::ClearInternal(Node** pTable, Size tableSize)
{
    for (Size i = 0; i < tableSize; ++i)
    {
        Node* currentNode = pTable[i];

        while (currentNode != nullptr)
        {
            Node* nextNode = currentNode->Next;

            if constexpr (!PodType<T>)
                currentNode->Data.~T();

            Allocator::Deallocate(currentNode);

            currentNode = nextNode;
        }

        pTable[i] = nullptr;
    }

    if constexpr (ClearTable)
    {
        if (pTable)
            Allocator::Deallocate(pTable);
    }
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class IteratorVariant>
inline IteratorVariant HashSet<T, Hasher, Comparer, Allocator>::Begin() const requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>)
{
    // If the table is empty, return an iterator to the end of the table
    if (_nodeCount == 0)
        return IteratorVariant(_pTable, _capacity, _nodeCount, nullptr);

    // Find the first node
    Size  index       = 0;
    Node* currentNode = nullptr;

    while (index < _capacity)
    {
        if (_pTable[index])
        {
            currentNode = _pTable[index];
            break;
        }

        ++index;
    }

    return IteratorVariant(_pTable, _capacity, index, currentNode);
}

template <RawType T, HasherType<T> Hasher, ComparerType<T> Comparer, AllocatorType Allocator>
template <class IteratorVariant>
inline IteratorVariant HashSet<T, Hasher, Comparer, Allocator>::End() const requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>)
{
    return IteratorVariant(_pTable, _capacity, _capacity, nullptr);
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_HASHSETIMPL_INL