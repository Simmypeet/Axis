/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATORIMPL_INL
#define AXIS_SYSTEM_ALLOCATORIMPL_INL
#pragma once

#include "../../Include/Axis/Allocator.hpp"
#include <limits>

namespace Axis
{

namespace System
{

template <DefaultType T>
inline constexpr typename AllocatorTraits<T>::PointerType AllocatorTraits<T>::Allocate(T&       allocator,
                                                                                       SizeType elementCount)
{
    return allocator.Allocate(elementCount);
}

template <DefaultType T>
inline constexpr void AllocatorTraits<T>::Deallocate(T&                                       allocator,
                                                     typename AllocatorTraits<T>::PointerType ptr,
                                                     SizeType                                 elementCount) noexcept
{
    allocator.Deallocate(ptr,
                         elementCount);
}


template <DefaultType T>
template <class... Args>
inline constexpr void AllocatorTraits<T>::Construct(T&                                       allocator,
                                                    typename AllocatorTraits<T>::PointerType ptr,
                                                    Args&&... args) noexcept(NoThrowConstructible<Args...>)
{
    if constexpr (Detail::AllocatorDefineConstruct<T>)
    {
        allocator.Construct(ptr, Forward<Args>(args)...);
    }
    else
    {
        (void)allocator;
        new (ptr) ValueType(Forward<Args>(args)...);
    }
}

template <DefaultType T>
inline constexpr void AllocatorTraits<T>::Destruct(T&                                       allocator,
                                                   typename AllocatorTraits<T>::PointerType ptr) noexcept
{
    if constexpr (Detail::AllocatorDefineDestruct<T>)
    {
        allocator.Destruct(ptr);
    }
    else
    {
        (void)allocator;
        ptr->~ValueType();
    }
}

template <DefaultType T>
inline constexpr T AllocatorTraits<T>::SelectOnContainerCopyConstruction(const T& allocator) noexcept(NoThrowSelectOnContainerCopyConstruction)
{
    if constexpr (Detail::AllocatorDefineSelectOnContainerCopyConstruction<T>)
    {
        return allocator.SelectOnContainerCopyConstruction();
    }
    else
    {
        return allocator;
    }
}

template <DefaultType T>
inline constexpr typename AllocatorTraits<T>::SizeType AllocatorTraits<T>::GetMaxSize(const T& allocator) noexcept
{
    if constexpr (Detail::AllocatorHasGetMaxSize<T>)
    {
        return allocator.GetMaxSize();
    }
    else
        return std::numeric_limits<SizeType>::max() / sizeof(ValueType);
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr typename Allocator<T, MemoryResource>::PointerType Allocator<T, MemoryResource>::Allocate(typename Allocator<T, MemoryResource>::SizeType elementCount)
{
    // Maximum element count
    if (elementCount > GetMaxSize())
        throw InvalidArgumentException("`elementCount` was too large for allocation");

    return (PointerType)MemoryResource::Allocate(elementCount * sizeof(T), alignof(T));
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr void Allocator<T, MemoryResource>::Deallocate(typename Allocator<T, MemoryResource>::PointerType pointer,
                                                               typename Allocator<T, MemoryResource>::SizeType    elementCount) noexcept
{
    return MemoryResource::Deallocate(pointer);
}

template <RawType T, MemoryResourceType MemoryResource>
template <class... Args>
inline constexpr void Allocator<T, MemoryResource>::Construct(typename Allocator<T, MemoryResource>::PointerType pointer,
                                                              Args&&... args) noexcept(IsNothrowConstructible<T, Args...>)
{
    new (pointer) T(Forward<Args>(args)...);
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr void Allocator<T, MemoryResource>::Destruct(typename Allocator<T, MemoryResource>::PointerType pointer) noexcept
{
    pointer->~T();
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr typename Allocator<T, MemoryResource>::SizeType Allocator<T, MemoryResource>::GetMaxSize() const noexcept
{
    constexpr SizeType MaximumElementCount = std::numeric_limits<SizeType>::max() / sizeof(T);
    constexpr SizeType MaximumDifference   = std::numeric_limits<DifferenceType>::max();

    return MaximumElementCount < MaximumDifference ? MaximumElementCount : MaximumDifference;
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr bool Allocator<T, MemoryResource>::operator==(const Allocator<T, MemoryResource>&) const noexcept
{
    return true;
}

template <RawType T, MemoryResourceType MemoryResource>
inline constexpr bool Allocator<T, MemoryResource>::operator!=(const Allocator<T, MemoryResource>&) const noexcept
{
    return false;
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_ALLOCATORIMPL_INL