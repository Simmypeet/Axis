/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
#define AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
#pragma once

#include "../../Private/Axis/CoreContainer.inl"
#include "Allocator.hpp"
#include "Iterator.hpp"
#include <list>

namespace Axis
{

namespace System
{

namespace Detail
{

namespace SinglyLinkedList
{

/// \note This class is not intended to be used directly
template <RawType T, class VoidPointerType>
struct Node // Singly linked list's node data structure
{
public:
    using ThisType        = Node<T, VoidPointerType>;                            // Refers to this data structure's type
    using ValueType       = T;                                                   // Value type
    using NodePointerType = typename VoidPointerType::template Rebind<ThisType>; // Node's pointer type

    NodePointerType Next;   // Pointer to the next node
    ValueType       Object; // Data stored in thie node

    template <class... Args>
    Node(const NodePointerType& nextPtr,
         Args&&... args) noexcept(IsNothrowConstructible<T, Args...>) :
        Next(nextPtr),
        Object(::Axis::System::Forward<Args>(args)...) {}
};

} // namespace SinglyLinkedList

} // namespace Detail

/// \brief Singly linked list container, stores the elements in the node manner, supports fast insertion and removal.
template <RawType T, AllocatorType Alloc = Allocator<T, DefaultMemoryResource>, Bool IteratorDebugging = Detail::CoreContainer::DefaultIteratorDebug>
class SinglyLinkedList final : private ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>
{
public:
    using ThisType             = SinglyLinkedList<T, Alloc, IteratorDebugging>; // Type of this class
    using AllocType            = Alloc;                                         // Type of the allocator base class
    using AllocTraits          = AllocatorTraits<AllocType>;                    // Allocator traits
    using ValueType            = typename AllocTraits::ValueType;               // Value type
    using SizeType             = typename AllocTraits::SizeType;                // Size type
    using DifferenceType       = typename AllocTraits::DifferenceType;          // Difference type
    using PointerType          = typename AllocTraits::PointerType;             // Pointer type
    using ConstPointerType     = typename AllocTraits::ConstPointerType;        // Const pointer type
    using VoidPointerType      = typename AllocTraits::VoidPointerType;         // Void pointer type
    using ConstVoidPointerType = typename AllocTraits::ConstVoidPointerType;    // Const void pointer type

    // AllocatorTraits' value type must be the same as `T`
    static_assert(IsSame<ValueType, T>, "Allocator's value type mismatch");

private:
    using NodeType        = Detail::SinglyLinkedList::Node<T, VoidPointerType>; // underlying node type
    using AllocNodeType   = typename AllocTraits::template Rebind<NodeType>;    // node type's allocator
    using AllocNodeTraits = AllocatorTraits<AllocNodeType>;                     // node type's allocator traits
    using NodePointerType = typename AllocNodeTraits::PointerType;              // node's pointer type.

    // Node's pointer type check
    static_assert(IsSame<NodePointerType, typename NodeType::NodePointerType>, "Node's pointer type mismatch!");

    // base type
    using BaseType = ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>;

    // Checks if default constructor is nooexcept
    static constexpr Bool DefaultConstructorNoexcept = IsNoThrowDefaultConstructible<AllocType>;

    // Checks if allocator copy constructor is noexcept
    static constexpr Bool AllocatorCopyConstructorNoexcept = IsNoThrowCopyConstructible<AllocType>;

    // Checks if move constructor is noexcept
    static constexpr Bool MoveConstructorNoexcept = IsNoThrowMoveConstructible<AllocType>;

    // Checks if move assignment is noexcept
    static constexpr Bool MoveAssignmentNoexcept = AllocTraits::IsAlwaysEqual || AllocTraits::PropagateOnContainerMoveAssignment;

    template <Bool IsConst>
    struct IteratorTemplate;

public:
    /// \brief Default constructor
    SinglyLinkedList() noexcept(DefaultConstructorNoexcept);

    using Iterator = IteratorTemplate<false>;

    using ConstIterator = IteratorTemplate<true>;

    template <class... Args>
    Iterator EmplaceFont(Args&&... args);

private:
    CompressedPair<NodePointerType, AllocNodeType> _nodeAllocPair = {};
};
} // namespace System

} // namespace Axis

#include "../../Private/Axis/SinglyLinkedListImpl.inl"

#endif // AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
