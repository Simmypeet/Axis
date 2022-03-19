/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_STRINGIMPL_INL
#define AXIS_SYSTEM_STRINGIMPL_INL
#pragma once

#include "../../Include/Axis/Assert.hpp"
#include "../../Include/Axis/String.hpp"

namespace Axis::System
{

namespace Detail::String
{

template <Bool NullTerminated, Concept::BasicChar T, Concept::BasicChar U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept;

template <Bool NullTerminated, Concept::BasicChar T, Concept::BasicChar U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(Concept::IsSame<T, U>)
{
    // use memcpy directly
    std::memcpy(destination, source, sourceSize * sizeof(T));

    if constexpr (NullTerminated)
        destination[sourceSize] = U(0);
}

template <Bool NullTerminated, Concept::BasicChar T, Concept::BasicChar U>
inline void CopyElement(const T* source,
                        U*       destination,
                        Size     sourceSize) noexcept requires(!Concept::IsSame<T, U>)
{
    // Convert each element
    for (Size i = 0; i < sourceSize; ++i)
        destination[i] = U(source[i]);

    if constexpr (NullTerminated)
        destination[sourceSize] = U(0);
}

} // namespace Detail::String

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
inline BasicString<T, Alloc, IteratorDebugging>::BasicString() noexcept(DefaultConstructorNoexcept) {}

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
inline BasicString<T, Alloc, IteratorDebugging>::~BasicString() noexcept
{
    if (!data.IsSmallString)
        AllocTraits::Deallocate(alloc, data.MemAllocInfo.Begin, data.MemAllocInfo.AllocatedLength + 1);

    if (!_dataAllocPair.GetFirst().IsSmallString)
        _dataAllocPair.GetFirst().MemAllocInfo.~DynamicMemoryAllocationInfo();
}

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
inline typename BasicString<T, Alloc, IteratorDebugging>::SizeType BasicString<T, Alloc, IteratorDebugging>::GetLength() const noexcept
{
    return _dataAllocPair.GetFirst().StringLength;
}

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
inline typename BasicString<T, Alloc, IteratorDebugging>::PointerType BasicString<T, Alloc, IteratorDebugging>::GetCurrentPointer() noexcept
{
    if (_dataAllocPair.GetFirst().IsSmallString)
        return PointerTraits<PointerType>::GetPointer(_dataAllocPair.GetFirst().SmallStringBuffer[0]);
    else
        return _dataAllocPair.GetFirst().MemAllocInfo.Begin;
}

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
inline void BasicString<T, Alloc, IteratorDebugging>::Tidy() noexcept
{
    if (!data.IsSmallString)
        AllocTraits::Deallocate(alloc, data.MemAllocInfo.Begin, data.MemAllocInfo.AllocatedLength + 1);
}

template <Concept::BasicChar T, template <Concept::Pure> class Alloc, Bool IteratorDebugging>
template <Bool CopyContent, Bool RoundAllocation>
inline typename BasicString<T, Alloc, IteratorDebugging>::PointerType BasicString<T, Alloc, IteratorDebugging>::ReserveMemoryFor(typename BasicString<T, Alloc, IteratorDebugging>::SizeType stringLength)
{
    auto& data  = _dataAllocPair.GetFirst();
    auto& alloc = _dataAllocPair.GetSecond();

    if (stringLength <= SmallStringSize && data.IsSmallString)
    {
        return PointerTraits<PointerType>::GetPointer(data.SmallStringBuffer[0]);
    }
    else if (!data.IsSmallString && stringLength <= data.MemAllocInfo.AllocatedLength)
    {
        return data.MemAllocInfo.Begin;
    }
    else
    {
        auto acutalAllocationLength = RoundAllocation ? Detail::CoreContainer::RoundToNextPowerOfTwo(stringLength + 1) : stringLength + 1;

        PointerType newBuffer = AllocTraits::Allocate(alloc, acutalAllocationLength);

        // Copies the old string to the new one
        if constexpr (CopyContent)
        {
            auto source = GetCurrentPointer();
            Detail::String::CopyElement<true>(AddressOf(*source), AddressOf(*newBuffer), data.StringLength);
        }

        if (!data.IsSmallString)
            AllocTraits::Deallocate(alloc, data.MemAllocInfo.Begin, data.MemAllocInfo.AllocatedLength + 1);

        data.IsSmallString                = false;
        data.MemAllocInfo.AllocatedLength = acutalAllocationLength - 1;
        data.MemAllocInfo.Begin           = newPointer;
    }
}

} // namespace Axis::System

#endif // AXIS_SYSTEM_STRINGIMPL_INL
