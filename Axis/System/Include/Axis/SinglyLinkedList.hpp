/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
#define AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
#pragma once

#include "../../Private/Axis/CoreContainer.inl"
#include "Allocator.hpp"
#include "Iterator.hpp"

namespace Axis
{

namespace System
{

namespace Detail
{

namespace SinglyLinkedList
{

/// \note This class is not intended to be used directly
template <RawType T, RawType IteratorType, Bool IteratorDebugging, RawType VoidPointerType>
struct Node : public ConditionalType<IteratorDebugging, ::Axis::System::Detail::CoreContainer::IteratorTrackingBase<IteratorType, VoidPointerType>, CoreContainer::Empty> // Singly linked list's node data structure
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

template <RawType AllocNodeTraits>
struct IteratorDebugBase
{
};

template <RawType AllocNodeTraits, Bool IteratorDebugging>
struct IteratorBase : protected ConditionalType<IteratorDebugging, IteratorDebugBase<AllocNodeTraits>, ::Axis::System::Detail::CoreContainer::Empty>
{
};

} // namespace SinglyLinkedList

} // namespace Detail

/// \brief Singly linked list container, stores the elements in the node manner, supports fast insertion and removal.
///
/// All the functions in this class are categorized as `strong exception guarantee`. Which means that
/// if an exception is thrown, the state of the object is rolled back to the state before the function.
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
    // Base iterator class
    class IteratorBase;

    // Iterator template class
    template <Bool IsConst>
    class IteratorTemplate;

    using NodeType        = Detail::SinglyLinkedList::Node<T, IteratorBase, IteratorDebugging, VoidPointerType>; // underlying node type
    using AllocNodeType   = typename AllocTraits::template Rebind<NodeType>;                                     // node type's allocator
    using AllocNodeTraits = AllocatorTraits<AllocNodeType>;                                                      // node type's allocator traits
    using NodePointerType = typename AllocNodeTraits::PointerType;                                               // node's pointer type.

    /* ==================================> Debugging <================================== */
    using IteratorTrackerNodeType        = Detial::SinglyLinkedList::IteratorTrackerNode<IteratorBase, VoidPointerType>; // Iterator tracker node type
    using AllocIteratorTrackerNodeType   = typename AllocTraits::template Rebind<IteratorTrackerNodeType>;               // Iterator tracker node's allocator
    using AllocIteratorTrackerNodeTraits = AllocatorTraits<AllocIteratorTrackerNodeType>;                                // Iterator tracker node's allocator traits
    using IteratorTrackerNodePointerType = typename AllocIteratorTrackerNodeTraits::PointerType;                         // Iterator tracker node's pointer type
    /* ==================================> Debugging <================================== */

    using PairNoDebugType = CompressedPair<NodePointerType, AllocNodeType>;                                               // Pair data for non-iterator debugging type
    using PairDebugType   = CompressedPair<CompressedPair<NodePointerType, AllocIteratorTrackerNodeType>, AllocNodeType>; // Pair data for iterator debugging type
    using PairType        = ConditionalType<IteratorDebugging, PairDebugType, PairNoDebugType>;                           // List's pair type

    // Node's pointer type check
    static_assert(IsSame<NodePointerType, typename NodeType::NodePointerType>, "Node's pointer type mismatch!");

    // base type
    using BaseType = ConditionalType<IteratorDebugging, Detail::CoreContainer::DebugIteratorContainer, Detail::CoreContainer::Empty>;

    // Checks if default constructor is nooexcept
    static constexpr Bool DefaultConstructorNoexcept = IsNoThrowDefaultConstructible<AllocNodeTraits> && (IteratorDebugging ? IsNoThrowDefaultConstructible<AllocIteratorTrackerNodeTyp> : true);

    // Checks if allocator copy constructor is noexcept
    static constexpr Bool AllocatorCopyConstructorNoexcept = IsNoThrowCopyConstructible<AllocType>;

    // Checks if move constructor is noexcept
    static constexpr Bool MoveConstructorNoexcept = IsNoThrowMoveConstructible<AllocType>;

    // Checks if move assignment is noexcept
    static constexpr Bool MoveAssignmentNoexcept = AllocTraits::IsAlwaysEqual || AllocTraits::PropagateOnContainerMoveAssignment;

public:
    /// \brief Default constructor
    SinglyLinkedList() noexcept(DefaultConstructorNoexcept);

    /// \brief Destructor
    ~SinglyLinkedList() noexcept;

    /// \brief Singly linked list's iterator
    using Iterator = IteratorTemplate<false>;

    /// \brief Singly linked list's const iterator
    using ConstIterator = IteratorTemplate<true>;

    /// \brief Emplaces the element at the front of the list
    ///
    /// \param[in] args Variadic template arguments used to construct the element.
    ///
    /// \return The iterator pointing to the inserted element.
    template <class... Args>
    Iterator EmplaceFront(Args&&... args);

private:
    // Tidies the container
    void Tidy() noexcept;

    // Gets the node pointer (for iterator debugging)
    NodePointerType& GetNodePointer() const noexcept requires(IteratorDebugging);

    // Gets the node pointer (for non-iterator debugging)
    NodePointerType& GetNodePointer() const noexcept requires(!IteratorDebugging);

    // Gets node's allocator
    AllocNodeType& GetAllocNode() const noexcept;

    // Gets the iterator to the specified node
    template <Bool IsConst>
    IteratorTemplate<IsConst> GetIterator(NodePointerType nodePointer);

    // Node's tidier
    struct NodeTidy;

    // Stores the node head of the list
    PairType _myPair = {};

    // Friend to tidy guard
    friend struct Detail::CoreContainer::TidyGuard<ThisType>;
};

} // namespace System

} // namespace Axis

#include "../../Private/Axis/SinglyLinkedListImpl.inl"

#endif // AXIS_SYSTEM_SINGLYLINKEDLIST_HPP
