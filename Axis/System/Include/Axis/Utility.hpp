/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

/// \file Utility.hpp
///
/// \brief Contains utility functions, data structures and template function
///        helpers.

#ifndef AXIS_SYSTEM_UTILITY_HPP
#define AXIS_SYSTEM_UTILITY_HPP
#pragma once

#include "Config.hpp"
#include <cassert>
#include <type_traits>
#include <utility>

namespace Axis
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

/// \brief Iterating through variadic template arguments.
///
/// \note Reference: https://gist.github.com/nabijaczleweli/37cdd8c28039ea41a999
template <class T, class... Rest>
struct VariadicIterate
{
    using Next                         = VariadicIterate<Rest...>;
    static const constexpr Uint64 Size = 1 + Next::Size;

    template <class C>
    inline constexpr static C ForEach(C cbk, T&& currentArg, Rest&&... restArgs)
    {
        cbk(std::forward<T>(currentArg));
        Next::ForEach(cbk, std::forward<Rest>(restArgs)...);
        return cbk;
    }

    template <class C>
    inline constexpr C operator()(C cbk, T&& currentArg, Rest&&... restArgs) const
    {
        return ForEach(cbk, std::forward<T>(currentArg), std::forward<Rest>(restArgs)...);
    }
};

/// \brief Struct that ignores assignment
struct IgnoreImpl
{
    template <class T> // Ignores the assignment
    inline constexpr const IgnoreImpl& operator=(const T&) const noexcept
    {
        return *this;
    }
};

/// \brief Objects which ignores assignment
inline constexpr IgnoreImpl Ignore = {};

/// \brief Iterating throught variadic template arguments.
///
/// \note Reference: https://gist.github.com/nabijaczleweli/37cdd8c28039ea41a999
template <class T>
struct VariadicIterate<T>
{
    static const constexpr Uint64 Size = 1;

    template <class C>
    inline constexpr static C ForEach(C cbk, T&& currentArg)
    {
        cbk(std::forward<T>(currentArg));
        return cbk;
    }

    template <class C>
    inline constexpr C operator()(C cbk, T&& currentArg) const
    {
        return ForEach(cbk, std::forward<T>(currentArg));
    }
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
    CPVoid GetStoragePtr() const noexcept { return &_staticStorage; };

    /// \brief Get the pointer to the storage.
    PVoid GetStoragePtr() noexcept { return &_staticStorage; };

private:
    alignas(StorageAlign) Uint8 _staticStorage[StorageSize];
};

} // namespace Axis

#endif // AXIS_SYSTEM_UTILITY_HPP