/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_MEMORYIMPL_INL
#define AXIS_SYSTEM_MEMORYIMPL_INL
#pragma once

#include "../../Include/Axis/Assert.hpp"
#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Memory.hpp"
#include <new>

namespace Axis::System
{

namespace Detail::Memory
{

template <Concept::Pure T>
struct TidyGuard // Calls function `Tidy()` on the target on destruction
{
    // Target to call `Tidy()` on (if not null)
    T* Target = nullptr;

    // Calls `Tidy()` on the target
    ~TidyGuard() noexcept
    {
        if (Target) Target->Tidy();
    }
};

template <Concept::Pure MemRes>
struct NewGuard
{
    NewGuard(PVoid ptr) noexcept :
        Ptr(ptr) {}
    PVoid       Ptr = nullptr;
    inline void Tidy() noexcept { MemRes::Deallocate(Ptr); }
};

template <Concept::Pure MemRes, Concept::Pure Type>
struct NewArrayGuard
{
    NewArrayGuard(Type* ptr) noexcept :
        Ptr(ptr) {}
    Type*       Ptr              = nullptr;
    Size        ConstructedCount = 0;
    inline void Tidy() noexcept
    {
        static_assert(IsNothrowDestructible<Type>, "Type was not nothrow destructible.");

        // Destructs the elements in reverse order
        for (Size i = 0; i < ConstructedCount; ++i)
        {
            Ptr[ConstructedCount - i - 1].~Type();
        }

        MemRes::Deallocate((PVoid)((UintPtr)Ptr - sizeof(Size)));
    }
};

} // namespace Detail::Memory

template <Concept::MemoryResource MemoryResource, Concept::Pure T, class... Args>
inline T* MemoryNew(Args&&... args)
{
    using NewGuardType = Detail::Memory::NewGuard<MemoryResource>;
    using TidyNewGuard = Detail::Memory::TidyGuard<NewGuardType>;

    auto ptr = (T*)MemoryResource::Allocate(sizeof(T));

    NewGuardType guard(static_cast<PVoid>(ptr));
    TidyNewGuard tidyGuard(AddressOf(guard));

    new (ptr) T(Forward<Args>(args)...);

    tidyGuard.Target = nullptr;

    return ptr;
}

template <Concept::MemoryResource MemoryResource, Concept::Pure T>
inline void MemoryDelete(const T* ptr) noexcept
{
    static_assert(IsNothrowDestructible<T>, "The type provided is not nothrow destructible!");

    ptr->~T();

    MemoryResource::Deallocate((PVoid)ptr);
}

template <Concept::Pure T>
inline constexpr Size GetMaxArraySize() noexcept
{
    return (std::numeric_limits<Size>::max() - sizeof(Size)) / sizeof(T);
}

template <Concept::MemoryResource MemoryResource, Concept::Pure T, class... Args>
inline T* MemoryNewArray(Size count, Args&&... args)
{
    AXIS_VALIDATE(count <= GetMaxArraySize<T>(), "The requested amount of elements exceeded maximum array size.");
    AXIS_VALIDATE(count > 0, "element count was zero.");

    using NewArrayGuardType = Detail::Memory::NewArrayGuard<MemoryResource, T>;
    using TidyNewArrayGuard = Detail::Memory::TidyGuard<NewArrayGuardType>;

    PVoid rawPtr       = (PVoid)MemoryResource::Allocate(sizeof(Size) + sizeof(T) * count);
    ((Size*)rawPtr)[0] = count;

    T* adjustedPointer = (T*)((UintPtr)rawPtr + sizeof(Size));

    NewArrayGuardType guard(adjustedPointer);
    TidyNewArrayGuard tidyGuard(AddressOf(guard));

    for (Size i = 0; i < count; ++i)
    {
        new (adjustedPointer + i) T(Forward<Args>(args)...);
        ++guard.ConstructedCount;
    }

    tidyGuard.Target = nullptr;

    return adjustedPointer;
}

template <Concept::MemoryResource MemoryResource, Concept::Pure T>
inline void MemoryDeleteArray(const T* ptr) noexcept
{
    static_assert(IsNothrowDestructible<T>, "The type provided is not nothrow destructible!");

    Size* headerPointer = (Size*)((UintPtr)ptr - sizeof(Size));
    Size  size          = *headerPointer;

    for (Size i = 0; i < size; ++i)
    {
        ptr[size - i - 1].~T();
    }

    MemoryResource::Deallocate((PVoid)headerPointer);
}

} // namespace Axis::System

#endif // AXIS_SYSTEM_MEMORYIMPL_INL
