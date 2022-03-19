/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_UTILITY_HPP
#define AXIS_SYSTEM_UTILITY_HPP
#pragma once

#include "Config.hpp"
#include "Trait.hpp"
#include <cstddef>
#include <utility>

namespace Axis::System
{

/// \brief Pair data structure, consists of 2 data members.
template <class TFirst, class TSecond>
struct Pair
{
    /// \brief First element
    TFirst First;

    /// \brief Second element
    TSecond Second;
};

/// \private These tuple implementations are taken from: https://stackoverflow.com/a/52208842
///
/// \see https://stackoverflow.com/a/52208842
template <Size Element, typename T>
struct TupleLeaf
{
public:
    T Value;
};

template <Size Element, typename... Args>
struct TupleImpl;

template <Size Element>
struct TupleImpl<Element>
{
};

template <Size Element, typename T, typename... Rest>
struct TupleImpl<Element, T, Rest...> :
    public TupleLeaf<Element, T>,
    public TupleImpl<Element + 1, Rest...>
{
};

/// \brief Obtains the reference of the object contained in the tuple.
template <Size Element, typename T, typename... Rest>
T& GetTuple(TupleImpl<Element, T, Rest...>& tuple)
{
    return tuple.TupleLeaf<Element, T>::Value;
}

/// \brief Tuple template type for containing multiple types in one type conveniently.
template <typename... Rest>
using Tuple = TupleImpl<0, Rest...>;

/// \brief Class meant to hold the memory storage for specified size.
template <Size StorageSize, Size StorageAlign = alignof(std::max_align_t)>
struct StaticStorage
{
public:
    /// \brief Gets the pointer to the storage.
    const void* GetStoragePtr() const noexcept { return &_staticStorage; };

    /// \brief Get the pointer to the storage.
    void* GetStoragePtr() noexcept { return &_staticStorage; };

private:
    alignas(StorageAlign) Byte _staticStorage[StorageSize];
};

/// \brief Creates rvalue reference to the given type
template <class T>
inline AddRValueReference<T> MakeValue() noexcept;

/// \brief Forwards an lvalue reference as either an rvalue or an lvalue reference.
template <class T>
AXIS_NODISCARD constexpr T&& Forward(RemoveReference<T>& arg) noexcept;

/// \brief Forwards an rvalue as an rvalue reference.
template <class T>
AXIS_NODISCARD constexpr T&& Forward(RemoveReference<T>&& arg) noexcept;

/// \brief Gets the addressof the give reference.
template <class T>
AXIS_NODISCARD constexpr T* AddressOf(T& value);

/// \brief Casts the given reference to rvalue reference.
template <class T>
AXIS_NODISCARD constexpr RemoveReference<T>&& Move(T&& value) noexcept;

/// \brief Returns rvalue reference to the given type if the given type is nothrow move assignable else returns const lvalue reference.
template <Concept::Pure T>
AXIS_NODISCARD constexpr ConditionalType<IsNothrowMoveAssignable<T>, T&&, const T&> MoveAssignIfNoThrow(T& value) noexcept;

/// \brief Returns rvalue reference to the given type if the given type is nothrow move constructible else returns const lvalue reference.
template <Concept::Pure T>
AXIS_NODISCARD constexpr ConditionalType<IsNothrowMoveConstructible<T>, T&&, const T&> MoveConstructIfNoThrow(T& value) noexcept;

/// \brief Used in SFINAE to check if the given template parameter
///       is well-formed or not.
template <class...>
using VoidTypeSink = void;

namespace Detail
{

/// \brief Struct that ignores assignment
struct IgnoreImpl
{
    template <class T> // Ignores the assignment
    constexpr const IgnoreImpl& operator=(const T&) const noexcept
    {
        return *this;
    }
};

} // namespace Detail

/// \brief Objects which ignores assignment
inline constexpr Detail::IgnoreImpl Ignore = {};

} // namespace Axis::System

/// \brief bit mask for the specified number of bits.
#define AXIS_BIT(x) (1 << (x - 1))

#include "../../Private/Axis/UtilityImpl.inl"

#endif // AXIS_SYSTEM_UTILITY_HPP
