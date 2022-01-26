/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file LinkedList.hpp
///
/// \brief Contains \a `Axis::LinkedList` class template.

#ifndef AXIS_SYSTEM_LINKEDLIST_HPP
#define AXIS_SYSTEM_LINKEDLIST_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include "Utility.hpp"

namespace Axis
{

/// \brief Container for storing objects in a linked list data structure.
///        The linked list is implemented as a doubly linked list.
///
/// \tparam T Type of object to store in the list.
/// \tparam Allocator Type of allocator to use for memory management.
template <RawType T, AllocatorType Allocator = DefaultAllocator>
class LinkedList final
{
public:
    /// \brief Linked list's node data structure.
    ///
    /// \warning This class is not intended to be used directly.
    struct Node final
    {
        /// \brief Type of object stored in the node.
        T Value = {};

        /// \brief Pointer to the next node.
        Node* Next = nullptr;

        /// \brief Pointer to the previous node.
        Node* Previous = nullptr;
    };

    /// \brief Default constructor.
    LinkedList() noexcept;

    /// \brief Copy constructor.
    ///
    /// \param[in] other Linked list to copy.
    LinkedList(const LinkedList& other) requires(std::is_copy_constructible_v<T>);

    /// \brief Move constructor
    ///
    /// \param[in] other Linked list to move.
    LinkedList(LinkedList&& other) noexcept;

    /// \brief Destructor.
    ~LinkedList() noexcept;

    /// \brief Copy assignment operator.
    ///
    /// \param[in] other Linked list to copy.
    ///
    /// \return Reference to the linked list.
    LinkedList& operator=(const LinkedList& other) requires(std::is_copy_constructible_v<T>);

    /// \brief Move assignment operator.
    ///
    /// \param[in] other Linked list to move.
    ///
    /// \return Reference to the linked list.
    LinkedList& operator=(LinkedList&& other) noexcept;

    /// \brief Linked list's iterator.
    template <class Reference, class Pointer>
    struct Iterator;

    /// \brief Non-const iterator.
    typedef Iterator<T&, T*> iterator;

    /// \brief Const iterator.
    typedef Iterator<const T&, const T*> const_iterator;

    /// \brief Adds an element to the front of the linked list.
    ///
    /// \param[in] value Element to add to the linked list.
    ///
    /// \return Returns an iterator to the newly added element.
    iterator AddFront(const T& value) requires(std::is_copy_constructible_v<T>);

    /// \brief Adds an element to the front of the list.
    ///
    /// \param[in] value Value to add to the list.
    ///
    /// \return Returns an iterator to the newly added element.
    iterator AddFront(T&& value) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

    /// \brief Adds and constructs an element at the front of the list.
    ///
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    ///
    /// \return Returns an iterator to the newly inserted element.
    template <class... Args>
    iterator EmplaceFront(Args&&... args) requires(std::is_constructible_v<T, Args...>);

    /// \brief Adds an element to the back of the linked list.
    ///
    /// \param[in] value Element to add to the linked list.
    ///
    /// \return Returns an iterator to the newly added element.
    iterator AddBack(const T& value) requires(std::is_copy_constructible_v<T>);

    /// \brief Adds an element to the back of the list.
    ///
    /// \param[in] value Value to add to the list.
    ///
    /// \return Returns an iterator to the newly added element.
    iterator AddBack(T&& value) requires(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

    /// \brief Adds and constructs an element at the back of the list.
    ///
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    ///
    /// \return Returns an iterator to the newly inserted element.
    template <class... Args>
    iterator EmplaceBack(Args&&... args) requires(std::is_constructible_v<T, Args...>);

    /// \brief Adds and constructs an element before the specified position.
    ///
    /// \param[in] position Position to insert the element at. Passes the end() iterator to add to the end of the list.
    /// \param[in] args Arguments to forward to the constructor of the object to insert.
    ///
    /// \return Returns an iterator to the newly inserted element.
    ///         If the position is invalid or memory allocation is failed, returns an iterator to the end of the list.
    template <class IteratorVariant = iterator, class... Args>
    IteratorVariant Emplace(const IteratorVariant& position, Args&&... args) requires(std::is_constructible_v<T, Args...> && (std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>));

    /// \brief Removes the element at the specified position.
    ///
    /// \param[in] position Position to remove the element at.
    ///
    /// \return Returns an iterator to the next element.
    ///         If the position is invalid, returns an iterator to the end of the list.
    template <class IteratorVariant = iterator>
    IteratorVariant Remove(const IteratorVariant& position) noexcept requires(std::is_same_v<IteratorVariant, iterator> || std::is_same_v<IteratorVariant, const_iterator>);

    /// \brief Iterator to the beginning of the linked list.
    ///
    /// \return Returns an iterator to the beginning of the linked list.
    AXIS_NODISCARD iterator begin() noexcept;

    /// \brief Iterator to the beginning of the linked list.
    ///
    /// \return Returns an iterator to the beginning of the linked list.
    AXIS_NODISCARD const_iterator begin() const noexcept;

    /// \brief Iterator to the end of the linked list.
    ///
    /// \return Returns an iterator to the end of the linked list.
    AXIS_NODISCARD iterator end() noexcept;

    /// \brief Iterator to the end of the linked list.
    ///
    /// \return Returns an iterator to the end of the linked list.
    AXIS_NODISCARD const_iterator end() const noexcept;

private:
    void Clear(Node* nodeHead) noexcept;

    /// Private members
    Size  _size = 0;       ///< Number of elements in the list.
    Node* _head = nullptr; ///< Pointer to the first node in the list.
    Node* _tail = nullptr; ///< Pointer to the last node in the list.
};

} // namespace Axis

#include "../../Private/Axis/LinkedListImpl.inl"

#endif // AXIS_SYSTEM_LINKEDLIST_HPP