/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_VARIANTIMPL_INL
#define AXIS_SYSTEM_VARIANTIMPL_INL
#pragma once

#include "../../Include/Axis/Variant.hpp"
#include <new>

namespace Axis::System
{

namespace Detail::Variant
{

template <Size T>
struct Max<T>
{
    static constexpr Size Value = T;
};

template <Size T, Size U, Size... Args>
struct Max<T, U, Args...>
{
    static constexpr Size Value = T >= U ? Max<T, Args...>::Value :
                                           Max<U, Args...>::Value;
};

template <Uint8 Index, Uint8 Current, class... Args>
struct GetTypeAtIndexImpl;

template <Uint8 Index, Uint8 Current, class Last>
struct GetTypeAtIndexImpl<Index, Current, Last>
{
    using Type = Last;
};

template <Uint8 Index, Uint8 Current, class First, class... Rest>
struct GetTypeAtIndexImpl<Index, Current, First, Rest...>
{
    using Type = ConditionalType<Index == Current, First, typename GetTypeAtIndexImpl<Index, Current + 1, Rest...>::Type>;
};

template <Uint8 Index, class... Args>
struct GetTypeAtIndex
{
    using Type = typename GetTypeAtIndexImpl<Index, 0, Args...>::Type;
};

template <class Last>
struct TypeChecker<Last>
{
    static constexpr Bool Value = Concept::Pure<Last>;
};

template <class First, class... Rest>
struct TypeChecker<First, Rest...>
{
    static constexpr Bool Value = Concept::Pure<First> ? TypeChecker<Rest...>::Value : false;
};

template <class Last>
struct VariantIsNothrowCopyConstructible<Last>
{
    static constexpr Bool Value = IsNothrowCopyConstructible<Last>;
};

template <class First, class... Rest>
struct VariantIsNothrowCopyConstructible<First, Rest...>
{
    static constexpr Bool Value = IsNothrowCopyConstructible<First> ? VariantIsNothrowCopyConstructible<Rest...>::Value : false;
};

// Destroy functions vvv

using DestroyFunc = void (*)(void*);

template <class T>
constexpr void Destroy(void* pointer)
{
    static_assert(IsNothrowDestructible<T>, "The type was not nothrow destructible.");

    auto objectPointer = reinterpret_cast<T*>(pointer);

    objectPointer->~T();
}

template <class... Types>
inline constexpr DestroyFunc DestroyFunctions[sizeof...(Types)] = {&Destroy<Types>...};

// Destroy functions ^^^
// Copy functions vvv

using CopyFunc = void (*)(void* /*Dest*/, const void* /*Source*/);

template <class T>
constexpr void Copy(void* dest, const void* source)
{
    static_assert(IsCopyConstructible<T>, "The type was not copy constructible.");

    auto destObjectPointer   = reinterpret_cast<T*>(dest);
    auto sourceObjectPointer = reinterpret_cast<const T*>(source);

    new (destObjectPointer) T(*sourceObjectPointer);
}

template <class... Types>
inline constexpr CopyFunc CopyFunctions[sizeof...(Types)] = {&Copy<Types>...};

} // namespace Detail::Variant

template <class... Types>
inline Variant<Types...>::Variant() noexcept :
    _typeIndex(ValuelessIndex) {}

template <class... Types>
inline Variant<Types...>::Variant(const Variant& other) noexcept(Detail::Variant::VariantIsNothrowCopyConstructible<Types...>::Value) :
    _typeIndex(other._typeIndex)
{
    if (_typeIndex != ValuelessIndex)
        Detail::Variant::CopyFunctions<Types...>[_typeIndex](_storage.GetStoragePtr(), other._storage.GetStoragePtr());
}

template <class... Types>
template <Uint8 Index, class... Args>
void Variant<Types...>::Construct(Args&&... args) noexcept(IsNothrowConstructible<TypeAt<Index>, Args...>)
{
    if (_typeIndex != ValuelessIndex)
        Detail::Variant::DestroyFunctions<Types...>[_typeIndex](_storage.GetStoragePtr());

    using TargetType        = TypeAt<Index>;
    using TargetTypePointer = TargetType*;

    auto typePointer = reinterpret_cast<TargetTypePointer>(_storage.GetStoragePtr());

    new (typePointer) TargetType(Forward<Args>(args)...);

    _typeIndex = Index;
}

template <class... Types>
inline Variant<Types...>::~Variant() noexcept
{
    if (_typeIndex != ValuelessIndex)
        Detail::Variant::DestroyFunctions<Types...>[_typeIndex](_storage.GetStoragePtr());
}

template <class... Types>
inline Uint8 Variant<Types...>::GetTypeIndex() const noexcept { return _typeIndex; }

template <class... Types>
inline Bool Variant<Types...>::IsValueless() const noexcept { return _typeIndex == ValuelessIndex; }

} // namespace Axis::System

#endif // AXIS_SYSTEM_VARIANTIMPL_INL