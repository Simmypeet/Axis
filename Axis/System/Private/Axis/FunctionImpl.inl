/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_FUNCTIONIMPL_INL
#define AXIS_SYSTEM_FUNCTIONIMPL_INL
#pragma once

#include "../../Include/Axis/Exception.hpp"
#include "../../Include/Axis/Function.hpp"

namespace Axis
{

namespace System
{

namespace Private
{

template <class Functor, class ReturnType, class... Args>
inline constexpr ReturnType InvokeFunction(Functor* functor, Args&&... args)
{
    return (*functor)(std::forward<Args>(args)...);
}

template <class Functor>
inline constexpr void DestructFunction(Functor* functor)
{
    functor->~Functor();
}

template <class Functor>
inline constexpr void CopyConstructFunction(Functor* dest, const Functor* source)
{
    new (dest) Functor(*source);
}

template <class Functor>
inline constexpr void MoveConstructFunction(Functor* dest, Functor* source)
{
    new (dest) Functor(std::move(*source));
}

template <class Functor, class ReturnType, class... Args>
struct VTableImpl
{
    using InvokePtr        = ReturnType (*)(Functor*, Args&&...);
    using DestructorPtr    = void (*)(Functor*);
    using CopyConstructPtr = void (*)(Functor*, const Functor*);
    using MoveConstructPtr = void (*)(Functor*, Functor*);

    InvokePtr        Invoke        = nullptr; // Invokes the functor's operator()
    DestructorPtr    Destruct      = nullptr; // Invokes the functor's destructor
    CopyConstructPtr CopyConstruct = nullptr; // Invokes copy constructor
    MoveConstructPtr MoveConstruct = nullptr; // Invokes copy constructor
};

template <class Functor, class ReturnType, class... Args>
inline constexpr auto CreateVTable() noexcept
{
    VTableImpl<Functor, ReturnType, Args...> table = {};
    table.Invoke                                   = InvokeFunction<Functor, ReturnType, Args...>;
    table.Destruct                                 = DestructFunction<Functor>;
    table.CopyConstruct                            = CopyConstructFunction<Functor>;
    table.MoveConstruct                            = MoveConstructFunction<Functor>;
    return table;
}

template <class Functor, class ReturnType, class... Args>
inline constexpr auto VTable = CreateVTable<Functor, ReturnType, Args...>();

} // namespace Private

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::Function() noexcept {}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::Function(decltype(nullptr)) noexcept {}

template <AllocatorType Allocator, class ReturnType, class... Args>
template <Callable<ReturnType, Args...> Functor, typename>
Function<ReturnType(Args...), Allocator>::Function(const Functor& f) :
    _pVtable(reinterpret_cast<const typename Function<ReturnType(Args...), Allocator>::VTable*>(&Private::VTable<Functor, ReturnType, Args...>)),
    _dynamicBufferSize(sizeof(Functor) > SmallBufferOptimizationSize ? sizeof(Functor) : 0)
{
    if (_pVtable)
    {
        if (_dynamicBufferSize)
        {
            DynamicBuffer = Allocator::Allocate(_dynamicBufferSize, alignof(std::max_align_t));

            _pVtable->CopyConstruct(DynamicBuffer, std::addressof(f));
        }
        else
        {
            _pVtable->CopyConstruct(Storage.GetStoragePtr(), std::addressof(f));
        }
    }
}

template <AllocatorType Allocator, class ReturnType, class... Args>
template <Callable<ReturnType, Args...> Functor, typename>
Function<ReturnType(Args...), Allocator>::Function(Functor&& f) :
    _pVtable(reinterpret_cast<const typename Function<ReturnType(Args...), Allocator>::VTable*>(&Private::VTable<Functor, ReturnType, Args...>)),
    _dynamicBufferSize(sizeof(Functor) > SmallBufferOptimizationSize ? sizeof(Functor) : 0)
{
    if (_pVtable)
    {
        if (_dynamicBufferSize)
        {
            DynamicBuffer = Allocator::Allocate(_dynamicBufferSize, alignof(std::max_align_t));

            _pVtable->MoveConstruct(DynamicBuffer, std::addressof(f));
        }
        else
        {
            _pVtable->MoveConstruct(Storage.GetStoragePtr(), std::addressof(f));
        }
    }
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::Function(const Function& other) :
    _pVtable(other._pVtable),
    _dynamicBufferSize(other._dynamicBufferSize)
{
    if (_pVtable)
    {
        if (_dynamicBufferSize)
        {
            DynamicBuffer = Allocator::Allocate(_dynamicBufferSize, alignof(std::max_align_t));

            _pVtable->CopyConstruct(DynamicBuffer, other.DynamicBuffer);
        }
        else
        {
            _pVtable->CopyConstruct(Storage.GetStoragePtr(), other.Storage.GetStoragePtr());
        }
    }
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::Function(Function&& other) noexcept :
    _pVtable(other._pVtable),
    _dynamicBufferSize(other._dynamicBufferSize)
{
    if (_pVtable)
    {
        if (_dynamicBufferSize)
            DynamicBuffer = other.DynamicBuffer;
        else
        {
            _pVtable->MoveConstruct(Storage.GetStoragePtr(), other.Storage.GetStoragePtr());
            _pVtable->Destruct(other.Storage.GetStoragePtr());
        }
    }

    other._pVtable           = nullptr;
    other._dynamicBufferSize = 0;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::~Function() noexcept
{
    Destroy();
}

template <AllocatorType Allocator, class ReturnType, class... Args>
ReturnType Function<ReturnType(Args...), Allocator>::operator()(Args&&... args)
{
    if (!_pVtable)
        throw InvalidOperationException("Attempted to call a null function");

    return _pVtable->Invoke(_dynamicBufferSize ? DynamicBuffer : Storage.GetStoragePtr(), std::forward<Args>(args)...);
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>& Function<ReturnType(Args...), Allocator>::operator=(const Function& other)
{
    if (this == std::addressof(other))
        return *this;

    if (other._pVtable)
    {
        // Allocates a new buffer if necessary
        if (other._dynamicBufferSize)
        {
            auto newBuffer = Allocator::Allocate(other._dynamicBufferSize, alignof(std::max_align_t));

            other._pVtable->CopyConstruct(newBuffer, other.DynamicBuffer);

            Destroy();

            DynamicBuffer = newBuffer;
        }
        else
        {
            Destroy();

            other._pVtable->CopyConstruct(Storage.GetStoragePtr(), other.Storage.GetStoragePtr());
        }
    }
    else
        Destroy();

    _pVtable           = other._pVtable;
    _dynamicBufferSize = other._dynamicBufferSize;

    return *this;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>& Function<ReturnType(Args...), Allocator>::operator=(Function&& other) noexcept
{
    if (this == std::addressof(other))
        return *this;

    Destroy();

    _pVtable           = other._pVtable;
    _dynamicBufferSize = other._dynamicBufferSize;

    other._pVtable           = nullptr;
    other._dynamicBufferSize = 0;

    if (_pVtable)
    {
        if (_dynamicBufferSize)
            DynamicBuffer = other.DynamicBuffer;
        else
        {
            _pVtable->MoveConstruct(Storage.GetStoragePtr(), other.Storage.GetStoragePtr());

            _pVtable->Destruct(other.Storage.GetStoragePtr());
        }
    }

    return *this;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Bool Function<ReturnType(Args...), Allocator>::operator==(decltype(nullptr)) const noexcept
{
    return _pVtable == nullptr;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Bool Function<ReturnType(Args...), Allocator>::operator!=(decltype(nullptr)) const noexcept
{
    return _pVtable != nullptr;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>& Function<ReturnType(Args...), Allocator>::operator=(decltype(nullptr)) noexcept
{
    Destroy();

    _pVtable = nullptr;

    return *this;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
Function<ReturnType(Args...), Allocator>::operator Bool() const noexcept
{
    return _pVtable != nullptr;
}

template <AllocatorType Allocator, class ReturnType, class... Args>
void Function<ReturnType(Args...), Allocator>::Destroy() noexcept
{
    if (_pVtable)
    {
        _pVtable->Destruct(_dynamicBufferSize ? DynamicBuffer : Storage.GetStoragePtr());

        if (_dynamicBufferSize)
            Allocator::Deallocate(DynamicBuffer);
    }
}

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_FUNCTIONIMPL_INL