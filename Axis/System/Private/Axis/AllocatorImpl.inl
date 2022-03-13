/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATORIMPL_INL
#define AXIS_SYSTEM_ALLOCATORIMPL_INL
#pragma once

#include "../../Include/Axis/Allocator.hpp"
#include <limits>

namespace Axis::System
{

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
inline Allocator<T, MemoryResource>::PointerType<T> Allocator<T, MemoryResource>::Allocate(SizeType size)
{
    AXIS_VALIDATE(size <= MaxAllocationSize, "Allocation size exceeded the maximum allowed size.");

    return reinterpret_cast<PointerType<T>>(MemoryResource::Allocate(size * sizeof(ValueType)));
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
inline void Allocator<T, MemoryResource>::Deallocate(PointerType<T> pointer,
                                                     SizeType       size) noexcept
{
    MemoryResource::Deallocate(reinterpret_cast<PVoid>(pointer));
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
template <class... Args>
inline void Allocator<T, MemoryResource>::Construct(PointerType<T> pointer,
                                                    Args&&... args) noexcept(IsNothrowConstructible<T, Args...>)
{
    static_assert(IsConstructible<T, Args..>, "Couldn't construct the element with the given arguments.");

    new (pointer) T(Forward<Args>(args)...);
}

template <Concept::Pure T, Concept::MemoryResource MemoryResource>
template <class... Args>
inline void Allocator<T, MemoryResource>::Destruct(PointerType<T> pointer) noexcept
{
    static_assert(IsNothrowDestructible<T>, "The element couldn't be nothrow destructed.");

    pointer->~T();
}

} // namespace Axis::System

#endif // AXIS_SYSTEM_ALLOCATORIMPL_INL
