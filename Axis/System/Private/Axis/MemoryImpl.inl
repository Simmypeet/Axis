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


namespace Detail
{

// The data structure contained in the array
struct ArrayMemoryHeader
{
    Size  ElementCount;
    PVoid OriginalPtr;
};

} // namespace Detail

template <AllocatorType AllocatorType, RawType T, class... Args>
inline T* New(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) requires(std::is_constructible_v<T, Args...>)
{
    // Allocates memory of the specified size
    PVoid memoryPtr = AllocatorType::Allocate(sizeof(T), alignof(T));

    if constexpr (::std::is_nothrow_constructible_v<T, Args...>)
    {
        // Placement new construct
        new ((T*)memoryPtr) T(std::forward<Args>(args)...);
    }
    else
    {
        try
        {
            // Placement new construct
            new ((T*)memoryPtr) T(std::forward<Args>(args)...);
        }
        catch (...)
        {
            // Frees the memory
            AllocatorType::Deallocate(memoryPtr);

            // Rethrows the exception
            throw;
        }
    }

    // Returns the pointer to the allocated memory
    return (T*)memoryPtr;
}

template <AllocatorType AllocatorType, RawType T>
inline T* NewArray(Size elementCount) noexcept(std::is_nothrow_default_constructible_v<T>) requires(std::is_default_constructible_v<T>)
{
    // Calculates the padding size.
    Int64 offset = alignof(T) - 1 + sizeof(Detail::ArrayMemoryHeader);

    // Malloc'ed memory
    PVoid originalMemory = AllocatorType::Allocate(elementCount * sizeof(T) + offset, 1);

    // Calculates the aligned memory address.
    PVoid* alignedMemory = (PVoid*)(((Size)(originalMemory) + offset) & ~(alignof(T) - 1)); // Aligned block

    // Stores the size of array in the first bytes of the memory block.
    Detail::ArrayMemoryHeader* header = ((Detail::ArrayMemoryHeader*)alignedMemory) - 1;

    // Stores the original memory address before the aligned memory address.
    header->OriginalPtr  = originalMemory;
    header->ElementCount = elementCount;

    if constexpr (::std::is_nothrow_default_constructible_v<T>)
    {
        for (Size i = 0; i < elementCount; ++i)
        {
            // Placement new construct
            new ((T*)(alignedMemory + i)) T();
        }
    }
    else
    {
        Size constructedElementCount = 0;

        try
        {
            for (Size i = 0; i < elementCount; ++i)
            {
                // Placement new construct
                new ((T*)(alignedMemory) + i) T();

                // Increments the constructed element count
                ++constructedElementCount;
            }
        }
        catch (...)
        {
            // Destructs the already constructed elements
            for (Size i = 0; i < constructedElementCount; ++i)
                ((T*)(alignedMemory) + i)->~T();

            // Frees the memory
            AllocatorType::Deallocate(originalMemory);

            // Rethrows the exception
            throw;
        }
    }

    // Returns the pointer to the allocated memory
    return (T*)alignedMemory;
}

template <AllocatorType AllocatorType, RawType T>
void Delete(T* instance) noexcept
{
    // Invokes the destructor
    instance->~T();

    // Frees the memory
    AllocatorType::Deallocate(instance);
}

template <AllocatorType AllocatorType, RawType T>
void DeleteArray(T* array) noexcept
{
    PVoid rawMemoryPointer = (PVoid)array;

    // Gets the header of the array
    Detail::ArrayMemoryHeader* header = ((Detail::ArrayMemoryHeader*)rawMemoryPointer) - 1;

    // Gets the original memory pointer
    PVoid originalMemory = header->OriginalPtr;

    // Gets the element count
    Size elementCount = header->ElementCount;

    // Destructs the elements
    for (Size i = 0; i < elementCount; ++i)
        ((T*)(rawMemoryPointer) + i)->~T();

    // Frees the memory
    AllocatorType::Deallocate(originalMemory);
}

} // namespace Axis

#endif // AXIS_SYSTEM_MEMORYIMPL_INL