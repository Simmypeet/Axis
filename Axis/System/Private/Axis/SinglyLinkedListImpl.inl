/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_SINGLYLINKEDLISTIMPL_INL
#define AXIS_SYSTEM_SINGLYLINKEDLISTIMPL_INL
#pragma once

#include "../../Include/Axis/SinglyLinkedList.hpp"

namespace Axis
{

namespace System
{

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
struct SinglyLinkedList<T, Alloc, IteratorDebugging>::NodeTidy
{
    AllocNodeType Allocator* = {};
    NodePointerType Node     = {};

    inline void Tidy() noexcept
    {
        if (Node)
            AllocNodeTraits::Deallocate(*Allocator, Node, 1);
    }
};

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline SinglyLinkedList<T, Alloc, IteratorDebugging>::SinglyLinkedList() noexcept(DefaultConstructorNoexcept) :
    _nodeAllocPair()
{
    GetNodePointer() = nullptr;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline SinglyLinkedList<T, Alloc, IteratorDebugging>::~SinglyLinkedList() noexcept
{
    Tidy();
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
template <class... Args>
inline typename SinglyLinkedList<T, Alloc, IteratorDebugging>::Iterator SinglyLinkedList<T, Alloc, IteratorDebugging>::EmplaceFront(Args&&... args)
{
    // Allocates new node
    auto newNodePointer = AllocNodeTraits::Allocate(GetAllocNode(), 1);

    NodeTidy nodeTidy = {AddressOf(GetAllocNode()), newNodePointer};

    Detail::CoreContainer::TidyGuard<NodeTidy> tidyGuard = {AddressOf(nodeTidy)};

    // Constructs new node
    AllocNodeTraits::Construct(GetAllocNode(), newNodePointer, GetNodePointer(), ::Axis::System::Forward<Args>(args)...);

    tidyGuard.Target = nullptr;

    // Inserts new node at head
    GetNodePointer() = newNodePointer;
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline void SinglyLinkedList<T, Alloc, IteratorDebugging>::Tidy() noexcept
{
    // Destroys all the nodes
    for (auto node = GetNodePointer(); node; node = GetNodePointer())
    {
        GetNodePointer() = GetNodePointer()->Next;
        AllocNodeTraits::Destruct(GetAllocNode(), node);
        AllocNodeTraits::Deallocate(GetAllocNode(), node, 1);
    }
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename SinglyLinkedList<T, Alloc, IteratorDebugging>::NodePointerType& SinglyLinkedList<T, Alloc, IteratorDebugging>::GetNodePointer() const noexcept requires(IteratorDebugging)
{
    return _myPair.GetFirst().GetFirst();
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename SinglyLinkedList<T, Alloc, IteratorDebugging>::NodePointerType& SinglyLinkedList<T, Alloc, IteratorDebugging>::GetNodePointer() const noexcept requires(!IteratorDebugging)
{
    return _myPair.GetFirst();
}

template <RawType T, AllocatorType Alloc, Bool IteratorDebugging>
inline typename SinglyLinkedList<T, Alloc, IteratorDebugging>::AllocNodeType& SinglyLinkedList<T, Alloc, IteratorDebugging>::GetAllocNode() const noexcept
{
    return _myPair.GetSecond();
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_SINGLYLINKEDLISTIMPL_INL
