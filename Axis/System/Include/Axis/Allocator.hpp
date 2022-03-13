/// \copyright Simmypeet - Copyright (C)
///            This file is subject to the terms and conditions defined in
///            file 'LICENSE', which is part of this source code package.

#ifndef AXIS_SYSTEM_ALLOCATOR_HPP
#define AXIS_SYSTEM_ALLOCATOR_HPP
#pragma once

#include "Memory.hpp"
#include "Trait.hpp"
#include <limits>

namespace Axis::System
{

/// \brief Indicates how the allocator equality should be evaluated.
enum class AllocatorEquality
{
    /// \brief Two allocators of the same type are always equal.
    AlwaysEqual,

    /// \brief Two allocators of the same type are never equal.
    AlwaysNotEqual,

    /// \brief Allocator equality is determined by (==) and (!=) operators.
    Compare
};

/// \brief The type handles memory allocation and deallocation for the given value type
///        using the provided memory resource type.
///
/// \tparam T The target value type
/// \tparam MemoryResource `MemoryResource` type used for memory allocation and deallocation
template <Concept::Pure T, Concept::MemoryResource MemoryResource>
class Allocator
{
public:
    /// \brief The pointer type that is used by the allocator.
    template <Concept::Pure U>
    using PointerType = U*;

    /// \brief The type of unsigned integer used to represent the count of elements.
    using SizeType = Size;

    /// \brief The type of the object to allocate.
    using ValueType = T;

    /// \brief Allocates memory for the given size and value type.
    ///
    /// The allocated memory is not initialized to any value; make sure to
    /// initialize the memory to a valid value by using the `Construct` function.
    ///
    /// \param[in] size The number of elements to allocate memory for.
    ///
    /// \return The pointer to the allocated memory.
    AXIS_NODISCARD PointerType<T> Allocate(SizeType size);

    /// \brief Deallocates the memory for the given pointer.
    ///
    /// If the memory is already initialized to a valid value, the destructor
    /// won't be called here; make sure to call the `Destruct` function before
    /// deallocating the memory.
    ///
    /// \param[in] memory The pointer to the allocated memory.
    /// \param[in] size The number of elements that were allocated.
    void Deallocate(PointerType<T> pointer,
                    SizeType       size) noexcept;

    /// \brief Constructs the element at the given pointer with the given arguments.
    ///
    /// \tparam Args The types of the arguments to pass to the constructor.
    ///
    /// \param[in] pointer The pointer to uninitialized memory to construct the element at.
    /// \param[in] args The arguments to pass to the constructor.
    template <class... Args>
    void Construct(PointerType<T> pointer,
                   Args&&... args) noexcept(IsNothrowConstructible<T, Args...>);

    /// \brief Destructs the element at the given pointer.
    ///
    /// \param[in] pointer The pointer to the element to destruct.
    template <class... Args>
    void Destruct(PointerType<T> pointer) noexcept;

    /// \brief Don't propagate the allocator when the container is copy assigned.
    static constexpr Bool PropagateOnContainerCopyAssignment = false;

    /// \brief Don't propagate the allocator when the container is move assigned.
    static constexpr Bool PropagateOnContainerMoveAssignment = false;

    /// \brief This allocator is always compare equal to another allocator of the same type.
    static constexpr AllocatorEquality Equality = AllocatorEquality::AlwaysEqual;

    /// \brief The maximum allowed element count that can be allocated at once.
    static constexpr Size MaxAllocationSize = std::numeric_limits<Size>::max() / sizeof(ValueType);

    /// \brief Equality comparison operator
    ///
    /// \param[in] other The other allocator to compare with.
    ///
    /// \return Always returns true.
    inline constexpr Bool operator==(const Allocator&) const noexcept { return true; }

    /// \brief Inequality comparison operator
    ///
    /// \param[in] other The other allocator to compare with.
    ///
    /// \return Always returns false.
    inline constexpr Bool operator!=(const Allocator&) const noexcept { return false; }
};

namespace Concept
{

/// \brief Allocator type that is used for memory allocation and deallocation for
///        the given value type. The allocator will be used in containers to provide
///        all sorts of memory allocation and deallocation functionality.
///
/// To satisfy the concept, the following requirements must be satisfied:
///     - Has a `PointerType` template that is a pointer type that is used by the allocator.
///     - Has a `SizeType` defined that is an unsigned integer type used to represent the count of elements.
///     - Has a `ValueType` defined that is the type of the object to allocate.
///     - Has a `PointerType<T> Allocate(SizeType size)` member function that allocates memory for the given size and value type.
///     - Has a `void Deallocate(PointerType<T> pointer, SizeType size)` member function that deallocates the memory for the given pointer.
///     - Has a `MaxAllocationSize` constexpr static member that is the maximum allowed element count that can be allocated at once.
///
/// \tparam T The allocator type
template <class T>
concept Allocator = requires(T                                                 allocator,
                             typename T::SizeType                              size,
                             typename T::ValueType                             value,
                             typename T::template PointerType<decltype(value)> pointer)
{
    {
        allocator.Allocate(size)
        } -> IsConvertible<decltype(pointer)>;
    {
        T::MaxAllocationSize
        } -> IsConvertible<decltype(size)>;
}
&&Pure<T>&& noexcept(MakeValue<T&>().Deallocate(MakeValue<typename T::template PointerType<typename T::ValueType>>(), MakeValue<typename T::SizeType>()));

} // namespace Concept

template <Concept::Allocator T>
class AllocatorTraits
{
public:
    using ValueType        = typename T::ValueType;
    using SizeType         = typename T::SizeType;
    using ValuePointerType = typename T::template PointerType<ValueType>;
};

} // namespace Axis::System

#include "../../Private/Axis/AllocatorImpl.inl"

#endif // AXIS_SYSTEM_ALLOCATOR_HPP
