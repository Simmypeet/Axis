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
inline SinglyLinkedList<T, Alloc, IteratorDebugging>::SinglyLinkedList() noexcept(DefaultConstructorNoexcept) :
    _nodeAllocPair()
{
    _nodeAllocPair.GetFirst() = nullptr;
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_SINGLYLINKEDLISTIMPL_INL
