/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ITERATOR_HPP
#define AXIS_SYSTEM_ITERATOR_HPP
#pragma once

#include "Allocator.hpp"
#include "Config.hpp"
#include "Trait.hpp"

namespace Axis
{

namespace System
{

/// \brief Iterator which can be randomly accessed and read the content of it.
///
/// - The iterator is random access (can be add and subtract by any difference values).
/// - The iterator is pre-post incrementable / decrementable.
/// - The iterator is comparable (greater then, less then, equal to).
/// - The iterator is differentiable (can be converted differenceType).
/// - The iterator can be dereferenced into const reference of the ValueType.
template <class Iterator, class ValueType>
concept RandomAccessReadIterator = requires(const Iterator constIterator,
                                            Iterator       iterator)
{
    {
        *constIterator // Dereferencable
        } -> IsConvertible<const ValueType&>;
    {
        constIterator + MakeValue<typename PointerTraits<Iterator>::DifferenceType>() // Addable
        } -> IsConvertible<Iterator>;
    {
        constIterator - MakeValue<typename PointerTraits<Iterator>::DifferenceType>() // Subtractable
        } -> IsConvertible<Iterator>;
    {
        constIterator - constIterator // Gets difference between two iterators
        } -> IsConvertible<typename PointerTraits<Iterator>::DifferenceType>;
    {
        iterator += MakeValue<typename PointerTraits<Iterator>::DifferenceType>() // Addable
        } -> IsConvertible<Iterator&>;
    {
        iterator -= MakeValue<typename PointerTraits<Iterator>::DifferenceType>() // Subtractable
        } -> IsConvertible<Iterator&>;
    {
        constIterator > constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        constIterator < constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        constIterator >= constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        constIterator <= constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        constIterator == constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        constIterator != constIterator // Comparable
        } -> IsConvertible<Bool>;
    {
        ++iterator // Prefix incrementable
        } -> IsConvertible<Iterator>;
    {
        iterator++ // Postfix incrementable
        } -> IsConvertible<Iterator>;
    {
        --iterator // Prefix decrementable
        } -> IsConvertible<Iterator>;
    {
        iterator-- // Postfix decrementable
        } -> IsConvertible<Iterator>;
};

} // namespace System

} // namespace Axis

#endif // AXIS_SYSTEM_ITERATOR_HPP