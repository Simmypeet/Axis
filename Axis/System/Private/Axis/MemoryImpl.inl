/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MEMORYIMPL_INL
#define AXIS_SYSTEM_MEMORYIMPL_INL
#pragma once

#include "../../Include/Axis/Memory.hpp"
#include <new>


namespace Axis
{

namespace System
{

namespace Detail
{

// The data structure contained in the array
struct Memory_ArrayMemoryHeader
{
    Size  ElementCount;
    PVoid OriginalPtr;
};

template <class T, MemoryResourceType MemRes>
struct Memory_TidyGuard // Calls function `Tidy()` on the target on destruction
{
    T*   Targets      = nullptr;
    Size ElementCount = 0;

    ~Memory_TidyGuard()
    {
        if (Targets)
        {
            for (Size i = ElementCount; i > 0; --i)
                Targets[i - 1].~T();

            MemRes::Deallocate(Targets);
        }
    }
};

} // namespace Detail

template <MemoryResourceType MemoryResource, RawType T, class... Args>
inline T* MemoryNew(Args&&... args) noexcept(IsNothrowConstructible<T, Args...>)
{
    Detail::Memory_TidyGuard<T, MemoryResource> guard = {(T*)MemoryResource::Allocate(sizeof(T), alignof(T)), 0};

    new (guard.Targets) T(Forward<Args>(args)...);

    auto targetCopy = guard.Targets;
    guard.Targets   = nullptr;

    // Returns the pointer to the allocated memory
    return targetCopy;
}

template <MemoryResourceType MemoryResource, RawType T, class... Args>
inline T* MemoryNewArray(Size elementCount, Args&&... args) noexcept(IsNothrowConstructible<T, Args...>)
{
    // Calculates the padding size.
    Int64 offset = alignof(T) - 1 + sizeof(Detail::Memory_ArrayMemoryHeader);

    // size of memory to allocate for the array
    auto memorySize = (elementCount * sizeof(T)) + offset;

    // Malloc'ed memory
    PVoid originalMemory = MemoryResource::Allocate(memorySize, 1);

    // Calculates the aligned memory address.
    PVoid* alignedMemory = (PVoid*)(((UintPtr)(originalMemory) + offset) & ~(alignof(T) - 1)); // Aligned block

    // Stores the size of array in the first bytes of the memory block.
    Detail::Memory_ArrayMemoryHeader* header = ((Detail::Memory_ArrayMemoryHeader*)alignedMemory) - 1;

    // Stores the original memory address before the aligned memory address.
    header->OriginalPtr  = originalMemory;
    header->ElementCount = elementCount;

    T* objectArray = (T*)alignedMemory;

    Detail::Memory_TidyGuard<T, MemoryResource> guard = {objectArray, 0};

    for (Size i = 0; i < elementCount; ++i)
    {
        new (guard.Targets + i) T(Forward<Args>(args)...);
        ++guard.ElementCount;
    }

    guard.Targets = nullptr;

    // Returns the pointer to the allocated memory
    return objectArray;
}

template <MemoryResourceType MemoryResource, RawConstableType T>
void MemoryDelete(T* instance) noexcept
{
    // Invokes the destructor
    instance->~T();

    // Frees the memory
    MemoryResource::Deallocate((PVoid) const_cast<std::remove_const_t<T*>>(instance));
}

template <MemoryResourceType MemoryResource, RawConstableType T>
void MemoryDeleteArray(T* array) noexcept
{
    PVoid rawMemoryPointer = (PVoid) const_cast<std::remove_const_t<T*>>(array);

    // Gets the header of the array
    Detail::Memory_ArrayMemoryHeader* header = ((Detail::Memory_ArrayMemoryHeader*)rawMemoryPointer) - 1;

    // Gets the original memory pointer
    PVoid originalMemory = header->OriginalPtr;

    // Gets the element count
    Size elementCount = header->ElementCount;

    // Destructs the elements
    for (Size i = elementCount; i > 0; --i)
        array[i - 1].~T();

    // Frees the memory
    MemoryResource::Deallocate(originalMemory);
}

template <RawType T, class... Args>
inline T* New(Args&&... args) noexcept(noexcept(MemoryNew<DefaultMemoryResource, T, Args...>(Forward<Args>(args)...)))
{
    return MemoryNew<DefaultMemoryResource, T, Args...>(Forward<Args>(args)...);
}

template <RawType T, class... Args>
inline T* NewArray(Size elementCount, Args&&... args) noexcept(noexcept(MemoryNewArray<DefaultMemoryResource, T, Args...>(elementCount, Forward<Args>(args)...)))
{
    return MemoryNewArray<DefaultMemoryResource, T, Args...>(elementCount, Forward<Args>(args)...);
}

template <RawConstableType T>
inline void Delete(T* instance) noexcept
{
    MemoryDelete<DefaultMemoryResource, T>(instance);
}

template <RawConstableType T>
inline void DeleteArray(T* array) noexcept
{
    MemoryDeleteArray<DefaultMemoryResource, T>(array);
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_MEMORYIMPL_INL