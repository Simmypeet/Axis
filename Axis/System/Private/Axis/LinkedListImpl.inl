/////////////////////////////////////////////////////////////////
/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE.txt', which is part of this source code package.
///
/////////////////////////////////////////////////////////////////

#ifndef AXIS_SYSTEM_LINKEDLISTIMPL_INL
#define AXIS_SYSTEM_LINKEDLISTIMPL_INL
#pragma once

#include "../../Include/Axis/LinkedList.hpp"

namespace Axis
{

// Default constructor
template <RawType T, AllocatorType Allocator>
inline LinkedList<T, Allocator>::LinkedList() noexcept {}

template <RawType T, AllocatorType Allocator>
inline LinkedList<T, Allocator>::~LinkedList() noexcept
{
    Clear(_head);
}

template <RawType T, AllocatorType Allocator>
inline LinkedList<T, Allocator>::LinkedList(const LinkedList<T, Allocator>& other) requires(std::is_copy_constructible_v<T>)
{
    if (other._size == 0)
        return;

    if constexpr (std::is_nothrow_copy_constructible_v<T>)
    {
        for (const Node* node = other._head; node != nullptr; node = node->Next)
        {
            Emplace<iterator, const T&>(iterator(nullptr), node->Value);
        }
    }
    else
    {
        try
        {
            for (const Node* node = other._head; node != nullptr; node = node->Next)
            {
                Emplace<iterator, const T&>(iterator(nullptr), node->Value);
            }
        }
        catch (...)
        {
            Clear(_head);
        }
    }
}

template <RawType T, AllocatorType Allocator>
inline LinkedList<T, Allocator>::LinkedList(LinkedList<T, Allocator>&& other) noexcept :
    _size(other._size),
    _head(other._head),
    _tail(other._tail)
{
    other._size = 0;
    other._head = nullptr;
    other._tail = nullptr;
}

template <RawType T, AllocatorType Allocator>
LinkedList<T, Allocator>& LinkedList<T, Allocator>::operator=(const LinkedList<T, Allocator>& other) requires(std::is_copy_constructible_v<T>)
{
    if (this == &other)
        return *this;

    if (other._size == 0)
    {
        Clear(_head);
        return *this;
    }

    // Uses the old linked list chain to avoid the reallocation if the copy constructor or copy assignment operator of the contained type is nothrow.
    if constexpr (std::is_nothrow_copy_constructible_v<T>)
    {
        // Iterates through the old linked list chain and invokes the copy constructor or copy assignment operator of the contained type.
        Node* thisNode       = _head;
        Node* otherNode      = other._head;
        bool  thisNodeLarger = _size > other._size;

        while (thisNode != nullptr && otherNode != nullptr)
        {
            if constexpr (std::is_nothrow_copy_assignable_v<T>)
            {
                // Uses the copy assignment operator of the contained type.
                thisNode->Value = otherNode->Value;
            }
            else
            {
                // Uses the copy constructor of the contained type.

                // Invokes the destructor first
                thisNode->Value.~T();

                // Invokes the copy constructor
                new (&thisNode->Value) T(otherNode->Value);
            }

            thisNode  = thisNode->Next;
            otherNode = otherNode->Next;
        }

        // If the old linked list chain is larger than the new one, the remaining nodes in the old linked list chain are destroyed.
        if (thisNodeLarger)
        {
            // Assigns tail node
            _tail = thisNode->Previous;

            while (thisNode != nullptr)
            {
                thisNode->Value.~T();

                auto next = thisNode->Next;

                // Frees the memory
                Allocator::Deallocate(thisNode);

                thisNode = next;
            }

            // Tail's next node is nullptr
            _tail->Next = nullptr;
        }
        else
        {
            // If the old linked list chain is smaller than the new one, we need to create new nodes for the remaining nodes in the new linked list chain.
            while (otherNode != nullptr)
            {
                // Creates a new node
                Node* newNode = (Node*)Allocator::Allocate(sizeof(Node), alignof(Node));

                // Invokes copy constructor
                new (&newNode->Value) T(otherNode->Value);

                // Inserts the new node to the end of the linked list chain
                if (_tail == nullptr)
                {
                    _head = newNode;
                    _tail = newNode;

                    newNode->Next     = nullptr;
                    newNode->Previous = nullptr;
                }
                else
                {
                    _tail->Next       = newNode;
                    newNode->Next     = nullptr;
                    newNode->Previous = _tail;
                    _tail             = newNode;
                }

                otherNode = otherNode->Next;
            }
        }

        _size = other._size;
    }
    else
    {
        // Both copy constructor and copy assignment operator of the contained type are throwable
        // We need to create new node to provide strong exception guarantee.

        // Creates the backups for every variables
        auto backupSize = _size;
        auto backupHead = _head;
        auto backupTail = _tail;

        _size = 0;
        _head = nullptr;
        _tail = nullptr;

        try
        {
            for (const Node* node = other._head; node != nullptr; node = node->Next)
            {
                Emplace<iterator, const T&>(iterator(nullptr), node->Value);
            }
        }
        catch (...)
        {
            // Restores the backups
            _size = backupSize;
            _head = backupHead;
            _tail = backupTail;

            // Destroys the new nodes
            Clear(_head);

            // Re-throws the exception
            throw;
        }

        // Frees the backups
        Clear(backupHead);
    }

    return *this;
}

template <RawType T, AllocatorType Allocator>
LinkedList<T, Allocator>& LinkedList<T, Allocator>::operator=(LinkedList<T, Allocator>&& other) noexcept
{
    if (this == &other)
        return *this;

    // Clears the current list.
    Clear(_head);

    // Copies all variables.
    _size = other._size;
    _head = other._head;
    _tail = other._tail;

    // Clears the other list.
    other._size = 0;
    other._head = nullptr;
    other._tail = nullptr;

    return *this;
}

template <RawType T, AllocatorType Allocator>
template <class Reference, class Pointer>
struct LinkedList<T, Allocator>::Iterator
{
public:
    using value_type = T;         ///< Type of object stored in the node.
    using pointer    = Pointer;   ///< Pointer type of the object stored in the node.
    using reference  = Reference; ///< Reference type of the object stored in the node.

    /// \brief Arrow operator to access the object stored in the node.
    inline pointer operator->() const noexcept { return &(_node->Value); }

    /// \brief Dereference operator to access the object stored in the node.
    inline reference operator*() const noexcept { return _node->Value; }

    /// \brief Pre-increment operator.
    inline Iterator& operator++() noexcept
    {
        // Checks if the node is the last node. If so, assigns _node to nullptr.
        if (_node->Next == nullptr)
            _node = nullptr;
        else
            _node = _node->Next;

        return *this;
    }

    /// \brief Post-increment operator.
    inline Iterator operator++(int) noexcept
    {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }

    /// \brief Pre-decrement operator.
    inline Iterator& operator--() noexcept
    {
        // Checks if the node is the first node. If so, do nothing.
        if (_node->Previous != nullptr)
            _node = _node->Previous;

        return *this;
    }

    /// \brief Post-decrement operator.
    inline Iterator operator--(int) noexcept
    {
        Iterator temp = *this;
        --(*this);
        return temp;
    }

    /// \brief Equality operator.
    template <class OtherPointer, class OtherReference>
    inline bool operator==(const Iterator<OtherPointer, OtherReference>& other) const noexcept { return _node == other._node; }

    /// \brief Inequality operator.
    template <class OtherPointer, class OtherReference>
    inline bool operator!=(const Iterator<OtherPointer, OtherReference>& other) const noexcept { return _node != other._node; }

private:
    Iterator(Node* node) noexcept :
        _node(node) {}

    LinkedList::Node* _node = nullptr; ///< Pointer to the current node.

    // Friend class to allow access to the private constructor.
    friend LinkedList;

    template <class, class>
    friend struct Iterator;
};

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::AddFront(const T& value) requires(std::is_copy_constructible_v<T>)
{
    return Emplace(iterator(_head), value);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::AddFront(T&& value) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>)
{
    return Emplace(iterator(_head), std::move(value));
}

template <RawType T, AllocatorType Allocator>
template <class... Args>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::EmplaceFront(Args&&... args) requires(std::is_constructible_v<T, Args...>)
{
    return Emplace(iterator(_head), std::forward<Args>(args)...);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::AddBack(const T& value) requires(std::is_copy_constructible_v<T>)
{
    return Emplace(iterator(nullptr), value);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::AddBack(T&& value) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>)
{
    return Emplace(iterator(nullptr), std::move(value));
}

template <RawType T, AllocatorType Allocator>
template <class... Args>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::EmplaceBack(Args&&... args) requires(std::is_constructible_v<T, Args...>)
{
    return Emplace(iterator(nullptr), std::forward<Args>(args)...);
}

template <RawType T, AllocatorType Allocator>
template <class IteratorVariant, class... Args>
inline IteratorVariant LinkedList<T, Allocator>::Emplace(const IteratorVariant& position, Args&&... args) requires(std::is_constructible_v<T, Args...> && (std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>))
{
    // Allocates a new node.
    Node* node = (Node*)Allocator::Allocate(sizeof(Node), alignof(Node));

    // Checks if the allocation failed.
    if (node == nullptr)
    {
        // Returns an iterator to the end of the list.
        return IteratorVariant(nullptr);
    }

    if constexpr (std::is_nothrow_constructible_v<T, Args...>)
    {
        // Constructs the node's value.
        new (&node->Value) T(std::forward<Args>(args)...);
    }
    else
    {
        // Attempts to construct the node's value.
        try
        {
            // Constructs the node's value.
            new (&node->Value) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            // Deallocates the node.
            Allocator::Deallocate(node);

            // Throws the exception.
            throw;
        }
    }

    // This is the first node in the list.
    if (_head == nullptr)
    {
        // Sets the head and tail to the new node.
        _head = node;
        _tail = node;

        _head->Next     = nullptr;
        _head->Previous = nullptr;
    }
    // Inserts the node at the end
    else if (position._node == nullptr)
    {
        // Sets the node's previous to the tail.
        node->Previous = _tail;

        // Sets the node's next to nullptr.
        node->Next = nullptr;

        // Sets the tail's next to the node.
        _tail->Next = node;

        // Sets the tail to the node.
        _tail = node;
    }
    else
    {
        // Sets the position's previous node's next to the node.
        position._node->Previous->Next = node;

        // Sets the node's previous to the position's previous node.
        node->Previous = position._node->Previous;

        // Sets the node's next to the position.
        node->Next = position._node;

        // Sets the position's previous node to the node.
        position._node->Previous = node;

        // Checks if the new node's previous is nullptr.
        if (node->Previous == nullptr)
            // Sets the head to the new node.
            _head = node;
    }

    // Increments the size.
    ++_size;

    return IteratorVariant(node);
}

template <RawType T, AllocatorType Allocator>
template <class IteratorVariant>
inline IteratorVariant LinkedList<T, Allocator>::Remove(const IteratorVariant& position) noexcept requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>)
{
    if (position._node == nullptr)
        return IteratorVariant(nullptr);

    // Checks if the node is the head.
    if (position._node == _head)
    {
        // Sets the head to the node's next.
        _head = position._node->Next;

        // Checks if the node is the tail.
        if (position._node == _tail)
            // Sets the tail to nullptr.
            _tail = nullptr;
    }

    // Checks if the node is the tail.
    if (position._node == _tail)
    {
        // Sets the tail to the node's previous.
        _tail = position._node->Previous;

        // Checks if the node is the head.
        if (position._node == _head)
            // Sets the head to nullptr.
            _head = nullptr;
    }

    // Checks if the node has a previous node.
    if (position._node->Previous != nullptr)
        // Sets the node's previous node's next to the node's next.
        position._node->Previous->Next = position._node->Next;

    // Checks if the node has a next node.
    if (position._node->Next != nullptr)
        // Sets the node's next node's previous to the node's previous.
        position._node->Next->Previous = position._node->Previous;

    // Destroys the node's value.
    position._node->Value.~T();

    // Deallocates the node.
    Allocator::Deallocate(position._node);

    // Decrements the size.
    --_size;

    return IteratorVariant(position._node->Next);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::begin() noexcept
{
    return iterator(_head);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::const_iterator LinkedList<T, Allocator>::begin() const noexcept
{
    return const_iterator(_head);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::iterator LinkedList<T, Allocator>::end() noexcept
{
    return iterator(nullptr);
}

template <RawType T, AllocatorType Allocator>
inline typename LinkedList<T, Allocator>::const_iterator LinkedList<T, Allocator>::end() const noexcept
{
    return const_iterator(nullptr);
}

template <RawType T, AllocatorType Allocator>
inline void LinkedList<T, Allocator>::Clear(Node* nodeHead) noexcept
{
    // Iterates though the chain of nodes.
    while (nodeHead != nullptr)
    {
        // Stores the next node.
        auto next = nodeHead->Next;

        // Destroys the node's value.
        nodeHead->Value.~T();

        // Deallocates the node.
        Allocator::Deallocate(nodeHead);

        // Sets the node to the next node.
        nodeHead = next;
    }
}

} // namespace Axis

#endif // AXIS_SYSTEM_LINKEDLISTIMPL_INL